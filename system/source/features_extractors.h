#ifndef __FEATURES_EXTRACTORS__
#define __FEATURES_EXTRACTORS__

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "../settings.h"
#include "wav_file.h"


/*
*	This header file contains interface for following classes:
*	
*	 - BaseFeaturesExtractor	(abstract class for all feature extractors)
*	 - MFCCFeaturesExtractor	(mfcc features extractor)
*	 - FbankFeaturesExtractor	(fbank features extractor)
*	 - FeaturesCombiner			(class to combine all extractors results)
*/



//----------------------------------------------------------------------------------------------------
//	BaseFeaturesExtractor class
//----------------------------------------------------------------------------------------------------


class BaseFeaturesExtractor{

	/*
	*	Base class for features extractors. Main interface - only extract(...).
	*	All feature extractors were meant to be just wrappers for python scripts work
	*	(foolish, maybe, but main features such as mfcc and fbank are easy extracted in python)
	*
	*	All features are extracted for wav file frames. Frame length and shift should be set via
	*	constructor. Result of all work - file, where each line contains features for one frame from 
	*	input wav file.
	*/

protected:
	std::string path_to_wav_file_;								// path to current wav file (absolute path)
	int sample_rate_;											// sample rate of input audio
	int frame_length_;											// length of audio file in samples to extract features for
	int frame_shift_;											// move each frame window in that specified number of samples

	std::vector<std::vector<double>> frames_features_;			// storage to save features for each frame


	// run python script with specified parameters
	void run_python_script(const std::string& path_to_script, const std::vector<std::string>& parameters);

	// load script work results
	void save_python_script_result(const std::string& path_to_script_result);

public:
	
	BaseFeaturesExtractor(
		const std::string& path_to_wav_file
		, int sample_rate
		, int frame_length
		, int frame_shift
	);

	// main function for feature extraction
	virtual void extract() = 0;

	// get result (using in FeatureCombiner)
	const std::vector<std::vector<double>>& get_frames_features();
};



//----------------------------------------------------------------------------------------------------
//	MFCCFeaturesExtractor(BaseFeatureExtractor) class
//----------------------------------------------------------------------------------------------------


class MFCCFeaturesExtractor : public BaseFeaturesExtractor{

	/*
	*	MFCC features extractor. Using Python script to do that.
	*/

private:

	int number_of_mfcc_features_;

public:

	MFCCFeaturesExtractor(
		const std::string& path_to_wav_file
		, int sample_rate
		, int frame_length
		, int frame_shift
		, int number_of_mfcc_features
	);

	// main method to extract MFCC features (for now using python script)
	void extract();
};



//----------------------------------------------------------------------------------------------------
//	FbankFeaturesExtractor(BaseFeatureExtractor) class
//----------------------------------------------------------------------------------------------------


class FbankFeaturesExtractor : public BaseFeaturesExtractor{

	/*
	*	Fbank features extractor. Using Python script to do that.
	*/

private:

	int number_of_fbank_features_;

public:

	FbankFeaturesExtractor(
		const std::string& path_to_wav_file
		, int sample_rate
		, int frame_length
		, int frame_shift
		, int number_of_fbank_features
	);

	// main method to extract Fbank features (for now using python script)
	void extract();
};



//----------------------------------------------------------------------------------------------------
//	FeaturesCombiner class
//----------------------------------------------------------------------------------------------------


class FeaturesCombiner{

	/*
	*	Wrapper for all feature extractors. Event when using only one FeatureExtractor class
	*	we need that class to corretly store result. 
	*
	*	First - adding feature extractors to be later runned. Then - run each extractor.
	*	At last - collect data from all of them and combine them in one output file. 
	*/

private:

	std::string path_to_wav_file_;													// path to current wav file
	std::string path_to_store_results_;												// path to store merged results
	std::vector<std::unique_ptr<BaseFeaturesExtractor>> all_feature_extractors_;	// save all feature extractor to run their routine later


public:

	FeaturesCombiner(const std::string& path_to_wav_file, const std::string& path_to_store_results);

	// add feature extractor to be merged later with all other extractors
	template <class FeatureExtractor, class ...Args>
	void add(Args&&... args);

	// combine all features extractors results. Note: run feature extracting here
	void combine();
};


#endif
