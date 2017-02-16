#include <vector>
#include <iostream>
#include <string.h>
#include <cstring>
#include <fstream>
#include "../include_all.h"


int main(){
	std::string files_directory = "/home/kolegor/Code/Wav/";
	std::string wav_filename = files_directory + "/data/recorded.wav";
	std::string channel_output_filename = files_directory + "data/_channel_values.txt";
	std::string frames_output_filename = files_directory + "data/_frames_values.txt";
	std::string python_mfcc_results_filename = files_directory + "data/_python_mfcc_work.txt";
	std::string python_fbank_results_filename = files_directory + "data/_python_fbank_work.txt";
	std::string complete_mfcc_features = files_directory + "data/_complete_mfcc_features.txt";
	std::string complete_fbank_features = files_directory + "data/_complete_fbank_features.txt";

	int frames_length = 25;
	int frames_shift = 10;
	int sound_sample_rate = 44100;
	int sound_seconds_length = 2;
	int number_of_mfcc_features = 13;

	try {
		/*
		*	Open wav file. Read first channel. 
		*	Write channel amplitude values to file in binary.
		*/
		
		std::cout << "\n* Reading wav file. Saving first channel. \n\n";
		WavFile file(wav_filename);
		file.write_first_channel(channel_output_filename, false);
		file.get_header().print();


		/*
		*	Running python mfcc and logfbank features
		*/

		std::cout << "\n* Initializing python work.";
		PythonMfcc pm(channel_output_filename, sound_sample_rate, sound_seconds_length);
		
		std::cout << "\n* Calculating MFCC features.";
		pm.calculate_python_mfcc(number_of_mfcc_features, python_mfcc_results_filename);
		pm.complete_python_mfcc_work(python_mfcc_results_filename);
		pm.write_completed_mfcc_features(complete_mfcc_features);

		std::cout << "\n* Calculating LOGFBANK features.";
		pm.calculate_python_fbank(python_fbank_results_filename);
		pm.complete_python_fbank_work(python_fbank_results_filename);
		pm.write_completed_fbank_features(complete_fbank_features);
	}
	catch(std::string& s) {
		std::cout << "\nException: " << s << "\n";
	}

	std::cout << "\n" << std::endl;
	return 0;
}
