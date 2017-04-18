#pragma once

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "wav_file.h"


std::string generate_model_folder_name(bool one_vs_all, int main_voice_class, int preprocess_type){
	/*
	*	Generating model folder path in following format:
	*	$1[__$2], where:
	*	 - $1: type of model training. One of ['one_vs_all', 'multiclass']
	*	 - $2: optional (if $1 == 'one_vs_all'). Contains class unique id (number >= 0)
	*	Both of them at the end have preprocess type info
	*/

	if(one_vs_all){
		return "one_vs_all__main_voice__" + std::to_string(main_voice_class) + "__preprocess__" + std::to_string(preprocess_type);
	}
	else{
		return "multiclass";
	}
}


void clear_folder(const std::string& folder_path){
	/*
	*	Clear content of given folder. Done by deleting folder reccursively
	*	and then creating it again.
	*/

	if(boost::filesystem::exists(folder_path)){
		boost::filesystem::remove_all(folder_path);
	}
	boost::filesystem::create_directory(folder_path);
}


std::vector<std::string> get_directory_entries(const std::string& directory_path, bool get_files){
	/*
	*	Load list of only files or only directories in specified directory.
	*	Store them in vector<string>.
	*/

	std::vector<std::string> directory_entries;

	if(!directory_path.empty()){
		for(boost::filesystem::directory_iterator entry(directory_path); entry != boost::filesystem::directory_iterator(); ++entry){
			boost::filesystem::path current_path = (*entry).path();
			if(get_files == boost::filesystem::is_regular_file(current_path) && current_path.filename().string()[0] != '.'){
				directory_entries.push_back(boost::filesystem::absolute(current_path).string());
			}
		}
	}

	return directory_entries;
}


bool check_wav_file_format(const WavHeader& wav_header){
	/*
	*	Check wav header to have info about correct format: 
	*	44100 sample rate, 1 channel, 16 bit for one sample
	*/
 
	return wav_header.sample_rate == 44100 && wav_header.num_channels == 1 && wav_header.bits_per_sample == 16;
}


std::string generate_features_output_filepath(const std::string& input_wav_filepath){
	/*
	*	With given wav file from train or test folder generate filepath to store features to. 
	*
	*	Train and test folder have two subfolders 'data' and 'features' with same directories structure.	
	*	All we need to do is replace '/data/' with '/features/'
	*	(and add extension of features file as described in SETTINGS::...)
	*/

	// check if input wav file is test wav file
	if(input_wav_filepath.compare(SETTINGS::TEST_WAV_FILE_SAVE_PATH) == 0){
		return SETTINGS::TEST_WAV_FEATURES_PATH;
	}

	std::string result = input_wav_filepath;
	
	// replace '/data/' on '/features/'
	size_t where_data = result.rfind("/data/");
	result.replace(result.begin() + where_data, result.begin() + where_data + 6, "/features/");

	// replace '.wav' on '.features'
	size_t where_wav = result.rfind(".wav");
	result.replace(result.begin() + where_wav, result.begin() + where_wav + 4, SETTINGS::FEATURES_FILES_EXTENSION);

	return result;
}

void run_python_script(const std::string& script_path, const std::vector<std::string>& parameters){
	/*
	*	Running specified script (path to script) with given parameters
	*/

	std::string command = "python " + script_path;
	for(const std::string& parameter : parameters){
		command += " \"" + parameter + '"';
	}
	
	std::system(command.c_str());
}
