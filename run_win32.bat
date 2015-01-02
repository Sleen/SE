@echo off
set BUILD="build"
set SE_HOME=%~dp0
::if exist %BUILD% rm -R %BUILD%
if not exist %BUILD% mkdir %BUILD%
cd %BUILD%
echo ���û�������..
call vcvars32
echo ����Makefile..
cmake .. -G"NMake Makefiles" -DBUILD_ANDROID=OFF
if errorlevel 1 (
pause
goto end
)
echo Build..
nmake CFG="Release"
if errorlevel 1 (
pause
goto end
)
echo ������Դ�ļ�..
xcopy /D /E /Y %SE_HOME%\res %SE_HOME%\bin\test\res\
cd %SE_HOME%\bin\test
SE_TEST
:end
cd %SE_HOME%