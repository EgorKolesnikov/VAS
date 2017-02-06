#include "auth_kernel.h"

/*
*	Constructors
*/

AuthenticationKernel::AuthenticationKernel(
	const std::string& wav_my_voice_folder
	, const std::string& wav_other_voice_folder
	, const std::string& output_folder
	, int split_window_length
	, int split_window_step
	, int s_rate
	, int nb_mfcc_features
	, int nb_fbank_features
	, bool normilize
	, bool detect_silence
)
	: data_folder_(AuthenticationKernel::MAIN_FOLDER + "data/")
	, wav_my_voice_folder_(data_folder_ + wav_my_voice_folder)
	, wav_other_voice_folder_(data_folder_ + wav_other_voice_folder)
	, output_folder_with_all_voices_(data_folder_ + output_folder)
	, wav_split_window_length_(split_window_length)
	, wav_split_window_step_(split_window_step)
	, sound_sample_rate_(s_rate)
	, number_of_mfcc_features_(nb_mfcc_features)
	, number_of_fbank_features_(nb_fbank_features)
	, path_to_train_(data_folder_ + "nn_data/_train.txt")
	, path_to_test_(data_folder_ + "nn_data/_test.txt")
	, normilize_audio(normilize)
	, check_for_silence(detect_silence)
{ }


/*
*	Protected utils
*/

std::vector<std::string> AuthenticationKernel::get_list_of_files(const std::string& directory_path){
	/*
	*	Load list of specified folder files.
	*	Store them in vector<string>.
	*/

	std::vector<boost::filesystem::directory_entry> directory_entries;
	try{
		std::copy(
    		boost::filesystem::directory_iterator(directory_path),
    		boost::filesystem::directory_iterator(), 
    		std::back_inserter(directory_entries)
    	);
	}
	catch(std::exception& e){
		std::cout << "AuthenticationKernel::get_list_of_files(const std::string&). Exception while getting list of files.\n";
		std::cout << e.what() << '\n';
	}

  	std::vector<std::string> list_of_files(directory_entries.size());
  	for(int i = 0; i < directory_entries.size(); ++i){
  		list_of_files[i] = directory_entries[i].path().string();
  	}
	return list_of_files;
}

void AuthenticationKernel::parse_wav_file(const std::string& path_to_wav, const std::string& path_to_store_result, FEATURES type){
	/*
	*	Parse specified wav file (create features from wav file). 
	*	
	*	Wav file can be any duration. It will be splitted in parts about 'this->wav_split_window_length_' entries
	*	in each part with 'this->wav_split_window_step_' entries step. For each that part create specified set of features. 
	*	Save all this features for each part in one file (one row for each part)
	*/

	try{
		// load first channel from file
		WavFile wf(path_to_wav);
		int size_in_bytes = wf.get_size_in_bytes();

		// create output file with result features (we will append rows in that file, where one row is
		// set of features for one piece of wav file, as described above)
		std::ofstream features_result_output(path_to_store_result);

		// Manage loaded channel by pieces of 'this->wav_split_window_length_' * 2 size
		// (* 2 because there 16 bits for one sample in wav file)
		int start_window_byte = 0;
		int end_window_byte = start_window_byte + this->wav_split_window_length_ * 2;
	
		// for each piece of large wav file
		while(end_window_byte < size_in_bytes){		
			// save channel data in file for PythonMfcc class
			wf.write_first_channel_piece(AuthenticationKernel::TEMP_CHANNEL_OUTPUT_PATH, start_window_byte, end_window_byte, false);
		
			// initialize PythonMfcc api
			PythonMfcc pm(AuthenticationKernel::TEMP_CHANNEL_OUTPUT_PATH, this->sound_sample_rate_, this->normilize_audio, this->check_for_silence);
			
			// which set of features we want to create
			switch(type){
				case FEATURES::MFCC:
					pm.calculate_python_mfcc(this->number_of_mfcc_features_, AuthenticationKernel::TEMP_PYTHON_MFCC_RESULTS_PATH);
					pm.complete_python_mfcc_work(AuthenticationKernel::TEMP_PYTHON_MFCC_RESULTS_PATH);
					pm.append_completed_mfcc_features(features_result_output);
					break;
				case FEATURES::FBANK:
					pm.calculate_python_fbank(this->number_of_fbank_features_, AuthenticationKernel::TEMP_PYTHON_FBANK_RESULTS_PATH);
					pm.complete_python_fbank_work(AuthenticationKernel::TEMP_PYTHON_FBANK_RESULTS_PATH);
					pm.append_completed_fbank_features(features_result_output);
					break;
				case FEATURES::MFCC_FBANK:
					pm.calculate_python_mfcc(this->number_of_mfcc_features_, AuthenticationKernel::TEMP_PYTHON_MFCC_RESULTS_PATH);
					pm.complete_python_mfcc_work(AuthenticationKernel::TEMP_PYTHON_MFCC_RESULTS_PATH);
					pm.calculate_python_fbank(this->number_of_fbank_features_, AuthenticationKernel::TEMP_PYTHON_FBANK_RESULTS_PATH);
					pm.complete_python_fbank_work(AuthenticationKernel::TEMP_PYTHON_FBANK_RESULTS_PATH);
					pm.append_completed_mfcc_and_fbank_features(features_result_output);
					break;
			};

			// move wav file window
			start_window_byte += this->wav_split_window_step_ * 2;
			end_window_byte += this->wav_split_window_step_ * 2;
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::parse_wav_file(...). Exception while parsing wav file.\n";
		std::cout << e.what() << '\n';
	}
}



/*
*	Main interface
*/

void AuthenticationKernel::parse_wav_files(FEATURES type){
	/*
	*	Collect all files from specified in constructor folders with legal and not legal voices.
	*	Parse each file in that folders and save result features in specified folder
	*	(one file with features for one wav file)
	*/

	try{
		int filename_index = 0;
		int file_class = 0;

		// folders with wav files to parse
		std::vector<std::string> folders_to_parse = { 
			this->wav_other_voice_folder_, 
			this->wav_my_voice_folder_ 
		};
		
		// parse each folder separately (so we know that in one of them legal and in another - illegal voices)
		// with that knowledge it is easier to set label (class for classification) for each voice sample
		for(std::string& folder : folders_to_parse){
			// load filenames from current folder
			std::cout << (boost::format("Parsing folder %1%\n") % folder).str();
			std::vector<std::string> list_of_files = this->get_list_of_files(folder);
			std::cout << (boost::format("In folder %1% found %2% files.\n") % folder % list_of_files.size()).str();
			
			int count_files_for_log = 0;

			// parse each file separately
			for(std::string& filepath : list_of_files){
				std::cout << (boost::format(
					"- Parsing file %1% (%2% of %3%).\n"
				) % filepath % ++count_files_for_log % list_of_files.size()).str();

				std::string filename = filepath;
				size_t last_pos = filename.find_last_of("\\/");
				if (last_pos != std::string::npos){
				    filename.erase(0, last_pos + 1);
				}

				// generate output filename for file with calculated features
				std::string output_filename = (
					boost::format(
						"%1%File_%2%_name_%3%_class_%4%.txt"
					) % this->output_folder_with_all_voices_ % std::to_string(++filename_index) % filename % file_class
				).str();

				// parse current wav file
				this->parse_wav_file(filepath, output_filename, type);
			}
			++file_class;
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::parse_wav_files(...). Exception while parsing wav files.\n";
		std::cout << e.what() << '\n';
	}
}

void AuthenticationKernel::create_train_test(double train_size){
	/*
	*	Using parsed folders with wav files (for more info see void AuthenticationKernel::parse_wav_files(...))
	*	to create train and test samples for neural network. (Shuffle loaded samples and then split them)
	*	Size of train is specified through 'double train_size' (range from 0 to 1) 
	*/
	try{
		// load all filenames, which contain features for each parsed wav file (one special folder for that files)
		std::vector<std::string> list_of_files = this->get_list_of_files(this->output_folder_with_all_voices_);
		std::vector<std::pair<int, std::vector<double>>> whole_sample;

		std::string temp_buffer = "";
		std::vector<std::string> temp_filename_tokens;
		double temp_feature = 0.0;
		int file_class = 0;
		int count_sample_entries = 0;

		// manage each file separately (note: for large wav files there may be a lot 
		// train and test samples in one files with features [as discussed earlier:
		// one line of features for one train or test sample])
		for(std::string& filepath : list_of_files){
			std::ifstream inf(filepath);

			// parse filaname to findout file class
			boost::split(temp_filename_tokens, filepath, boost::is_any_of("\\_."));
			file_class = std::stoi(temp_filename_tokens[temp_filename_tokens.size() - 2]);

			// get file data
			while (std::getline(inf, temp_buffer)){
				std::istringstream iss(temp_buffer);
				std::vector<double> one_piece_features;

				// read features for one train or test sample element
				while (iss >> temp_feature) { 
					one_piece_features.push_back(temp_feature); 
				}

				// save one string of features as one entry of whole sample
				whole_sample.push_back(std::make_pair(file_class, one_piece_features));
			}

			inf.close();
		}

		// saving sample data and split in train, test if necessary
		std::shuffle(
			std::begin(whole_sample), std::end(whole_sample),
			std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
		);

		int record_index = 0;
		std::ofstream outf_train(this->path_to_train_);
		std::ofstream outf_test(this->path_to_test_);

		// train
		outf_train << "Class,Features\n";
		for(record_index = 0; record_index < whole_sample.size() * train_size; ++record_index){
			outf_train << whole_sample[record_index].first << ",";
			for(int record_data_index = 0; record_data_index < whole_sample[record_index].second.size(); ++record_data_index){
				outf_train << whole_sample[record_index].second[record_data_index] << " "; 
			}
			outf_train << "\n";
		}

		// test
		outf_test << "Class,Features\n";
		for( ; record_index < whole_sample.size(); ++record_index){
			outf_test << whole_sample[record_index].first << ",";
			for(int record_data_index = 0; record_data_index < whole_sample[record_index].second.size(); ++record_data_index){
				outf_test << whole_sample[record_index].second[record_data_index] << " "; 
			}
			outf_test << "\n";
		}

		outf_train.close();
		outf_test.close();
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::create_train_test(double). Exception while creating train and test.\n";
	}
}


int AuthenticationKernel::fit(){
	/*
	*	Run python script to train neural network (and save dump to specified location)
	*	Parameters:
	*	 - mode (fit or predict)
	*	 - path to file with train data
	*	 - path to file with test data
	*	 - path to save dump file
	*	 - number of features for one wav file
	*/

	try{
		std::string command = "python " + AuthenticationKernel::MAIN_FOLDER + "system/source/py_nn.py fit ";
		command += this->path_to_train_;
		command += " " + this->path_to_test_;
		command += " " + AuthenticationKernel::TRAINED_MODEL_DUMP_PATH;
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "AuthenticationKernel::fit(). Exception while running python script to train nn.\n";
		std::cout << e.what() << '\n';
		return 1;
	}
	return 0;
}


int AuthenticationKernel::predict(const std::string& filepath_to_predict, const std::string& path_to_store_result){
	/*
	*	Run python script to predict class of last recorded voice sample.
	*	Parameters:
	*	 - mode (fit or predict)
	*	 - path to wav file features for prediction
	*	 - path to neural network dump
	*	 - path to store prediction results
	*	 - bool flag: normilize audio or not
	*/

	try{
		std::string command = "python " + AuthenticationKernel::MAIN_FOLDER + "system/source/py_nn.py predict ";
		command += filepath_to_predict;
		command += " " + AuthenticationKernel::TRAINED_MODEL_DUMP_PATH;
		command += " " + path_to_store_result;
		command += " " + std::to_string(this->normilize_audio);
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "AuthenticationKernel::predict(). Exception while running python script to predict nn.\n";
		std::cout << e.what() << '\n';
		return 1;
	}
	return 0;
}

int AuthenticationKernel::predict_wav(const std::string& path_to_wav, FEATURES type){
	/*
	*	Prepare (parse) last recorded wav file to be proceeded in neural network for prediction.
	*	(parse wav file and then predict)
	*/

	try{
		this->parse_wav_file(AuthenticationKernel::MAIN_FOLDER + path_to_wav, AuthenticationKernel::TEST_WAV_FEATURES_PATH, type);
		this->predict(AuthenticationKernel::TEST_WAV_FEATURES_PATH, AuthenticationKernel::TEST_WAV_PREDICTION_PATH);
	}
	catch(std::exception& e){
		std::cout << "int AuthenticationKernel::predict_wav(const std::string&). Exception while predicting wav file.\n";
		std::cout << e.what() << '\n';
		return 1;
	}
	return 0;
}
