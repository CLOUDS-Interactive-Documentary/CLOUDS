@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0stage-local-release.ps1" %*
exit /b %ERRORLEVEL%
