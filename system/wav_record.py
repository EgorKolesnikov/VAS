import numpy as np
import pyaudio
import wave
import sys
from struct import pack
from sys import byteorder
from array import array
from datetime import datetime


class Recorder:

    def __init__(self, rate=44100, chunk=1024, channels=1, format=pyaudio.paInt16):
        self.rate = rate
        self.chunk = chunk
        self.channels = channels
        self.format = format
        self.listener = pyaudio.PyAudio()

    def record(self, seconds=5):
        audio_stream = self.listener.open(
            format=self.format, channels=self.channels,
            rate=self.rate, input=True, frames_per_buffer=self.chunk
        )
        
        frames = []
        for i in range(0, int(self.rate / self.chunk * seconds)):
            amplitude_values = array('h', audio_stream.read(self.chunk))
            frames.extend(amplitude_values)
            
        if byteorder == 'big':
            frames.byteswap()
        
        audio_stream.stop_stream()
        audio_stream.close()
        print 'Done recording.'
        return frames

    def save(self, frames, filepath_to_save='/home/kolegor/Code/VAS/data/_last_recorded.wav'):        
        waveFile = wave.open(filepath_to_save, 'wb')
        waveFile.setnchannels(self.channels)
        waveFile.setframerate(self.rate)
        waveFile.setsampwidth(self.listener.get_sample_size(self.format))
        waveFile.writeframes(pack('<' + ('h'*len(frames)), *frames))
        waveFile.close()


def record(seconds=5, return_frames=False):
    recorder = Recorder()
    frames = recorder.record(seconds=seconds)
    recorder.save(frames)
    return frames if return_frames else 0


if __name__ == '__main__':
    record()
