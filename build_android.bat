@echo off

set path=%PATH%;E:\Android\android-sdk\tools;H:\工具及其它\apache-ant-1.9.2\bin;
set JAVA_HOME=C:\Program Files\Java\jdk1.7.0_45
set NDK=E:\android-ndk-r9b
set SE_HOME=%~dp0
set BUILD_DIR=%cd%\build_android
set NDK_TOOLCHAIN_VERSION=4.8

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo 复制文件...
xcopy /D /E /Y %SE_HOME%\src jni\
if errorlevel 1 goto end
xcopy /D /E /Y %SE_HOME%\res assets\
if errorlevel 1 goto end
xcopy /D /E /Y %SE_HOME%\src\platform\android .\
if errorlevel 1 goto end

echo 创建项目...
call android update project --path . --target android-14
if errorlevel 1 goto end

echo 编译.so库...
call %NDK%\ndk-build
if errorlevel 1 goto end

echo 打包apk...
call ant debug > nul
if errorlevel 1 goto end

echo 清理...
::del bin\*.d > nul
if not exist ..\bin mkdir ..\bin
xcopy /D /Y bin\*.apk ..\bin\

echo build成功...
adb devices | findstr "\<device\>" > nul
if errorlevel 1 goto end2

echo 安装apk...
adb install -r bin\*debug.apk
if errorlevel 1 goto end

echo 启动...
adb shell am start com.example.native_activity/android.app.NativeActivity
if errorlevel 1 goto end
goto end2

:end
echo 失败！ [ ERRORLEVEL: %errorlevel% ]
pause

:end2
cd ..