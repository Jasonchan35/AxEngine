@REM #change current directory to this file
%~d0
cd %~dp0

_build\vs2022-x64-windows\bin\Debug\AxCppHeaderTool -outPath=AxEngine/AxEngine/src AxEngine/AxEngine/src

@pause 