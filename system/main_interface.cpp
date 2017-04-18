#include "source/include.h"


//
//	For specific bash script usage only
//

int main(int argc, char * argv[]){
	std::string info = 	"Parameters:\n"
						"  1)  mode      			('train' or 'test' or 'none')\n"
						"  2)  nb_mfcc   			(int, number of mfcc features)\n"
						"  3)  nb_fbank  			(int, number of fbank features)\n"
						"  4)  reparse   			('0' or '1'. Reparse all wav files ot not)\n"
						"  5)  normalize 			('0' or '1'. Normilize audio or not)\n"
						"  6)  frame_window			(length of frame window for wav files parse)\n"
						"  7)  frame_step			(length of frame step for wav files parse)\n"
						"  8)  one-vs-all			('0' or '1'. Train model in one-vs-all mode or not)\n"
						"  9)  main_voice_class		(number of main class (voice id) in one-vs-all train mode)\n"
						" 10)  model 				(available model name: ['NN', 'RF'])\n"
						" 11)  features_preprocess  (features preprocess algorithm. See more in python script)\n";

	try{
		if(argc != 12){
			std::cout << "NN:  Invalid number of parameters. Need 11 of them.\n" << info;
			return 1;
		}


		/*
		*	Parse parameters
		*/

		bool train_mode = (strcmp(argv[1], "train") == 0);
		bool test_mode = (strcmp(argv[1], "test") == 0);
		int number_of_mfcc_features = std::stoi(argv[2]);
		int number_of_fbank_features = std::stoi(argv[3]);
		bool reparse_wav_files = strcmp(argv[4], "0") == 0 ? false : true;
		bool normilize_or_not = strcmp(argv[5], "0") == 0 ? false : true;
		double split_window_length_seconds = std::stof(argv[6]);
		double split_window_step_seconds = std::stof(argv[7]);
		bool one_vs_all = strcmp(argv[8], "0") == 0 ? false : true;
		int main_voice_class = std::stoi(argv[9]);
		std::string model_name = std::string(argv[10]);
		FEATURES_PREPROCESS features_preprocess = static_cast<FEATURES_PREPROCESS>(std::stoi(argv[11]));

		
		/*
		*	Run authentication system
		*/

		// initializing kernel
		AuthenticationKernel ak(			
			int(SETTINGS::SAMPLE_RATE * split_window_length_seconds)
			, int(SETTINGS::SAMPLE_RATE * split_window_step_seconds)
			, number_of_mfcc_features
			, number_of_fbank_features
			, normilize_or_not
			, model_name
			, one_vs_all
			, main_voice_class
			, features_preprocess
			, main_voice_class
		);

		// init current model directory
		std::string model_folder_path = SETTINGS::TRAINED_MODELS_DUMPS_FOLDER + generate_model_folder_name(
			one_vs_all, main_voice_class, static_cast<int>(features_preprocess)
		) + "/";
		boost::filesystem::create_directory(model_folder_path);

		// reparse if we want to
		if(!test_mode && reparse_wav_files){
			clear_folder(SETTINGS::TRAIN_FILES_FEATURES_FOLDER);
			ak.extract_features(SETTINGS::TRAIN_WAV_FILES_FOLDER, SETTINGS::TRAIN_FILES_FEATURES_FOLDER);
			
			clear_folder(SETTINGS::TEST_FILES_FEATURES_FOLDER);
			ak.extract_features(SETTINGS::TEST_WAV_FILES_FOLDER, SETTINGS::TEST_FILES_FEATURES_FOLDER);
		}

		if(train_mode){
			ak.create_train_test(model_folder_path);			
			ak.fit(model_folder_path);
		}
		else if(test_mode){
			ak.predict(model_folder_path);
		}
	}
	catch(std::exception& e){
		std::cout << "Error. Just error. Deal with it. \n" << e.what() << "\n";
	}

	std::cout << std::endl;
	return 0;
}
