#!/bin/bash

mkdir ./Vcash.app/Contents/Resources
cp -f ../../resources/osx/AppIcon.icns ./Vcash.app/Contents/Resources/AppIcon.icns
mkdir ./Vcash.app/Contents/MacOS
cp -f ../../vcash ./Vcash.app/Contents/MacOS/vcash

