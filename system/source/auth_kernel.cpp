#include "auth_kernel.h"

/*
*	Constructors
*/

AuthenticationKernel::AuthenticationKernel(
	const std::string& wav_my_voice_folder
	, const std::string& wav_other_voice_folder
	, const std::string& output_folder
	, int s_rate
	, int s_seconds_length
	, int nb_mfcc_features
	, int nb_fbank_features
)
	: data_folder_(AuthenticationKernel::MAIN_FOLDER + "data/")
	, wav_my_voice_folder_(wav_my_voice_folder)
	, wav_other_voice_folder_(wav_other_voice_folder)
	, output_folder_with_all_voices_(data_folder_ + output_folder)
	, sound_sample_rate_(s_rate)
	, sound_seconds_length_(s_seconds_length)
	, number_of_mfcc_features_(nb_mfcc_features)
	, number_of_fbank_features_(nb_fbank_features)
	, path_to_train_(data_folder_ + "nn_data/_train.txt")
	, path_to_test_(data_folder_ + "nn_data/_test.txt")
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
    		boost::filesystem::directory_iterator(this->data_folder_ + directory_path),
    		boost::filesystem::directory_iterator(), 
    		std::back_inserter(directory_entries)
    	);
	}
	catch(std::exception& e){
		std::cout << "AuthenticationKernel::get_list_of_files(const std::string&). Exception while getting list of files.\n";
	}

  	std::vector<std::string> list_of_files(directory_entries.size());
  	for(int i = 0; i < directory_entries.size(); ++i){
  		list_of_files[i] = directory_entries[i].path().string();
  	}
	return list_of_files;
}

void AuthenticationKernel::parse_wav_file(const std::string& path_to_wav, const std::string& path_to_store_result, FEATURES type, int wav_split_length){
	/*
	*	Parse wav file. Wav file can be any duration. It will be splitted in parts about 88200
	*   (or can be specified through parameter) elements in each part. For each that part 
	*	create specified set of features. Save all this features for each part in one file
	*   (one row for each part)
	*/

	// load first channel from file
	WavFile wf(path_to_wav);
	int size_in_bytes = wf.get_size_in_bytes();

	// create output file with result features (we will append rows in that file, where one row is
	// set of features for one piece of wav file, as described above)
	std::ofstream features_result_output(path_to_store_result);

	// Manage loaded channel by pieces of 'wav_split_length' * 2 size
	// (* 2 because there 16 bits for one sample in wav file)
	int start_channel_byte = 0;
	int end_channel_byte = start_channel_byte + wav_split_length * 2;
	
	while(end_channel_byte < size_in_bytes){
		std::cout << " * " << start_channel_byte << "\n";
		wf.write_first_channel_piece(AuthenticationKernel::TEMP_CHANNEL_OUTPUT_PATH, start_channel_byte, end_channel_byte, false);
	
		PythonMfcc pm(AuthenticationKernel::TEMP_CHANNEL_OUTPUT_PATH, this->sound_sample_rate_, this->sound_seconds_length_);
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

		start_channel_byte = end_channel_byte;
		end_channel_byte = start_channel_byte + wav_split_length * 2;
	}
}



/*
*	Main interface
*/

void AuthenticationKernel::parse_wav_files(FEATURES type, int wav_split_length){
	try{
		int filename_index = 0;
		int file_class = 0;
		double temp_feature = 0.0;
		std::string temp_buffer = "";

		std::vector<std::string> folders_to_parse = { 
			this->wav_other_voice_folder_, 
			this->wav_my_voice_folder_ 
		};
		
		for(std::string& folder : folders_to_parse){
			std::cout << (boost::format("Parsing folder %1%\n") % folder).str();
			std::vector<std::string> list_of_files = this->get_list_of_files(folder);
			std::cout << (boost::format("In folder %1% found %2% files.\n") % folder % list_of_files.size()).str();
			
			int count_files_for_log = 0;
			for(std::string& filepath : list_of_files){
				std::cout << (boost::format(
					"- Parsing file %1% (%2% of %3%).\n"
				) % filepath % ++count_files_for_log % list_of_files.size()).str();

				std::string output_filename = (
					boost::format(
						"%1%File_%2%_name_%3%_class_%4%.txt"
					) % this->output_folder_with_all_voices_ % std::to_string(++filename_index) % 123 % file_class
				).str();

				this->parse_wav_file(filepath, output_filename, type, wav_split_length);
			}
			++file_class;
		}
	}
	catch(std::exception& e){
		std::cout << "void AuthenticationKernel::parse_wav_files(...). Exception while parsing wav files.\n";
	}
}


// void AuthenticationKernel::parse_wav_file(const std::string& path_to_wav, const std::string& path_to_store_result, FEATURES type){
// 	try{
// 		// open wav file and save first channel
// 		WavFile file(path_to_wav);
// 		file.write_first_channel(this->_temp_channel_output_filename, false);

// 		// create mfcc features
// 		PythonMfcc pm(this->_temp_channel_output_filename, this->sound_sample_rate, this->sound_seconds_length);

// 		switch(type){
// 			case FEATURES::MFCC:
// 				pm.calculate_python_mfcc(this->number_of_mfcc_features, this->_temp_python_mfcc_results_filename);
// 				pm.complete_python_mfcc_work(this->_temp_python_mfcc_results_filename);
// 				pm.write_completed_mfcc_features(path_to_store_result);
// 				break;
// 			case FEATURES::FBANK:
// 				pm.calculate_python_fbank(this->_temp_python_fbank_results_filename);
// 				pm.complete_python_fbank_work(this->_temp_python_fbank_results_filename);
// 				pm.write_completed_fbank_features(path_to_store_result);
// 				break;
// 			case FEATURES::MFCC_FBANK:
// 				pm.calculate_python_mfcc(this->number_of_mfcc_features, this->_temp_python_mfcc_results_filename);
// 				pm.complete_python_mfcc_work(this->_temp_python_mfcc_results_filename);
// 				pm.calculate_python_fbank(this->_temp_python_fbank_results_filename);
// 				pm.complete_python_fbank_work(this->_temp_python_fbank_results_filename);
// 				pm.write_completed_mfcc_and_fbank_features(path_to_store_result);
// 				break;
// 		};
// 	}
// 	catch(std::exception& e){
// 		std::cout << "void AuthenticationKernel::parse_wav_file(string, string). Exception while parsing wav files.\n";
// 	}
// }


// AuthenticationKernel::AuthenticationKernel(
// 	const std::string& my_voice_wav_folder
// 	, const std::string& other_voice_wav_folder
// 	, const std::string& output_folder
// 	, int s_rate
// 	, int s_seconds_length
// 	, int n_of_mfcc_features
// 	, int n_of_fbank_features
// )
// 	: data_folder(AuthenticationKernel::_files_directory + "data/")
// 	, wav_folder_with_my_voice(data_folder + my_voice_wav_folder)
// 	, wav_folder_with_other_voice(data_folder + other_voice_wav_folder)
// 	, output_folder_with_all_voices(data_folder + output_folder)
// 	, sound_sample_rate(s_rate)
// 	, sound_seconds_length(s_seconds_length)
// 	, number_of_mfcc_features(n_of_mfcc_features)
// 	, number_of_fbank_features(n_of_fbank_features)
// 	, path_to_train(data_folder + "nn_data/_train.txt")
// 	, path_to_test(data_folder + "nn_data/_test.txt")
// { }


// void AuthenticationKernel::parse_wav_files(FEATURES type){
// 	try{
// 		int filename_index = 0;
// 		int files_class = 0;
// 		double temp_feature = 0.0;
// 		std::string temp_buffer = "";

// 		std::vector<std::string> folders_to_parse = { this->wav_folder_with_other_voice, this->wav_folder_with_my_voice };
// 		for(std::string& folder : folders_to_parse){
// 			std::cout << (boost::format("Parsing folder %1%\n") % folder).str();
// 			std::vector<std::string> list_of_files = this->get_list_of_files(folder);
// 			std::cout << (boost::format("In folder %1% found %2% files.\n") % folder % list_of_files.size()).str();
			
// 			int count_files_for_log = 0;
// 			for(std::string& filepath : list_of_files){
// 				std::cout << (boost::format(
// 					"- Parsing file %1% (%2% of %3%).\n"
// 				) % filepath % ++count_files_for_log % list_of_files.size()).str();

// 				std::string output_filename = (
// 					boost::format(
// 						"%1%File_%2%_class_%3%.txt"
// 					) % this->output_folder_with_all_voices % std::to_string(++filename_index) % files_class
// 				).str();

// 				this->parse_wav_file(filepath, output_filename, type);
// 			}
// 			++files_class;
// 		}
// 	}
// 	catch(std::exception& e){
// 		std::cout << "void AuthenticationKernel::parse_wav_files(). Exception while parsing wav files.\n";
// 	}
// }


// void AuthenticationKernel::create_train_test(double train_size){
// 	try{
// 		std::vector<std::string> list_of_files = this->get_list_of_files(this->output_folder_with_all_voices);
// 		std::vector<std::pair<int, std::vector<double>>> whole_sample(list_of_files.size());

// 		std::string temp_buffer = "";
// 		std::vector<std::string> temp_filename_tokens;
// 		double temp_feature = 0.0;
// 		int file_class = 0;
// 		int count_sample_entries = 0;

// 		for(std::string& filepath : list_of_files){
// 			std::ifstream inf(filepath);

// 			// get file data
// 			std::vector<double> file_data;
// 			while (std::getline(inf, temp_buffer)){
// 				std::istringstream iss(temp_buffer);
// 				while (iss >> temp_feature) { 
// 					file_data.push_back(temp_feature); 
// 				}
// 			}

// 			// parse filaname to findout file class
// 			boost::split(temp_filename_tokens, filepath, boost::is_any_of("\\_."));
// 			file_class = std::stoi(temp_filename_tokens[temp_filename_tokens.size() - 2]);

// 			// save as one entry of whole sample
// 			whole_sample[count_sample_entries++] = std::make_pair(file_class, file_data);
// 			inf.close();
// 		}

// 		// saving sample data and split in train, test if necessary
// 		std::shuffle(
// 			std::begin(whole_sample), std::end(whole_sample),
// 			std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
// 		);

// 		int record_index = 0;
// 		std::ofstream outf_train(this->path_to_train);
// 		std::ofstream outf_test(this->path_to_test);

// 		// train
// 		outf_train << "Class,Features\n";
// 		for(record_index = 0; record_index < whole_sample.size() * train_size; ++record_index){
// 			outf_train << whole_sample[record_index].first << ",";
// 			for(int record_data_index = 0; record_data_index < whole_sample[record_index].second.size(); ++record_data_index){
// 				outf_train << whole_sample[record_index].second[record_data_index] << " "; 
// 			}
// 			outf_train << "\n";
// 		}

// 		// test
// 		outf_test << "Class,Features\n";
// 		for( ; record_index < whole_sample.size(); ++record_index){
// 			outf_test << whole_sample[record_index].first << ",";
// 			for(int record_data_index = 0; record_data_index < whole_sample[record_index].second.size(); ++record_data_index){
// 				outf_test << whole_sample[record_index].second[record_data_index] << " "; 
// 			}
// 			outf_test << "\n";
// 		}

// 		outf_train.close();
// 		outf_test.close();
// 	}
// 	catch(std::exception& e){
// 		std::cout << "void AuthenticationKernel::create_train_test(double). Exception while creating train and test.\n";
// 	}
// }


// int AuthenticationKernel::fit(){
// 	try{
// 		std::string command = "python " + AuthenticationKernel::_files_directory + "source/py_nn.py fit ";
// 		command += this->path_to_train;
// 		command += " " + this->path_to_test;
// 		command += " " + AuthenticationKernel::_trained_model_dump_path;
// 		command += " " + std::to_string(this->number_of_mfcc_features);
// 		system(command.c_str());
// 	}
// 	catch(std::exception& e){
// 		std::cout << "AuthenticationKernel::fit(). Exception while running python script to train nn.\n";
// 		return 1;
// 	}
// 	return 0;
// }


// int AuthenticationKernel::predict(const std::string& filepath_to_predict, const std::string& path_to_store_result){
// 	try{
// 		std::string command = "python " + AuthenticationKernel::_files_directory + "source/py_nn.py predict ";
// 		command += filepath_to_predict;
// 		command += " " + AuthenticationKernel::_trained_model_dump_path;
// 		command += " " + path_to_store_result;
// 		system(command.c_str());
// 	}
// 	catch(std::exception& e){
// 		std::cout << "AuthenticationKernel::predict(). Exception while running python script to predict nn.\n";
// 		return 1;
// 	}
// 	return 0;
// }

// int AuthenticationKernel::predict_wav(const std::string& path_to_wav, FEATURES type){
// 	try{
// 		this->parse_wav_file(path_to_wav, AuthenticationKernel::_store_test_wav_file_features, type);
// 		this->predict(AuthenticationKernel::_store_test_wav_file_features, AuthenticationKernel::_store_test_wav_file_prediction);
// 	}
// 	catch(std::exception& e){
// 		std::cout << "int AuthenticationKernel::predict_wav(const std::string&). Exception while predicting wav file.\n";
// 		return 1;
// 	}
// 	return 0;
// }
