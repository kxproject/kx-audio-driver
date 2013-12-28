# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2012
# All rights reserved

!include "../makefile.inc"

skin: $(DONE)\kxskin.kxs update_inis

$(DONE)\kxskin.kxs: inis\lang.ini $(OBJ_PATH)\$(SKIN).kxs $(OBJ_PATH)\plugin.kxs
	$(SAY) Installing skin file...
	$(COPY) $(OBJ_PATH)\$(SKIN).kxs $(DONE)\kxskin.kxs

inis\lang.ini: $(OBJ_PATH)\$(O)\parser.exe english mix inp
	$(SAY) Compiling languages...
    $(OBJ_PATH)\$(O)\parser.exe inis\lang.ini

$(OBJ_PATH)\$(SKIN).kxs:
	cd $(SKIN)
	$(SAY) Zipping $(SKIN).kxs...
	pkzip25 -add -nozipextension -directories $(OBJ_PATH)\$(SKIN).kxs
	cd ..

$(OBJ_PATH)\plugin.kxs:
	cd plugin
	$(SAY) Zipping pluging.kxs...
	pkzip25 -add -nozipextension -directories $(OBJ_PATH)\plugin.kxs
	cd ..

update_inis:
	cd inis
	$(SAY) Updating INI files...
	pkzip25 -add -nozipextension -directories $(DONE)\kxskin.kxs *.ini
	cd ..

clean:
	del $(OBJ_PATH)\plugin.kxs
	del $(OBJ_PATH)\$(SKIN).kxs
	del $(DONE)\kxskin.kxs
