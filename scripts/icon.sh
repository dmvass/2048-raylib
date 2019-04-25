#!/bin/bash

if [ "$1" = "" ]
then
  echo "Destination path is required"
  exit 1
fi

ORIGIN_ICON=resources/icon.png
ICON_DIR="$1/icon.iconset"
ICON_NAME="$1/icon.icns"

mkdir $ICON_DIR

# Normal screen icons
for SIZE in 16 32 64 128 256 512; do
        sips -z $SIZE $SIZE $ORIGIN_ICON --out $ICON_DIR/icon_${SIZE}x${SIZE}.png;
done

# Retina display icons
for SIZE in 32 64 256 512; do
        sips -z $SIZE $SIZE $ORIGIN_ICON --out $ICON_DIR/icon_$(expr $SIZE / 2)x$(expr $SIZE / 2)x2.png;
done

# Make a multi-resolution Icon
iconutil -c icns -o $ICON_NAME $ICON_DIR
rm -rf $ICON_DIR
