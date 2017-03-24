#include "features_extractors.h"



//----------------------------------------------------------------------------------------------------
//	BaseFeaturesExtractor class
//----------------------------------------------------------------------------------------------------


BaseFeaturesExtractor::BaseFeaturesExtractor(
	const std::string& path_to_wav_file
	, int sample_rate
	, int frame_length
	, int frame_shift
	, bool normilize_audio
) 
	: path_to_wav_file_(path_to_wav_file)
	, sample_rate_(sample_rate)
	, frame_length_(frame_length)
	, frame_shift_(frame_shift)
	, normilize_audio_(normilize_audio)
{ }



/*
*	Protected methods
*/


void BaseFeaturesExtractor::run_python_script(const std::string& path_to_script, const std::vector<std::string>& parameters){
	/*
	*	Create another process and run there specified python script with specified parameters.
	*/

	try{
		std::string command = "python " + path_to_script;
		for(const std::string& parameter : parameters){
			command += " \"" + parameter + '"';
		}
		std::system(command.c_str());
	}
	catch(std::exception& e){
		std::cout << "BaseFeaturesExtractor::run_python_script(...). Exception while running python script '" + path_to_script + "'.\n";
		std::cout << e.what() << '\n';
	}
}


void BaseFeaturesExtractor::save_python_script_result(const std::string& path_to_script_result){
	/*
	*	Load result of python script work.
	*	Each line of file contains set of features for one frame of wav file.
	*	Save loaded features
	*/

	try{
		std::ifstream inf(path_to_script_result);
		std::string line;
		
		// parse each line separately
		while (std::getline(inf, line)){
		    std::istringstream iss(line);
		    std::vector<double> current_frame_features;

		    // read features values in current file line
		    double feature;
		    while (iss >> feature) {
		    	current_frame_features.push_back(feature); 
		    }

		    // saved parsed line
		    this->frames_features_.push_back(current_frame_features);
		}
		inf.close();
	}
	catch(std::exception& e){
		std::cout << "void BaseFeaturesExtractor::load_python_script_result(...). Exception while loading python script work.\n";
		std::cout << e.what() << '\n';
	}
}


const std::vector<std::vector<double>>& BaseFeaturesExtractor::get_frames_features(){
	return this->frames_features_;
}



//----------------------------------------------------------------------------------------------------
//	MFCCFeatures(BaseFeaturesExtractor) class
//----------------------------------------------------------------------------------------------------


MFCCFeaturesExtractor::MFCCFeaturesExtractor(
	const std::string& path_to_wav_file
	, int sample_rate
	, int frame_length
	, int frame_shift
	, int number_of_mfcc_features
	, bool normilize_audio
) :
	BaseFeaturesExtractor(
		path_to_wav_file
		, sample_rate
		, frame_length
		, frame_shift
		, normilize_audio
	)
	, number_of_mfcc_features_(number_of_mfcc_features)
{ }


void MFCCFeaturesExtractor::extract(){
	/*
	*	Computing MFCC features here. Running python script to do that.
	*	More info about script parameters see in script
	*/

	try{
		// running python script to compute MFCC features
		this->run_python_script(
			SETTINGS::PYTHON_MFCC_FEATURES_SCRIPT_PATH, {
				this->path_to_wav_file_
				, SETTINGS::TEMP_MFCC_FEATURES_OUTPUT_PATH
				, std::to_string(this->sample_rate_)
				, std::to_string(this->frame_length_)
				, std::to_string(this->frame_shift_)
				, std::to_string(this->number_of_mfcc_features_)
				, std::to_string(this->normilize_audio_)
			}
		);

		// load computed features
		this->save_python_script_result(SETTINGS::TEMP_MFCC_FEATURES_OUTPUT_PATH);
	}
	catch(std::exception& e){
		std::cout << "MFCCFeaturesExtractor::extract_frame_features(...). Exception while extracting MFCC features.\n";
		std::cout << e.what() << '\n';
	}
}



//----------------------------------------------------------------------------------------------------
//	FbankFeaturesExtractor(BaseFeaturesExtractor) class
//----------------------------------------------------------------------------------------------------


FbankFeaturesExtractor::FbankFeaturesExtractor(
	const std::string& path_to_wav_file
	, int sample_rate
	, int frame_length
	, int frame_shift
	, int number_of_fbank_features
	, bool normilize_audio
) :
	BaseFeaturesExtractor(
		path_to_wav_file
		, sample_rate
		, frame_length
		, frame_shift
		, normilize_audio
	)
	, number_of_fbank_features_(number_of_fbank_features)
{ }


void FbankFeaturesExtractor::extract(){
	/*
	*	Computing FBANK features here. Running python script to do that.
	*/

	try{
		// running python script to compute MFCC features
		this->run_python_script(
			SETTINGS::PYTHON_FBANK_FEATURES_SCRIPT_PATH, {
				this->path_to_wav_file_
				, SETTINGS::TEMP_FBANK_FEATURES_OUTPUT_PATH
				, std::to_string(this->sample_rate_)
				, std::to_string(this->frame_length_)
				, std::to_string(this->frame_shift_)
				, std::to_string(this->number_of_fbank_features_)
				, std::to_string(this->normilize_audio_)
			}
		);

		// load computed features
		this->save_python_script_result(SETTINGS::TEMP_FBANK_FEATURES_OUTPUT_PATH);
	}
	catch(std::exception& e){
		std::cout << "MFCCFeaturesExtractor::extract_frame_features(...). Exception while extracting MFCC features.\n";
		std::cout << e.what() << '\n';
	}
}



//----------------------------------------------------------------------------------------------------
//	GlobalWavFeaturesExtractor(BaseFeatureExtractor) class
//----------------------------------------------------------------------------------------------------


GlobalWavFeaturesExtractor::GlobalWavFeaturesExtractor(
	const std::string& path_to_wav_file
	, int sample_rate
	, int frame_length
	, int frame_shift
	, int number_of_global_wav_features
	, bool normilize_wav
) :
	BaseFeaturesExtractor(
		path_to_wav_file
		, sample_rate
		, frame_length
		, frame_shift
		, normilize_wav
	)
	, number_of_global_wav_features_(number_of_global_wav_features)
{ }


void GlobalWavFeaturesExtractor::extract(){
	/*
	*	Computing FBANK features here. Running python script to do that.
	*/

	try{
		// running python script to compute MFCC features
		this->run_python_script(
			SETTINGS::PYTHON_GLOBAL_WAV_FEATURES_SCRIPT_PATH, {
				this->path_to_wav_file_
				, SETTINGS::TEMP_GLOBAL_WAV_FEATURES_OUTPUT_PATH
				, std::to_string(this->sample_rate_)
				, std::to_string(this->frame_length_)
				, std::to_string(this->frame_shift_)
				, std::to_string(this->number_of_global_wav_features_)
				, std::to_string(this->normilize_audio_)
			}
		);

		// load computed features
		this->save_python_script_result(SETTINGS::TEMP_GLOBAL_WAV_FEATURES_OUTPUT_PATH);
	}
	catch(std::exception& e){
		std::cout << "MFCCFeaturesExtractor::extract_frame_features(...). Exception while extracting MFCC features.\n";
		std::cout << e.what() << '\n';
	}
}



//----------------------------------------------------------------------------------------------------
//	FeaturesCombiner class
//----------------------------------------------------------------------------------------------------


FeaturesCombiner::FeaturesCombiner(
	const std::string& path_to_wav_file
	, const std::string& path_to_store_results
) 
	: path_to_wav_file_(path_to_wav_file)
	, path_to_store_results_(path_to_store_results)
{ }


template <class FeatureExtractor, class ...Args>
void FeaturesCombiner::add(Args&&... args){
	all_feature_extractors_.emplace_back(new FeatureExtractor(std::forward<Args>(args)...));
}


void FeaturesCombiner::combine(){
	/*
	*	Combinind all feature extractors results in one output file 
	*	(path to output file: this->path_to_store_results).
	*/

	try{
		// run each extractor
		// TODO : run in separate threads/processes
		for(auto& feature_extractor : this->all_feature_extractors_){
			feature_extractor->extract();
		}

		// load all extracted features in one vector
		std::vector<std::vector<std::vector<double>>> all_features;
		all_features.reserve(this->all_feature_extractors_.size());
		for(auto& feature_extractor : this->all_feature_extractors_){
			all_features.emplace_back(feature_extractor->get_frames_features());
		}

		// if there no results - something wrong
		if(all_features.empty()){
			std::cout << " * Found no features for current wav file (feature extractors return nothing).\n";
			throw std::exception();
		}

		// check if each feature extractor extracted features for equal number of frames
		int number_of_frames = all_features[0].size();
		for(size_t extractor = 1; extractor < all_feature_extractors_.size(); ++extractor){
			if(all_features[extractor].size() != number_of_frames){
				std::cout << " * Feature extractors generated feautres for different number of frames.\n";
				throw std::exception();
			}
		}

		// write features to output file
		std::ofstream outf(this->path_to_store_results_);
		for(size_t frame_index = 0; frame_index < number_of_frames; ++frame_index){
			for(auto& feature_extractor_results : all_features){
				for(double frame_feature : feature_extractor_results[frame_index]){
					outf << frame_feature << " ";
				}
			}
			outf << "\n";
		}
		outf.close();
	}
	catch(std::exception& e){
		std::cout << "FeaturesCombiner::combine(). Exception combining features results.\n";
		std::cout << e.what() << '\n';
	}
}
