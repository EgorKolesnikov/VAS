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
						"  5) reparse   ('0' or '1'. Reparse all wav files ot not)\n"
						"  6) normalize ('0' or '1'. Normilize audio or not)\n"
						"  7) silence   ('0' or '1'. Exclude silence audio parts or not)";

	try{
		if(argc < 8){
			std::cout << "NN:  Invalid number of parameters. Need 7 of them.\n" << info;
			return 1;
		}

		
		/*
		*	Declare system variables. Don't touch string constants (they should be the save as in auth kernel)
		*/
		
		std::string my_voice_folder = "wav_files/wav_my_voice/";
		std::string other_voice_folder = "wav_files/wav_other_voice/";
		std::string output_files_folder = "wav_files_features/";
		std::string filepath_to_predict_features = "../data/nn_data/last_recorded_wav_features.txt";
		std::string filepath_to_store_result = "../data/nn_data/last_recorded_wav_prediction.txt";
		std::string filepath_to_store_testing_wav = "/data/recorded.wav";

		int number_of_mfcc_features = std::stoi(argv[3]);
		int number_of_fbank_features = std::stoi(argv[4]);
		bool reparse_wav_files = strcmp(argv[5], "0") == 0 ? false : true;
		bool normilize_or_not = strcmp(argv[6], "0") == 0 ? false : true;
		int sound_sample_rate = 44100;
		double split_window_length_seconds = 2.0;
		double split_window_step_seconds = 1.0;
		bool check_for_silence = strcmp(argv[7], "0") == 0 ? false : true;

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
			int(sound_sample_rate * split_window_length_seconds), int(sound_sample_rate * split_window_step_seconds),
			sound_sample_rate, number_of_mfcc_features, number_of_fbank_features,
			normilize_or_not, check_for_silence
		);

		if(strcmp(argv[1], "train") == 0){
			if(reparse_wav_files){
				ak.parse_wav_files(features_type);
			}
			ak.create_train_test();
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
