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

#include "../settings.h"
#include "wav_file.h"
#include "features_extractors.h"


class AuthenticationKernel{

	/*
	*	AuthenticationKernel class provides interface to manage training model routine work, such as
	*	 - extract features from wav files
	*	 - train neural network with parsed data
	*	 - test neural network with newly recorded voice sample
	*	
	*	All wav files are to be splitted into smaller frames. All features are generated
	*	over these frames (frames are not being saved anywhere, they are proceeded online)
	*/


private:

	int wav_split_frame_length_;			// length (number of samples, not in bytes) to split each wav file while creating features 
	int wav_split_frame_step_;				// step for each splitted window
	int sound_sample_rate_;					// sample rate of each wav file (should be same)
	int number_of_mfcc_features_;			// number of mfcc coeffs to create for each wav file
	int number_of_fbank_features_;			// number of fbank coeffs to create for each wav file
	bool normilize_audio_;					// normilize audio files or not


protected:

	// get list of only files or only directories (bool get_files) in specified folder
	std::vector<std::string> get_directory_entries(const std::string& directory_path, bool get_files = false);

	// check wav file header to have info about correct wav file
	bool check_wav_file_format(const WavHeader& wav_header);

	// extract features from one specified wav file
	void extract_features_from_wav_file(const std::string& path_to_wav_file, const std::string& path_to_store_results);

	// load all parsed files with their classes
	std::vector<std::pair<int, std::vector<double>>> load_parsed_files();


public:

	AuthenticationKernel(
		int wav_split_frame_length
		, int wav_split_frame_step
		, int sound_sample_rate
		, int number_of_mfcc_features
		, int number_of_fbank_features
		, bool normilize_audio = false
	);

	// extract features from all wav files (from folders specified in SETTINGS::)
	void extract_features();

	// create train test from parsed audio files (see more info in method docstring)
	void create_train_test(const std::string& folder_to_save, bool one_vs_all = true, int main_class = 1, double train_size = 0.8);
	
	// train model and save dump (in model format we store model dump and train_test data)
	int fit(const std::string& model_folder);
	
	// run python script to test recorded voice in neural network
	int predict(const std::string& model_folder);
};


#endif
