@REM #change current directory to this file
%~d0
cd %~dp0

set HeaderTool=_build\vs2022-x64-windows\bin\Debug\AxCppHeaderTool

%HeaderTool% -moduleName=AxEngine -outPath=AxEngine/AxEngine/src AxEngine/AxEngine/src
%HeaderTool% -moduleName=AxEditor -outPath=AxEngine/AxEditor/src AxEngine/AxEditor/src


@pause 