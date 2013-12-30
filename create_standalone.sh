#/bin/bash

#making stand alone folder
echo "making stand alone folder"
mkdir CLOUDSStandAlone
mkdir CLOUDSStandAlone/data

rm -r CLOUDSStandAlone/data/CloudsData/VisualSystems
mkdir CLOUDSStandAlone/data/CloudsData/VisualSystems

echo "copying CLOUDS binary"
rm -r CLOUDSStandAlone/CLOUDS.app
cp -r CLOUDS/bin/CLOUDS.app CLOUDSStandAlone/

#copy data path
#echo "copying all CloudsData assets"
#cp -r CloudsData/* CLOUDSStandAlone/data/CloudsData

#copy all presets
echo "copy all presets"


cd CloudsLibrary/src/VisualSystems

for i in `ls .` ; do
	echo 
	echo 
	echo
	echo "COPYING PRESETS FOR  $i"
	mkdir "../../../CLOUDSStandAlone/data/CloudsData/VisualSystems/$i"
	for f in `ls $i/bin/data/` ; do
		cp -r "$i/bin/data/$f" "../../../CLOUDSStandAlone/data/CloudsData/visualsystems/$i/"
	done

done
