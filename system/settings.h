#ifndef __SETTINGS__
#define __SETTINGS__


#include <string>

struct SETTINGS{
	static std::string MAIN_FOLDER;								// path to VAS system folder
	
	static std::string SYSTEM_FOLDER;							// path to system main folder
	static std::string DATA_FOLDER;								// path to data folder
	static std::string PYTHON_SCRIPTS_FOLDER;					// path to folder with python scripts (did not see that comming)
	static std::string TRAINED_MODELS_DUMPS_FOLDER;				// path to save training models dumps
	static std::string WAV_FILES_FOLDER;						// path to folder containing folders with different recorded voices
	static std::string WAV_FILES_FEATURES_FOLDER;				// path to save features for files from WAV_FILES_FOLDER

	static std::string TEMP_WAV_AMPLITUDES_OUTPUT_PATH;			// filepath to save wav file amplitudes
	static std::string TEMP_MFCC_FEATURES_OUTPUT_PATH;			// filepath to save mfcc features for current frame
	static std::string TEMP_FBANK_FEATURES_OUTPUT_PATH;			// filepath to save fbank features for current frame
	
	static std::string TEST_WAV_FILE_SAVE_PATH;					// filepath to store recorded (for testing) wav file
	static std::string TEST_WAV_FEATURES_PATH;					// filepath to store features, extracted from testing wav file
	static std::string TEST_WAV_PREDICTION_PATH;				// filepath to store result of classification

	static std::string PYTHON_MFCC_FEATURES_SCRIPT_PATH;		// filepath to python script for extracting mfcc features
	static std::string PYTHON_FBANK_FEATURES_SCRIPT_PATH;		// filepath to python script for extracting fbank features
	static std::string PYTHON_MODEL_TRAINING_SCRIPT_PATH;		// filepath to python script for training model

	static std::string TRAIN_OUTPUT_NAME;						// filename for file with train data (without directory)
	static std::string TEST_OUTPUT_NAME;						// filename for file with test data (without directory)
	static std::string MODEL_DUMP_OUTPUT_NAME;					// filename for trained models dump (without directory)
	static std::string MODEL_TRAIN_TEST_SCORE_INFO_NAME;		// filename for training and testing score data output
};


std::string SETTINGS::MAIN_FOLDER 								= "/home/kolegor/Code/VAS/";

std::string SETTINGS::SYSTEM_FOLDER 							= SETTINGS::MAIN_FOLDER 			+ "system/";
std::string SETTINGS::DATA_FOLDER 								= SETTINGS::MAIN_FOLDER 			+ "data/";
std::string SETTINGS::PYTHON_SCRIPTS_FOLDER 					= SETTINGS::SYSTEM_FOLDER 			+ "source/python_feature_exctractors/";
std::string SETTINGS::TRAINED_MODELS_DUMPS_FOLDER 				= SETTINGS::DATA_FOLDER 			+ "models_data/";
std::string SETTINGS::WAV_FILES_FOLDER 							= SETTINGS::DATA_FOLDER 			+ "wav_files/";
std::string SETTINGS::WAV_FILES_FEATURES_FOLDER 				= SETTINGS::DATA_FOLDER 			+ "wav_files_features/";

std::string SETTINGS::TEMP_WAV_AMPLITUDES_OUTPUT_PATH 			= SETTINGS::DATA_FOLDER 			+ "_current_wav_amplitudes.txt";
std::string SETTINGS::TEMP_MFCC_FEATURES_OUTPUT_PATH			= SETTINGS::DATA_FOLDER				+ "_frames_mfcc_features.txt";
std::string SETTINGS::TEMP_FBANK_FEATURES_OUTPUT_PATH			= SETTINGS::DATA_FOLDER				+ "_frames_fbank_features.txt";

std::string SETTINGS::TEST_WAV_FILE_SAVE_PATH					= SETTINGS::DATA_FOLDER				+ "_last_recorded.wav";
std::string SETTINGS::TEST_WAV_FEATURES_PATH 					= SETTINGS::DATA_FOLDER 			+ "_last_recorded_wav_features.txt";
std::string SETTINGS::TEST_WAV_PREDICTION_PATH 					= SETTINGS::DATA_FOLDER 			+ "_last_recorded_wav_prediction.txt";

std::string SETTINGS::PYTHON_MFCC_FEATURES_SCRIPT_PATH			= SETTINGS::PYTHON_SCRIPTS_FOLDER	+ "py_mfcc_features.py";
std::string SETTINGS::PYTHON_FBANK_FEATURES_SCRIPT_PATH			= SETTINGS::PYTHON_SCRIPTS_FOLDER	+ "py_fbank_features.py";
std::string SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH			= SETTINGS::PYTHON_SCRIPTS_FOLDER	+ "py_model.py";

std::string SETTINGS::TRAIN_OUTPUT_NAME							= "_train.txt";
std::string SETTINGS::TEST_OUTPUT_NAME							= "_test.txt";
std::string SETTINGS::MODEL_DUMP_OUTPUT_NAME					= "trained_model.dump";
std::string SETTINGS::MODEL_TRAIN_TEST_SCORE_INFO_NAME			= "training_info.txt";


#endif
