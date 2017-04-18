#include "wav_file.h"


//----------------------------------------------------------------------------------------------------
//	Wav Header
//----------------------------------------------------------------------------------------------------


WavHeader::WavHeader(
	int set_chunk_ID
	, int set_chunk_size
	, int set_format
	, int set_subchunk1_ID
	, int set_subchunk1_Size
	, short int set_audio_format
	, short int set_num_channels
	, int set_sample_rate
	, int set_byte_rate
	, short int set_block_align
	, short int set_bits_per_sample
	, int set_subchunk2_ID
	, int set_subchunk2_size
) : 
	chunk_ID(set_chunk_ID)
	, chunk_size(set_chunk_size)
	, format(set_format)
	, subchunk1_ID(set_subchunk1_ID)
	, subchunk1_Size(set_subchunk1_Size)
	, audio_format(set_audio_format)
	, num_channels(set_num_channels)
	, sample_rate(set_sample_rate)
	, byte_rate(set_byte_rate)
	, block_align(set_block_align)
	, bits_per_sample(set_bits_per_sample)
	, subchunk2_ID(set_subchunk2_ID)
	, subchunk2_size(set_subchunk2_size)
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
	this->data = new char[42];
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

void WavFile::load(const std::string& filepath){
	this->init(filepath);
}

int WavFile::get_size_in_bytes(){
	return (this->wav_header).subchunk2_size;
}

WavHeader WavFile::get_header(){
	return this->wav_header;
}

std::vector<short> WavFile::get_amplitudes(){
	/*
	*	Extract amplitudes values from current wav file.
	*	For now it is only for {1 channel, 16-bit per sample} format.
	*/

	try{
		// raw parse is good here
		short* new_data = reinterpret_cast<short *>(data);
		
		// 16 bit = 2 bytes
		int number_of_samples = wav_header.subchunk2_size / 2;
		
		// convert to vector
		std::vector<short> amplitudes;
		amplitudes.reserve(number_of_samples);
		std::move(new_data, new_data + number_of_samples, std::back_inserter(amplitudes));

		return amplitudes;
	}
	catch(std::exception& e){
		std::cout << "WavFile::get_amplitudes(). Exception while converting wav file data to amplitudes values.\n";
		std::cout << e.what() << '\n';
	}
}


void WavFile::write_amplitudes(const std::string& destination_file){
	/*
	*	Writing wav file amplitudes to specified wav file separating amplitudes with spaces. 
	*	First need to get data in amplitudes format.
	*/

	try{
		std::ofstream outf(destination_file);	
		for(short current_amplitude : this->get_amplitudes()){
			outf << current_amplitude << " ";
		}
		outf << "\n";
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "WavFile::write_amplitudes(...). Exception while writing amplitudes to file.\n";
		std::cout << e.what() << '\n';
	}
}


/*
*	Secondary functions for managing WavFile work and processing.
*/

void WavFile::init(const std::string& filename){
	/*
	*	Initializing class instance data (WavHeader and char *data).
	*	Read wav file as binary file (WavHeader::HEADER_SIZE for header data,
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
		std::cout << "WavFile::init(). Exception while initializing \n";
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