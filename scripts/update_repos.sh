#/bin/bash

for i in `ls .` ; do
	echo
    echo
	if [ -e "$i/.git" ]; then
		echo "UPDATING $i"
		cd $i
		git fetch origin
		git checkout master
		git add *.cpp *.h .gitignore
		git commit -m "updating presets"
		git pull origin master
		if [ $? -eq 0 ]; then
		  git push origin master
		fi
		cd VisualSystemsLibrary
		git pull origin master
		cd ../..
	fi
done

