@echo off
REM SLF Combat System Automation Tests
REM Runs all SLF.* tests without opening the editor GUI

echo ============================================================
echo            SLF COMBAT SYSTEM AUTOMATION TESTS
echo ============================================================
echo.

set UE_CMD="C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set PROJECT="C:\scripts\SLFConversion\SLFConversion.uproject"

echo Running tests...
echo.

REM Run all SLF tests
%UE_CMD% %PROJECT% -ExecCmds="Automation RunTests SLF" -unattended -nopause -nosplash -stdout -NullRHI 2>&1

echo.
echo ============================================================
echo                     TESTS COMPLETE
echo ============================================================

pause
