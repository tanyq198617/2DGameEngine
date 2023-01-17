#!/bin/bash

rm -rf include/mac
mkdir -p include/mac

cp src/GameEngineConfig.h include/mac/

cpfiles()
{
	for file in $1/*
	do
		if [ -d $file ]; then
			cpfiles $file
		else
			fileext=${file##*.}
			if [ "$fileext" == "h" ] || [ "$fileext" == "hpp" ]; then
				cp $file include/mac/`basename $file`
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
cpfiles src/jsscripting
cpfiles src/network
cpfiles src/textures
cpfiles src/mac
