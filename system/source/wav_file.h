#ifndef __WAV_FILE__
#define __WAV_FILE__


#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>


//----------------------------------------------------------------------------------------------------
//	WavHeader helper structure
//----------------------------------------------------------------------------------------------------

struct WavHeader{

	/*
	*	Structure contains information about wav file header.
	*/

	static const char HEADER_SIZE = 44;

	int chunk_ID;							// Contains 'RIFF' (0x52494646 big-endian)
	int chunk_size;							// Entire file size - 8 (exluding first two fields)
	int format;								// Contains 'WAVE' (0x57415645 big-endian)
	int subchunk1_ID;						// Contains 'fmt ' (0x666d7420 big-endian)
	int subchunk1_Size;						// 16 for PCM. Size of the rest of the subchink 'fmt'
	short int audio_format;					// Form of compression. For PCM = 1.
	short int num_channels;					// Number of channels. Mono = 1, Stereo = 2 ...
	int sample_rate;						// Number of samples per second (44100, ...)
	int byte_rate;							// sample_rate * num_channels * bits_per_sample / 8
	short int block_align;					// num_channels * bits_per_pample / 8
	short int bits_per_sample;				// Bits to decode one sample value
	int subchunk2_ID;						// Contains 'data' (0x64617461 big-endian)
	int subchunk2_size;						// Size of all data.


public:

	WavHeader(
		int set_chunk_ID = -1
		, int set_chunk_size = -1
		, int set_format = -1
		, int set_subchunk1_ID = -1
		, int set_subchunk1_Size = -1
		, short int set_audio_format = -1
		, short int set_num_channels = -1
		, int set_sample_rate = -1
		, int set_byte_rate = -1
		, short int set_block_align = -1
		, short int set_bits_per_sample = -1
		, int set_subchunk2_ID = -1
		, int set_subchunk2_size = -1
	);
	WavHeader(const WavHeader& another_header);

	void print();
};


//----------------------------------------------------------------------------------------------------
//	WavFile class
//----------------------------------------------------------------------------------------------------

class WavFile {

	/*
	*	Class to work with wav files. Methods implements only routine
	*	which was needed for VAS correct work. More info see in declarations below
	*	or in methods implementation comments. 
	*/

private:

	WavHeader wav_header;
	char* data;


protected:

	// initialize this->data with wav file data (raw bytes)
	void init(const std::string& filename);

	// deletes data (this->data)
	void delete_file();

	// clones data (another_file.data to this->data)
	void clone_file(const WavFile& another_file);

public:

	/*
	*	Constructors
	*/

	// default constructor
	WavFile();

	// initialize data from wav file specified by filepath
	WavFile(const std::string& filepath);

	// copy constructor
	WavFile(const WavFile& another_file);

	// just for pretty code need to comment this too. THIS IS DESTRUCTOR!
	~WavFile();

	// deprecated, no need of that anymore
	WavFile& operator=(const WavFile& another_file);


	/*
	*	Main interface
	*/

	// loads data from file (no need of that if contructor with string parameter has been used)
	void load(const std::string& filename);

	// returns loaded wav file header
	WavHeader get_header();

	// count total file info size in bytes (so, basically, length of this->data)
	int get_size_in_bytes();

	// get wav file amplitudes (for now we can do that only for 16-bit)
	std::vector<short> get_amplitudes();

	// write extracted amplitudes to specified file
	void write_amplitudes(const std::string& destination_file);

};

#endif
