#include "../include_all.h"

int main(){
	std::string my_voice_folder = "wav_files/wav_my_voice/";
	std::string other_voice_folder = "wav_files/wav_other_voice/";
	std::string output_files_folder = "files_features/";
	std::string filepath_to_predict_features = "../data/nn_data/test_record.txt";
	std::string filepath_to_store_result = "../data/nn_data/last_prediction.txt";
	std::string filepath_to_store_testing_wav = "../data/recorded.wav";

	int number_of_mfcc_features = 13;
	int number_of_fbank_features = 26;
	int sound_sample_rate = 44100;
	int sound_seconds_length = 2;
	bool normilize = false;

	AuthenticationKernel ak(
		my_voice_folder, other_voice_folder, output_files_folder,
		sound_sample_rate, sound_seconds_length, 
		number_of_mfcc_features, number_of_fbank_features, normilize
	);

	// ak.parse_wav_files(FEATURES::MFCC, 88200);
	// ak.create_train_test(0.8);

	// ak.fit();

	ak.predict_wav(filepath_to_store_testing_wav, FEATURES::MFCC);

	std::cout << "\nDone" << std::endl;
	return 0;
}