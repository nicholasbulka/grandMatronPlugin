#!/bin/bash
var=$(pwd)
basename $(pwd)
thisdir="$(basename $PWD)"
# Parse command line options
while getopts "o:p:" opt; do
  case $opt in
    o) option1="$OPTARG" ;;
    p) option2="$OPTARG" ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
    :) echo "Option -$OPTARG requires an argument" >&2; exit 1 ;;
  esac
done

fileName1="$option1"
fileName1+="-"
fileName1+="$thisdir"
printf "$fileName1"

fileName2="$option2"
fileName2+="-"
fileName2+="$thisdir"
printf "$fileName2"

# Run ffmpeg command
#for i in $(seq 1 $duration); do
#for i in {1..4}; do
    #sox -m -v 1 $fileName1.wav -v -1 $fileName2.wav $fileName1-$fileName2-difference-$i.wav
#done

sox -m -v 1 $fileName1.wav -v -1 $fileName2.wav $fileName1-$fileName2-difference-$i.wav
