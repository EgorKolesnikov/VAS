#!/bin/bash

#################################################################################################################

usage_help="
    -r, --recompile             [Default: false]    : if we want to recompile all source or not.
    -m, --mode=...              [Default: none]     : 'train' or 'test' (or 'none')
    --nb-mfcc=...               [Default: 13]       : number of mfcc coefficients (int).
    --nb-fbank=...              [Default: 26]       : number of filterbanks (int).
    --reparse-wav               [Default: false]    : if we want to reparse all wav files (for train).
    -n, --norm                  [Default: false]    : normilize audio file or not.
    --frame-window=...          [Default: 2.0]      : frame window in seconds to create train and test.
    --frame-step=...            [Default: 1.0]      : frame step in seconds to create train and test.
    --one-vs-all                [Default: false]    : whether or not we want to train model in one-vs-all mode. No impact if mode=test
    --main-voice-class=...      [Default: -1]       : main class (one of voice unique ids) in one-vs-all train mode. No impact if --one-vs-all was not specified
    -lc, --load-config          [Default: true]     : load config files to initialize all variables. Loading after all other parameters parsed. Be sure not to rewrite parameters.
    --model=...                 [Default: NN]       : Choose model to train. Available:
                                                        - NN = NeuralNetworkModel
                                                        - RF = RandomForestModel
    --features-preprocess=...   [Default: 0]        : Features preprocess algorithm. Available:
                                                        - 0 = No preprocess
                                                        - 1 = Normalization (mean and std)
"

#################################################################################################################


#
#   Util functions
#

function check_and_change() {
    # Check if assigned parameter value is in possible values.
    # $1 - parameter name; $2 - parameter value; $3, $4, ... - possible values 

    valid=0
    for ((i=3;i<=$#;i++)); do
        if [[ "${!i}" == $2 ]]; then
            valid=1
            break
        fi
    done;

    if [[ $valid == 0 ]]; then
        echo "ERROR: Invalid $1 parameter option ($2)."
        exit
    fi

    parameters["$1"]="$2"
}

function check_number_parameter(){
    # Check if parameter has number (int or float) format. If it is - change parameter value
    # $1 - parameter name, $2 - parameter value

    if [ -z $2 ]; then
        printf "ERROR: $1 requires a non-empty integer option argument.\n"
        exit
    fi

    if [[ "$2" =~ "^[0-9]+([.][0-9]+)?$" ]]; then
        printf "ERROR: value for $1 parameter ($2) is not an integer value.\n"
        exit
    fi
    
    parameters[$1]="$2"
}

function load_config_file(){
    # Loading configuration file 'config.conf', where all parameters stored in format:
    # 'key=value'. Reading these key value entries and overwrite existing parameters
    
    IFS="="
    while read -r name value
    do
        parameters["$name"]=$value
    done < 'config.conf'
}

function write_config_file(){
    # Write config file with correct (current) parameters
    # for user usage (when  user want to use system - he do not need to specify
    # running parameters, so we write them back in file, from where we read them)

    # clear file
    : > config.conf

    # write all config parameters
    for i in "${!parameters[@]}"
    do
        echo $i=${parameters[$i]} >> config.conf
    done
}


function run_program_with_parameters(){
    # Running auth system with specified parameters
    # (all parameters should be specified by now)

    #debug output
    echo
    echo 'Running system with parameters:'
    for i in "${!parameters[@]}"
    do
        echo " - $i = ${parameters[$i]}"
    done
    echo

    # run system
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
parameters[reparse_wav]=0
parameters[norm]=0
parameters[frame_window]=2.0
parameters[frame_step]=1.0
parameters[one_vs_all]=0
parameters[main_voice_class]=1
parameters[load_config]=0
parameters[model]="NN"
parameters[features_preprocess]=0

# declare some paths to be able to run scripts and etc 
# (NOTE: need to sync with settings.h)
path_to_script="/home/kolegor/Code/VAS/"
main_interface_folder=$path_to_script"system/executable"
source_folder=$main_interface"source/"
output_executable_name="executable"
classification_result_file=$path_to_script"data/_last_recorded_wav_prediction.txt"


#
#   Parse arguments
#

while :; do 
    case $1 in
        -h|-\?|--help)
            echo "$usage_help"
            exit
            ;;
        -r|--recompile)
            parameters[recompile]=1
            ;;
        -m)
            check_and_change "mode" $2 "train" "test" "none"
            ;;
        --mode=?*|--mode=)
            check_and_change "mode" ${1#*=} "train" "test" "none"
            ;;
        --nb-mfcc=?*|--nb-mfcc=)
            check_number_parameter "nb_mfcc" ${1#*=}
            ;;
        --nb-fbank=?*|--nb-fbank=)
            check_number_parameter "nb_fbank" ${1#*=}
            ;;
        --reparse-wav)
            parameters[reparse_wav]=1
            ;;
        -n|--norm)
            parameters[norm]=1
            ;;
        --frame-window=?*|--frame-window=)
            check_number_parameter "frame_window" ${1#*=}
            ;;
        --frame-step=?*|--frame-step=)
            check_number_parameter "frame_step" ${1#*=}
            ;;
        --one-vs-all)
            parameters[one_vs_all]=1
            ;;
        --main-voice-class=?*|--main-voice-class=)
            check_number_parameter "main_voice_class" ${1#*=}
            ;;
        -lc|--load-config)
            parameters[load_config]=1
            ;;
        --model=?*|--model=)
            check_and_change "model" ${1#*=} "NN" "RF"
            ;;
        --features-preprocess=?*|--features-preprocess=)
            check_number_parameter "features_preprocess" ${1#*=}
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


#   Loading configuration file if we want to
if [[ ${parameters[load_config]} == 1 ]]; then
    echo 'SYS. Loading parameters from config file'
    load_config_file
fi


#
#   Run system
#

# if we need to recompile all source files
if [[ ${parameters[recompile]} == 1 ]]; then
    echo 'SYS: Compiling...'
    g++ -std=c++11 system/main_interface.cpp \
        -lboost_regex -lboost_filesystem -lboost_system -lm -pthread\
        -o system/executable
    printf "SYS: Done.\n"
fi

# save current script parameters in config file
write_config_file

# run main programm with collected parameters
run_program_with_parameters
