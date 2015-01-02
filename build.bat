@echo off
set TARGET=%1
if "%TARGET%"=="" ( set TARGET=WINDOWS )
set BUILD="build"
::if exist %BUILD% rm -R %BUILD%
mkdir %BUILD%
cd %BUILD%
if "%TARGET%"=="ANDROID" (
cmake .. -G"NMake Makefiles" -DBUILD_ANDROID=ON
) else (
call vcvars32
cmake .. -G"NMake Makefiles" -DBUILD_ANDROID=OFF
)
nmake install

cd ..