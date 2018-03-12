# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2018
# All rights reserved

# Configuration

COPY=copy /b /d
REGSVR32=regsvr32
DONE=$(OBJECT_ROOT)\done\$(TARGET_CPU)
SAY=@echo =====

INF_SOURCE=kx
BLD_INF_FNAME=kx
BLD_CAT_FNAME=kx

HTTPD_ROOT=..\www
C_DEFINES=$(C_DEFINES) -DKX_DEBUG 
SETUP_SETUP_FILE=kxdrv$(KX_VERSION)-full.exe
KXSDK_ROOT=$(DONE)\..\SDK

# generate map files:
LINKER_FLAGS=$(LINKER_FLAGS) /MAP /MAPINFO:EXPORTS

# don't ignore error codes
!CMDSWITCHES -I

# common targets:

$(DONE)\$(TARGETNAME).$(TARGETEXT): create_folders $(OBJ_PATH)\$(O)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installing into $(DONE)...
	$(COPY) $(OBJ_PATH)\$(O)\$(TARGETNAME).$(TARGETEXT) $(SKINFILE) $(DONE)\$(TARGETNAME).$(TARGETEXT)

install: $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installed into $(DONE)

$(DONE)\$(TARGETNAME).pdb: create_folders $(OBJ_PATH)\$(O)\$(TARGETNAME).pdb
	$(SAY) Installing PDB into $(DONE)...
	$(COPY) $(OBJ_PATH)\$(O)\$(TARGETNAME).pdb $(DONE)\$(TARGETNAME).pdb

pdb: $(DONE)\$(TARGETNAME).pdb
	$(SAY) PDB copied into $(DONE)

$(DONE):
	$(SAY) Creating $(DONE) folder
	if not exist $(DONE) mkdir $(DONE)

create_folders: $(DONE)

# INF file generation

inf: $(DONE)\$(BLD_INF_FNAME).inf
	$(SAY) Inf file generated

$(DONE)\$(BLD_INF_FNAME).inf: ..\inf\$(INF_SOURCE).cpp create_folders
	$(SAY) Generating .INF file..
	cl -nologo -I..\h -EP ..\inf\$(INF_SOURCE).cpp > $(DONE)\$(BLD_INF_FNAME).inf

# generating NSIS setup

!if "$(TARGET_ARCH)" == "amd64"
distr: help readme inf sdk $(DONE)\$(SETUP_SETUP_FILE)
!else
distr: help readme inf sdk
	$(SAY) x86 build does not create NSIS installer
!endif

$(DONE)\kx-full.nsi: ../nsis/kx-full.cpp
	$(SAY) Precompiling NSIS script
	cl -nologo -I..\h -EP ../nsis/kx-full.cpp -DKXSDK_ROOT="$(KXSDK_ROOT)" -DCE_DONE="$(DONE)" -DKX_HTTPD_ROOT="$(HTTPD_ROOT)" -DKX_SYSTEM_ROOT="$(WINDIR)\system32" > $(DONE)\kx-full.nsi

$(DONE)\$(SETUP_SETUP_FILE): $(DONE)\kx-full.nsi $(DONE)\$(BLD_INF_FNAME).inf $(DONE)\kx.sys $(DONE)\..\i386\kx.sys create_folders
	$(SAY) Signing all binaries....
	$(SIGNTOOL) $(DONE)\cmax20.dll $(DONE)\edspctrl.exe $(DONE)\kx.sys $(DONE)\kxaddons.kxa \
				$(DONE)\kxapi.dll $(DONE)\kxasio.dll $(DONE)\kxctrl.exe $(DONE)\kxdynamica.kxl \
				$(DONE)\kxefx.dll $(DONE)\kxefxreverb.kxl $(DONE)\kxefxtube.kxl $(DONE)\kxfxadc.kxl \
				$(DONE)\kxfxlib.kxl $(DONE)\kxfxloudness.kxl $(DONE)\kxfxmixy42.kxl $(DONE)\kxfxmixy82.kxl \
				$(DONE)\kxfxpack.kxl $(DONE)\kxfxrouter.kxl $(DONE)\kxgui.dll $(DONE)\kXi.dll \
				$(DONE)\kxm120.kxl $(DONE)\kxmixer.exe $(DONE)\kxsetup.exe $(DONE)\kXSFi.dll $(DONE)\nccg.exe $(DONE)\sfman32.dll \
				$(DONE)\..\i386\cmax20.dll $(DONE)\..\i386\edspctrl.exe $(DONE)\..\i386\kx.sys $(DONE)\..\i386\kxaddons.kxa \
				$(DONE)\..\i386\kxapi.dll $(DONE)\..\i386\kxasio.dll $(DONE)\..\i386\kxctrl.exe $(DONE)\..\i386\kxdynamica.kxl \
				$(DONE)\..\i386\kxefx.dll $(DONE)\..\i386\kxefxreverb.kxl $(DONE)\..\i386\kxefxtube.kxl $(DONE)\..\i386\kxfxadc.kxl \
				$(DONE)\..\i386\kxfxlib.kxl $(DONE)\..\i386\kxfxloudness.kxl $(DONE)\..\i386\kxfxmixy42.kxl $(DONE)\..\i386\kxfxmixy82.kxl \
				$(DONE)\..\i386\kxfxpack.kxl $(DONE)\..\i386\kxfxrouter.kxl $(DONE)\..\i386\kxgui.dll $(DONE)\..\i386\kXi.dll \
				$(DONE)\..\i386\kxm120.kxl $(DONE)\..\i386\kxmixer.exe $(DONE)\..\i386\kxsetup.exe $(DONE)\..\i386\kXSFi.dll \
				$(DONE)\..\i386\nccg.exe $(DONE)\..\i386\sfman32.dll
	$(SAY) !!! FIXME NOW - need to submit kx.sys+kx.inf to Microsoft here
	$(SAY) (re)Generating .CAT files
	$(INF2CAT) /driver:$(DONE) /os:$(TARGET_INF_OS_64)
	$(INF2CAT) /driver:$(DONE)\..\i386 /os:$(TARGET_INF_OS)
	$(SIGNTOOL) $(DONE)\kx.cat $(DONE)\..\i386\kx.cat
	$(SAY) Generating NSIS installer
	$(NSIS_ROOT)\makensis.exe /NOCD $(DONE)\kx-full.nsi > $(DONE)\kx-full.log
	$(SAY) Signing NSIS installer
	$(SIGNTOOL) $(DONE)\$(SETUP_SETUP_FILE)

help: $(DONE)\kxhelp.chm
	$(SAY) Help file is not re-compiled, re-compile manually from \www\help folder

$(DONE)\kxhelp.chm: $(HTTPD_ROOT)\help\kxhelp.chm
	$(SAY) Installing help file...
	$(COPY) $(HTTPD_ROOT)\help\kxhelp.chm $(DONE)\kxhelp.chm>nul

readme: $(DONE)\readme.html

$(DONE)\readme.html: ..\texts\readme.cpp
	$(SAY) Creating readme file
	cl /EP -I$(HTTPD_ROOT) -nologo ..\texts\readme.cpp > $(DONE)\readme.html

sdk:
    if not exist $(KXSDK_ROOT) mkdir $(KXSDK_ROOT)
# create $(KXSDK_ROOT)\lib\$(TARGET_CPU)
    if not exist $(KXSDK_ROOT)\lib mkdir $(KXSDK_ROOT)\lib
    if not exist $(KXSDK_ROOT)\lib\$(TARGET_CPU) mkdir $(KXSDK_ROOT)\lib\$(TARGET_CPU)

    $(COPY) $(OBJ_PATH)\..\kxapi\$O\kxapi.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 
    $(COPY) $(OBJ_PATH)\..\kxgui\$O\kxgui.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 
    $(COPY) $(OBJ_PATH)\..\kxmixer\$O\kxmixer.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 

# create \h
    if not exist $(KXSDK_ROOT)\h mkdir $(KXSDK_ROOT)\h
    if not exist $(KXSDK_ROOT)\h\hw mkdir $(KXSDK_ROOT)\h\hw
    if not exist $(KXSDK_ROOT)\h\interface mkdir $(KXSDK_ROOT)\h\interface
    if not exist $(KXSDK_ROOT)\h\dane mkdir $(KXSDK_ROOT)\h\dane

    $(COPY) ..\h\interface\*.* $(KXSDK_ROOT)\h\interface 
    $(COPY) ..\h\dane\*.* $(KXSDK_ROOT)\h\dane 
    $(COPY) ..\h\vers.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\defs.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\debug.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\i386.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\hw\8010.h $(KXSDK_ROOT)\h\hw\8010.h 
    $(COPY) ..\h\hw\ac97.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\ecard.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\emu.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\p16v.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\p17v.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\tina.h $(KXSDK_ROOT)\h\hw 

    if not exist $(KXSDK_ROOT)\h\gui mkdir $(KXSDK_ROOT)\h\gui
    $(COPY) ..\h\gui\*.* $(KXSDK_ROOT)\h\gui > nul
    if not exist $(KXSDK_ROOT)\h\gui\efx mkdir $(KXSDK_ROOT)\h\gui\efx
    $(COPY) ..\h\gui\efx\*.* $(KXSDK_ROOT)\h\gui\efx > nul

    if not exist $(KXSDK_ROOT)\kxfxlib mkdir $(KXSDK_ROOT)\kxfxlib
    $(COPY) ..\kxfxlib\*.* $(KXSDK_ROOT)\kxfxlib 

    if not exist $(KXSDK_ROOT)\kxfx_pack mkdir $(KXSDK_ROOT)\kxfx_pack
    $(COPY) ..\kxfx_pack\*.* $(KXSDK_ROOT)\kxfx_pack 

    if not exist $(KXSDK_ROOT)\kxfx_kxm120 mkdir $(KXSDK_ROOT)\kxfx_kxm120
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\presets mkdir $(KXSDK_ROOT)\kxfx_kxm120\presets
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\settings mkdir $(KXSDK_ROOT)\kxfx_kxm120\settings
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\da mkdir $(KXSDK_ROOT)\kxfx_kxm120\da
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\docs mkdir $(KXSDK_ROOT)\kxfx_kxm120\docs
    $(COPY) ..\kxfx_kxm120\*.* $(KXSDK_ROOT)\kxfx_kxm120 
    $(COPY) ..\kxfx_kxm120\presets\*.* $(KXSDK_ROOT)\kxfx_kxm120\presets 
    $(COPY) ..\kxfx_kxm120\da\*.* $(KXSDK_ROOT)\kxfx_kxm120\da 
    $(COPY) ..\kxfx_kxm120\settings\*.* $(KXSDK_ROOT)\kxfx_kxm120\settings 
    $(COPY) ..\kxfx_kxm120\docs\*.* $(KXSDK_ROOT)\kxfx_kxm120\docs 

    if not exist $(KXSDK_ROOT)\fx_demo mkdir $(KXSDK_ROOT)\fx_demo
    $(COPY) ..\kxfxlib\da_demo.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\demo.* $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\debug.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\fxlib.h $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\fxlib.rc $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\simple.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\stdafx.* $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\*.inc $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\demo.da $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\makefile.demo $(KXSDK_ROOT)\fx_demo\makefile > nul

    if not exist $(KXSDK_ROOT)\kxfx_dynamica mkdir $(KXSDK_ROOT)\kxfx_dynamica
    $(COPY) ..\kxfx_dynamica\*.* $(KXSDK_ROOT)\kxfx_dynamica 

    if not exist $(KXSDK_ROOT)\kxfx_efx_tube mkdir $(KXSDK_ROOT)\kxfx_efx_tube
    $(COPY) ..\kxfx_efx_tube\*.* $(KXSDK_ROOT)\kxfx_efx_tube 

    if not exist $(KXSDK_ROOT)\kxfx_efx_reverb mkdir $(KXSDK_ROOT)\kxfx_efx_reverb
    $(COPY) ..\kxfx_efx_reverb\*.* $(KXSDK_ROOT)\kxfx_efx_reverb

    if not exist $(KXSDK_ROOT)\kxfx_efx_library mkdir $(KXSDK_ROOT)\kxfx_efx_library
    $(COPY) ..\kxfx_efx_library\*.* $(KXSDK_ROOT)\kxfx_efx_library

    if not exist $(KXSDK_ROOT)\kxaddons mkdir $(KXSDK_ROOT)\kxaddons
    $(COPY) ..\kxaddons\*.* $(KXSDK_ROOT)\kxaddons 

    if not exist $(KXSDK_ROOT)\sample_addon mkdir $(KXSDK_ROOT)\sample_addon
    $(COPY) ..\sample_addon\*.* $(KXSDK_ROOT)\sample_addon 

    if not exist $(KXSDK_ROOT)\kxapi mkdir $(KXSDK_ROOT)\kxapi
    $(COPY) ..\kxapi\*.* $(KXSDK_ROOT)\kxapi > nul

    if not exist $(KXSDK_ROOT)\kxmixer mkdir $(KXSDK_ROOT)\kxmixer
    $(COPY) ..\kxmixer\addon.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\addonmgr.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\addonmgr.h    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\manager.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\plugin.cpp    $(KXSDK_ROOT)\kxmixer > nul

    if not exist $(KXSDK_ROOT)\fx_skin mkdir $(KXSDK_ROOT)\fx_skin
    $(COPY) ..\kxskin\plugin\*.* $(KXSDK_ROOT)\fx_skin 

    if not exist $(KXSDK_ROOT)\da mkdir $(KXSDK_ROOT)\da
    $(COPY) ..\da\*.* $(KXSDK_ROOT)\da

    if not exist $(KXSDK_ROOT)\kxctrl mkdir $(KXSDK_ROOT)\kxctrl
    $(COPY) ..\kxctrl\*.* $(KXSDK_ROOT)\kxctrl 

    if not exist $(KXSDK_ROOT)\kxsfman mkdir $(KXSDK_ROOT)\kxsfman
    $(COPY) ..\kxsfman\*.* $(KXSDK_ROOT)\kxsfman 

    if not exist $(KXSDK_ROOT)\edspctrl mkdir $(KXSDK_ROOT)\edspctrl
    $(COPY) ..\edspctrl\*.* $(KXSDK_ROOT)\edspctrl 

# Mac OS X sources

    if not exist $(KXSDK_ROOT)\macosx mkdir $(KXSDK_ROOT)\macosx
    $(COPY) ..\macosx\*. $(KXSDK_ROOT)\macosx > nul
    xcopy /E /R /H /Y /I /G ..\macosx\package $(KXSDK_ROOT)\macosx\package > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kxmanager $(KXSDK_ROOT)\macosx\kxmanager > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kxac97 $(KXSDK_ROOT)\macosx\kxac97 > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kext $(KXSDK_ROOT)\macosx\kext > nul
    xcopy /E /R /H /Y /I /G ..\macosx\"10kx driver.xcodeproj" $(KXSDK_ROOT)\macosx\"10kx driver.xcodeproj" > nul
    xcopy /E /R /H /Y /I /G ..\macosx\pcmblitterlib $(KXSDK_ROOT)\macosx\pcmblitterlib > nul

# licenses
    $(COPY) $(HTTPD_ROOT)\help\license3.txt $(KXSDK_ROOT)\copying 
    $(COPY) ..\texts\kxapi.txt $(KXSDK_ROOT)\readme.txt > nul

