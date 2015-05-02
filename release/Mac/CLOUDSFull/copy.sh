#!/bin/sh
if ! [ -d "/Library/Application Support/CLOUDS" ]; then 
mkdir /Library/Application\ Support/CLOUDS;
fi

if [ -d "/Library/Application Support/CLOUDS" ]; then
mv $DSTROOT/CloudsData /Library/Application\ Support/CLOUDS;
rm -rf $DSTROOT/CloudsData;
fi

if [ -d "/Volume/CLOUDS/CloudsDataMedia" ]; then
mkdir /Library/Application\ Support/CLOUDS/CloudsDataMedia;
cp -r /Volume/CLOUDS/CloudsDataMedia /Library/Application\ Support/CLOUDS/CloudsDataMedia;
fi

mv $DSTROOT/dataRoot.txt /Library/Application\ Support/CLOUDS;
mv $DSTROOT/mediaRoot.txt /Library/Application\ Support/CLOUDS;

exit 0