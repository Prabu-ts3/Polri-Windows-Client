set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Tentukan path cross-compiler (sesuaikan dengan lokasi instalasi mingw-w64 Anda)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# PENTING: Arahkan ke lokasi Qt6 Windows Anda
# Contoh: set(QT_PATH "/Users/prabu/Qt/6.x.x/mingw_64")
# set(CMAKE_PREFIX_PATH ${QT_PATH})
