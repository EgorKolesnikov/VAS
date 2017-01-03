#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "../wav_file.cpp"


int main(){
	std::string path_to_big_wav = "/home/kolegor/011A-11.wav"; // "/home/kolegor/Code/Wav/data/raw_data/big_1.wav";
	std::string path_to_save_parsed_results = "/home/kolegor/Code/Wav/data/raw_data/parsed_big_wavs/";

	WavFile wf(path_to_big_wav);
	wf.get_header().print();

	std::cout << std::endl;
	return 0;
}