@REM #change current directory to this file
@%~d0
@cd %~dp0

@REM @echo off

cls

set projectRoot=..\..\..
set outputDir=%projectRoot%\deploy\ImportedAssets\Shaders

set ninja=%projectRoot%\dev\ninja.exe
set vulkan_sdk_bin=%VK_SDK_PATH%
set slang_sdk=%projectRoot%\dev\external\slang\slang-2026.1-windows-x86_64
set windows_sdk_bin=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64

set AxIncludeDir=include
set AxEngine_bin=%projectRoot%\dev\_build\vs2022-x64-windows\bin\Debug
set AxShaderTool=%AxEngine_bin%\AxShaderTool.exe

@rem jobCount=0 for parallel build
set jobCount=0
set quiet=1

%AxShaderTool% 	-genNinja -file=src -quiet=%quiet% -job=%jobCount% -out="%outputDir%"


@IF NOT %ERRORLEVEL% EQU 0 GOTO :error_handler

@set CLICOLOR_FORCE=1
@rem %ninja% --quiet -C "%outputDir%"
%ninja% --quiet -j %jobCount% -k 1 -C "%outputDir%"


@REM --- If everything succeeded, exit gracefully ---
@GOTO :eof

:error_handler
@ECHO ********************************************************
@ECHO * ERROR ! *
@ECHO ********************************************************
@ECHO.
@ECHO The program failed with Exit Code: %ERRORLEVEL%
@ECHO.
@PAUSE
EXIT /B %ERRORLEVEL%

REM --- End of file tag (exits the script) ---
:eof

@REM @pause