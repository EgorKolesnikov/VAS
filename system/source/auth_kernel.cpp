#include "auth_kernel.h"
#include "wav_file.cpp"


/*
*	Constructor
*/

AuthenticationKernel::AuthenticationKernel(
	int wav_split_frame_length
	, int wav_split_frame_step
	, int sound_sample_rate
	, int number_of_mfcc_features
	, int number_of_fbank_features
	, int number_of_global_wav_features
	, bool normilize_audio
	, std::string model_name
	, bool two_step_classification
)
	: wav_split_frame_length_(wav_split_frame_length)
	, wav_split_frame_step_(wav_split_frame_step)
	, sound_sample_rate_(sound_sample_rate)
	, number_of_mfcc_features_(number_of_mfcc_features)
	, number_of_fbank_features_(number_of_fbank_features)
	, number_of_global_wav_features_(number_of_global_wav_features)
	, normilize_audio_(normilize_audio)
	, model_name_(model_name)
	, two_step_classification_(two_step_classification)
{ }


/*
*	Protected utils
*/


std::vector<std::string> AuthenticationKernel::get_directory_entries(const std::string& directory_path, bool get_files){
	/*
	*	Load list of only files or only directories in specified directory.
	*	Store them in vector<string>.
	*/

	std::vector<std::string> directory_entries;

	if(!directory_path.empty()){
		for(boost::filesystem::directory_iterator entry(directory_path); entry != boost::filesystem::directory_iterator(); ++entry){
			boost::filesystem::path current_path = (*entry).path();
			if(get_files == boost::filesystem::is_regular_file(current_path) && current_path.filename().string()[0] != '.'){
				directory_entries.push_back(boost::filesystem::absolute(current_path).string());
			}
		}
	}

	return directory_entries;
}


bool AuthenticationKernel::check_wav_file_format(const WavHeader& wav_header){
	/*
	*	Check wav header to have info about correct format: 
	*	44100 sample rate, 1 channel, 16 bit for one sample
	*/
 
	return wav_header.sample_rate == 44100 && wav_header.num_channels == 1 && wav_header.bits_per_sample == 16;
}


void AuthenticationKernel::extract_features_from_wav_file(const std::string& path_to_wav, const std::string& path_to_store_result, const std::string& path_to_store_global_file_features){
	/*
	*	Parse specified wav file (create features from wav file).
	*	More info in BaseFeatureExtractor and it derivatives.
	*/

	try{
		// load first channel from file
		WavFile wf(path_to_wav);
		int size_in_bytes = wf.get_size_in_bytes();

		// check current wav file to have correct format
		if(!this->check_wav_file_format(wf.get_header())){
			std::cout << "Wav file " << path_to_wav << " have incorrect format. Skipping.\n";
			return;
		}

		// using FeatureExtractors classes to extract features from files frames
		FeaturesCombiner combiner(path_to_wav, path_to_store_result);
		combiner.add<MFCCFeaturesExtractor>(
			path_to_wav
			, this->sound_sample_rate_
			, this->wav_split_frame_length_
			, this->wav_split_frame_step_
			, this->number_of_mfcc_features_
			, this->normilize_audio_
		);
		combiner.add<FbankFeaturesExtractor>(
			path_to_wav
			, this->sound_sample_rate_
			, this->wav_split_frame_length_
			, this->wav_split_frame_step_
			, this->number_of_fbank_features_
			, this->normilize_audio_
		);
		combiner.combine();

		// if we want secondary model to run - parse global wav files features
		if(this->two_step_classification_){
			FeaturesCombiner combiner(path_to_wav, path_to_store_global_file_features);
			combiner.add<GlobalWavFeaturesExtractor>(
				path_to_wav
				, this->sound_sample_rate_
				, this->wav_split_frame_length_ * SETTINGS::GLOBAL_FEATURES_WINDOW_MULTIPLIER
				, this->wav_split_frame_step_ * SETTINGS::GLOBAL_FEATURES_WINDOW_MULTIPLIER
				, this->number_of_global_wav_features_
				, this->normilize_audio_
			);
			combiner.combine();
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::parse_wav_file(...). Exception while parsing wav file.\n";
		std::cout << e.what() << '\n';
	}
}


std::vector<std::pair<int, std::vector<double>>> AuthenticationKernel::load_parsed_files(const std::string& folders_to_parse_prefix, const std::string& filename_substring){
	/*
	*	Load extracted features from all wav files. 
	*	Folder with features: SETTINGS::WAV_FILES_FEATURES_FOLDER
	*	
	*	SETTINGS::WAV_FILES_FEATURES_FOLDER folder has same structure as SETTINGS::WAV_FILES_FOLDER
	*	(but except of wav files there are features for those wav files)
	*
	*	Extracting features only from those files, which contains ${filename_substring} in their names
	*	(if ${filename_substring} is empty - extract all_files)
	*/

	try{
		// load all filenames, which contain features for each parsed wav file (one special folder for that files)
		std::vector<std::string> list_of_folders = this->get_directory_entries(SETTINGS::WAV_FILES_FEATURES_FOLDER, false);

		// here store features for all files, pair(A, B) - features=B, class_of_file=A
		std::vector<std::pair<int, std::vector<double>>> whole_sample;

		for(const std::string& folder_path : list_of_folders){
			// check if we want to parse current folder
			std::string current_directory_name(folder_path.begin() + folder_path.find_last_of("\\/") + 1, folder_path.end());

			if(current_directory_name.size() >= folders_to_parse_prefix.size()){
				if(current_directory_name.substr(0, folders_to_parse_prefix.size()) != folders_to_parse_prefix){
					continue;
				}
			}
			else{
				continue;
			}

			// get current folder voice class from current folder filename (folder name format described in this->extract_features())
			int current_folder_voice_class = std::stoi(
				std::string(folder_path.begin() + folder_path.find_last_of("_") + 1, folder_path.end())
			);
			
			for(const std::string& file_path : this->get_directory_entries(folder_path, true)){
				std::ifstream inf(file_path);
				std::string temp_buffer;

				// parse only files with ${filename_substring} in their names
				if(!filename_substring.empty() && file_path.find(filename_substring) == std::string::npos){
					continue;
				}

				// extracting file data line by line
				while (std::getline(inf, temp_buffer)){
					std::istringstream iss(temp_buffer);
					std::vector<double> one_frame_features;

					// read features for one train or test sample element (line of file)
					double temp_feature;
					while (iss >> temp_feature) { 
						one_frame_features.push_back(temp_feature); 
					}

					// save one string of features as one entry of whole sample
					whole_sample.push_back(std::make_pair(current_folder_voice_class, one_frame_features));
				}

				inf.close();
			}
		}

		return whole_sample;
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::load_parsed_files(...). Exception while loading parsed wav files.\n";
		std::cout << e.what() << '\n';
	}
}



/*
*	Main interface
*/

void AuthenticationKernel::extract_features(){
	/*
	*	Collect all files from specified in constructor folders. Each folder should be 
	*	in following format:	
	*		'voice_$_#', where
	*		 - $ - any string (describing voice in folder)
	*		 - # - class of voice (number from 0 to infty). Classes are unique for different voices
	*	
	*	Creating same folders in result folder with features. Store in that newly created folders
	*	features, extracted from each folder. (one file with features for one wav file)
	*/

	try{
		// folders with wav files to parse
		std::vector<std::string> folders_to_parse = this->get_directory_entries(SETTINGS::WAV_FILES_FOLDER, false);
		
		// parse each folder separately
		for(std::string& folder : folders_to_parse){
			// load filenames from current folder
			std::cout << "\nParsing folder " << folder << "\n";
			std::vector<std::string> list_of_files = this->get_directory_entries(folder, true);
			std::cout << "In folder " << folder << " found " << list_of_files.size() << " files.\n";

			// get directory name and create same directory in folder with features (see mode in SETTINGS::)
			std::string output_directory_name(folder.begin() + folder.find_last_of("\\/") + 1, folder.end());
			if(!boost::filesystem::create_directory(SETTINGS::WAV_FILES_FEATURES_FOLDER + output_directory_name)){
				std::cout << "Can not create directory " + SETTINGS::WAV_FILES_FEATURES_FOLDER + output_directory_name << "\n";
				throw std::exception();
			}

			// create output directory with global wav file features if we are using secondary classificatio model
			std::string global_wav_output_directory_name(folder.begin() + folder.find_last_of("\\/") + 1, folder.end());
			if(this->two_step_classification_){
				global_wav_output_directory_name = "global_" + global_wav_output_directory_name;
				
				if(!boost::filesystem::create_directory(SETTINGS::WAV_FILES_FEATURES_FOLDER + global_wav_output_directory_name)){
					std::cout << "Can not create directory " + SETTINGS::WAV_FILES_FEATURES_FOLDER + global_wav_output_directory_name << "\n";
					throw std::exception();
				}
			}
			
			int count_files_for_log = 0;

			// parse each file separately
			for(std::string& filepath : list_of_files){
				std::cout << "- Parsing file " << filepath << "(" << ++count_files_for_log << " of " << list_of_files.size() << ").\n";

				// get only wav file name (without path and extension)
				std::string filename(filepath.begin() + filepath.find_last_of("\\/") + 1, filepath.begin() + filepath.find_last_of("."));

				// generate output filename for current wavfile
				std::string output_filepath = SETTINGS::WAV_FILES_FEATURES_FOLDER + output_directory_name + "/" + filename + ".features";

				// generate output filename for secondary model features
				std::string secondary_model_features_output_filename = SETTINGS::WAV_FILES_FEATURES_FOLDER + global_wav_output_directory_name + "/" + filename + ".features";

				// parse current wav file
				this->extract_features_from_wav_file(filepath, output_filepath, secondary_model_features_output_filename);
			}
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::extract_features(...). Exception while parsing wav files.\n";
		std::cout << e.what() << '\n';
	}
}

void AuthenticationKernel::create_train_test(
	const std::string& folder_to_save
	, const std::string& train_filename
	, const std::string& test_filename
	, const std::string& folders_to_parse_prefix
	, bool one_vs_all
	, int main_class
){
	/*
	*	Using extracted before wav files features to create test and train samples.
	*	Can create one_vs_all (with 2 classes) train and test, or just leave many classes.
	*	
	*	In each directory those files which have 'part_train' substring in their names
	*	will be written into train sample. Those with 'part_test' - into test sample
	*/

	try{
		std::vector<std::vector<std::string>> routine_configurations = {
			{"train", SETTINGS::TRAIN_FILES_FILENAME_SUBSTRING, folder_to_save + "/" + train_filename}		// train
			, {"test",  SETTINGS::TEST_FILES_FILENAME_SUBSTRING, folder_to_save + "/" + test_filename}		// test
		};

		// same routine for train and test samples creation
		for(auto& current_config : routine_configurations){
			// unpack routine variables
			std::string routine_name = current_config[0];
			std::string files_substring = current_config[1];
			std::string output_filepath = current_config[2];

			// load current sample
			std::cout << "\nCreating " << routine_name << " for model with description: " << folder_to_save << "\n";
			std::vector<std::pair<int, std::vector<double>>> current_sample = this->load_parsed_files(folders_to_parse_prefix, files_substring);

			// if we want one vs all training - reassign classes to frames (main_frame - class 1, else - 0)
			if(one_vs_all){
				for(auto& frame : current_sample){
					if(frame.first == main_class){
						frame.first = 1;
					}
					else{
						frame.first = 0;
					}
				}
			}
			std::cout << "Total found " << current_sample.size() << " samples for " << routine_name << "\n";

			std::shuffle(
				std::begin(current_sample), std::end(current_sample),
				std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
			);

			// saving current sample part
			std::ofstream outf(output_filepath);

			outf << "Class,Features\n";
			for(int record_index = 0; record_index < current_sample.size(); ++record_index){
				outf << current_sample[record_index].first << ",";
				for(int record_data_index = 0; record_data_index < current_sample[record_index].second.size(); ++record_data_index){
					outf << current_sample[record_index].second[record_data_index] << " "; 
				}
				outf << "\n";
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
	*	Run python script to train neural network (and save dump to specified location)
	*	Parameters:
	*	 - mode (fit or predict. Here - fit)
	*	 - path to file with train data
	*	 - path to file with test data
	*	 - path to save trained model dump
	*	 - path to file to write training and testing scores to
	*/

	try{
		std::string command = "python " + SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH + " fit";
		command += " " + model_folder + "/" + SETTINGS::TRAIN_OUTPUT_NAME;
		command += " " + model_folder + "/" + SETTINGS::TEST_OUTPUT_NAME;
		command += " " + model_folder + "/" + SETTINGS::MODEL_DUMP_OUTPUT_NAME;
		command += " " + model_folder + "/" + SETTINGS::MODEL_TRAIN_TEST_SCORE_INFO_NAME;
		command += " " + this->model_name_;
		command += " " + std::to_string(this->two_step_classification_);
		command += " " + model_folder + "/" + SETTINGS::SECONDARY_MODEL_DUMP_OUTPUT_NAME;
		command += " " + model_folder + "/" + SETTINGS::SECONDARY_MODEL_TRAIN_OUTPUT_NAME;
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::fit(). Exception while training model.\n";
		std::cout << e.what() << '\n';
	}
}


int AuthenticationKernel::predict(const std::string& model_folder){
	/*
	*	Predict class for current wav file. It has been already recorded. saved in specified in SETTINGS
	*	folder. Need to extract_features and run python script, which will load specified model dump
	*	(there may be many of them, while testing for example).
	*	
	*	Parameters for python script:
	*	 - mode (fit or predict. Here - predict)
	*	 - path to current file saved features
	*	 - path to saved trained model dump
	*	 - path to store prediction results
	*/

	try{
		// first we need to parse latest recorded wav file
		this->extract_features_from_wav_file(SETTINGS::TEST_WAV_FILE_SAVE_PATH, SETTINGS::TEST_WAV_FEATURES_PATH, SETTINGS::TEST_WAV_GLOBAL_FEATURES_PATH);

		// running python script and saving prediction results
		std::string command = "python " + SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH + " predict";
		command += " " + SETTINGS::TEST_WAV_FEATURES_PATH;
		command += " " + model_folder + "/" + SETTINGS::MODEL_DUMP_OUTPUT_NAME;
		command += " " + SETTINGS::TEST_WAV_PREDICTION_PATH;
		command += " " + this->model_name_;
		command += " " + std::to_string(this->two_step_classification_);
		command += " " + model_folder + "/" + SETTINGS::SECONDARY_MODEL_DUMP_OUTPUT_NAME;
		command += " " + model_folder + "/" + SETTINGS::TEST_WAV_GLOBAL_FEATURES_PATH;
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::fit(). Exception while predicting current recorded voice class.\n";
		std::cout << e.what() << '\n';
	}
}
