#!/bin/bash

#
#   Util variables
#

save_wav_file_path="/home/kolegor/Code/VAS/data/_last_recorded.wav"
path_to_prediction_results="/home/kolegor/Code/VAS/data/_last_recorded_wav_prediction.txt"


# storage for all parameters
declare -A parameters

# assign default parameters values (most accurate parameters for testing mode)
parameters[recompile]=0
parameters[mode]="test"
parameters[nb_mfcc]=13
parameters[nb_fbank]=26
parameters[reparse_wav]=0
parameters[norm]=0
parameters[frame_window]=2.0
parameters[frame_step]=1.0
parameters[one_vs_all]=1
parameters[main_voice_class]=1
parameters[load_config]=0
parameters[model]="NN"
parameters[features_preprocess]=0


#
#   Util functions
#

function load_config_file(){
	# Loading configuration file 'config.conf', where all parameters stored in format:
	# 'key=value'. Reading these key value entries and overwrite existing parameters
	
	IFS="="
	while read -r name value
	do
		parameters["$name"]=$value
	done < 'config.conf'

    # those parameters can not change (when testing)
    parameters[reparse_wav]=0
    parameters[recompile]=1
    parameters[mode]="test"
}

function run_program_with_parameters(){
	# Running auth system with specified parameters
	# (all parameters should be specified by now)
	system/executable \
        ${parameters[mode]} \
        ${parameters[nb_mfcc]} \
        ${parameters[nb_fbank]} \
        ${parameters[reparse_wav]} \
        ${parameters[norm]} \
        ${parameters[frame_window]} \
        ${parameters[frame_step]} \
        ${parameters[one_vs_all]} \
        ${parameters[main_voice_class]} \
        ${parameters[model]} \
        ${parameters[features_preprocess]}
}

#  Loading configuration file
echo 'SYS. Loading parameters from config file'
load_config_file


#
#   Run system
#

echo 'SYS: Compiling...'
    g++ -std=c++11 system/main_interface.cpp \
        -lboost_regex -lboost_filesystem -lboost_system -lm -pthread\
        -o system/executable
    printf "SYS: Done.\n"

# Prepare user for recording
printf "SYS: You will have 5 seconds to record your voice. Recording will start in\n"
sleep 3
for i in {3..1}; do echo "$i..." && sleep 1; done
echo "RECORDING..."
(python system/wav_record.py "$save_wav_file_path")

# Run classification
printf "\nSYS: Request to authentication system kernel to classify your voice.\n"
run_program_with_parameters

# Check the results
read -d $'\x04' name < "$path_to_prediction_results"
echo "Classification result: $name"
