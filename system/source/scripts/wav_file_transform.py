import numpy as np
import scipy.io.wavfile as wav
import os


def add_noise(signal, pure_rate=0.94, noise_rate=0.05, lower=-2**16 + 1, upper=2**16 - 1):
    """
    Add noise to given signal. Generating noise as random integers between
    specified boundaries (by default assuming that signal is 16 bit rate)
    
    :param pure_rate    - use input signal with that factor in result signal
    :param noise_rate   - use noise signal with that factor in result signal
    :param lower        - generate noise values with that lower bound
    :paran upper        - generate noise values with that upper bound
    :return             Signal with noise 
    """
    noise = np.random.randint(lower, upper, len(signal))
    return (signal * pure_rate + noise * noise_rate).astype(signal.dtype)


def change_loudness(signal, factor=1.0):
    """
    Change signal loudness. Achieving that via multiplying 
    input signal by specified factor ( < 1.0 )

    :param factor - Change signal loudness by current factor
    :return       Signal with new loudness
    """
    return signal if factor >= 1.0 else (signal * factor).astype(signal.dtype)


def transform_one_wav(path_to_wav):
    """
    Generating transformation for one wav file.

    :param path_to_wav - absolute path to wav file wich we want to transofrm
    :return            None (saving transformed files in the same directory as input file)
    """

    # load file
    rate, signal = wav.read(path_to_wav)

    # init transformation parameters
    loudness_factors = [0.02, 0.9, 0.4]
    noise_pairs = [(0.997, 0.001), (0.998, 0.001), (0.997, 0.002)]  # [(0.95, 0.04), (0.99, 0.01), (0.97, 0.025)]

    # run each transformation
    for loudness, noise_pair in zip(loudness_factors, noise_pairs):
        # generate wav signals with transformation
        with_noise = add_noise(signal, pure_rate=noise_pair[0], noise_rate=noise_pair[1])
        with_loudness = change_loudness(signal, factor=loudness)
        with_both = change_loudness(with_noise, factor=loudness)

        # init output filenames
        path_tokens = path_to_wav.split('.')
        loudness_output_path = '.'.join(path_tokens[:-1]) + path_tokens[1] + '__loudness_{}.wav'.format(loudness)
        noise_output_path = '.'.join(path_tokens[:-1]) + path_tokens[1] + '__noise_{}.wav'.format(noise_pair)
        both_output_path = '.'.join(path_tokens[:-1]) + path_tokens[1] + '__loudness_{}__noise_{}.wav'.format(loudness, noise_pair)

        # write transformed wav files to the same directory
        wav.write(noise_output_path, rate, with_noise)
        wav.write(loudness_output_path, rate, with_loudness)
        wav.write(both_output_path, rate, with_both)


def transform_wav_files(wav_directory='/home/kolegor/Code/VAS/data/wav_files/', delete_old_transformatins=True):
    """
    Transform all wav files found in specified folder. Here assuming that specified
    folder contains directories with wav files. Parse only those folders .

    :param wav_directory                - directory with directories with wav files
    :param delete_old_transformatins    - Variable name speaks for itself
    :return                             None
    """

    for folder_name in os.listdir(wav_directory):
        # parse only files in directories
        if os.path.isfile(wav_directory + folder_name):
            continue

        print 'Parsing folder {}'.format(folder_name)
        # first - delete old transformations if we need to
        if delete_old_transformatins:
            for filename in os.listdir(wav_directory + folder_name):
                if '__loudness_' not in filename and '__noise_' not in filename:
                    continue

                filepath = wav_directory + folder_name + '/' + filename
                if not os.path.isfile(filepath):
                    continue
                os.remove(filepath)

        # generate transformations
        for filename in os.listdir(wav_directory + folder_name):
            filepath = wav_directory + folder_name + '/' + filename
            if not os.path.isfile(filepath):
                continue
            transform_one_wav(filepath)


if __name__ == '__main__':
    transform_wav_files()
