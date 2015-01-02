@echo off

set path=%PATH%;E:\Android\android-sdk\tools;H:\���߼�����\apache-ant-1.9.2\bin;
set JAVA_HOME=C:\Program Files\Java\jdk1.7.0_45
set NDK=E:\android-ndk-r9b
set SE_HOME=%~dp0
set BUILD_DIR=%cd%\build_android
set NDK_TOOLCHAIN_VERSION=4.8

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo �����ļ�...
xcopy /D /E /Y %SE_HOME%\src jni\
if errorlevel 1 goto end
xcopy /D /E /Y %SE_HOME%\res assets\
if errorlevel 1 goto end
xcopy /D /E /Y %SE_HOME%\src\platform\android .\
if errorlevel 1 goto end

echo ������Ŀ...
call android update project --path . --target android-14
if errorlevel 1 goto end

echo ����.so��...
call %NDK%\ndk-build
if errorlevel 1 goto end

echo ���apk...
call ant debug > nul
if errorlevel 1 goto end

echo ����...
::del bin\*.d > nul
if not exist ..\bin mkdir ..\bin
xcopy /D /Y bin\*.apk ..\bin\

echo build�ɹ�...
adb devices | findstr "\<device\>" > nul
if errorlevel 1 goto end2

echo ��װapk...
adb install -r bin\*debug.apk
if errorlevel 1 goto end

echo ����...
adb shell am start com.example.native_activity/android.app.NativeActivity
if errorlevel 1 goto end
goto end2

:end
echo ʧ�ܣ� [ ERRORLEVEL: %errorlevel% ]
pause

:end2
cd ..