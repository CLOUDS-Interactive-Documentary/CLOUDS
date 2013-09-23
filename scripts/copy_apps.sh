#/bin/bash

for i in `ls .` ; do
	echo
    echo
	if [ -e "$i/.git" ]; then
		echo "BUILDING $i"
		cd $i
		xcodebuild -project *.xcodeproj
		cp -r bin/*.app ../CloudsVisualSystemStandalones/
		cd ../
	fi
done

