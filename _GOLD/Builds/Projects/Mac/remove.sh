#!/bin/sh
if [ -d "~/Library/Application Support/CLOUDS/CloudsData" ]; then
rm -rf ~/Library/Application\ Support/CLOUDS/CloudsData;
fi

if [ -d "/Application/CLOUDS.app" ]; then
rm -rf /Application/CLOUDS.app;
fi

if [ -d "/Application/CLOUDS_OCULUS.app" ]; then
rm -rf /Application/CLOUDS_OCULUS.app;
fi

exit 0