#!/bin/bash
# Proper header for a Bash script.
#
# Move to the Directory where the mp3 files you wish to convert are stored
FILES=*.mp3
for f in $FILES
do
  echo "Converting $f "
  ffmpeg -i $f -c:a aac -strict -2 -b:a 192k -f mp4 "${f%%.*}".mp4
   echo "to ${f%%.*}.mp4" 
done