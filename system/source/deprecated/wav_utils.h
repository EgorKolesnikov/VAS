#ifndef __WAV_UTILS__
#define __WAV_UTILS__


#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <fftw3.h>
#include <algorithm>
#include <cmath>
#include <cassert>
#include "wav_file.h"


//----------------------------------------------------------------------------------------------------
//	ChannelFrames class
//----------------------------------------------------------------------------------------------------

/*
*	Managing initial amplitudes and frames processing.
*	For amplitudes - normalization (Available: peak, rms, max_min)
*	For frames - windowing (Available: hamming and hanning)
*/

enum NORMALIZATION {
	PEAK, RMS, MAX_MIN
};

enum WINDOW {
	HAMMING, HANNING
};

class ChannelFrames{

private:
	
	std::vector<short int> initial_amplitudes;
	std::vector<double> preprocessed_amplitudes;
	std::vector<std::vector<double>> frames;
	int frame_length;
	int frame_shift;


protected:
	
	int get_file_byte_size(std::fstream& file);

	/*
	*	Windows
	*/

	void window_hamming();
	void window_hanning();

	/*
	*	Normalization
	*/

	void normalization_peak();
	void normalization_rms();
	void normalization_max_min();


public:
	
	ChannelFrames(int f_length = 40, int f_shift = 25);
	ChannelFrames(const std::string& channel_binary_filename, int f_length = 40, int f_shift = 25);

	/*
	*	Process
	*/

	void initialize_frames();
	void preprocess_frames(WINDOW window_type);
	void preprocess_amplitudes(NORMALIZATION normalization_type);

	/*
	*	Input output
	*/

	std::vector<std::vector<double>> get_frames();
	std::vector<short> get_initial_amplitudes();
	std::vector<double> get_preprocessed_amplitudes();

	void write_frames(const std::string& output_filename);
	void write_amplitudes(const std::string& output_filename, bool type = true);
};


//----------------------------------------------------------------------------------------------------
//	FeaturesGenerator class
//----------------------------------------------------------------------------------------------------

/*
*	Continue managing frames, generated in ChannelFrames by wav file.
*	(including FFT, mel-cepstral scale and generating features)
*/

class FeaturesGenerator {

private:

	struct _fft_complex{
		double real;
		double img;

		_fft_complex(double r = 0.0, double i = 0.0)
			: real(r)
			, img(i)
		{ }
	};

	std::vector<std::vector<double>> frames;
	std::vector<std::vector<_fft_complex>> frames_after_fft;
	std::vector<std::vector<double>> frames_spectrum;
	std::vector<std::vector<double>> mfcc_features;
	std::vector<std::vector<double>> filterbank_features;
	
	int frame_length;
	int frame_shift;
	int sound_sample_rate;
	int sound_seconds_length;
	int number_of_mfcc;


protected:
	void parse_frames_file(std::ifstream& file);
	void init_frames_spectrum();

	// Applying a FFT of length N results into N frequency bins
	// starting from 0 up to (N-1)/N * f_s with an interval of f_s / N 
	// where f_s is the sampling rate frequency
	
	// DEPRECATED
	double get_frequency_by_index_in_frame(int index);
	
	// 1225 * ln(1 + f / 700)
	double convert_frequency_to_mel_frequency(double frequency);

	// 700 * (exp(m / 1125) - 1)
	double convert_mel_frequency_to_frequency(double mel_frequency);


public:
	FeaturesGenerator(
		const std::string& frames_filename
		, int f_length = 40
		, int f_shift = 25
		, int sound_sample_rate = 44100
		, int sound_seonds_length = 2
		, int n_of_mfcc = 13
	);

	/*
	*	Process
	*/

	void run_fft();

	// DEPRECATED
	void create_mfcc_features();

	// using python_speech_features
	void create_filterbank_features();

	/*
	*	Input Output
	*/

};

#endif
