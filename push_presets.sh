#/bin/bash

VS_DIR=CloudsLibrary/src/VisualSystems
cd $VS_DIR
for i in `ls .` ; do

  if [ -e "$i/.git" ]; then
	echo "UPDATING $i"
	cd $i
	git checkout master
    git add *.xml *.cpp *.h
    git pull
	if [ $? -eq 0 ]; then
  	  git commit -m "updating presets"
	  git push
 	fi
    cd ..
  fi
done

