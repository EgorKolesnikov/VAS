#pragma once

#include <algorithm>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>

#include "../settings.h"
#include "features.h"
#include "util.cpp"


class AuthenticationKernel{

	/*
	*	AuthenticationKernel class provides interface to manage training model routine work, such as
	*	 - extract features from wav files
	*	 - train neural network with parsed data
	*	 - test neural network with newly recorded voice sample
	*	
	*	All wav files will be splitted into smaller frames. All features are generated
	*	over these frames (frames are not being saved anywhere, they are proceeded online)
	*
	*	All routine is done with the help of python scripts. So, basically, this is just
	*	C++ wrapper for voice classificator.
	*/


private:

	int wav_split_frame_length_;			// length (number of samples, not in bytes) to split each wav file while creating features 
	int wav_split_frame_step_;				// step for each splitted window
	int number_of_mfcc_features_;			// number of mfcc coeffs to create for each wav file
	int number_of_fbank_features_;			// number of fbank coeffs to create for each wav file
	bool normilize_audio_;					// normilize audio files or not
	std::string model_name_;			    // model to train (one of available ['NN', 'RF'])

	bool one_vs_all_;						// run one_vs_all train and test
	int main_voice_class_;					// use only if one_vs_all = true

	FEATURES_PREPROCESS preprocess_type_;	// preprocess features with this function (see more in settings.h)
	int main_preprocess_voice_class_;		// main voice class in features preprocess routine (may be None)


public:

	AuthenticationKernel(
		int wav_split_frame_length
		, int wav_split_frame_step
		, int number_of_mfcc_features
		, int number_of_fbank_features
		, bool normilize_audio
		, std::string model_name
		, bool one_vs_all = false
		, int main_voice_class_ = -1
		, FEATURES_PREPROCESS preprocess_type = FEATURES_PREPROCESS::NO_PREPROCESS
		, int main_preprocess_voice_class = -1
	);

	// extract features from all wav files (from folders specified in SETTINGS::)
	void extract_features(const std::string& folder_with_wavs, const std::string& folder_to_save);

	// create train test files for current model
	void create_train_test(const std::string& folder_to_save);
	
	// train model and save dump (python script)
	int fit(const std::string& model_folder);
	
	// test recorded voice (python script)
	int predict(const std::string& model_folder);
};
