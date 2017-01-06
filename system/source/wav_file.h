#ifndef __WAV_FILE__
#define __WAV_FILE__

#include <iostream>
#include <vector>
#include <fstream>


//----------------------------------------------------------------------------------------------------
//	WavHeader class
//----------------------------------------------------------------------------------------------------

class WavHeader{

	/*
	*	Class contains information about wav file header.
	*	You are able only to watch that info.
	*/

private:

	friend class WavFile;
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
		int set_chunk_ID = -1, int set_chunk_size = -1, int set_format = -1, int set_subchunk1_ID = -1,
		int set_subchunk1_Size = -1, short int set_audio_format = -1, short int set_num_channels = -1,
		int set_sample_rate = -1, int set_byte_rate = -1, short int set_block_align = -1,
		short int set_bits_per_sample = -1, int set_subchunk2_ID = -1, int set_subchunk2_size = -1
	);
	WavHeader(const WavHeader& another_header);

	void print();
};


//----------------------------------------------------------------------------------------------------
//	WavFile class
//----------------------------------------------------------------------------------------------------

class WavFile {

	/*
	*	Helper class to work with wav files. Methods implements only routine
	*	which was needed for VAS correct work. More info see in declarations below
	*	or in methods implementation comments. 
	*/

private:

	WavHeader wav_header;
	char* data;


protected:

	/*
	*	Secondary functions for managing WavFile work and processing.
	*/

	// calculate file size in bytes
	int get_file_byte_size(std::fstream& file);

	// initialize this->data by saving wav file dump in this->data (bytes)
	void init(const std::string& filename);

	// deletes data (this->data)
	void delete_file();

	// clones data (another_file.data to this->data)
	void clone_file(const WavFile& another_file);

public:

	/*
	*	Constructors
	*/

	// default constructor (NOTE:no info in this->data)
	WavFile();

	// initialize data from wav file specified by filename
	WavFile(const std::string& filename);

	// copy constructor (no need of that in VAS)
	WavFile(const WavFile& another_file);

	// just for pretty code need to comment this too. THIS IS DESTRUCTOR!
	~WavFile();

	// deprecated, no need of that anymore
	WavFile& operator=(const WavFile& another_file);


	/*
	*	Main interface
	*/

	// loads data from file (no need of that if contructor with string parameter have been used)
	void load(const std::string& filename);

	// returns loaded wav file header (remember - only for watch)
	WavHeader get_header();

	// count total file info size in bytes (so, basically, length of this->data)
	int get_size_in_bytes();

	// if we want to work with many channels (in VAS wav files only with one channel)
	std::vector<std::vector<char>> get_channels();

	// saving first channel (need only first one in VAS) in file (binary or not - your choice)
	void write_first_channel(const std::string& destination_file, bool binary = true);

	// saving specified by start and end indices subarray of wav file data (when we splitting large wav files)
	void write_first_channel_piece(const std::string& destination_file, int start_index, int end_index, bool binary = true);

	// deprecated (used for testing WavHeader and WavFile classes)
	static void create_wav(std::string& filename, const WavHeader& wav_header, const std::vector<std::vector<char>>& channels);
};

#endif
