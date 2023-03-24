#!/bin/bash
basename $(pwd)
thisdir="$(basename $PWD)"

#create the right folder
if [ ! -d "./splits" ]; then
  mkdir "./splits"
  echo "Created './splits' directory."
else
  echo "'./splits' directory already exists."
fi


# Parse command line options
while getopts "o:" opt; do
  case $opt in
    o) option="$OPTARG" ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
    :) echo "Option -$OPTARG requires an argument" >&2; exit 1 ;;
  esac
done

#create the right folder
if [ ! -d "./splits/$option" ]; then
  mkdir "./splits/$option"
  echo "Created './splits/$option' directory."
else
  echo "'./splits/$option' directory already exists."
fi

fileName="$option"
fileName+="-"
fileName+="$thisdir"
printf "$fileName"

# Extract duration of audio file
duration=$(ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 $fileName.wav)

# Round duration to nearest integer
duration=$(printf "%.0f" $duration)
printf $duration

#@TODO: maybe make this training / validation / test split more random...
#
# r is a random value from 0 to 32767 mod (190 - 38) . 19 seconds of training data. 19 seconds of validation.
#r=$(( $RANDOM % ($duration - 38) ));
#printf "$r"
# Run ffmpeg command
for i in $(seq 1 $duration); do
    if [ $i -gt 40 ] && [ $i -lt 60 ]; then
      ffmpeg -i $fileName.wav -ss ${i-1} -t 1 -c:a pcm_f32le "splits/$option/$i.test.wav"
    elif [ $i -gt 130 ] && [ $i -lt 150 ]; then
      ffmpeg -i $fileName.wav -ss ${i-1} -t 1 -c:a pcm_f32le "splits/$option/$i.validation.wav"
    else
      ffmpeg -i $fileName.wav -ss ${i-1} -t 1 -c:a pcm_f32le "splits/$option/$i.train.wav"
    fi
done

