#ifndef __WAV_FILE__
#define __WAV_FILE__


#include <iostream>
#include <vector>
#include <fstream>


class WavHeader{

private:

	friend class WavFile;
	static const char HEADER_SIZE = 44;

	int chunk_ID;					// Contains 'RIFF' (0x52494646 big-endian)
	int chunk_size;					// Entire file size - 8 (exluding first two fields)
	int format;						// Contains 'WAVE' (0x57415645 big-endian)
	int subchunk1_ID;				// Contains 'fmt ' (0x666d7420 big-endian)
	int subchunk1_Size;				// 16 for PCM. Size of the rest of the subchink 'fmt'
	short int audio_format;			// Form of compression. For PCM = 1.
	short int num_channels;			// Number of channels. Mono = 1, Stereo = 2 ...
	int sample_rate;				// Number of samples per second (44100, ...)
	int byte_rate;					// sample_rate * num_channels * bits_per_sample / 8
	short int block_align;			// num_channels * bits_per_pample / 8
	short int bits_per_sample;		// Bits to decode one sample value
	int subchunk2_ID;				// Contains 'data' (0x64617461 big-endian)
	int subchunk2_size;				// Size of all data.


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




class WavFile {

private:

	WavHeader wav_header;
	char* data;


protected:

	/*
	*	Secondary functions for managing WavFile work and processing.
	*/

	int get_file_byte_size(std::fstream& file);
	void init(const std::string& filename);
	void delete_file();
	void clone_file(const WavFile& another_file);

public:

	/*
	*	Constructors (+ operator=)
	*/

	WavFile();
	WavFile(const std::string& filename);
	WavFile(const WavFile& another_file);
	~WavFile();

	WavFile& operator=(const WavFile& another_file);


	/*
	*	Main interface
	*/

	void load(const std::string& filename);
	WavHeader get_header();

	int get_size_in_bytes();
	std::vector<std::vector<char>> get_channels();
	void write_first_channel(const std::string& destination_file, bool binary = true);
	void write_first_channel_piece(const std::string& destination_file, int start_index, int end_index, bool binary = true);

	static void create_wav(std::string& filename, const WavHeader& wav_header, const std::vector<std::vector<char>>& channels);
};

#endif
