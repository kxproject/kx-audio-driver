# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2012
# All rights reserved

!include "../makefile.inc"

kxefx_skin: $(DONE)\kxefx.kxs

$(DONE)\kxefx.kxs:
	pkzip25 -add $(DONE)\kxefx.kxs -nozipextension *.* -exclude *.bat
	$(SAY) Generated kxefx.kxs...
