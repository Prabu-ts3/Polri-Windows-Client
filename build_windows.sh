#!/bin/bash

# Folder tujuan build
BUILD_DIR="build_windows"
QT_MINGW_PATH="/Users/prabu/Qt/6.x.x/mingw_64" # GANTI KE PATH QT WINDOWS ANDA DI MAC

echo "--- Starting Windows Build (Cross-Compile) ---"

if [ ! -d "$QT_MINGW_PATH" ]; then
    echo "ERROR: Qt MinGW path not found at $QT_MINGW_PATH"
    echo "Please update build_windows.sh with your actual Qt Windows SDK path on this Mac."
    exit 1
fi

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
