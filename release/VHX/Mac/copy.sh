#!/bin/sh
if ! [ -d "/Library/Application Support/CLOUDS" ]; then 
mkdir /Library/Application\ Support/CLOUDS;
fi

if [ -d "/Library/Application Support/CLOUDS" ]; then
mv $DSTROOT/CloudsData /Library/Application\ Support/CLOUDS;
rm -rf $DSTROOT/CloudsData;
fi

mv $DSTROOT/dataRoot.txt /Library/Application\ Support/CLOUDS;

exit 0