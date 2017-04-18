import numpy as np
import struct
import ctypes
import scipy.io.wavfile as wav
import pandas


def normilize_wav(signal):
    return (np.array(signal, dtype=np.float) / 2**16.) * 2 - 1


def get_wav_amplitudes(path_to_wav_file, normilize=True):   
    """
    Read amplitudes from wav file. If normalization = True,
    then expect only 16-bit wav files.
    """ 
    rate, signal = wav.read(path_to_wav_file)
    if normilize:
        signal = normilize_wav(signal)
    return rate, signal


def load_file_info(filepath):
    """
    Read train data. Expecting header.
    """
    df, X = pandas.read_csv(filepath), []
    for record in df['Features'].values:
        X.extend([float(a) for a in record.split(' ')])

    X = np.array(X).reshape(len(df), (len(X) / len(df)))
    y = df['Class'].values

    if 0 not in y:
        y = y - 1
    return X, y


def load_test_wav_features(path_to_features):
    """
    Loading test data
    """
    with open(path_to_features, 'r') as inf:
        lines = []
        for line in inf:
            lines.append(list(map(float, filter(lambda x: len(x.strip()) > 0, line.split(' ')))))

        return np.array(lines)


class FeaturesPreprocess:
    """
    Wrapper for features preprocess.
    Preprocess may be done for train and test data. 
    
    If mode='test' specified then 'main_class' parameter should be one of the following:
     - None (by default. Means that in train preprocess we did not do anything with one separate class)
     - Any other not None value (Means that we have to preprocess with respect to result on train stage preprocess)

    Logical errors:
     - If 'y' vector is None and main_class is not None - exception.
     - If mode='test' and main_class is not None and preprocess_help_data is None - exception.

    If mode='test' and 'preprocess_help_data' is not None, then 'preprocess_help_data' data should be the same
    as was returned from same function on mode='train' stage
    """

    @staticmethod
    def check_params(X, y=None, main_class=None, mode='train', preprocess_help_data=None):
        if mode == 'train' and y is None and main_class is not None:
            raise Exception('FeaturesPreprocess::check_params(). Invalid parameters.')
        if mode == 'test' and main_class is not None and preprocess_help_data is None:
            raise Exception('FeaturesPreprocess::check_params(). Invalid parameters.')

    @staticmethod
    def no_preprocess(X, y=None, main_class=None, mode='train', preprocess_help_data=None):
        FeaturesPreprocess.check_params(X, y, main_class, mode, preprocess_help_data)
        return X, 0

    @staticmethod
    def normalization(X, y=None, main_class=None, mode='train', preprocess_help_data=None):
        FeaturesPreprocess.check_params(X, y, main_class, mode, preprocess_help_data)

        if main_class is None:
            X -= np.mean(X, axis=0)
            X /= np.std(X, axis=0)
            return X, None
        else:
            if mode == 'train':
                main_class_data = np.array([x for x, y_ in zip(X, y) if y_ == main_class])
                mean_ = np.mean(main_class_data, axis=0)
                std_ = np.std(main_class_data, axis=0)

                X -= mean_
                X /= std_
                return X, (mean_, std_)
            else:
                mean_, std_ = preprocess_help_data
                X -= mean_
                X /= std_
                return X

    @staticmethod
    def pca_whitening(X, y=None, main_class=None, mode='train', preprocess_help_data=None):
        raise Exception('FeaturesPreprocess::pca_whitening(). Not implemented yet.')
