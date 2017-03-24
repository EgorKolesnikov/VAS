#include "source/include.h"

//
//	For specific bash script usage only
//


std::string generate_model_folder_path(bool one_vs_all, int main_voice_class){
	/*
	*	Generating model folder path in following format:
	*	$1[__$2], where:
	*	 - $1: type of model training. One of ['one_vs_all', 'multiclass']
	*	 - $2: optional (if $1 == 'one_vs_all'). Contains class unique id (number >= 0)
	*/

	if(one_vs_all){
		return "one_vs_all__main_voice__" + std::to_string(main_voice_class);
	}
	else{
		return "multiclass";
	}
}


void clear_folders_for_features(){
	if(boost::filesystem::exists(SETTINGS::WAV_FILES_FEATURES_FOLDER)){
		boost::filesystem::remove_all(SETTINGS::WAV_FILES_FEATURES_FOLDER);
	}
	boost::filesystem::create_directory(SETTINGS::WAV_FILES_FEATURES_FOLDER);
} 


int main(int argc, char * argv[]){
	std::string info = 	"Parameters:\n"
						"  1)  mode      		('train' or 'test' or 'none')\n"
						"  2)  nb_mfcc   		(int, number of mfcc features)\n"
						"  3)  nb_fbank  		(int, number of fbank features)\n"
						"  4)  nb_global		(int, number of most strongest wav file frequencies for secondary classifier)"
						"  5)  reparse   		('0' or '1'. Reparse all wav files ot not)\n"
						"  6)  normalize 		('0' or '1'. Normilize audio or not)\n"
						"  7)  frame_window		(length of frame window for wav files parse)\n"
						"  8)  frame_step		(length of frame step for wav files parse)\n"
						"  9)  one-vs-all		('0' or '1'. Train model in one-vs-all mode or not)\n"
						" 10)  main_voice_class	(number of main class (voice id) in one-vs-all train mode)\n"
						" 11)  model 			(available model name: ['NN', 'RF'])\n"
						" 12)  two_step			(use secondary trained model after first model work)\n";

	try{
		if(argc != 13){
			std::cout << "NN:  Invalid number of parameters. Need 12 of them.\n" << info;
			return 1;
		}
		
		int sound_sample_rate = 44100;


		/*
		*	Parse parameters
		*/

		bool train_mode = (strcmp(argv[1], "train") == 0);
		bool test_mode = (strcmp(argv[1], "test") == 0);
		int number_of_mfcc_features = std::stoi(argv[2]);
		int number_of_fbank_features = std::stoi(argv[3]);
		int number_of_global_wav_features = std::stoi(argv[4]); 
		bool reparse_wav_files = strcmp(argv[5], "0") == 0 ? false : true;
		bool normilize_or_not = strcmp(argv[6], "0") == 0 ? false : true;
		double split_window_length_seconds = std::stof(argv[7]);
		double split_window_step_seconds = std::stof(argv[8]);
		bool one_vs_all = strcmp(argv[9], "0") == 0 ? false : true;
		int main_voice_class = std::stoi(argv[10]);
		std::string model_name = std::string(argv[11]);
		bool two_step_classification = strcmp(argv[12], "0") == 0 ? false : true;

		
		/*
		*	Run authentication system
		*/

		// initializing kernel
		AuthenticationKernel ak(			
			int(sound_sample_rate * split_window_length_seconds)
			, int(sound_sample_rate * split_window_step_seconds)
			, sound_sample_rate
			, number_of_mfcc_features
			, number_of_fbank_features
			, number_of_global_wav_features
			, normilize_or_not
			, model_name
			, two_step_classification
		);

		// init current model directory
		std::string model_folder_path = SETTINGS::TRAINED_MODELS_DUMPS_FOLDER + generate_model_folder_path(one_vs_all, main_voice_class);
		boost::filesystem::create_directory(model_folder_path);

		// reparse if we want to
		if(train_mode && reparse_wav_files){
			clear_folders_for_features();
			ak.extract_features();
		}

		// train or test or none
		if(train_mode){
			// create train and test samples
			ak.create_train_test(
				model_folder_path
				, SETTINGS::TRAIN_OUTPUT_NAME
				, SETTINGS::TEST_OUTPUT_NAME
				, "voice_"
				, one_vs_all
				, main_voice_class
			);
			
			if(two_step_classification){
				ak.create_train_test(
					model_folder_path
					, SETTINGS::SECONDARY_MODEL_TRAIN_OUTPUT_NAME
					, SETTINGS::SECONDARY_MODEL_TEST_OUTPUT_NAME
					, "global_"
					, one_vs_all
					, main_voice_class
				);
			}
			
			ak.fit(model_folder_path);
		}
		else if(test_mode){
			ak.predict(model_folder_path);
		}
	}
	catch(std::exception& e){
		std::cout << "Error. Just error. Deal with it. \n";
	}

	std::cout << std::endl;
	return 0;
}
