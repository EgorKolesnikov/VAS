#include "features.h"


PoolFeaturesExtractor::PoolFeaturesExtractor(int nb_workers)
	: nb_workers_(nb_workers)
{ }

/*
*	Thread workers utils
*/

void PoolFeaturesExtractor::run_python_feature_extractor(const std::vector<std::string>& parameters){
	/*
	*	Wrapper to run python script to extract features. Running with given parameters.
	*	No checks are done for parameters count and order. You need to worry about that.
	*/

	run_python_script(SETTINGS::PYTHON_FEATURES_SCRIPT_PATH, parameters);
}

void PoolFeaturesExtractor::thread_worker(){
	/*
	*	One thread routine. Watching for files queue. While it is not empty - running
	*	script to extract features. Using one global mutex for all threads (for queue).
	*/

	std::unique_lock<std::mutex> lock(this->m_files_storage_lock_);

	while(!this->all_files_paths_.empty()){		
		// get next file to extract features from
		std::string filepath_to_proceed = this->all_files_paths_.front();
		this->all_files_paths_.pop();

		// create parameters for python script. More info about parameters format see in script
		std::vector<std::string> parameters;

		// first two parameters - path to load from and save to
		parameters.emplace_back(filepath_to_proceed);
		parameters.emplace_back(generate_features_output_filepath(filepath_to_proceed));

		// other script parameters (see more in script)
		for(std::string& parameter : this->script_parameters_){
			parameters.emplace_back(parameter);
		}

		std::cout << "THREAD ID: " << std::this_thread::get_id() << ". Files left: " << this->all_files_paths_.size() << ". Proceeding file " << filepath_to_proceed << std::endl;

		// run current work without lock
		lock.unlock();
		this->run_python_feature_extractor(parameters);
		lock.lock();
	}
}


/*
*	Main intefrace
*/

int PoolFeaturesExtractor::get_nb_workers(){
	return this->nb_workers_;
}

void PoolFeaturesExtractor::add_file(const std::string& path_to_file){
	this->all_files_paths_.push(path_to_file);
}

void PoolFeaturesExtractor::extract(const std::vector<std::string>& parameters){
	// save parameters so all threads can read them
	this->script_parameters_ = parameters;

	// run all threads
	this->workers_.reserve(this->nb_workers_);

	for(int i = 0; i < this->nb_workers_; ++i){
		this->workers_.emplace_back(&PoolFeaturesExtractor::thread_worker, this);
	}

	// wait for threads to finish
	for(auto& worker : this->workers_){
		worker.join();
	}
}
