#!/bin/sh
if [ -d ~/Library/Application\ Support/CLOUDS/ ]; then
    sudo rm -rf ~/Library/Application\ Support/CLOUDS;
fi

if [ -d /Applications/CLOUDS.app ]; then
    sudo rm -rf /Applications/CLOUDS.app;
fi

if [ -d /Applications/CLOUDS_OCULUS.app ]; then
    sudo rm -rf /Applications/CLOUDS_OCULUS.app;
fi

exit 0