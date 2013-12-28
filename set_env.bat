@echo off

rem Copyright (c) Eugene Gavrilov. 2003-2014. All rights reserved

rem (utils)
rem set to appropriate folder for storing utilities, e.g. final\sdk\bin
set UTIL_ROOT=f:\audio\tools\util
set NSIS_ROOT=f:\audio\tools\nsis
set HHC_ROOT=f:\audio\tools\hhc
set DDKROOT=f:\ntddk


echo --- setting up build environment...

if %1.==64. (
set TARGET_OS=WNet
set TARGET_ARCH=amd64
set TARGET_CPU=amd64
rem note: WinXP+x64 are not supported
rem note: TARGET_ARCH=amd64 is not supported by 'native' DDK setenv.bat, need to add this
shift
)

rem -- user-tweakable defaults

if %BUILD_TYPE%.==. set BUILD_TYPE=CHK
rem can be: CHK or FRE [c / f for win2k]
if %TARGET_OS%.==. set TARGET_OS=wxp
rem (should be wnet; also supported: win7, Wlh, WXP, w2k)
if %TARGET_ARCH%.==. set TARGET_ARCH=x86
rem architecture: folder name for build_output_dir (should be: x86 or amd64) == used for folder suffix
if %TARGET_CPU%.==. set TARGET_CPU=i386
rem cpu: x86, amd64 or ia64 == used for folder name

set OBJECT_ROOT=%TEMP%\Build
rem set OBJECT_ROOT=d:\Build
if not exist %OBJECT_ROOT% mkdir %OBJECT_ROOT%

set BUILD=build

rem (configuration)
set BUILD_UNICODE=NO
set NO_SAFESEH=1

rem -- following settings are not great and might fail
rem -- enable them if you really trust build.exe :)
rem set BUILD_OPTIONS=-M -I -s -g -w
rem -M: multiprocessor build, -I: do not display thread index
rem -s: display status; -g: use colored text; -w: display warnings
if NOT %OBJECT_ROOT%.==. set BUILD_OPTIONS=%BUILD_OPTIONS% -jpath %OBJECT_ROOT%\done\%TARGET_CPU%
if not exist %OBJECT_ROOT%\done\%TARGET_CPU% mkdir %OBJECT_ROOT%\done\%TARGET_CPU%
rem custom temp folder for objects? move logs there, too


rem VST plugins path
set VST_ROOT="c:\Program Files\Steinberg\VSTPlugins"
if not exist %VST_ROOT% (
 set VST_ROOT="c:\Program Files\Steinberg\VSTPlugins"
 if not exist %VST_ROOT% echo !! warning: VST plugins path not found in %VST_ROOT%
 )

rem -- no more user-tweakable parameters....
rem ----------------------------------------
rem ========================================

echo --- DDK build environment

rem -- check the environment
set bad=DDK environment
if not exist %DDKROOT%\bin\setenv.bat goto BAD_ENV
set bad=

set include=
set lib=

rem -- set compiler defines and init the DDK

if %BUILD_UNICODE%.==YES. set C_DEFINES=-DUNICODE -D_UNICODE %C_DEFINES%
pushd %DDKROOT%

goto %BUILD_TYPE%

:FRE

echo --- build: RELEASE, FREE
echo --- %DDKROOT%\bin\setenv.bat %DDKROOT% %TARGET_OS% %TARGET_CPU% %BUILD_TYPE%
call %DDKROOT%\bin\setenv.bat %DDKROOT% %TARGET_OS% %TARGET_CPU% %BUILD_TYPE% no_oacr
popd
goto DOIT

:CHK

echo --- build: DEBUG, CHECKED
echo --- %DDKROOT%\bin\setenv.bat %DDKROOT% %TARGET_OS% %TARGET_CPU% %BUILD_TYPE%
call %DDKROOT%\bin\setenv.bat %DDKROOT% %TARGET_OS% %TARGET_CPU% %BUILD_TYPE% no_oacr
popd
goto DOIT

:DOIT

echo --- configuring environment...

if %BUILD_TYPE%.==FRE. (
	set MSC_OPTIMIZATION=%MSC_OPTIMIZATION% /O2 /Otib2 /GL /DCE_OPTIMIZE
)
rem if %BUILD_TYPE%.==CHK. set USER_C_FLAGS=%USER_C_FLAGS% /RTC1 /RTCs
rem /O2			maximum speed
rem /Ox         maximum optimizations (assumes /Ob2 /Og /Oi /Ot /Oy)
rem /Otiyb2     t: speed i: instric enabled b2: inline expansion level y: frame pointers on stack
rem /GL         link-time code generation
rem === overriden by build.exe ===
rem /Ot			optimize speed (/Os)
rem /Ob2		inline expansion level (/Ob1)
rem === incompatible ===
rem /RTCu		uninitialized local usage checks
rem /RTC1		enable fast checks
rem /RTCs		stack frame runtime checking
rem === deprecated ===
rem /Og         global optimizations
rem /GS-        disable security check (enabled by default)

rem -- 1. path ---------------------
set path=%UTIL_ROOT%;%PATH%

echo --- setting includes...

rem -- include ---------------------
set include=%include%;%DDK_INC_PATH%;%WDM_INC_PATH%;%CRT_INC_PATH%;%MFC_INC_PATH%;%ATL_INC_PATH%\atl30;%OBJECT_ROOT%
rem zmouse.h etc...
set include=%include%;%DDKROOT%\inc\compat

echo --- setting libraries...

rem -- lib -------------------------
rem (this is necessary to allow command-line compilation with cl.exe w/o 'sources' file)
set lib=%lib%;%SDK_LIB_DEST%\%TARGET_CPU%;%DDK_LIB_DEST%\%TARGET_CPU%;%DDKROOT%\lib\crt\%TARGET_CPU%;%DDKROOT%\lib\mfc\%TARGET_CPU%

echo --- environment configured properly
rem set-up custom path for custom build
set path=%UTIL_ROOT%;%path%
if NOT %1.==. cmd /c %1 %2 %3 %4 %5 %6 %7 %8 %9
goto END

:BAD_ENV
echo !! Invalid Build Environment -- %BAD% not found
pause

:END
rem
