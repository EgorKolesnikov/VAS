#!/bin/bash

#################################################################################################################

usage_help="
    -h, --help                          : Print manual  
   --two-step		[Default: false]	: Use secondary trained model to classify voice if first model
   					                      was not sure about classification	
"

#################################################################################################################


#
#   Util variables
#

# storage for all parameters
declare -A parameters

# assign default parameters values (most accurate parameters for testing mode)
parameters[recompile]=0
parameters[mode]="test"
parameters[nb_mfcc]=13
parameters[nb_fbank]=26
parameters[nb_global]=20
parameters[reparse_wav]=0
parameters[norm]=0
parameters[frame_window]=2.0
parameters[frame_step]=1.0
parameters[one_vs_all]=1
parameters[main_voice_class]=1
parameters[load_config]=0
parameters[model]="NN"
parameters[two_step]=0


#
#   Util variables
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
    parameters[recompile]=0
    parameters[mode]="test"
}

function run_program_with_parameters(){
	# Running auth system with specified parameters
	# (all parameters should be specified by now)
	system/executable \
        ${parameters[mode]} \
        ${parameters[nb_mfcc]} \
        ${parameters[nb_fbank]} \
        ${parameters[nb_global]} \
        ${parameters[reparse_wav]} \
        ${parameters[norm]} \
        ${parameters[frame_window]} \
        ${parameters[frame_step]} \
        ${parameters[one_vs_all]} \
        ${parameters[main_voice_class]} \
        ${parameters[model]} \
        ${parameters[two_step]}
}


#
#   Parse arguments
#

while :; do 
    case $1 in
        -h|-\?|--help)
            echo "$usage_help"
            exit
            ;;
        --two-step)
            parameters[two_step]=1
            ;;
        -?*)
            printf "ERROR: Unknown option: $1\n"
            exit
            ;;
        *)
            break
    esac
    shift
done

#  Loading configuration file if we want to
echo 'SYS. Loading parameters from config file'
load_config_file


#
#   Run system
#

# Prepare user for recording
printf "SYS: You will have 5 seconds to record your voice. Recording will start in\n"
sleep 3
for i in {3..1}; do echo "$i..." && sleep 1; done
echo "RECORDING..."
(python system/wav_record.py "data/recorded.wav")

# Run nn classification
printf "\nSYS: Request to authentication system kernel to classify your voice.\n"
run_program_with_parameters

# Check the results
read -d $'\x04' name < "$classification_result_file"
if [[ $name == "0" ]]; then
    echo 'DANGER! DANGER!'
else
    echo "Welcome, master Egor!"
fi
