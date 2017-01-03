import sys
import numpy as np
import struct
import ctypes
from python_speech_features import mfcc
from python_speech_features import logfbank


def run_python_mfcc(path_to_wav_amplitudes, path_to_output_features, sample_rate, file_sounds_length, number_of_mfcc_features):
	with open(path_to_wav_amplitudes, 'rb') as inf:
		values = inf.read().split(' ')[:-1]
		amplitudes = map(lambda x: ctypes.c_ubyte(int(x)).value, values)
		amplitudes = [struct.unpack("h", bytearray([a, b]))[0] for a, b in zip(amplitudes[:-1], amplitudes[1:])]

	result_frames_features = mfcc(
		np.array(amplitudes), samplerate=int(sample_rate), numcep=int(number_of_mfcc_features),
		lowfreq=20, highfreq=20000, winfunc=lambda x: np.hamming(x)
	)

	with open(path_to_output_features, 'w') as outf:
		for frame_index in xrange(len(result_frames_features)):
			outf.write(' '.join(map(lambda x: str(x), result_frames_features[frame_index])) + '\n')


def run_python_filterbank(path_to_wav_amplitudes, path_to_output_features, sample_rate):
	with open(path_to_wav_amplitudes, 'rb') as inf:
		values = inf.read().split(' ')[:-1]
		amplitudes = map(lambda x: ctypes.c_ubyte(int(x)).value, values)
		amplitudes = [struct.unpack("h", bytearray([a, b]))[0] for a, b in zip(amplitudes[:-1], amplitudes[1:])]

	result_frames_features = logfbank(
		np.array(amplitudes), samplerate=int(sample_rate),
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
