#!/bin/sh
homedir=~
eval homedir=$homedir
echo $homedir # prints home path

if ! [ -d $homedir/Library/Application\ Support/CLOUDS ]; then
	mkdir $homedir/Library/Application\ Support/CLOUDS;
fi

if [ -d $homedir/Library/Application\ Support/CLOUDS ]; then
	mv $DSTROOT/CloudsData $homedir/Library/Application\ Support/CLOUDS;
	rm -rf $DSTROOT/CloudsData;
fi

#mv $DSTROOT/dataRoot.txt ~/Library/Application\ Support/CLOUDS;

open /Applications/

exit 0