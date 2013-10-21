#/bin/bash

#script to rename vs presets
find . -name "*.xml" | while read f
do
  mv "${f}" "${f/$1/}"
done
