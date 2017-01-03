#include "source/include_all.h"

//
//	For specific bash script usage only
//
int main(int argc, char * argv[]){
	std::string info = 	"Parameters:\n"
						"  1) mode      ('train' or 'test')\n"
						"  2) features  ('mfcc', 'fbank', 'both')\n"
						"  3) nb_mfcc   (int, number of mfcc features)\n"
						"  4) nb_fbank  (int, number of fbank features)\n"
						"  5) reparse   ('0' or '1'. Reparse all wav files ot not.)\n";

	try{
		if(argc < 6){
			std::cout << "NN:  Invalid number of parameters. Need 5 of them.\n" << info;
			return 1;
		}

		
		/*
		*	Declare system variables.
		*/
		
		std::string my_voice_folder = "wav_my_voice/";
		std::string other_voice_folder = "wav_other_voice/";
		std::string output_files_folder = "files_features/";
		std::string filepath_to_predict_features = "../data/nn_data/test_record.txt";
		std::string filepath_to_store_result = "../data/nn_data/last_prediction.txt";
		std::string filepath_to_store_testing_wav = "../data/recorded.wav";

		int number_of_mfcc_features = std::stoi(argv[3]);
		int number_of_fbank_features = std::stoi(argv[4]);
		bool reparse_wav_files = strcmp(argv[5], "0") ? false : true;
		int sound_sample_rate = 44100;
		int sound_seconds_length = 2;
		
		FEATURES features_type;
		if(strcmp(argv[2], "mfcc") == 0){
			features_type = FEATURES::MFCC;
		}
		else if(strcmp(argv[2], "fbank") == 0){
			features_type = FEATURES::FBANK;
		}
		else{
			features_type = FEATURES::MFCC_FBANK;
		}

		
		/*
		*	Run authentication system
		*/

		AuthenticationKernel ak(
			my_voice_folder, other_voice_folder, output_files_folder,
			sound_sample_rate, sound_seconds_length, 
			number_of_mfcc_features, number_of_fbank_features
		);

		if(strcmp(argv[1], "train") == 0){
			if(reparse_wav_files == 0){
				ak.parse_wav_files(features_type);
				ak.create_train_test();
			}
			ak.fit();
		}
		else{
			ak.predict_wav(filepath_to_store_testing_wav, features_type);
		}
	}
	catch(std::exception& e){
		std::cout << "Error.\n";
	}

	std::cout << std::endl;
	return 0;
}
