import sys
import numpy as np
from python_speech_features import mfcc, logfbank
from utilities import get_wav_amplitudes


def extract_features(
    path_to_wav_file            # absolute path to wav file to extract features from
    , path_to_store_results     # absolute path to text file to store extracted results to
    , frame_length              # size of frame of wav file to extract features for [seconds]
    , frame_step                # shift frame window on that amount of time [seconds]
    , nb_fbank_features         # number of filterbank features to extract
    , nb_mfcc_features          # number of mfcc features to extract
    , normilize                 # number of mfcc features to extract
):
    # prepare parameters
    frame_length = int(frame_length)
    frame_step = int(frame_step)
    nb_fbank_features = int(nb_fbank_features)
    nb_mfcc_features = int(nb_mfcc_features)
    normilize = True if normilize == '1' else False

    # extract wav file amplitude values
    sample_rate, amplitudes = get_wav_amplitudes(path_to_wav_file, normilize)

    # extract features
    mfcc_frames_features = mfcc(
        amplitudes
        , numcep=nb_mfcc_features
        , nfilt=nb_mfcc_features
        , samplerate=sample_rate
        , lowfreq=20, highfreq=20000
        , winfunc=lambda x: np.hamming(x)
        , winlen=float(frame_length) / sample_rate
        , winstep=float(frame_step) / sample_rate)
    
    fbank_frames_features = logfbank(
        amplitudes
        , nfilt=nb_fbank_features
        , samplerate=sample_rate
        , lowfreq=20, highfreq=20000
        , winlen=float(frame_length) / sample_rate
        , winstep=float(frame_step) / sample_rate
    )

    # combine and save in file
    assert(len(mfcc_frames_features) == len(fbank_frames_features))

    with open(path_to_store_results, 'w') as outf:
        for frame_features in np.concatenate((mfcc_frames_features, fbank_frames_features), axis=1):
            outf.write(' '.join(map(str, frame_features)))
            outf.write('\n')


if __name__ == '__main__':
    extract_features(*sys.argv[1:])
    