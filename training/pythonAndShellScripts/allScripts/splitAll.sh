#!/bin/bash

./makeMono.sh -o groundTruth
./makeMono.sh -o matriarchPlugin
./makeMono.sh -o matronPlugin
./makeMono.sh -o moogFilteredResult

./split.sh -o mono-44100-groundTruth
./split.sh -o mono-44100-matriarchPlugin
./split.sh -o mono-44100-matronPlugin
./split.sh -o mono-44100-moogFilteredResult

cd splits
pwd
cd mono-44100-groundTruth
pwd

#remove incomplete audio files at the end if they exist.
rm 190.train.wav
rm 189.train.wav
rm 188.train.wav
cd ..
cd mono-44100-matriarchPlugin
pwd
rm 190.train.wav
rm 189.train.wav
rm 188.train.wav
cd ..
cd mono-44100-matronPlugin
pwd
rm 190.train.wav
rm 189.train.wav
rm 188.train.wav
cd ..
cd mono-44100-moogFilteredResult
pwd
rm 190.train.wav
rm 189.train.wav
rm 188.train.wav

cd ..

cd ..

python numpyfy.py splits
