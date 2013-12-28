@echo off

rem increment driver version

set KX_BUILD_ROOT=%~dp0\..
set HTTPD_ROOT=%KX_BUILD_ROOT%\www

genvers.exe
