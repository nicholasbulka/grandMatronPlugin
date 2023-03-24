import numpy as np
import os
import sys
from scipy.io.wavfile import read, write

def main(directory):

    # Load WAV file
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_name = os.fsdecode(file)
            if file_name.endswith(".wav"):

                sr, audio = read(root + '/' + file_name)
                file_path = os.path.join(root, file)
                # Save as .npy file
                np.save(root + '/' +file_name + '.npy', audio)


if __name__ == '__main__':
    a = str(sys.argv[1])
    main(a)

