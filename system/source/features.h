#pragma once

#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "../settings.h"
#include "util.cpp"


class PoolFeaturesExtractor{

	/*
	*	Thread pool to extract features from wav files.
	*	
	*	Features extraction procedure done via python scripts. This class manages
	*	only multithread work for feature extraction.
	*	
	*	High-level idea: store all files paths that should be parsed in one queue
	*	and run N threads to pick files from that queue. Those threads are running
	*	python scripts, not the main thread.
	*/

private:

	std::queue<std::string> all_files_paths_;		// accumulate all files that need to be parsed here
	std::vector<std::string> script_parameters_;	// parameters for python script (see more in: py_features.py)
	
	// threads utils
	int nb_workers_;								// number of threads (std::thread::hardware_concurrency)
	std::vector<std::thread> workers_;				// saving all threads here
	std::mutex m_files_storage_lock_;				// one global lock for files paths queue

	// python script (for extracting features) wrapper 
	void run_python_feature_extractor(const std::vector<std::string>& parameters);

	// one thread routine (watching for queue and running python scripts)
	void thread_worker();


public:

	PoolFeaturesExtractor(int nb_workers = std::thread::hardware_concurrency());

	int get_nb_workers();

	// add new file to parse (in queue)
	void add_file(const std::string& path_to_file);

	// extract features from all files that are in queue
	void extract(const std::vector<std::string>& script_parameters);
};
