import librosa
import os
import numpy as np
import matplotlib.pyplot as plt

prefix = 'a00'
folder_dir = 'path/to/mlData/'
for i in range(1,2):
    for fileName in ['groundTruth', 'moogFilteredResult', 'matronPlugin', 'matriarchPlugin']:

        songName = 'unlabeled'
        for file in os.listdir(folder_dir + prefix + str(i)):
            if(file.startswith("_")):
                songName = file

        y, sr = librosa.load('../mlData/'+ prefix + str(i)+'/'+fileName+'-'+ prefix +str(i)+'.wav', sr=None, duration=20, offset=25, mono=True)
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
        plt.title(songName+'-'+fileName+'-'+str(i))
        plt.show()


