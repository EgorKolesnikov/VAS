#!/bin/bash

#################################################################################################

usage_help="
   -r, --recompile      : if we want to recompile all source or not
   -m, --mode=...       : 'train' or 'test'
   --features=...       : features to create for each wav file ('mfcc', 'fbank', 'both')
   --nb-mfcc=...        : number of mfcc coefficients (int)
   --nb-fbank=...       : number of filterbanks (int)
   --reparse_wav        : if we want to reparse all wav files (only when training)
   -n, --norm           : normilize audio file or not
   -s, --silence        : do not work with silence parts of wav file
"

################################################################################################


#
#   Util functions
#

function check_and_change() {
    # Check if assigned parameter value is in possible values.
    # $1 - parameter name; $2 - parameter value; $3, $4, ... - possible values 

    if [ -z $2 ]; then
        printf 'ERROR: "???" requires a non-empty option argument: "train" or "test".\n'
        exit
    fi

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

function check_integer_parameter(){
    # Check if parameter is integet number. If it is - change parameter value
    # $1 - parameter name, $2 - parameter value

    if [ -z $2 ]; then
        printf 'ERROR: "--mfcc" requires a non-empty integer option argument.\n'
        exit
    fi

    if [[ $2 != +([0-9]) ]]; then
        printf 'ERROR: value for "--mfcc" parameter is not an integer value.\n'
        exit
    fi
    parameters[$1]="$2"
}


#
#   Util variables
#

declare -A parameters
parameters[recompile]=0
parameters[mode]="test"
parameters[features]="both"
parameters[nb_mfcc]=13
parameters[nb_fbank]=26
parameters[reparse_wav]=0
parameters[norm]=0
parameters[silence]=0

source_folder="system/source/"
main_interface_folder="system/"
output_executable_name="executable"
classification_result_file="data/nn_data/last_recorded_wav_prediction.txt"


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
            check_and_change "mode" $2 "train" "test"
            ;;
        --mode=?*|--mode=)
            check_and_change "mode" ${1#*=} "train" "test"
            ;;
        --features=?*|--features=)
            check_and_change "features" ${1#*=} "mfcc" "fbank" "both"
            ;;
        --nb-mfcc=?*|--nb-mfcc=)
            check_integer_parameter "nb_mfcc" ${1#*=}
            ;;
        --nb-fbank=?*|--nb-fbank=)
            check_integer_parameter "nb_fbank" ${1#*=}
            ;;
        --reparse_wav)
            parameters[reparse_wav]=1
            ;;
        -n|--norm)
            parameters[norm]=1
            ;;
        -s|--silence)
            parameters[silence]=1
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


#
#   Run system
#

# if we need to recompile all source files
if [[ ${parameters[recompile]} == 1 ]]; then
    echo 'SYS: Compiling...'
    (g++ -std=c++11 "$main_interface_folder"main_interface.cpp -lboost_regex -lboost_filesystem -lboost_system -lm -o $main_interface_folder$output_executable_name)
    printf "SYS: Done.\n"
fi

# train or test out system
if [[ ${parameters[mode]} == "train" ]]; then
    $main_interface_folder./executable ${parameters[mode]} ${parameters[features]} ${parameters[nb_mfcc]} ${parameters[nb_fbank]} ${parameters[reparse_wav]} ${parameters[norm]} ${parameters[silence]}
else
    # Prepare user for recording
    printf "SYS: You will have 2 seconds to record your voice. Recording will start in\n"
    sleep 3
    for i in {3..1}; do echo "$i..." && sleep 1; done
    echo "RECORDING..."
    (python "$main_interface_folder"wav_record.py "data/recorded.wav")

    # Run nn classification
    printf "\nSYS: Running neural network to classify your voice.\n"
    $main_interface_folder./executable ${parameters[mode]} ${parameters[features]} ${parameters[nb_mfcc]} ${parameters[nb_fbank]} ${parameters[reparse_wav]} ${parameters[norm]} ${parameters[silence]}

    # Check the results
    read -d $'\x04' name < "$classification_result_file"
    if [[ $name == "0" ]]; then
        echo 'DANGER! DANGER!'
    else
        echo "Welcome, master Egor!"
    fi
fi
