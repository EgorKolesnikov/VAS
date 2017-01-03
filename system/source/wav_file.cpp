#include "wav_file.h"


//----------------------------------------------------------------------------------------------------
//	Wav Header
//----------------------------------------------------------------------------------------------------

/*
*	Constructors
*/

WavHeader::WavHeader(
	int set_chunk_ID, int set_chunk_size, int set_format, int set_subchunk1_ID,
	int set_subchunk1_Size, short int set_audio_format, short int set_num_channels,
	int set_sample_rate, int set_byte_rate, short int set_block_align,
	short int set_bits_per_sample, int set_subchunk2_ID, int set_subchunk2_size
) : 
	chunk_ID(set_chunk_ID), chunk_size(set_chunk_size), format(set_format), subchunk1_ID(set_subchunk1_ID),
	subchunk1_Size(set_subchunk1_Size), audio_format(set_audio_format), num_channels(set_num_channels),
	sample_rate(set_sample_rate), byte_rate(set_byte_rate), block_align(set_block_align),
	bits_per_sample(set_bits_per_sample), subchunk2_ID(set_subchunk2_ID), subchunk2_size(set_subchunk2_size)
{ }


WavHeader::WavHeader(const WavHeader& another_header){
	this->chunk_ID = another_header.chunk_ID;
	this->chunk_size = another_header.chunk_size;
	this->format = another_header.format;
	this->subchunk1_ID = another_header.subchunk1_ID;
	this->subchunk1_Size = another_header.subchunk1_Size;
	this->audio_format = another_header.audio_format;
	this->num_channels = another_header.num_channels;
	this->sample_rate = another_header.sample_rate;
	this->byte_rate = another_header.byte_rate;
	this->block_align = another_header.block_align;
	this->bits_per_sample = another_header.bits_per_sample;
	this->subchunk2_ID = another_header.subchunk2_ID;
	this->subchunk2_size = another_header.subchunk2_size;
}


/*
*	Main Interface
*/

void WavHeader::print(){
	std::cout << "ChunkID: " << std::hex << chunk_ID << "\n"
			  << "ChunkSize: " << std::dec << chunk_size << "\n"
			  << "Format: " << std::hex << format << "\n"
			  << "Subchunk1ID: " << std::hex << subchunk1_ID << "\n"
			  << "Subchunk1Size: " << std::dec << subchunk1_Size << "\n"
			  << "AudioFormat: " << audio_format << "\n"
			  << "NumChannels: " << num_channels << "\n"
			  << "SampleRate: " << std::dec << sample_rate << "\n"
			  << "ByteRate: " << byte_rate << "\n"
			  << "BlockAligh: " << block_align << "\n"
			  << "BitsPerSample: " << bits_per_sample << "\n"
			  << "Subchumk2ID: " << std::hex << subchunk2_ID << "\n"
			  << "Subchunk2Size: " << std::dec << subchunk2_size << "\n";
}



//----------------------------------------------------------------------------------------------------
//	Wav File
//----------------------------------------------------------------------------------------------------


/*
*	Constructors (+ operator=)
*/

WavFile::WavFile() {
	this->data = new char[2];
}

WavFile::WavFile(const std::string& filename){
	this->init(filename);
}

WavFile::WavFile(const WavFile& another_file){
	this->clone_file(another_file);
}

WavFile::~WavFile(){
	this->delete_file();
}

WavFile& WavFile::operator=(const WavFile& another_file){
	if(&another_file == this){
		return *this;
	}
	this->delete_file();
	this->clone_file(another_file);
	return *this;
}


/*
*	Main interface
*/

void WavFile::load(const std::string& filename){
	this->init(filename);
}

WavHeader WavFile::get_header(){
	return this->wav_header;
}

std::vector<std::vector<char>> WavFile::get_channels() {
	try {
		int bytes_in_one_channel = this->wav_header.subchunk2_size / this->wav_header.num_channels;
		int one_channel_tick_length = this->wav_header.bits_per_sample / 8;
		
		std::vector<std::vector<char>> channels(this->wav_header.num_channels);
		for(int channel = 0; channel < this->wav_header.num_channels; ++channel){
			channels[channel].reserve(bytes_in_one_channel);
			for(int input_byte = 0; input_byte < this->wav_header.subchunk2_size; input_byte += this->wav_header.num_channels){
				channels[channel].push_back(this->data[input_byte + channel]);
			}
		}

		return channels;
	}
	catch(std::exception& e){
		std::cout << "Exception while extracting channels from wav file.\n";
	}
}

void WavFile::write_first_channel(const std::string& destination_file, bool binary){
	std::vector<std::vector<char>> channels = this->get_channels();

	std::ofstream outf(destination_file);
	if(binary){
		for(int byte_in_channel = 0; byte_in_channel < channels[0].size(); ++byte_in_channel){
			outf << channels[0][byte_in_channel];
		}
	}
	else{
		for(int byte_in_channel = 0; byte_in_channel < channels[0].size(); ++byte_in_channel){
			outf << int(channels[0][byte_in_channel]) << " ";
		}
		outf << "\n";
	}
	outf.close();
}

void WavFile::create_wav(std::string& filename, const WavHeader& wav_header, const std::vector<std::vector<char>>& channels){
	std::fstream outf(filename, std::fstream::out | std::fstream::binary);
	outf.write((char*)&wav_header, sizeof(wav_header));

	for(int byte_in_channel = 0; byte_in_channel < channels[0].size(); ++byte_in_channel){
		for(int channel = 0; channel < channels.size(); ++channel){
			outf.write(&channels[channel][byte_in_channel], 1);
		}
	}

	outf.close();
}


/*
*	Secondary functions for managing WavFile work and processing.
*/

int WavFile::get_file_byte_size(std::fstream& file){
	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

void WavFile::init(const std::string& filename){
	try {
		std::fstream inf(filename, std::fstream::in | std::fstream::binary);
		if(inf.is_open()){
			inf.read((char*)&(this->wav_header), sizeof(this->wav_header));
			data = new char[(this->wav_header).subchunk2_size];
			inf.read(data, (this->wav_header).subchunk2_size);
			inf.close();	
		}
		else{
			throw "Can't open file: " + filename;
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception in WavFile init()\n";
	}
}

void WavFile::delete_file(){
	delete[] this->data;
}

void WavFile::clone_file(const WavFile& another_file){
	this->wav_header = another_file.wav_header;
	this->delete_file();
	this->data = new char[another_file.wav_header.subchunk2_size];

	for(size_t byte = 0; byte < another_file.wav_header.subchunk2_size; ++byte){
		this->data[byte] = another_file.data[byte];
	}
}