import sys
import numpy as np
from python_speech_features import logfbank
from utilities import get_wav_amplitudes, silence


def run_python_filterbank(
    path_to_wav_file
    , path_to_output_features
    , sample_rate
    , frame_length
    , frame_step
    , number_of_fbank_features
    , normilize=True
):
    # prepare parameters
    sample_rate = int(sample_rate)
    frame_length = int(frame_length)
    frame_step = int(frame_step)
    number_of_fbank_features = int(number_of_fbank_features)
    normilize = True if normilize == '1' else False

    # extract wav file amplitude values
    amplitudes = get_wav_amplitudes(path_to_wav_file, normilize)

    # extract features
    frames_features = logfbank(
        amplitudes, samplerate=int(sample_rate),
        lowfreq=20, highfreq=20000, nfilt=number_of_fbank_features,
        winlen=float(frame_length) / sample_rate, winstep=float(frame_step) / sample_rate
    )
    
    # we do not want to have empty files with features
    if len(frames_features) != 0:
        with open(path_to_output_features, 'w') as outf:
            for frame in frames_features:
                outf.write(' '.join(map(lambda x: str(x), frame)) + '\n')


if __name__ == '__main__':
    run_python_filterbank(*sys.argv[1:])