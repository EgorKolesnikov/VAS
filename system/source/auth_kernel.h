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

	/*
	*	AuthenticationKernel class provides interface to manage neural network work, such as
	*	 - parse wav file data (voice samples) to train neural network
	*	 - train neural network with parsed data
	*	 - test neural network with newly recorded voice sample
	*	
	*	If large wav files are passed to be trained on, then these wav files are splitted
	*	into smaller pieces (2 seconds duration) and later work is managed with that pieces separately.
	*	(those pieces are not being saved anywhere, they are proceeded online)
	*/

	static std::string MAIN_FOLDER;							// path to VAS folder (main project folder)
	static std::string TEMP_CHANNEL_OUTPUT_PATH;			// filepath to save first channel from wav file
	static std::string TEMP_FRAMES_OUTPUT_PATH;				// filepath to save frames (splitted from wav file)
	static std::string TEMP_PYTHON_MFCC_RESULTS_PATH;		// filepath to save mfcc features of each frame of wav file
	static std::string TEMP_PYTHON_FBANK_RESULTS_PATH;		// filepath to save fbank features of each frame of wav file
	static std::string TEMP_COMPLETE_MFCC_FEATURES;			// filepath to save temp result mfcc features of whole wav file
	static std::string TEMP_COMPLETE_FBANK_FEATURES;		// filepath to save temp result fbank features of whole wav file
	static std::string TRAINED_MODEL_DUMP_PATH;				// filepath to neural network dump
	static std::string TEST_WAV_FEATURES_PATH;				// filepath to store features, extracted from testing wav file
	static std::string TEST_WAV_PREDICTION_PATH;			// filepath to store result of classification


private:

	std::string data_folder_;								// path to folder with data (all wav files, parsed wav files etc.)
	std::string path_to_train_;								// path to train file
	std::string path_to_test_;								// path to test file

	std::string wav_my_voice_folder_;						// path to folder with legal users voice data
	std::string wav_other_voice_folder_;					// path to folder with other users voice data
	std::string output_folder_with_all_voices_;				// path to store features file for each wav file (more in parse_wav_file(...))

	int wav_split_window_length_;							// length (number audio measures) to split each wav file while creating features 
	int wav_split_window_step_;								// step for each splitted window
	int sound_sample_rate_;									// sample rate of each wav file (should be same)
	int number_of_mfcc_features_;							// number of mfcc coeffs to create for each wav file
	int number_of_fbank_features_;							// number of fbank coeffs to create for each wav file
	bool normilize_audio;									// normilize audio files or not
	bool check_for_silence;									// parse and train only on not silence wav file pieces


protected:

	// get list of filenames in specified folder
	std::vector<std::string> get_list_of_files(const std::string& directory_path);

	// parse wav file (more info in method implementation)
	void parse_wav_file(const std::string& path_to_wav, const std::string& path_to_store_result, FEATURES type = FEATURES::MFCC_FBANK);


public:
	AuthenticationKernel(
		const std::string& wav_my_voice_folder
		, const std::string& wav_other_voice_folder
		, const std::string& output_folder
		, int split_window_length
		, int split_window_step
		, int sound_sample_rate
		, int nb_mfcc_features
		, int nb_fbank_features
		, bool normilize = false
		, bool detect_silence = false
	);

	// parse all wav files in 'this->wav_my_voice_folder_' and 'this->wav_other_voice_folder_' folders
	void parse_wav_files(FEATURES type = FEATURES::MFCC_FBANK);

	// create train test from parsed audio files
	void create_train_test(double train_size = 1.0);
	
	// train neural network and save dump of trained model
	int fit();
	
	// run python script to test recorded voice in neural network
	int predict(const std::string& filepath_to_predict, const std::string& path_to_store_result);

	// parse recorded voice audio and prepare data for neural network
	int predict_wav(const std::string& path_to_wav, FEATURES type = FEATURES::MFCC_FBANK);
};


std::string AuthenticationKernel::MAIN_FOLDER = "/home/kolegor/Code/VAS/";
std::string AuthenticationKernel::TEMP_CHANNEL_OUTPUT_PATH = AuthenticationKernel::MAIN_FOLDER + "data/_channel_values.txt";
std::string AuthenticationKernel::TEMP_FRAMES_OUTPUT_PATH = AuthenticationKernel::MAIN_FOLDER + "data/_frames_values.txt";
std::string AuthenticationKernel::TEMP_PYTHON_MFCC_RESULTS_PATH = AuthenticationKernel::MAIN_FOLDER + "data/_python_mfcc_work.txt";
std::string AuthenticationKernel::TEMP_PYTHON_FBANK_RESULTS_PATH = AuthenticationKernel::MAIN_FOLDER + "data/_python_fbank_work.txt";
std::string AuthenticationKernel::TEMP_COMPLETE_MFCC_FEATURES = AuthenticationKernel::MAIN_FOLDER + "data/_complete_mfcc_features.txt";
std::string AuthenticationKernel::TEMP_COMPLETE_FBANK_FEATURES = AuthenticationKernel::MAIN_FOLDER + "data/_complete_fbank_features.txt";
std::string AuthenticationKernel::TRAINED_MODEL_DUMP_PATH = AuthenticationKernel::MAIN_FOLDER + "data/nn_data/trained_model.dump";
std::string AuthenticationKernel::TEST_WAV_FEATURES_PATH = AuthenticationKernel::MAIN_FOLDER + "data/nn_data/last_recorded_wav_features.txt";
std::string AuthenticationKernel::TEST_WAV_PREDICTION_PATH = AuthenticationKernel::MAIN_FOLDER + "/data/nn_data/last_recorded_wav_prediction.txt";


#endif
