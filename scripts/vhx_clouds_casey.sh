#!/bin/bash

if [ "$#" -ne 2 ]; then 
    echo "Wrong number of parameters!"
    echo "$ ./vhx_clouds.sh <input_path> <output_path>"
    exit 1
fi

mkdir $2

#JAMES IS DOING THIS ONE
#files=( $(ls $1 -1 | grep "^[A-K].*.mov") )

#CASEY DO THIS ONE
files=( $(ls $1 -1 | grep "^[L-Z].*.mov") )

for file in "${files[@]}"
do
    echo "Processing $file"
    ffmpeg -i $1/$file -y -b:v 1M -s 1280x1200 -pass 1 -f mp4 -an /dev/null
	ffmpeg -i $1/$file -y -b:v 1M -s 1280x1200 -pass 2 -f mp4 $2/$file
done
