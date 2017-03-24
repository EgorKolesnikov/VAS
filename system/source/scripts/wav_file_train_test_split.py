import numpy as np
import scipy.io.wavfile as wav
import os
import time


def run_one_file_sox(filepath, file_duration, train_piece=0.8):
    template_train = 'sox {0} {0}part_train.wav --show-progress trim 00:00 ={1}'
    template_test = 'sox {0} {0}part_test.wav --show-progress trim {1}'

    end_train  = time.strftime("%M:%S", time.gmtime(file_duration * train_piece))
    start_test = time.strftime("%M:%S", time.gmtime(file_duration * train_piece + 1))

    os.system(template_train.format(filepath, end_train))
    os.system(template_test.format(filepath, start_test))


def split_wav_files(directory='/home/kolegor/Code/VAS/data/wav_files/'):
    for folder_name in os.listdir(directory):
        # parse only files in directories
        if os.path.isfile(directory + folder_name):
            continue

        print 'Parsing folder {}'.format(folder_name)
        for filename in os.listdir(directory + folder_name):
            absolute_filepath = directory + folder_name + '/' + filename

            # open to find out size
            rate, signal = wav.read(absolute_filepath)
            
            # split on trian and test
            run_one_file_sox(absolute_filepath, len(signal) / rate)
            
            # remove old version
            os.remove(absolute_filepath)


if __name__ == '__main__':
    split_wav_files()
