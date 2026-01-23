@REM #change current directory to this file
%~d0
cd %~dp0

cmake -G "Visual Studio 17 2022" ^
	-B _build/vs2022-x64-windows ^
	assimp-master

cmake --build _build/vs2022-x64-windows --config Release	

@pause