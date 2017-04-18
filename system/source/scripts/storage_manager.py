import numpy as np
import scipy.io.wavfile as wav
import os
import re
import shutil
import time


#
#   Script utils (main interface see below)
#

def add_noise(signal, pure_rate=0.94, noise_rate=0.05, lower=-2**16 + 1, upper=2**16 - 1):
    """
    Add noise to given signal. Generating noise as random integers between
    specified boundaries (by default assuming that signal is 16 bit rate)
    
    :param pure_rate    - use input signal with that factor in result signal
    :param noise_rate   - use noise signal with that factor in result signal
    :param lower        - noise value lower bound
    :paran upper        - noise value upper bound
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
        loudness_output_path = '.'.join(path_tokens[:-1]) + '__loudness_{}.wav'.format(loudness)
        noise_output_path = '.'.join(path_tokens[:-1]) + '__noise_{}.wav'.format(noise_pair)
        both_output_path = '.'.join(path_tokens[:-1]) + '__loudness_{}__noise_{}.wav'.format(loudness, noise_pair)

        # write transformed wav files to the same directory
        wav.write(noise_output_path, rate, with_noise)
        wav.write(loudness_output_path, rate, with_loudness)
        wav.write(both_output_path, rate, with_both)


def augmentate_data(folders_with_voices):
    """
    Transform all wav files found in specified folders. Here assuming that specified
    folders contains directories with wav files. Parse only those folders.

    :param folders_with_voices: directory with directories with wav files
    """

    for folder_path in folders_with_voices:
        for current_folder_name in os.listdir(folder_path):
            current_folder_absolute_path = os.path.join(folder_path, current_folder_name)
            if not os.path.isdir(current_folder_absolute_path):
                continue

            print 'Parsing folder {}'.format(current_folder_absolute_path)
            for filename in os.listdir(current_folder_absolute_path):
                filepath = os.path.join(current_folder_absolute_path, filename)
                if not os.path.isfile(filepath):
                    continue
                transform_one_wav(filepath)


def split_and_save_one_file(filepath, path_to_train, path_to_test, file_duration, test_size, show_progress=False):
    """
    Split given wav file in train and test part. 
    Save those files in train and test directories.

    TODO: random split (not from begining)

    :param filepath:            folder with all wav files (folder with folder for each voice)
    :param path_to_train:       path to save train part
    :param path_to_test:        path to save test part
    :param file_duration:       wav file duration in seconds
    :param test_size:           size of test part
    """
    template_test = 'sox {0} {1}part_test.wav {2} trim 00:00 ={3}'
    template_train = 'sox {0} {1}part_train.wav {2} trim {3}'

    end_test = time.strftime("%M:%S", time.gmtime(file_duration * test_size))
    start_train = time.strftime("%M:%S", time.gmtime(file_duration * test_size + 1))

    os.system(template_test.format(filepath, path_to_test.replace('.wav', '_'), '--show-progress' if show_progress else '', end_test))
    os.system(template_train.format(filepath, path_to_train.replace('.wav', '_'), '--show-progress' if show_progress else '', start_train))


def split_on_train_test(path_to_wav_files, path_to_train, path_to_test, test_size=0.3):
    """
    Split each file in storage into train and test part.

    :param path_to_wav_files:   folder with all wav files (folder with folder for each voice)
    :param path_to_train:       path to save train part
    :param path_to_test:        path to save test part
    :param test_size:           size of test part
    """
    for folder_name in os.listdir(path_to_wav_files):
        folder_absolute_path = os.path.join(path_to_wav_files, folder_name)
        if not os.path.isdir(folder_absolute_path):
            continue

        print 'Parsing folder {}'.format(folder_name)
        for filename in os.listdir(folder_absolute_path):
            absolute_filepath = os.path.join(folder_absolute_path, filename)

            # open to find out size
            rate, signal = wav.read(absolute_filepath)
            
            # split on trian and test
            split_and_save_one_file(
                absolute_filepath
                , os.path.join(path_to_train, folder_name, filename)
                , os.path.join(path_to_test, folder_name, filename)
                , len(signal) / rate
                , test_size
            )


def delete_old_data(path_to_storage, path_to_train_folder, path_to_test_folder):
    """
    Delete old train and test splits (with all augmentations)
    Create the same sa in storage folder structure of directories in train and test folders
    """

    # delete old data
    for folder in [path_to_train_folder, path_to_test_folder]:
        shutil.rmtree(folder)
        os.mkdir(folder)

    # recreate directories structure
    for folder_name in os.listdir(path_to_storage):
        if not os.path.isdir(os.path.join(path_to_storage, folder_name)):
            continue

        os.mkdir(os.path.join(path_to_train_folder, folder_name))
        os.mkdir(os.path.join(path_to_test_folder, folder_name))


def split_wav_file(original_wav_file_path, number_of_parts):
    """
    Split given wav file into given number of parts (separate wav files)
    
    :param original_wav_file_path:  path to wav file to split
    :param number_of_parts:         number of new wav files (original wav files parts)
    """

    rate, original_signal = wav.read(original_wav_file_path)
    one_part_size = len(original_signal) / number_of_parts

    for part_number in xrange(number_of_parts):
        wav.write(
            original_wav_file_path.replace('.wav', '_part_{}.wav'.format(part_number))
            , rate
            , original_signal[part_number * one_part_size : min(len(original_signal), (part_number + 1) * one_part_size)]
        )


#
#   Main script interface
#

def create_train_test(
    path_to_wav_files='/home/kolegor/Code/VAS/data/storage/'
    , path_to_train='/home/kolegor/Code/VAS/data/train/data/'
    , path_to_test='/home/kolegor/Code/VAS/data/test/data/'
    , test_size=0.3
):
    """
    Create train and test data. Split wav in sotrage on train and test
    parts. Augmentate each part.
    """

    print ' - Deleting old data'
    delete_old_data(path_to_wav_files, path_to_train, path_to_test)
    
    print ' - Splitting on train and test'
    split_on_train_test(path_to_wav_files, path_to_train, path_to_test, test_size)
    
    print ' - Data augmentation'
    augmentate_data([path_to_train, path_to_test])


def split_big_storage_files(
    path_to_storage='/home/kolegor/Code/VAS/data/storage/'
    , file_size_threshold=35000000  # 35MB
):
    """
    Big wav files (more that 30-40MB or more than 5-6 minutes) are causing memory error.
    Split those files in parts. Leave save filename, only adding 'part_x' suffix (x - part_number)

    Also check folder names to be in format "voice_###_@", where ### - any string in any format
    and @ - voice class (integer) from 0 to infty

    :param path_to_storage:         global storage folder
    :param file_size_threshold:     split wav file if size of file more than this threshold (bytes)
    """

    folder_name_pattern = re.compile('voice_(.*?)_\d+$')

    for folder_name in os.listdir(path_to_storage):
        absolute_folder_path = os.path.join(path_to_storage, folder_name)

        # check only folders
        if not os.path.isdir(absolute_folder_path):
            continue

        # check folder_name format
        #if folder_name_pattern.match(folder_name) is None:
        #    raise Exception('Invalid folder name: {}'.format(folder_name))

        for file_name in os.listdir(absolute_folder_path):
            absolute_file_path = os.path.join(absolute_folder_path, file_name)
            # check only wav files
            if not file_name.endswith('.wav'):
                continue

            file_size = os.path.getsize(absolute_file_path)

            # split if size bigger that threshold
            if file_size > file_size_threshold:
                nb_parts = int(file_size / (0.9 * file_size_threshold)) + 1
                split_wav_file(absolute_file_path, nb_parts)

            # delete original file (we have parts now)
            os.remove(absolute_file_path)


if __name__ == '__main__':
    split_big_storage_files()
    create_train_test()
