@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0stage-usb-release.ps1" %*
exit /b %ERRORLEVEL%
