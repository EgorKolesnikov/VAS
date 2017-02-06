#ifndef __WAV_PYTHON_WORK__
#define __WAV_PYTHON_WORK__

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>


class PythonMfcc{

	/*
	*	PythonMfcc class provides interface to use Python libraries
	*	for MFCC and Filterbank features calculation 
	*/

	static std::string MAIN_FOLDER; 								// Path to VAS directory (with VAS in path)

private:
	std::string path_to_channel_amplitudes;							// filepath to load wav file first channel data (should be saved in binary)
	std::vector<double> wav_channel_amplitudes;						// loaded channel data (double for if we want to normilize data)
	std::vector<std::vector<double>> frames_mfcc_features;			// vector of mfcc features for each frame of loaded wav file data
	std::vector<std::vector<double>> frames_fbank_features;			// same as above but for filterbank features
	std::vector<double> complete_file_mfcc_features;				// result of integration of each frame mfcc features (in VAS using mean)
	std::vector<double> complete_file_fbank_features;				// same as above but for filterbank features
	
	int sample_rate;												// wav file sample rate
	bool normilize_audio;											// whether or not we want to normilize channel data
	bool check_for_silence;											// parse and train only on not silence wav file pieces


protected:

	// get files size in bytes
	int get_file_byte_size(std::fstream& file);
	
	// initializing this->wav_channel_amplitudes
	void init_amplitudes_from_file();
	
	// read result of Python work of calculating MFCC feautres (init this->frames_mfcc_features)
	void load_python_mfcc_work(const std::string& filepath);
	
	// read result of Python work of calculating fbank feautres (init this->frames_fbank_features)
	void load_python_fbank_work(const std::string& filepath);


public:
	PythonMfcc(
		const std::string& amplitudes_filepath
		, int audio_sample_rate
		, bool normilize = false
		, bool detect_silence = false
	);


	/*
	*	Main interface
	*/

	// run python script to calculate mfcc features
	int calculate_python_mfcc(int number_of_mfcc_features, const std::string& path_to_store_python_results);
	
	// manage previous python script results (init this->complete_file_mfcc_features)
	void complete_python_mfcc_work(const std::string& path_to_python_mfcc_results);

	// run python script to calculate fbank features
	int calculate_python_fbank(int number_of_fbank_features, const std::string& path_to_store_python_fbank_results);
	
	// manage previous python script results (init this->complete_file_fbank_features)
	void complete_python_fbank_work(const std::string& path_to_python_fbank_results);


	/*
	*	Output
	*/

	// save calculated mfcc features to file
	void write_completed_mfcc_features(const std::string& path_to_store_complete_mfcc_features);
	
	// save calculated fbank features to file
	void write_completed_fbank_features(const std::string& path_to_store_complete_fbank_features);
	
	// save calculated mfcc and fbank features to file
	void write_completed_mfcc_and_fbank_features(const std::string& path_to_store_complete_fbank_features);

	// using when parsing big wav files (saving separate wav file pieces features [one line of features - one piece of file])
	void append_completed_mfcc_features(std::ostream& outf);
	void append_completed_fbank_features(std::ostream& outf);
	void append_completed_mfcc_and_fbank_features(std::ostream& outf);

};

std::string PythonMfcc::MAIN_FOLDER = "/home/kolegor/Code/VAS/";

#endif
