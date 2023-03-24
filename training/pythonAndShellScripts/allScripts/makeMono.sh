#!/bin/bash
var=$(pwd)
basename $(pwd)
thisdir="$(basename $PWD)"

# Parse command line options
while getopts "o:" opt; do
  case $opt in
    o) option="$OPTARG" ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
    :) echo "Option -$OPTARG requires an argument" >&2; exit 1 ;;
  esac
done

fileName="$option"
fileName+="-"
fileName+="$thisdir"
printf "$fileName"

ffmpeg -i $fileName.wav -ar 44100 -ac 1 "mono-44100-$fileName.wav"

