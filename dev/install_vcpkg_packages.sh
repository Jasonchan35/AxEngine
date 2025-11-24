#change current directory to this file
SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
    cd $SCRIPT_PATH
fi

git clone https://github.com/microsoft/vcpkg.git _vcpkg

# Mac
if [ "$(uname)" == "Darwin" ]; then
    brew install pkg-config
    brew install cmake
fi

sh _vcpkg/bootstrap-vcpkg.sh

_vcpkg/vcpkg install vcpkg-tool-ninja
# _vcpkg/vcpkg install tracy --recurse
# _vcpkg/vcpkg install directx-dxc --recurse
# _vcpkg/vcpkg install mozjpeg --recurse
# _vcpkg/vcpkg install libpng --recurse
# _vcpkg/vcpkg install freetype --recurse

# _vcpkg/vcpkg install vulkan --recurse


