@echo off

rem Copyright (c) Eugene Gavrilov. 2003-2014. All rights reserved

rem debug or release build:
set BUILD_TYPE=FRE
rem set BUILD_TYPE=CHK

if not exist %TEMP%\Build mkdir %TEMP%\Build

rem re-set current folder if called as administrator
cd /d %~dp0

if %BUILD_TYPE%.==. set BUILD_TYPE=FRE
rem timestamp signed executables (slow, since requires internet connection)
if %BUILD_TYPE%.==FRE. set TIMESTAMP_SIGN=YES

echo ===================================================
echo            Generate x86 and x64 drivers
echo ===================================================

echo.
echo ===================================================
echo   x86 %BUILD_TYPE% build
echo ===================================================
echo.

cmd /c set_env.bat do.bat %1 %2 %3 %4 %5 %6 %7 %8 %9

pause

echo.
echo ===================================================
echo   x64 %BUILD_TYPE% build
echo ===================================================
echo.

cmd /c set_env.bat 64 do.bat %1 %2 %3 %4 %5 %6 %7 %8 %9

pause
