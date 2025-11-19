@REM #change current directory to this file
%~d0
cd %~dp0

set triplets=x64-windows

git clone https://github.com/microsoft/vcpkg.git _vcpkg

call _vcpkg\bootstrap-vcpkg.bat

_vcpkg\vcpkg install vcpkg-tool-ninja
_vcpkg\vcpkg install libpng:%triplets% --recurse
_vcpkg\vcpkg install freetype:%triplets% --recurse
_vcpkg\vcpkg install imgui:%triplets% --recurse
@rem _vcpkg\vcpkg install directxtex:%triplets% --recurse

@pause