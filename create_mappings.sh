#/bin/bash

cd CloudsLibrary/src/VisualSystems

for i in `ls .` ; do
	echo "#include \"CloudsVisualSystem$i.h\""
done

echo
echo
echo

for i in `ls .` ; do

	echo "{ \"$i\", &fCreate<CloudsVisualSystem$i> },"
done


