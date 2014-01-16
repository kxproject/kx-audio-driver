
rem. additional tools:
set NSIS_CMD=E:\Tools\NSIS\makensis

if %BUILD_TYPE%.==CHK. set NSIS_CMD=%NSIS_CMD% /DDBG
%NSIS_CMD% /V2 main.nsi
@IF ERRORLEVEL 1 PAUSE