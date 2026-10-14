@echo off
chcp 65001 >nul
set "SCRIPT=%~dp0run_coremark_1014_oneclick.ps1"
if not exist "%SCRIPT%" (
    echo Missing PowerShell script: "%SCRIPT%"
    pause
    exit /b 1
)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT%" %*
set "RC=%ERRORLEVEL%"
echo.
if "%RC%"=="0" (
    echo CoreMark completed. Results above.
) else (
    echo CoreMark FAILED. Check D:\1014\coremark_quick\logs
)
pause
exit /b %RC%
