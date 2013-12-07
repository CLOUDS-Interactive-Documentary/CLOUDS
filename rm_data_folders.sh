#/bin/bash

cd CloudsLibrary/src/VisualSystems

for i in `ls .` ; do
	git rm -r "$i/bin/data"
done
