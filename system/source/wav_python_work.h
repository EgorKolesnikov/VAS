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


class PythonMfcc{

	static std::string _main_folder;

private:
	std::string path_to_channel_amplitudes;
	std::vector<double> wav_channel_amplitudes;
	std::vector<std::vector<double>> frames_mfcc_features;
	std::vector<std::vector<double>> frames_fbank_features;
	std::vector<double> complete_file_mfcc_features;
	std::vector<double> complete_file_fbank_features;
	int sample_rate;
	int file_seconds_length;


protected:
	int get_file_byte_size(std::fstream& file);
	void init_amplitudes_from_file();
	void load_python_mfcc_work(const std::string& filepath);
	void load_python_fbank_work(const std::string& filepath);


public:
	PythonMfcc(
		const std::string& amplitudes_filepath
		, int s_rate
		, int f_seconds_length
	);


	/*
	*	Main interface
	*/

	int calculate_python_mfcc(int number_of_mfcc_features, const std::string& path_to_store_python_results);
	void complete_python_mfcc_work(const std::string& path_to_python_mfcc_results);

	int calculate_python_fbank(int number_of_fbank_features, const std::string& path_to_store_python_fbank_results);
	void complete_python_fbank_work(const std::string& path_to_python_fbank_results);


	/*
	*	Output
	*/

	void write_completed_mfcc_features(const std::string& path_to_store_complete_mfcc_features);
	void write_completed_fbank_features(const std::string& path_to_store_complete_fbank_features);
	void write_completed_mfcc_and_fbank_features(const std::string& path_to_store_complete_fbank_features);

	void append_completed_mfcc_features(std::ostream& outf);
	void append_completed_fbank_features(std::ostream& outf);
	void append_completed_mfcc_and_fbank_features(std::ostream& outf);

};

std::string PythonMfcc::_main_folder = "/home/kolegor/Code/VAS/";

#endif
