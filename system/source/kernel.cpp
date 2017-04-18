#include "kernel.h"
#include "wav_file.cpp"


AuthenticationKernel::AuthenticationKernel(
	int wav_split_frame_length
	, int wav_split_frame_step
	, int number_of_mfcc_features
	, int number_of_fbank_features
	, bool normilize_audio
	, std::string model_name
	, bool one_vs_all
	, int main_voice_class
	, FEATURES_PREPROCESS preprocess_type
	, int main_preprocess_voice_class
)
	: wav_split_frame_length_(wav_split_frame_length)
	, wav_split_frame_step_(wav_split_frame_step)
	, number_of_mfcc_features_(number_of_mfcc_features)
	, number_of_fbank_features_(number_of_fbank_features)
	, normilize_audio_(normilize_audio)
	, model_name_(model_name)
	, one_vs_all_(one_vs_all)
	, main_voice_class_(main_voice_class)
	, preprocess_type_(preprocess_type)
	, main_preprocess_voice_class_(main_preprocess_voice_class_)
{ }


/*
*	Main interface
*/

void AuthenticationKernel::extract_features(const std::string& folder_with_wavs, const std::string& folder_to_save_features){
	/*
	*	Collect all wav files from each folder in specified directory. 
	*	Each folder with wav file voices should be in following format:	'voice_$_#', where
	*	 - $ - any string (describing voice in folder)
	*	 - # - class of voice (number from 0 to infty). Classes are unique for different voices
	*
	*	First collecting all wav files paths (which we want to parse). Then running PoolFeaturesExtractor,
	*	who creates many threads to extract features from files.
	*
	*	Creating same directories structure in 'folder_to_save_features' as in folder with wav files
	*	('folder_with_wavs'). This done only to manage storage data properly.
	*	
	*	See also: features_extractor.h, scripts/py_features.py
	*/

	try{
		// accumulate all files that need to be parsed here
		PoolFeaturesExtractor features_extractor;

		// folders with wav files to parse
		std::vector<std::string> folders_to_parse = get_directory_entries(folder_with_wavs, false);
		
		int total_files_count = 0;
		
		for(std::string& folder : folders_to_parse){
			// load filenames from current folder
			std::vector<std::string> list_of_files = get_directory_entries(folder, true);
			std::cout << "In folder " << folder << " found " << list_of_files.size() << " files.\n";

			total_files_count += list_of_files.size();

			// get directory name and create same directory in folder with features (folder_to_save_features)
			std::string output_directory_name(folder.begin() + folder.find_last_of("\\/") + 1, folder.end());
			if(!boost::filesystem::create_directory(folder_to_save_features + output_directory_name)){
				std::cout << "Can not create directory " + folder_to_save_features + output_directory_name << "\n";
				throw std::exception();
			}

			for(std::string& filepath : list_of_files){
				features_extractor.add_file(filepath);
			}
		}

		std::cout << "Ready to parse " << total_files_count << " files. Running " << features_extractor.get_nb_workers() << " threads." << std::endl;
		
		// run parsing procedure (pool executer)
		features_extractor.extract({
			std::to_string(this->wav_split_frame_length_)
			, std::to_string(this->wav_split_frame_step_)
			, std::to_string(this->number_of_fbank_features_)
			, std::to_string(this->number_of_mfcc_features_)
			, std::to_string(this->normilize_audio_)
		});
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::extract_features(...). Exception while parsing wav files.\n";
		std::cout << e.what() << '\n';
	}
}

void AuthenticationKernel::create_train_test(const std::string& folder_to_save){
	/*
	*	Creating train and test samples with wav file features (extracted before, no checks).
	*	Path from where to load all wav files features specified in SETTINGS.
	*	
	*	Here we can manage creation of train and test samples using multiclass classification
	*	or one-vs-all classification. In one-vs-all classification using this->main_voice_class_
	* 	as main class (with label '1') in classification routine.
	*	
	*	'folder_to_save' - speaks for itself. This is path to current model data folder
	*	(in this folder saving model dump, train and test samples)
	*	
	*	See also:	source/settings.h, source/scripts/feature.py
	*/

	try{
		// We do not want to call this routine for train and for test samples separately
		// So we using this hack to run same procedure for train and test samples creation
		std::vector<std::vector<std::string>> routine_configurations = {
			{"train", SETTINGS::TRAIN_FILES_FILENAME_SUBSTRING, folder_to_save + SETTINGS::TRAIN_OUTPUT_NAME, SETTINGS::TRAIN_FILES_FEATURES_FOLDER }	// train routine
			, {"test", SETTINGS::TEST_FILES_FILENAME_SUBSTRING, folder_to_save + SETTINGS::TEST_OUTPUT_NAME,  SETTINGS::TEST_FILES_FEATURES_FOLDER  }	// test routine
		};

		// same routine for train and test samples creation
		for(auto& current_config : routine_configurations){
			// unpack routine variables
			std::string routine_name = current_config[0];
			std::string files_substring = current_config[1];
			std::string output_filepath = current_config[2];
			std::string data_folderpath = current_config[3];

			std::cout << "Creating " << routine_name << " for model with description: " << folder_to_save << "\n";	

			// delete old file and create anew
			std::fstream outf(output_filepath, std::fstream::out);

			// add header in result file
			outf << "Class,Features\n";

			// data in train or test will be collected from all files with *.features signature
			// those files are stored separately in folders for each voice
			for(std::string& folder : get_directory_entries(data_folderpath, false)){
				// get voice class (id) from folder name (here is no need it to be int)
				int current_voice_class = std::stoi(std::string(folder.begin() + folder.find_last_of("_") + 1, folder.end()));

				// remake class id if one-vs-all specified
				if(this->one_vs_all_){
					if(current_voice_class == this->main_voice_class_){
						current_voice_class = 1;
					}
					else{
						current_voice_class = 0;
					}
				}

				// copy all data from *.features files to train (or test) sample
				for(std::string& current_filepath : get_directory_entries(folder, true)){
					// combine only files with features
					if(std::string(current_filepath.begin() + current_filepath.find_last_of("."), current_filepath.end()) != SETTINGS::FEATURES_FILES_EXTENSION){
						continue;
					}

					// read current file
					std::fstream inf(current_filepath, std::fstream::in);

					// line by line
					std::string line_buffer;
					while(std::getline(inf, line_buffer)){
						outf << current_voice_class << "," << line_buffer << "\n";
					}

					inf.close();
				}
			}

			outf.close();
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::create_train_test(...). Exception while creating train and test.\n";
		std::cout << e.what() << '\n';
	}
}


int AuthenticationKernel::fit(const std::string& model_folder){
	/*
	*	Run python script to train model (classificator) (and save dump to specified location)
	*	Parameters for python script:
	*	 - mode (fit or predict. Here - fit)
	*	 - path to file with train data
	*	 - path to file with test data
	*	 - path to save trained model dump
	*	 - preprocess features function
	*	 - main voice class in preprocess features routine
	*
	*	See also:	settings.h
	*/

	try{
		std::string command = "python " + SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH + " fit";
		command += " " + model_folder + SETTINGS::TRAIN_OUTPUT_NAME;
		command += " " + model_folder + SETTINGS::TEST_OUTPUT_NAME;
		command += " " + model_folder + SETTINGS::MODEL_DUMP_OUTPUT_NAME;
		command += " " + this->model_name_;
		command += " " + std::to_string(static_cast<int>(this->preprocess_type_));
		command += " " + std::to_string(this->main_preprocess_voice_class_);
		
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::fit(). Exception while training model.\n";
		std::cout << e.what() << '\n';
	}
}


int AuthenticationKernel::predict(const std::string& model_folder){
	/*
	*	Predict class for current wav file. It has been already recorded and saved in 
	*	specified in SETTINGS folder. 
	*
	*	Extracting features and running python script, which will load specified model dump
	*	(our system can manage many models (classificators)).
	*	
	*	Parameters for python script:
	*	 - mode (fit or predict. Here - predict)
	*	 - path to current file saved features
	*	 - path to saved trained model dump
	*	 - path to store prediction results
	*	 - preprocess features function
	*	 - main voice class in preprocess features routine
	*
	*	See also:	settings.h, features_extractors.h
	*/

	try{
		// extract and save features from test file
		// (as long as we have 1 file - we only need max 1 thread)
		PoolFeaturesExtractor features_extractor(1);
		features_extractor.add_file(SETTINGS::TEST_WAV_FILE_SAVE_PATH);

		std::cout << "Ready to extract features from test file.\n";
		features_extractor.extract({
			std::to_string(this->wav_split_frame_length_)
			, std::to_string(this->wav_split_frame_step_)
			, std::to_string(this->number_of_fbank_features_)
			, std::to_string(this->number_of_mfcc_features_)
			, std::to_string(this->normilize_audio_)
		});

		// running python script and saving prediction results
		std::string command = "python " + SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH + " predict";
		command += " " + SETTINGS::TEST_WAV_FEATURES_PATH;
		command += " " + model_folder + SETTINGS::MODEL_DUMP_OUTPUT_NAME;
		command += " " + SETTINGS::TEST_WAV_PREDICTION_PATH;
		command += " " + this->model_name_;
		
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::fit(). Exception while predicting current recorded voice class.\n";
		std::cout << e.what() << '\n';
	}
}
