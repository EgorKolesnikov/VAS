#include <vector>
#include <iostream>
#include <string.h>
#include <cstring>
#include <fstream>
#include "wav_file.h"
#include "wav_utils.h"


int main(){
	std::string files_directory = "/home/kolegor/Code/Wav/";
	std::string wav_filename = files_directory + "recorded.wav";
	std::string channel_output_filename = files_directory + "channel_values.txt";
	std::string frames_output_filename = files_directory + "frames_values.txt";

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
		file.write_first_channel(channel_output_filename);
		file.get_header().print();
		
		/*
		*	Read binary amplitude values.
		*/
		
		std::cout << "\n* Loading channel info (amplitudes).\n";
		ChannelFrames cf(channel_output_filename, frames_length, frames_shift);

		/*
		*	Preprocess channel values: 
		*	normilize, create frames, apply windowing on frames
		*/

		std::cout << "\n* Preprocessing channels. Creating frames. Preprocessing frames.\n";
		cf.preprocess_amplitudes(NORMALIZATION::PEAK);
		cf.initialize_frames();
		cf.preprocess_frames(WINDOW::HAMMING);

		/*
		*	Write result frames in file in raw mode
		*	(not binary, just values separated by space)
		*/

		std::cout << "\n* Saving preprocessing frames to file.\n";
		cf.write_frames(frames_output_filename);

		/*
		*	Loading frames and run fft on each frame.
		*/

		std::cout << "\n* Loading frames from file. Running fft.\n";
		FeaturesGenerator fg(frames_output_filename, frames_length, frames_shift, sound_sample_rate, sound_seconds_length);
		fg.run_fft();

		/*
		*	Creating mfcc features (in here also doing filterbanks and etc.)
		*/
		fg.create_mfcc_features();
	}
	catch(std::string& s) {
		std::cout << "\nException: " << s << "\n";
	}

	std::cout << std::endl;
	return 0;
}
