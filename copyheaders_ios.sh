#!/bin/bash

rm -rf include/ios
mkdir -p include/ios

cp src/GameEngineConfig.h include/ios/
cp src/jsscripting/JSComponent.h include/ios/
cp src/jsscripting/JSScriptingCore.h include/ios/
cp src/mac/semaphore_darwin.h include/ios/

cpfiles()
{
	for file in $1/*
	do
		if [ -d $file ]; then
			cpfiles $file
		else
			fileext=${file##*.}
			if [ "$fileext" == "h" ]; then
				cp $file include/ios/`basename $file`
			fi
		fi
	done
}

cpfiles src/application
cpfiles src/base
cpfiles src/common
cpfiles src/core
cpfiles src/glhelpers
cpfiles src/gui
cpfiles src/jsscripting/jswrappers
cpfiles src/network
cpfiles src/textures
cpfiles src/ios
