#!/bin/sh
if ! [ -d "/Library/Application Support/CLOUDS" ]; then 
mkdir /Library/Application\ Support/CLOUDS;
fi

if ! [ -d "/Library/Application Support/CLOUDS/data" ]; then 
mkdir /Library/Application\ Support/CLOUDS/data;
fi

if [ -d "/Library/Application Support/CLOUDS/data" ]; then
rm -rf /Library/Application\ Support/CLOUDS/data/*;
mv /Applications/CloudsData/* /Library/Application\ Support/CLOUDS/data;
rm -rf /Applications/CloudsData/;
fi

mv /Applications/dataRoot.txt /Library/Application\ Support/CLOUDS;
mv /Applications/mediaRoot.txt /Library/Application\ Support/CLOUDS;

exit 0