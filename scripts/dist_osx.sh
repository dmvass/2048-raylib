#!/bin/bash

PROJECT_NAME="2048"
DIST_DIR="dist/osx"
TMP_DMG="build/osx/$PROJECT_NAME.dmg"
TMP_DIR="build/osx/tmp"
OUT_DMG="$DIST_DIR/$PROJECT_NAME.dmg"

if [[ ! -e $DIST_DIR ]]; then
    mkdir $DIST_DIR
else
    rm -r $DIST_DIR
fi

mkdir $TMP_DIR

cp pkg/osx/pkg.template.dmg $TMP_DMG

hdiutil attach $TMP_DMG -noautoopen -mountpoint $TMP_DIR

cp -r build/osx/$PROJECT_NAME.app $TMP_DIR

DEV=$(hdiutil info | grep /dev/disk | grep partition | cut -f 1)

hdiutil detach $DEV
hdiutil convert $TMP_DMG -format UDZO -imagekey zlib-level=9 -o $OUT_DMG

rm -r $TMP_DIR
