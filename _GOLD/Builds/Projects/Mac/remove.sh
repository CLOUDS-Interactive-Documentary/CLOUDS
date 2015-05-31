#!/bin/sh
if [ -d ~/Library/Application\ Support/CLOUDS/ ]; then
    sudo rm -rf ~/Library/Application\ Support/CLOUDS;
fi

if [ -d /Application/CLOUDS.app ]; then
    sudo rm -rf /Application/CLOUDS.app;
fi

if [ -d /Application/CLOUDS_OCULUS.app ]; then
    sudo rm -rf /Application/CLOUDS_OCULUS.app;
fi

exit 0