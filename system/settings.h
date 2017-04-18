#pragma once

#include <string>


enum class FEATURES_PREPROCESS : int {
	NO_PREPROCESS, NORMALIZATION, WHITENING
};


struct SETTINGS{
	static std::string MAIN_FOLDER;								// path to VAS system folder
	
	static std::string SYSTEM_FOLDER;							// path to system main folder
	static std::string DATA_FOLDER;								// path to data folder
	static std::string PYTHON_SCRIPTS_FOLDER;					// path to folder with python scripts (did not see that comming)
	static std::string TRAINED_MODELS_DUMPS_FOLDER;				// path to save training models dumps
	
	static std::string GLOBAL_DATA_WAV_FILES_FOLDER;			// path to folder containing folders with all wav files
	static std::string TRAIN_DATA_FOLDER;						// path to folder containing folders with train wav files
	static std::string TRAIN_WAV_FILES_FOLDER;					// path to folder with train wav files
	static std::string TRAIN_FILES_FEATURES_FOLDER;				// path to folder with train wav files features
	static std::string TEST_DATA_FOLDER;						// path to folder containing folders with test wav files
	static std::string TEST_WAV_FILES_FOLDER;					// path to folder with test wav files
	static std::string TEST_FILES_FEATURES_FOLDER;				// path to folder with test wav files features
	
	static std::string TEST_WAV_FILE_SAVE_PATH;					// filepath to store recorded (for testing) wav file
	static std::string TEST_WAV_FEATURES_PATH;					// filepath to store features, extracted from testing wav file
	static std::string TEST_WAV_PREDICTION_PATH;				// filepath to store result of classification

	static std::string PYTHON_FEATURES_SCRIPT_PATH;				// filepath to python script for extracting features
	static std::string PYTHON_MODEL_TRAINING_SCRIPT_PATH;		// filepath to python script for training model

	static std::string TRAIN_OUTPUT_NAME;						// filename for file with train data (without directory)
	static std::string TEST_OUTPUT_NAME;						// filename for file with test data (without directory)
	static std::string MODEL_DUMP_OUTPUT_NAME;					// filename for trained models dump (without directory)

	static std::string TRAIN_FILES_FILENAME_SUBSTRING;			// files with features with this substring in their names will be written into train sample
	static std::string TEST_FILES_FILENAME_SUBSTRING;			// files with features with this substring in their names will be written into test sample
	static std::string MAIN_WAV_FILE_FEATURES_FOLDER_PREFIX;	// prefix of folder name with main wav file features
	static std::string FEATURES_FILES_EXTENSION;				// extension of files containing wav files features

	static int SAMPLE_RATE;										// sample rate of all wav files in system
};


std::string SETTINGS::MAIN_FOLDER 								= "/home/kolegor/Code/VAS/";

std::string SETTINGS::SYSTEM_FOLDER 							= SETTINGS::MAIN_FOLDER 			+ "system/";
std::string SETTINGS::DATA_FOLDER 								= SETTINGS::MAIN_FOLDER 			+ "data/";
std::string SETTINGS::PYTHON_SCRIPTS_FOLDER 					= SETTINGS::SYSTEM_FOLDER 			+ "source/scripts/";
std::string SETTINGS::TRAINED_MODELS_DUMPS_FOLDER 				= SETTINGS::DATA_FOLDER 			+ "models/";

std::string SETTINGS::GLOBAL_DATA_WAV_FILES_FOLDER				= SETTINGS::DATA_FOLDER 			+ "storage/";
std::string SETTINGS::TRAIN_DATA_FOLDER							= SETTINGS::DATA_FOLDER 			+ "train/";
std::string SETTINGS::TRAIN_WAV_FILES_FOLDER					= SETTINGS::TRAIN_DATA_FOLDER		+ "data/";
std::string SETTINGS::TRAIN_FILES_FEATURES_FOLDER				= SETTINGS::TRAIN_DATA_FOLDER		+ "features/";
std::string SETTINGS::TEST_DATA_FOLDER							= SETTINGS::DATA_FOLDER				+ "test/";
std::string SETTINGS::TEST_WAV_FILES_FOLDER						= SETTINGS::TEST_DATA_FOLDER		+ "data/";							
std::string SETTINGS::TEST_FILES_FEATURES_FOLDER				= SETTINGS::TEST_DATA_FOLDER		+ "features/";

std::string SETTINGS::TEST_WAV_FILE_SAVE_PATH					= SETTINGS::DATA_FOLDER				+ "_last_recorded.wav";
std::string SETTINGS::TEST_WAV_FEATURES_PATH 					= SETTINGS::DATA_FOLDER 			+ "_last_recorded_wav_features.txt";
std::string SETTINGS::TEST_WAV_PREDICTION_PATH 					= SETTINGS::DATA_FOLDER 			+ "_last_recorded_wav_prediction.txt";

std::string SETTINGS::PYTHON_FEATURES_SCRIPT_PATH				= SETTINGS::PYTHON_SCRIPTS_FOLDER	+ "features.py";
std::string SETTINGS::PYTHON_MODEL_TRAINING_SCRIPT_PATH			= SETTINGS::PYTHON_SCRIPTS_FOLDER	+ "run_auth.py";

std::string SETTINGS::TRAIN_OUTPUT_NAME							= "_train.txt";
std::string SETTINGS::TEST_OUTPUT_NAME							= "_test.txt";
std::string SETTINGS::MODEL_DUMP_OUTPUT_NAME					= "trained_model.dump";

std::string SETTINGS::TRAIN_FILES_FILENAME_SUBSTRING			= "part_train";
std::string SETTINGS::TEST_FILES_FILENAME_SUBSTRING				= "part_test";
std::string SETTINGS::MAIN_WAV_FILE_FEATURES_FOLDER_PREFIX		= "voice_";
std::string SETTINGS::FEATURES_FILES_EXTENSION					= ".features";

int 		SETTINGS::SAMPLE_RATE								= 44100;
