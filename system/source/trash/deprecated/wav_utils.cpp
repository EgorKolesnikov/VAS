#include "wav_utils.h"


//----------------------------------------------------------------------------------------------------
//	ChannelFrames
//----------------------------------------------------------------------------------------------------


/*
*	Constructors
*/

ChannelFrames::ChannelFrames(int f_length, int f_shift)
	: frame_length(f_length)
	, frame_shift(f_shift)
{ }


ChannelFrames::ChannelFrames(const std::string& channel_binary_filename, int f_length, int f_shift)
	: frame_length(f_length)
	, frame_shift(f_shift)
{
	try{
		std::fstream inf(channel_binary_filename, std::fstream::in | std::fstream::binary);
		if (inf.is_open()){
			int file_byte_size = this->get_file_byte_size(inf);
			short* data = new short[file_byte_size];
			inf.read(reinterpret_cast<char *>(data), file_byte_size);

			this->initial_amplitudes.resize(file_byte_size / sizeof(short));
			for(int tick = 0; tick < this->initial_amplitudes.size(); ++tick){
				this->initial_amplitudes[tick] = data[tick];
			}

			delete[] data;
		}
		else{
			std::cout << "ChannelFrames::ChannelFrames(const std::string&). Can't open file.";
		}
	}
	catch(const std::string& error){
		std::cout << "ChannelFrames::ChannelFrames(const std::string&). Exception.\n";
		std::cout << error << "\n";
	}
}


/*
*	Main interface
*/

void ChannelFrames::initialize_frames() {
	try{
		if(this->preprocessed_amplitudes.empty()){
			std::cout << "ChannelFrames::initialize_frames(). You did not applied amplitudes preprocess." 
					  << "By default PEAK NORMALIZATION will be proceed.\n";
			this->preprocess_amplitudes(NORMALIZATION::PEAK);
		}

		int number_of_frames = ((this->preprocessed_amplitudes.size() - this->frame_length) / this->frame_shift) + 1;
		this->frames.resize(number_of_frames, std::vector<double>(this->frame_length));

		for(int frame_index = 0; frame_index < number_of_frames; ++frame_index){
			std::copy(
				this->preprocessed_amplitudes.begin() + frame_index * this->frame_shift,
				this->preprocessed_amplitudes.begin() + frame_index * this->frame_shift + this->frame_length,
				this->frames[frame_index].begin()
			);
		}
	}
	catch(const std::string& error){
		std::cout << "ChannelFrames::initialize_frames(). Exception.\n";
		std::cout << error << "\n";
	}
}

void ChannelFrames::preprocess_frames(WINDOW frame_window_type){
	switch(frame_window_type){
		case WINDOW::HAMMING:
			this->window_hamming();
			break;
		case WINDOW::HANNING:
			this->window_hanning();
			break;
	};
}

void ChannelFrames::preprocess_amplitudes(NORMALIZATION normalization_type){
	switch(normalization_type){
		case NORMALIZATION::PEAK:
			this->normalization_peak();
			break;

		case NORMALIZATION::RMS:
			this->normalization_rms();
			break;

		case NORMALIZATION::MAX_MIN:
			this->normalization_max_min();
	};
}


/*
*	Input output
*/

std::vector<std::vector<double>> ChannelFrames::get_frames(){
	return this->frames;
}

std::vector<short> ChannelFrames::get_initial_amplitudes(){
	return this->initial_amplitudes;
}

std::vector<double> ChannelFrames::get_preprocessed_amplitudes(){
	return this->preprocessed_amplitudes;
}

void ChannelFrames::write_frames(const std::string& output_filename){
	try{
		std::ofstream outf(output_filename);
		if (outf.is_open()){
			for(int number = 0; number < this->frames.size(); ++number){
				for(int value = 0; value < this->frames[number].size(); ++value){
					outf << this->frames[number][value] << " ";
				}
				outf << "\n";
			}
		}
		else{
			std::cout << "ChannelFrames::write_frames(const std::string&). Can't open file " << output_filename << " for writing.";
		}
	}
	catch(const std::string& error){
		std::cout << "ChannelFrames::write_frames(const std::string&). Exception while writing frames to file.\n";
		std::cout << error << "\n";
	}
}

void ChannelFrames::write_amplitudes(const std::string& output_filename, bool type){
	try{
		std::ofstream outf(output_filename);
		if (outf.is_open()){
			if(type){
				for(int index = 0; index < this->initial_amplitudes.size(); ++index){
					outf << this->initial_amplitudes[index] << " ";
				}
			} else{
				for(int index = 0; index < this->preprocessed_amplitudes.size(); ++index){
					outf << this->preprocessed_amplitudes[index] << " ";
				}
			}
		}
		else{
			std::cout << "ChannelFrames::write_amplitudes(const std::string&). Can't open file " << output_filename << " for writing.";
		}
	}
	catch(const std::string& error){
		std::cout << "ChannelFrames::write_amplitudes(const std::string&). Exception while writing frames to file.\n";
		std::cout << error << "\n";
	}
}


/*
*	Secondary functions for managing WavFile work and processing.
*/

int ChannelFrames::get_file_byte_size(std::fstream& file){
	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}


/*
*	Normalization
*/

void ChannelFrames::normalization_peak(){
	this->preprocessed_amplitudes = std::vector<double>(this->initial_amplitudes.begin(), this->initial_amplitudes.end());
	double normalizator = std::max(
		abs(*std::max_element(this->initial_amplitudes.begin(), initial_amplitudes.end())),
		abs(*std::min_element(this->initial_amplitudes.begin(), initial_amplitudes.end()))
	);

	for(int index=0; index < this->initial_amplitudes.size(); ++index){
		this->preprocessed_amplitudes[index] /= normalizator;
	}
}

void ChannelFrames::normalization_rms(){
	double sum = 0.0;
	for(int i = 0; i < this->initial_amplitudes.size(); ++i){
		sum += pow(this->initial_amplitudes[i], 2.0) / this->initial_amplitudes.size();
	}
	double normalizator = sqrt(sum);

	for(int index=0; index < this->initial_amplitudes.size(); ++index){
		this->preprocessed_amplitudes[index] = double(this->initial_amplitudes[index]) / normalizator;
	}
}

void ChannelFrames::normalization_max_min(){
	double max = *std::max_element(this->initial_amplitudes.begin(), initial_amplitudes.end());
	double min = *std::min_element(this->initial_amplitudes.begin(), initial_amplitudes.end());

	for(int index=0; index < this->initial_amplitudes.size(); ++index){
		this->preprocessed_amplitudes[index] = double(this->initial_amplitudes[index] - min) / (max - min);
	}
}


/*
*	Windows
*/

void ChannelFrames::window_hamming(){
	for(int frame_number = 0; frame_number < this->frames.size(); ++frame_number){
		for(int element = 0; element < this->frame_length; ++element){
			this->frames[frame_number][element] *= (0.53836 - 0.46164 * cos(2 * M_PI * element / (this->frame_length - 1)));
		}
	}
}

void ChannelFrames::window_hanning(){
	for(int frame_number = 0; frame_number < this->frames.size(); ++frame_number){
		for(int element = 0; element < this->frame_length; ++element){
			this->frames[frame_number][element] *= 0.5 * (1 - cos(2 * M_PI * element / (this->frame_length - 1)));
		}
	}
}


//----------------------------------------------------------------------------------------------------
//	FeaturesGenerator
//----------------------------------------------------------------------------------------------------

/*
*	Constructor
*/

FeaturesGenerator::FeaturesGenerator(const std::string& frames_filename, int f_length, int f_shift, int s_sample_rate, int s_seconds_length, int n_of_mfcc)
	: frame_length(f_length)
	, frame_shift(f_shift)
	, sound_sample_rate(s_sample_rate)
	, sound_seconds_length(s_seconds_length)
	, number_of_mfcc(n_of_mfcc)
{
	try{
		assert(number_of_mfcc < frame_length);
		std::ifstream inf(frames_filename);
		this->parse_frames_file(inf);
	}
	catch(const std::string& error){
		std::cout << "FeaturesGenerator::FeaturesGenerator(const std::string&, int, int). Exception while reading frames to file.\n";
		std::cout << error << "\n";
	}
}

/*
*	Secondary functions for managing WavFile work and processing.
*/

void FeaturesGenerator::parse_frames_file(std::ifstream& file){
	try{
		std::string line;
		while (std::getline(file, line)) {
		    std::istringstream iss(line);
		    std::vector<double> frame(this->frame_length);
		    
		    double frame_value = 0.0;
		    int frame_value_index = -1;
		    while(iss >> frame_value){
		    	frame[++frame_value_index] = frame_value; 
		    }
		    this->frames.push_back(frame);
		}
	}
	catch(const std::string& error){
		std::cout << "FeaturesGenerator::parse_frames_file(std::ifstream&). Exception while parsing frames from file.\n";
		std::cout << error << "\n";
	}
}

void FeaturesGenerator::init_frames_spectrum(){
	try{
		this->frames_spectrum.resize(this->frames.size(), std::vector<double>(this->frame_length, 0.0));

		for(int frame_index = 0; frame_index < this->frames.size(); ++frame_index){
			for(int value_index = 0; value_index < this->frame_length; ++value_index){
				this->frames_spectrum[frame_index][value_index] = sqrt(
					pow(this->frames_after_fft[frame_index][value_index].real, 2.0) * 
					pow(this->frames_after_fft[frame_index][value_index].img, 2.0)
				) / this->frame_length;
			}
		}
	}
	catch(const std::string& error){
		std::cout << "FeaturesGenerator::init_frames_spectrum(). Exception while initializeing frames spectrum.\n";
		std::cout << error << "\n";
	}
}

double FeaturesGenerator::get_frequency_by_index_in_frame(int index){
	throw std::runtime_error("get_frequency_by_index_in_frame(int) not implemented yet");
}

double FeaturesGenerator::convert_frequency_to_mel_frequency(double frequency){
	return 1125 * log(1 + frequency / 700.0);
}

double FeaturesGenerator::convert_mel_frequency_to_frequency(double mel_frequency){
	return 700.0 * (exp(mel_frequency / 1125) - 1);
}

/*
*	Main interface
*/

void FeaturesGenerator::run_fft(){
	try {
		int number_of_frames = this->frames.size();
	 	this->frames_after_fft.resize(number_of_frames, std::vector<_fft_complex>(this->frame_length, _fft_complex()));

	 	/*
	 	*	Each frame has same size. 
	 	*	So we do not need to reallocate memory each time.
	 	* 	Reallocating once.
	 	*/

	 	fftw_complex *in, *out;
		in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * number_of_frames);
		out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * number_of_frames);

		/*
	 	*	Do FFT for each frame separately.
	 	*/

	 	for(int current_frame = 0; current_frame < number_of_frames; ++current_frame){
	 		
	 		/*
	 		*	Initialize input data for FFT for current frame.
	 		*/

			for(int frame_value_position = 0; frame_value_position < this->frame_length; ++frame_value_position){
				in[frame_value_position][0] = this->frames[current_frame][frame_value_position];
				in[frame_value_position][1] = 0.0;
			}

			/*
			*	Run FFT
			*/
			
			fftw_plan my_plan;
			my_plan = fftw_plan_dft_1d(this->frame_length, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(my_plan);

			/*
			*	Saving results.
			*/

			for(int frame_value_position = 0; frame_value_position < this->frame_length; ++frame_value_position){
				this->frames_after_fft[current_frame][frame_value_position] = _fft_complex(out[frame_value_position][0], out[frame_value_position][1]);
			}
			fftw_destroy_plan(my_plan);
	 	}

	 	/*
	 	*	Free FFTW allocated data.	
	 	*/
		
		fftw_free(in);
		fftw_free(out);
		fftw_cleanup();

		/*
		*	Initialize frames spectrum
		*/
		this->init_frames_spectrum();
	}
	catch(const std::string& error){
		std::cout << "FeaturesGenerator::run_fft(). Exception while running fft.\n";
		std::cout << error << "\n";
	}
}


// DEPRECATED
void FeaturesGenerator::create_mfcc_features(){
	try{
		
	}
	catch(const std::string& error){
		std::cout << "FeaturesGenerator::create_mfcc_features(). Exception while creating mfcc features.\n";
		std::cout << error << "\n";
	}
}