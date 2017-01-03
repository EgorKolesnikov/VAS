#ifndef __AUTH_KERNEL__
#define __AUTH_KERNEL__

#include <iostream>
#include <fstream>
#include <cstring>
#include <string.h>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "wav_file.h"
#include "wav_python_work.h"


enum class FEATURES{
	MFCC, FBANK, MFCC_FBANK
};


class AuthenticationKernel{

	static std::string _files_directory;
	static std::string _temp_channel_output_filename;
	static std::string _temp_frames_output_filename;
	static std::string _temp_python_mfcc_results_filename;
	static std::string _temp_python_fbank_results_filename;
	static std::string _temp_complete_mfcc_features;
	static std::string _temp_complete_fbank_features;
	static std::string _trained_model_dump_path;
	static std::string _store_test_wav_file_features;
	static std::string _store_test_wav_file_prediction;


private:

	std::string data_folder;
	std::string wav_folder_with_my_voice;
	std::string wav_folder_with_other_voice;
	std::string output_folder_with_all_voices;
	std::string path_to_train;
	std::string path_to_test;

	int sound_sample_rate;
	int sound_seconds_length;
	int number_of_mfcc_features;
	int number_of_fbank_features;


protected:

	std::vector<std::string> get_list_of_files(const std::string& directory_path);
	void parse_wav_file(const std::string& path_wo_wav, const std::string& path_to_store_result, FEATURES type = FEATURES::MFCC_FBANK);


public:
	AuthenticationKernel(
		const std::string& my_voice_wav_folder
		, const std::string& other_voice_wav_folder
		, const std::string& output_folder
		, int sound_sample_rate
		, int sound_seconds_length
		, int number_of_mfcc_features
		, int number_of_fbank_features
	);

	void parse_wav_files(FEATURES type = FEATURES::MFCC_FBANK);
	void create_train_test(double train_size = 1.0);
	
	int fit();
	int predict(const std::string& filepath_to_predict, const std::string& path_to_store_result);
	int predict_wav(const std::string& path_to_wav, FEATURES type = FEATURES::MFCC_FBANK);

};


std::string AuthenticationKernel::_files_directory = "/home/kolegor/Code/Wav/";
std::string AuthenticationKernel::_temp_channel_output_filename = AuthenticationKernel::_files_directory + "data/_channel_values.txt";
std::string AuthenticationKernel::_temp_frames_output_filename = AuthenticationKernel::_files_directory + "data/_frames_values.txt";
std::string AuthenticationKernel::_temp_python_mfcc_results_filename = AuthenticationKernel::_files_directory + "data/_python_mfcc_work.txt";
std::string AuthenticationKernel::_temp_python_fbank_results_filename = AuthenticationKernel::_files_directory + "data/_python_fbank_work.txt";
std::string AuthenticationKernel::_temp_complete_mfcc_features = AuthenticationKernel::_files_directory + "data/_complete_mfcc_features.txt";
std::string AuthenticationKernel::_temp_complete_fbank_features = AuthenticationKernel::_files_directory + "data/_complete_fbank_features.txt";
std::string AuthenticationKernel::_trained_model_dump_path = AuthenticationKernel::_files_directory + "data/nn_data/trained_model.dump";
std::string AuthenticationKernel::_store_test_wav_file_features = AuthenticationKernel::_files_directory + "data/nn_data/last_recorded_wav_features.txt";
std::string AuthenticationKernel::_store_test_wav_file_prediction = AuthenticationKernel::_files_directory + "/data/nn_data/last_recorded_wav_prediction.txt";


#endif
