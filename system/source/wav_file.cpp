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

int WavFile::get_size_in_bytes(){
	return (this->wav_header).subchunk2_size;
}

WavHeader WavFile::get_header(){
	return this->wav_header;
}

std::vector<std::vector<char>> WavFile::get_channels() {
	/*
	*	Return data from wav files as vector of channels data.
	*	Channel is vector of bytes. Data splitted into channels
	*	as follows: 1 2 3 .. n 1 2 3 ... n 1 2 3 ... n ...
	*	(where 1 2 3 ... n - in which channel to save byte at current index)
	*/

	try {
		int bytes_in_one_channel = this->wav_header.subchunk2_size / this->wav_header.num_channels;
		int one_channel_tick_length = this->wav_header.bits_per_sample / 8;
		
		std::vector<std::vector<char>> channels(this->wav_header.num_channels);

		// saving channel by channel (first full first channel data, then second and so on)
		for(int channel = 0; channel < this->wav_header.num_channels; ++channel){
			channels[channel].reserve(bytes_in_one_channel);

			// fill current channels data (step bytes by this->wav_header.num_channels positions)
			for(int input_byte = 0; input_byte < this->wav_header.subchunk2_size; input_byte += this->wav_header.num_channels * one_channel_tick_length){
				for(int tick = 0; tick < one_channel_tick_length; ++tick){
					channels[channel].push_back(this->data[input_byte + channel + tick]);
				}
			}
		}

		return channels;
	}
	catch(std::exception& e){
		std::cout << "Exception while extracting channels from wav file.\n";
		std::cout << e.what() << '\n';
	}
}

void WavFile::write_first_channel_piece(const std::string& destination_file, int start_index, int end_index, bool binary){
	/*
	*	Save specified piece of data bytes into destination_file (same as above).
	*	Interval of bytes to save is represented through start_index and end_index.
	*	Note: no checks for segmentation errors.
	*/
	
	std::ofstream outf(destination_file);
	
	if(binary){
		for(int bytes_iterator = start_index; bytes_iterator < end_index; ++bytes_iterator){
			outf << this->data[bytes_iterator];
		}
	}
	else{
		for(int bytes_iterator = start_index; bytes_iterator < end_index; ++bytes_iterator){
			outf << int(this->data[bytes_iterator]) << " ";
		}
		outf << "\n";
	}
	
	outf.close();
}

void WavFile::write_first_channel(const std::string& destination_file, bool binary){
	/*
	*	Saving first channel data (bytes) into destination file.
	*	If binary = true then result file will be in binary format
	*	else - destination file will contain bytes splitted by ' '
	*/

	this->write_first_channel_piece(destination_file, 0, strlen(this->data), binary);
}

void WavFile::create_wav(std::string& filename, const WavHeader& wav_header, const std::vector<std::vector<char>>& channels){
	/*
	*	Used for testing WavFile and WavHeader classes.
	*   Open wav file, read data and header into different structures
	*	and then save file anew (with same data and header)
	*/

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
	/*
	*	Find out file size in bytes (file as stream)	
	*/

	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

void WavFile::init(const std::string& filename){
	/*
	*	Initializing class instance data (WavHeader and char *data).
	*	Read wav file as binary file (WavHeader::HEADER_SIZE for header data;
	*	rest is for this->data)
	*/

	try {
		std::fstream inf(filename, std::fstream::in | std::fstream::binary);
		if(inf.is_open()){
			// initialize this->header
			inf.read((char*)&(this->wav_header), sizeof(this->wav_header));
			
			// from this->header find out data size in bytes
			data = new char[(this->wav_header).subchunk2_size];

			// read the rest of wav file data into this->data
			inf.read(data, (this->wav_header).subchunk2_size);
			inf.close();	
		}
		else{
			std::cout << "Can't open file: " + filename << "\n";
			throw std::exception();
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception in WavFile init()\n";
		std::cout << e.what() << '\n';
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