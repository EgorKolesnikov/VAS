import numpy as np
import struct
import ctypes
import scipy.io.wavfile as wav
import pandas


def normilize_wav(path_to_wav):
    pass


def get_wav_amplitudes(path_to_wav_file, normilize=True):
    if normilize:
        normilize_wav(path_to_wav_file)
    
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


def load_file_info(filepath):
    df, X = pandas.read_csv(filepath), []
    for record in df['Features'].values:
        X.extend([float(a) for a in record.split(' ')[:-1]])
    X = np.array(X).reshape(len(df), (len(X) / len(df)))
    y = df['Class'].values
    return X, y


def load_test_wav_features(path_to_features):
    with open(path_to_features, 'r') as inf:
        lines = []
        for line in inf:
            lines.append(list(map(float, filter(lambda x: len(x.strip()) > 0, line.split(' ')))))

        return np.array([np.mean(lines, axis=0)])
