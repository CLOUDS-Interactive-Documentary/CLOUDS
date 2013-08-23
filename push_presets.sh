#/bin/bash

for i in CloudsLibrary/src/VisualSystems/ ; do
  echo "UPDATING $i"
  if [ -d "$i" ]; then
	cd $i
    git add *.xml *.cpp *.h
    git pull
# 	if [ $? -e 0 ]; then
    	git commit -m "updating presets"
		git push
#   fi
	cd -
  fi
done

