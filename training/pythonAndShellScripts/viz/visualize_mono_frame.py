import librosa
import os
import numpy as np
import matplotlib.pyplot as plt
import sys, getopt

def main(single_file_name):
    y, sr = librosa.load(single_file_name, sr=None, duration=1, offset=0, mono=True)
    D = librosa.stft(y)
    mag, phase = librosa.magphase(D)
    freqs = librosa.fft_frequencies()
    times = librosa.times_like(D)
    phase_exp = 2*np.pi*np.multiply.outer(freqs,times)
    fig, ax = plt.subplots()
    img = librosa.display.specshow(np.diff(np.unwrap(np.angle(phase)-phase_exp, axis=1), axis=1, prepend=0),
                             cmap='hsv',
                             alpha=librosa.amplitude_to_db(mag, ref=np.max)/80 + 1,
                             ax=ax,
                             y_axis='log',
                             x_axis='time')
    ax.set_facecolor('#000')
    cbar = fig.colorbar(img, ticks=[-np.pi, -np.pi/2, 0, np.pi/2, np.pi])
    cbar.ax.set(yticklabels=['-π', '-π/2', "0", 'π/2', 'π']);
    plt.title(single_file_name)
    plt.show()

if __name__ == '__main__':
    a = str(sys.argv[1])
    main(a)
