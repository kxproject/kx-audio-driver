# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2014
# All rights reserved

!include "../makefile.inc"

clean:
	if exist *.exe (
	del /Q /F *.exe
	)
	if exist *.log (
	del /Q /F *.log
	)
	if exist *.nsi (
	del /Q /F *.nsi
	)
