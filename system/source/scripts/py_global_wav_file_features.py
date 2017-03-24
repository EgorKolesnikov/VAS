import sys
import numpy as np
from bottleneck import argpartition
from scipy.fftpack import fft
from utilities import get_wav_amplitudes


def extract_global_wav_file_features(
    path_to_wav_file
    , path_to_output_features
    , sample_rate
    , frame_length
    , frame_step
    , number_of_features=10
    , normilize=True
):
    # get fft frequencies only in human voice diapason
    freq_begin = 200
    freq_end = 4000

    # prepare parameters
    sample_rate = int(sample_rate)
    frame_length = int(frame_length)
    frame_step = int(frame_step)
    number_of_features = int(number_of_features)
    normilize = True if normilize == '1' else False

    # extract wav file amplitude values
    amplitudes = get_wav_amplitudes(path_to_wav_file, normilize)

    # extract features frame by frame
    frame_start = 0
    frames_features = []

    while frame_start + frame_length <= len(amplitudes):
        current_frame = amplitudes[frame_start:frame_start + frame_length]

        # extract features for current frame
        fft_frame = abs(fft(current_frame)[freq_begin:freq_end])
        frames_features.append(abs(argpartition(-fft_frame, number_of_features)[:number_of_features]))

        # move window
        frame_start += frame_step


    # we do not want to have empty files with features
    if len(frames_features) == 0:
    	return

    # save features
    with open(path_to_output_features, 'w') as outf:
        for frame in frames_features:
            outf.write(' '.join(map(lambda x: str(x), frame)) + '\n')


if __name__ == '__main__':
    extract_global_wav_file_features(*sys.argv[1:])
