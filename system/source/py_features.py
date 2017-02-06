import sys
import numpy as np
import struct
import ctypes
from python_speech_features import mfcc
from python_speech_features import logfbank


#
#   Utils
#

def values_to_int(data):
    values = data.split(' ')[:-1]
    amplitudes = map(lambda x: ctypes.c_ubyte(int(x)).value, values)
    amplitudes = [struct.unpack("h", bytearray([a, b]))[0] for a, b in zip(amplitudes[:-1:2], amplitudes[1::2])]
    return np.array(amplitudes)


def normilize(amplitudes):
    return np.array(amplitudes, dtype=float) / max(amplitudes)


def silence(amplitudes, amplitude_threshold=1000, counter_threshold=35000):
    count_lower, count_upper = 0, 0
            
    for a in amplitudes:
        if abs(a) < amplitude_threshold:
            count_lower += 1
        else:
            count_upper += 1

    return True if count_lower >= counter_threshold else False


#
#   Features
#

def run_python_mfcc(path_to_wav_amplitudes, path_to_output_features, sample_rate, number_of_mfcc_features, normilize, check_silence):
    with open(path_to_wav_amplitudes, 'rb') as inf:
        amplitudes = values_to_int(inf.read())

    # if normilize == '1':
    #     amplitures = normilize(amplitudes)

    if check_silence == '1' and silence(amplitudes):
        return

    result_frames_features = mfcc(
        amplitudes, samplerate=int(sample_rate), numcep=int(number_of_mfcc_features),
        lowfreq=20, highfreq=20000, winfunc=lambda x: np.hamming(x)
    )
    
    with open(path_to_output_features, 'w') as outf:
        for frame_index in xrange(len(result_frames_features)):
            outf.write(' '.join(map(lambda x: str(x), result_frames_features[frame_index])) + '\n')


def run_python_filterbank(path_to_wav_amplitudes, path_to_output_features, sample_rate, normilize, check_silence):
    with open(path_to_wav_amplitudes, 'rb') as inf:
        amplitudes = values_to_int(inf.read())

    # if normilize == '1':
    #     amplitures = normilize(amplitudes)
    
    if check_silence == '1' and silence(amplitudes):
        return

    result_frames_features = logfbank(
        amplitudes, samplerate=int(sample_rate),
        lowfreq=20, highfreq=20000
    )

    with open(path_to_output_features, 'w') as outf:
        for frame_index in xrange(len(result_frames_features)):
            outf.write(' '.join(map(lambda x: str(x), result_frames_features[frame_index])) + '\n')


if __name__ == '__main__':
    if sys.argv[1] == 'mfcc':
        run_python_mfcc(*sys.argv[2:])
    elif sys.argv[1] == 'fbank':
        run_python_filterbank(*sys.argv[2:])
