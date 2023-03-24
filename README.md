README.md


INTRO:

This is a plugin built using Logic to automate a digital filter at the same rate as an analog Moog filter. The goal was to calibrate them to be as closely in sync regarding latency and frequency as possible. This generated 16 .wav files of about 187 seconds each. One f32 .wav file was generated for the unfiltered audio data, the data fed through a Moog Matriarch and recorded, and a digital low pass filter. Thanks to matkatmusic and the JUCE dsp modules for example code on creating a simple digital low pass filter.

Each .wav file was then split into training, test and validation data. This data was fed into a tensorflow model. The final training took 4 hours only using CPU and this is definitely an area of further augmentation. Thanks to Jatin Chowdhury and RTNeural for this great Real Time machine learning library. 

After the model was trained, a JUCE plugin was created that multiplies the modeled analog signal with the digital low pass filter with some other plugin gain adjustments. This removes some of the model's artifacts while smoothing out the digital bits one can hear when sweeping the strictly digital implementation. Librosa images taken of some results in this README show more detail.

PLUGIN VIDEO:

www.youtube.com/here



BUILD
To build, all you have to do is load this project in your favorite IDE (CLion/Visual Studio/VSCode/etc) and click 'build' for one of the targets (templates, JUCE examples, Projucer, etc).
You can also generate a project for an IDE by using (Mac):
cmake -G Xcode -B build

Much thanks to Eyal Amir for sharing the JUCECmakeRepoPrototype repository.
More information available here:
https://github.com/eyalamirmusic/JUCECmakeRepoPrototype

Plugin should be built in Release mode in order to take advantage of optimizations. Debug will probably be too slow for the audio callback.




DATA COLLECTION DEEP DIVE:
https://youtu.be/XkcoEQ1IlQY
Logic template:

DATA PREPROCESSING DEEP DIVE:
https://youtu.be/8xwIotfP9zk 



python code is available in the training directory. Reach out if you'd like the sample data, it is slightly over 9 GB.




Thanks to the whole NAP Competition for the opportunity, and many unnamed "theaudioprogrammer" Discord chat members for all of their help.


