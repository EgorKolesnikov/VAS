#include "wav_python_work.h"

/*
*	Secondary (protected) functions.
*/

int PythonMfcc::get_file_byte_size(std::fstream& file){
	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

void PythonMfcc::init_amplitudes_from_file(){
	std::fstream inf(this->path_to_channel_amplitudes, std::fstream::in | std::fstream::binary);
	if (inf.is_open()){
		int file_byte_size = this->get_file_byte_size(inf);
		short* data = new short[file_byte_size];
		inf.read(reinterpret_cast<char *>(data), file_byte_size);

		this->wav_channel_amplitudes.resize(file_byte_size / sizeof(short));
		for(int tick = 0; tick < this->wav_channel_amplitudes.size(); ++tick){
			this->wav_channel_amplitudes[tick] = data[tick];
		}

		delete[] data;
	}
	else{
		std::cout << "ChannelFrames::ChannelFrames(const std::string&). Can't open file.";
	}
	inf.close();
}

void PythonMfcc::load_python_mfcc_work(const std::string& filepath){
	try{
		std::ifstream inf(filepath);
		std::string line;
		double feature;
		
		while (std::getline(inf, line)){
		    std::istringstream iss(line);
		    std::vector<double> frame_features;

		    while (iss >> feature) { 
		    	frame_features.push_back(feature); 
		    }
		    this->frames_mfcc_features.push_back(std::move(frame_features));
		}
		inf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::load_python_mfcc_work(const std::string&). Exception while loading python frames work.\n";
	}
}

void PythonMfcc::load_python_fbank_work(const std::string& filepath){
	try{
		std::ifstream inf(filepath);
		std::string line;
		double feature;
		
		while (std::getline(inf, line)){
		    std::istringstream iss(line);
		    std::vector<double> frame_features;

		    while (iss >> feature) { 
		    	frame_features.push_back(feature); 
		    }
		    this->frames_fbank_features.push_back(std::move(frame_features));
		}
		inf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::load_python_fbank_work(const std::string&). Exception while loading python frames work.\n";
	}
}



/*
*	Main interface
*/

PythonMfcc::PythonMfcc(const std::string& amplitudes_filepath, int s_rate, int f_seconds_length)
	: path_to_channel_amplitudes(amplitudes_filepath)
	, sample_rate(s_rate)
	, file_seconds_length(f_seconds_length)
{ 
	try{
		this->init_amplitudes_from_file();
	}
	catch(std::exception& e) {
		std::cout << "PythonMfcc::PythonMfcc(const std::string&). Exception while loading channel amplitudes.\n";
	}
}

int PythonMfcc::calculate_python_mfcc(int number_of_mfcc_features, const std::string& path_to_store_python_results){
	try{
		std::string command = "python " + PythonMfcc::_main_folder + "system/source/py_features.py mfcc ";
		command += this->path_to_channel_amplitudes;
		command += " " + path_to_store_python_results;
		command += " " + std::to_string(this->sample_rate);
		command += " " + std::to_string(this->file_seconds_length);
		command += " " + std::to_string(number_of_mfcc_features);
		system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "PythonMfcc::calculate_python_mfcc(). Exception while running python script to calculating mfcc.\n";
		return 1;
	}
	return 0;
}


void PythonMfcc::complete_python_mfcc_work(const std::string& path_to_python_mfcc_results) {
	try{
		this->load_python_mfcc_work(path_to_python_mfcc_results);

		this->complete_file_mfcc_features.resize(this->frames_mfcc_features[0].size());
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			for(int frame_index = 0; frame_index < this->frames_mfcc_features.size(); ++frame_index){
				this->complete_file_mfcc_features[feature_index] += this->frames_mfcc_features[frame_index][feature_index];
			}
			this->complete_file_mfcc_features[feature_index] /= this->frames_mfcc_features.size();
		}
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::complete_python_mfcc_work(string, string). Exception while completing python mfcc work.\n";
	}
}


int PythonMfcc::calculate_python_fbank(int number_of_fbank_features, const std::string& path_to_store_python_fbank_results){
	try{
		std::string command = "python " + PythonMfcc::_main_folder + "system/source/py_features.py fbank ";
		command += this->path_to_channel_amplitudes;
		command += " " + path_to_store_python_fbank_results;
		command += " " + std::to_string(this->sample_rate);
		system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "PythonMfcc::calculate_python_fbank(const std::string&). Exception while running python script to calculating fbank.\n";
		return 1;
	}
	return 0;
}

void PythonMfcc::complete_python_fbank_work(const std::string& path_to_python_fbank_results) {
	try{
		this->load_python_fbank_work(path_to_python_fbank_results);

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
	}
}



/*
*	Output
*/

void PythonMfcc::write_completed_mfcc_features(const std::string& path_to_store_complete_mfcc_features){
	try{
		std::ofstream outf(path_to_store_complete_mfcc_features);
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::write_completed_mfcc_features(const std::string&). Exception while saving complete mfcc features.\n";
	}
}

void PythonMfcc::write_completed_fbank_features(const std::string& path_to_store_complete_fbank_features){
	try{
		std::ofstream outf(path_to_store_complete_fbank_features);
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::write_completed_fbank_features(const std::string&).Exception while saving complete fbank features.\n";
	}
}

void PythonMfcc::write_completed_mfcc_and_fbank_features(const std::string& path_to_store_complete_fbank_features){
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
		std::cout << "void PythonMfcc::write_completed_mfcc_and_fbank_features(const std::string&).Exception while saving complete fbank features.\n";
	}
}

void PythonMfcc::append_completed_mfcc_features(std::ostream& outf){
	try{
		for(int feature_index = 0; feature_index < this->complete_file_mfcc_features.size(); ++feature_index){
			outf << double(this->complete_file_mfcc_features[feature_index]) << " ";
		}
		outf << "\n";
		outf.flush();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::append_completed_mfcc_features(std::ostream& outf). Exception while saving complete fbank features.\n";
	}
}

void PythonMfcc::append_completed_fbank_features(std::ostream& outf){
	try{
		for(int feature_index = 0; feature_index < this->complete_file_fbank_features.size(); ++feature_index){
			outf << double(this->complete_file_fbank_features[feature_index]) << " ";
		}
		outf << "\n";
		outf.flush();
	}
	catch(std::exception& e){
		std::cout << "void PythonMfcc::append_completed_fbank_features(std::ostream& outf). Exception while saving complete fbank features.\n";
	}
}

void PythonMfcc::append_completed_mfcc_and_fbank_features(std::ostream& outf){
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
		std::cout << "void PythonMfcc::append_completed_mfcc_and_fbank_features(std::ostream& outf). Exception while saving complete fbank features.\n";
	}
}
