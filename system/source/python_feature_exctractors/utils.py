import numpy as np
import struct
import ctypes
import scipy.io.wavfile as wav


def get_wav_amplitudes(path_to_wav_file):
    rate, signal = wav.read(path_to_wav_file)
    return signal


def silence(amplitudes, amplitude_threshold=1000, counter_threshold=35000):
    count_lower, count_upper = 0, 0
            
    for a in amplitudes:
        if abs(a) < amplitude_threshold:
            count_lower += 1
        else:
            count_upper += 1

    return True if count_lower >= counter_threshold else False
