#include "wav_python_work.h"

/*
*	Secondary (protected) functions.
*/

int PythonMfcc::get_file_byte_size(std::fstream& file){
	/*
	*	Find out file size in bytes (using stream seek method)	
	*/

	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

void PythonMfcc::init_amplitudes_from_file(){
	/*
	*	Loading wav file data (saved by WavFile class). In this VAS 16 bit wav files were used
	*	so read bytes from file by pairs and then merge these pairs into one short int (16 bit)
	*	Maybe TODO: pass number of bits for one sample as parameter
	*/

	// open stream to read as binary file
	std::fstream inf(this->path_to_channel_amplitudes, std::fstream::in | std::fstream::binary);
	
	if (inf.is_open()){
		// load and recast data (from char to 'double char' = short int)
		int file_byte_size = this->get_file_byte_size(inf);
		short* data = new short[file_byte_size];
		inf.read(reinterpret_cast<char *>(data), file_byte_size);

		// save loaded data
		this->wav_channel_amplitudes.resize(file_byte_size / sizeof(short));
		for(int tick = 0; tick < this->wav_channel_amplitudes.size(); ++tick){
			this->wav_channel_amplitudes[tick] = data[tick];
		}

		delete[] data;
		inf.close();
	}
	else{
		std::cout << "ChannelFrames::ChannelFrames(const std::string&). Can't open file.\n";
	}
}

void PythonMfcc::load_python_mfcc_work(const std::string& filepath){
	/*
	*	Load result of python script of calculation mfcc features.
	*	Each line of file contains set of features for one frame of wav file.
	*/

	try{
		std::ifstream inf(filepath);
		std::string line;
		double feature;
		
		// parse each line separately
		while (std::getline(inf, line)){
		    std::istringstream iss(line);
		    std::vector<double> frame_features;

		    // read features values in current file line
		    while (iss >> feature) { 
		    	frame_features.push_back(feature); 
		    }

		    // saved parsed line
		    this->frames_mfcc_features.push_back(std::move(frame_features));
		}
		inf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::load_python_mfcc_work(const std::string&). Exception while loading python frames work.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::load_python_fbank_work(const std::string& filepath){
	/*
	*	Load result of python script of calculation fbank features.
	*	Each line of file contains set of features for one frame of wav file.
	*/

	try{
		std::ifstream inf(filepath);
		std::string line;
		double feature;
		
		// parse each line separately
		while (std::getline(inf, line)){
		    std::istringstream iss(line);
		    std::vector<double> frame_features;

		    // read features values in current file line
		    while (iss >> feature) { 
		    	frame_features.push_back(feature); 
		    }

		    // saved parsed line
		    this->frames_fbank_features.push_back(std::move(frame_features));
		}
		inf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::load_python_fbank_work(const std::string&). Exception while loading python frames work.\n";
		std::cout << e.what() << '\n';
	}
}



/*
*	Main interface
*/

PythonMfcc::PythonMfcc(const std::string& amplitudes_filepath, int audio_sample_rate, bool normilize, bool detect_silence)
	: path_to_channel_amplitudes(amplitudes_filepath)
	, sample_rate(audio_sample_rate)
	, normilize_audio(normilize)
	, check_for_silence(detect_silence)
{ 
	try{
		this->init_amplitudes_from_file();
	}
	catch(std::exception& e) {
		std::cout << "PythonMfcc::PythonMfcc(const std::string&). Exception while loading channel amplitudes.\n";
		std::cout << e.what() << '\n';
	}
}

int PythonMfcc::calculate_python_mfcc(int number_of_mfcc_features, const std::string& path_to_store_python_results){
	/*
	*	Run python script to calculate mfcc features. Parameters for script:
	*	 - type of features (mfcc or fbank)
	*    - path to saved wav file data (which is used for mfcc features calculation)
	*	 - wav file sample rate
	*	 - wav file duration in seconds
	*	 - number of mfcc features to generate
	*	 - bool flag: normilize wav audio data ot not
	*	 - bool flag: delete silence parts of wav or not
	*/

	try{
		std::string command = "python " + PythonMfcc::MAIN_FOLDER + "system/source/py_features.py mfcc ";
		command += this->path_to_channel_amplitudes;
		command += " " + path_to_store_python_results;
		command += " " + std::to_string(this->sample_rate);
		command += " " + std::to_string(number_of_mfcc_features);
		command += " " + std::to_string(this->normilize_audio);
		command += " " + std::to_string(this->check_for_silence);
		std::system(command.c_str());

		// std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	catch(std::exception& e){
		std::cout << "PythonMfcc::calculate_python_mfcc(). Exception while running python script to calculating mfcc.\n";
		std::cout << e.what() << '\n';
		return 1;
	}
	return 0;
}


void PythonMfcc::complete_python_mfcc_work(const std::string& path_to_python_mfcc_results) {
	/*
	*	Final manage of python work on mfcc features.
	*	In this VAS calculate mean value for each feature (over all frames)
	*/

	try{
		// load python script work results (more info in 'this->load_python_mfcc_work()')
		this->load_python_mfcc_work(path_to_python_mfcc_results);
		this->complete_file_mfcc_features.resize(this->frames_mfcc_features[0].size());
		
		// manage each feature separately (to store them one by one)
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			for(int frame_index = 0; frame_index < this->frames_mfcc_features.size(); ++frame_index){
				this->complete_file_mfcc_features[feature_index] += this->frames_mfcc_features[frame_index][feature_index];
			}
			this->complete_file_mfcc_features[feature_index] /= this->frames_mfcc_features.size();
		}
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::complete_python_mfcc_work(string, string). Exception while completing python mfcc work.\n";
		std::cout << e.what() << '\n';
	}
}


int PythonMfcc::calculate_python_fbank(int number_of_fbank_features, const std::string& path_to_store_python_fbank_results){
	/*
	*	Run python script to calculate mfcc features. Parameters for script:
	*	 - type of features (mfcc or fbank)
	*	 - filepath to store results
	*	 - wav file audio sample rate
	*	 - bool flag: normilize wav file data or not
	*	 - bool flag: delete silence parts of wav or not
	*/

	try{
		std::string command = "python " + PythonMfcc::MAIN_FOLDER + "system/source/py_features.py fbank ";
		command += this->path_to_channel_amplitudes;
		command += " " + path_to_store_python_fbank_results;
		command += " " + std::to_string(this->sample_rate);
		command += " " + std::to_string(this->normilize_audio);
		command += " " + std::to_string(this->check_for_silence);
		std::system(command.c_str());

		// std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	catch(std::exception& e){
		std::cout << "PythonMfcc::calculate_python_fbank(const std::string&). Exception while running python script to calculating fbank.\n";
		std::cout << e.what() << '\n';
		return 1;
	}
	return 0;
}

void PythonMfcc::complete_python_fbank_work(const std::string& path_to_python_fbank_results) {
	/*
	*	Final manage of python work on fbank features.
	*	In this VAS calculate mean value for each feature (over all frames)
	*/

	try{
		// load python script work results (more info in 'this->load_python_fbank_work()')
		this->load_python_fbank_work(path_to_python_fbank_results);

		// manage each feature separately (to store them one by one)
		this->complete_file_fbank_features.resize(this->frames_fbank_features[0].size());
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			for(int frame_index = 0; frame_index < this->frames_fbank_features.size(); ++frame_index){
				this->complete_file_fbank_features[feature_index] += this->frames_fbank_features[frame_index][feature_index];
			}
			this->complete_file_fbank_features[feature_index] /= this->frames_fbank_features.size();
		}
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::complete_python_mfcc_work(string, string). Exception while completing python mfcc work.\n";
		std::cout << e.what() << '\n';
	}
}



/*
*	Output
*/

void PythonMfcc::write_completed_mfcc_features(const std::string& path_to_store_complete_mfcc_features){
	/*
	*	Save final result of calculating mfcc features.
	*/

	try{
		std::ofstream outf(path_to_store_complete_mfcc_features);
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::write_completed_mfcc_features(const std::string&). Exception while saving complete mfcc features.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::write_completed_fbank_features(const std::string& path_to_store_complete_fbank_features){
	/*
	*	Save final result of calculating fbank features.
	*/

	try{
		std::ofstream outf(path_to_store_complete_fbank_features);
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::write_completed_fbank_features(const std::string&).Exception while saving complete fbank features.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::write_completed_mfcc_and_fbank_features(const std::string& path_to_store_complete_fbank_features){
	/*
	*	Save final result of calculating mfcc and fbank features.
	*   Write them all in one line (separated by ' ')
	*/
	try{
		std::ofstream outf(path_to_store_complete_fbank_features);
		
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::write_completed_mfcc_and_fbank_features(const std::string&). Exception while saving complete mfcc and fbank features.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::append_completed_mfcc_features(std::ostream& outf){
	/*
	*	Append sprcified file (represented as stream) with calculated mfcc features.
	*/

	try{
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		outf << "\n";
		outf.flush();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::append_completed_mfcc_features(std::ostream& outf). Exception while saving complete mfcc features.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::append_completed_fbank_features(std::ostream& outf){
	/*
	*	Append sprcified file (represented as stream) with calculated fbank features.
	*/

	try{
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		outf << "\n";
		outf.flush();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::append_completed_fbank_features(std::ostream& outf). Exception while saving complete fbank features.\n";
		std::cout << e.what() << '\n';
	}
}

void PythonMfcc::append_completed_mfcc_and_fbank_features(std::ostream& outf){
	/*
	*	Append sprcified file (represented as stream) with calculated mfcc and fbank features.
	*/

	try{
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		outf << "\n";
		outf.flush();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::append_completed_mfcc_and_fbank_features(std::ostream& outf). Exception while saving complete mfcc and fbank features.\n";
		std::cout << e.what() << '\n';
	}
}
