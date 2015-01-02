@echo off

echo ===== build minizip..
cd minizip\build
call build
cd ..\..

echo ===== build png..
cd png\build
call build
cd ..\..

echo ===== build freetype2..
cd freetype2\build
call build
cd ..\..

pause