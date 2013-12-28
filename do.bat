@echo off
rem build script
rem Copyright (c) Eugene Gavrilov. All rights reserved

if NOT exist .\dirs (
	if NOT exist .\sources (
		if NOT exist .\makefile (
	echo do.bat: This folder contains no build scripts, makefiles or subfolder lists
	exit 1 )))

if NOT exist .\sources (
	if NOT exist .\dirs (
		rem this is just a 'makefile' folder
		nmake /nologo %1 %2 %3 %4 %5 %6 %7 %8 %9
		goto END ))

%BUILD% %1 %2 %3 %4 %5 %6 %7 %8 %9

:END
rem Done
