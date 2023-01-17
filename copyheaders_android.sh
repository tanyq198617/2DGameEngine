#!/bin/bash

rm -rf include/android
mkdir -p include/android

cp src/GameEngineConfig.h include/android/
cp src/jsscripting/JSComponent.h include/android/
cp src/jsscripting/JSScriptingCore.h include/android/
cp src/android/HLApkTools.h include/android/

cpfiles()
{
	for file in $1/*
	do
		if [ -d $file ]; then
			cpfiles $file
		else
			fileext=${file##*.}
			if [ "$fileext" == "h" ]; then
				cp $file include/android/`basename $file`
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
cpfiles src/android/jni
