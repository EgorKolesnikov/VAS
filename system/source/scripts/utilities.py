import numpy as np
import struct
import ctypes
import scipy.io.wavfile as wav
import pandas


def normilize_wav(signal):
    return (np.array(signal, dtype=np.float) / 2**16.) * 2 - 1


def get_wav_amplitudes(path_to_wav_file, normilize=True):    
    rate, signal = wav.read(path_to_wav_file)
    if normilize:
        signal = normilize_wav(signal)
    return signal


def load_file_info(filepath):
    df, X = pandas.read_csv(filepath), []
    for record in df['Features'].values:
        X.extend([float(a) for a in record.split(' ')[:-1]])
    X = np.array(X).reshape(len(df), (len(X) / len(df)))
    y = df['Class'].values

    if 0 not in y:
        y = y - 1
    return X, y


def load_test_wav_features(path_to_features):
    with open(path_to_features, 'r') as inf:
        lines = []
        for line in inf:
            lines.append(list(map(float, filter(lambda x: len(x.strip()) > 0, line.split(' ')))))

        return np.array([np.mean(lines, axis=0)])
