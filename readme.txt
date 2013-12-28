===================================

Welcome to kX Audio Driver sources.

Review official FAQ on http://www.kxproject.com
for details on this project and its goals.

Mac OS X sources are included, but 
without any build instructions. Use 
XCode and supplied project in 
\macosx folder.

===================================


Build instructions for Windows:


==== Preparing for build

0. Download all driver sources and install them.

Preferred file location:

e.g. d:\audio\10kx

==== DDK and tools installation

1. Install Windows Driver Kit version 7.1.0

Download en_windows_driver_kit_version_7.1.0_x86_x64_ia64_dvd_496758.ISO from Microsoft.
Preferred install location: d:\audio\NTDDK or similar, without spaces or any non-English folder names, on non-system disk.

Update DDK files by using included \ddk_fix folder.
(DDK's Setenv.bat file has been edited to better handle AMD64 targets, Default DDK is missing one .H file and one .LIB file).

Edit \set_env.bat. Find and choose correct DDK root folder:

e.g. set DDKROOT=d:\audio\ntddk

2. Install NSIS from nsis.sourceforge.net. Tested version: 2.46

Preferred install location: d:\audio\tools\nsis

Find in \set_env.bat and choose correct NSIS root folder:

e.g. set NSIS_ROOT=d:\audio\tools\nsis

3. Install Microsoft Help Compiler.

Find in \set_env.bat and choose correct Help Compiler root folder:

e.g. set HHC_ROOT=d:\audio\tools\hhc

4. Install various utilities into a folder and configure \set_env.bat accordingly:

e.g. set UTIL_ROOT=d:\audio\tools\util

Place there command-line versions of WinRar, pkzip25. If they are installed elsewhere, set UTIL_ROOT to that path.
You may also need 'sync' utility from SysInternals.

==== Building the driver

Execute \genall.bat

If code-signing is required, \genall.bat needs to run as Administrator. This is a limitation of Microsoft CodeSign.exe.
If executed under regular user account, CodeSign will fail.

Options found in \genall.bat:

set BUILD_TYPE=FRE

- chooses 'Release' build, set to 'FRE' for 'Debug' build.

'Release' builds are timestamped using VeriSign servers (if correct code signature is present).
To avoid timestamping, edit \genall.bat and set TIMESTAMP_SIGN=NO

CodeSign certificate is configured via \makefile.inc
Default is: SIGNTOOL=SignTool sign /ac ..\VeriSignG5.cer /s my /n "CEntrance, Inc."
Certificate (or test certificate) needs to be installed into local certificate storage ("Personal").

If no signing is required or possible, edit \makefile2.inc, find 'SIGNTOOL' command and use siple 'echo' instead.

The driver will not boot on x64 (AMD64) Windows without a valid digital signature.
Search the net for a workaround (self-signing, test-signing, bypassing digital signatures).

\genall.bat will call \setenv.bat twice (for 32-bit and 64-bit builds).
\setenv.bat will configure DDK build environment and paths, and will call do.bat for actual build.
\do.bat will either call DDK 'build.exe', or 'nmake.exe' depending on folder content.

All build logs are stored in %TEMP%\Build\{CPU}.

Driver installer (NSIS-based) is only generated for 64-bit build, and includes both 32-bit and 64-bit driver and tools.
\genall.bat generates both 32-bit and 64-bit builds.

==== Build Output

All build output is stored here:

%TEMP%\Build:
  \Done\{CPU}: final executables for specific CPU

%TEMP%\Build\:
  temporary build files

Unified driver installer for both 32-bit and 64-bit architectures (NSIS-based) is located in %TEMP%\Build\Done\amd64\ folder.

==== Out-of-build stuff

\da: contains various DSP effects in DA format.

\ddk_fix: see above

\design: images and icons

\kxskin: full skin for kX Mixer
   \kxskin\english - that's basic English language file;
   					 in order to add new language or update old one, copy updated file over \kxskin\inp
   					 re-build 'parser' and launch it
   					 when you get new language file, place it to \kxskin\mix and clean-up \kxskin\inp
\mybuild: very simple and very legacy versioning tool
	to use:
		cd \mybuild
		nmake
	this will automatically generate various .h files in \h, and also \oem_env.mak
	set KX_BUILD_ROOT to the root of the sources prior to running mybuild
	set HTTPD_ROOT to %KX_BUILD_ROOT%\www
	if executed, \mybuild.exe will automatically increment driver version
	typical usage example is in \mybuild\inc_vers.bat
	version number prior to increment is taken from \h\vers.h

	this tool needs complete re-write

\nccg: see readme.txt there; command-line tool for generating DSP microcode
    This tool is not included with the driver.

\testaudio: special tool for testing bit accuracy, supports ASIO and KS APIs, and kX DSP API 

\MSCV-VSClass3.cer: cross-ceriticate. If your digital certificate uses different cross cerificate, edit \makefile.inc accordingly.

\oem_env.mak:
	is generated automatically by \mybuild
	contains driver version

\spy:
	tool for spying original driver provided by manufacturer
	needs 'giveio.sys'
	supports various modes, which need to be enabled in the source code by updating #if's

\www\help:
	various HTML files and CHM help file are stored on the web site (www.kxproject.com)
	help file is generated manually, outside of the driver build script
	see \www\help\do.bat for details

x86 version of the driver includes binary-only plugins:
	- asynth.kxl
	- drumsynth.kxl
	- organ.kxl
	- ProFx.kxl
    - ufx.kxl and its companion DLL: ufxrc.dll

==== Build issues

On multiprocessor PCs, Microsoft build.exe might fail to synchronize the tasks properly.
For instance, it may fail to digitally sign EXEs or DLLs while they are being compiled in parallel.
The same applies to certain 'COPY' actions while another task is being executed.
Or it may try to create .CAT catalog file by using Inf2Cat before KX.SYS has been produced.
Similar problem causes various targets depending on kxmixer.lib to fail to build on 1st
attempt.

Re-running 'genall.bat' usually fixes all these problems.
It is also possible to disable multithreaded builds by editing BUILD_OPTIONS in \set_env.bat

TODO!

==== Final notes

All source code is Copyright (c) Eugene Gavrilov, 2001-2014, unless specified otherwise.
The code can be re-distributed under GPL v2, unless specified otherwise.

Many thanks to kX Community.

  - Eugene Gavrilov
