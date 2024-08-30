# mingw-w64-toolchain.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Target architecture (can be changed to i686 for 32-bit)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/local/Cellar/mingw-w64/)

set(CMAKE_AR /usr/local/bin/x86_64-w64-mingw32-ar)
set(CMAKE_RANLIB /usr/local/bin/x86_64-w64-mingw32-ranlib)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# For libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Search for the default paths on the build host (e.g., for CMake modules)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
