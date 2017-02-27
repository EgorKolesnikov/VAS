import sys
import numpy as np
from python_speech_features import mfcc
from utilities import get_wav_amplitudes, silence


def run_python_mfcc(
    path_to_wav_file
    , path_to_output_features
    , sample_rate
    , frame_length
    , frame_step
    , number_of_mfcc_features
    , normilize=True
):
    # prepare parameters
    sample_rate = int(sample_rate)
    frame_length = int(frame_length)
    frame_step = int(frame_step)
    number_of_mfcc_features = int(number_of_mfcc_features)
    normilize = True if normilize == '1' else False

    # extract wav file amplitude values
    amplitudes = get_wav_amplitudes(path_to_wav_file, normilize)

    # init iteration
    frames_features = mfcc(
        amplitudes, 
        samplerate=int(sample_rate), numcep=int(number_of_mfcc_features),
        lowfreq=20, highfreq=20000, winfunc=lambda x: np.hamming(x),
        winlen=float(frame_length) / sample_rate, winstep=float(frame_step) / sample_rate
    )
    
    # we do not want to have empty files with features
    if len(frames_features) != 0:
        with open(path_to_output_features, 'w') as outf:
            for frame in frames_features:
                outf.write(' '.join(map(lambda x: str(x), frame)) + '\n')


if __name__ == '__main__':
    run_python_mfcc(*sys.argv[1:])
