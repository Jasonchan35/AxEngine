@REM #change current directory to this file
@%~d0
@cd %~dp0

cmake -G "Visual Studio 17 2022" ^
	-T "ClangCL" ^
	-B _build/AxEngine-x64-windows-ClangCL ^
	.

@pause