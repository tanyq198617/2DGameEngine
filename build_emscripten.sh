#!/bin/bash

mkdir -p build_emscripten
cd build_emscripten
cmake -DCMAKE_TOOLCHAIN_FILE="$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake" -DEMSCRIPTEN_GENERATE_BITCODE_STATIC_LIBRARIES=1 -DCMAKE_BUILD_TYPE=Debug  ..
make -j4
