#!/bin/bash

# Folder tujuan build
BUILD_DIR="build_windows"
# Mencari folder mingw_64 secara otomatis di folder ~/Qt
QT_MINGW_PATH=$(find /Users/prabu/Qt -name "mingw_64" -type d -maxdepth 3 | head -n 1)

echo "--- Starting Windows Build (Cross-Compile) ---"

if [ -z "$QT_MINGW_PATH" ] || [ ! -d "$QT_MINGW_PATH" ]; then
    echo "ERROR: Qt MinGW SDK for Windows not found!"
    echo "Please install 'MinGW 64-bit' component via Qt Online Installer."
    echo "Expected location: ~/Qt/6.x.x/mingw_64"
    exit 1
fi

echo "Using Qt SDK at: $QT_MINGW_PATH"

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Jalankan CMake dengan Toolchain
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake \
    -DCMAKE_PREFIX_PATH=$QT_MINGW_PATH \
    -DCMAKE_BUILD_TYPE=Release

# Compile
make -j$(sysctl -n hw.ncpu)

echo "--- Build Finished ---"
echo "Executable can be found in: $BUILD_DIR/PolriWindowsClient.exe"
