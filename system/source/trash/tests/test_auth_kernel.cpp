#include "../include_all.h"


int main() {
	std::string my_voice_folder = "wav_my_voice/";
	std::string other_voice_folder = "wav_other_voice/";
	std::string output_files_folder = "files_features/";
	std::string filepath_to_predict = "/home/kolegor/Code/Wav/data/nn_data/test_record.txt";
	std::string filepath_to_store_result = "/home/kolegor/Code/Wav/data/nn_data/last_prediction.txt";

	int sound_sample_rate = 44100;
	int sound_seconds_length = 2;
	int number_of_mfcc_features = 13;
	

	AuthenticationKernel ak(
		my_voice_folder, other_voice_folder, output_files_folder,
		sound_sample_rate, sound_seconds_length, number_of_mfcc_features
	);

	ak.parse_wav_files();
	ak.create_train_test();
	ak.fit();
	ak.predict(filepath_to_predict, filepath_to_store_result);

	std::cout << "\n";
	return 0;
}