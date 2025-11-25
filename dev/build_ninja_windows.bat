@REM #change current directory to this file
@%~d0
@cd %~dp0

set build_dir=_build/ninja-windows

cmake ^
    -G "Ninja" ^
	-DCMAKE_MAKE_PROGRAM="W:\Jason\github\AxEngine\dev\ninja.exe" ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_CXX_COMPILER=clang++ ^
	-B %build_dir% ^
	.

cmake --build %build_dir%