@echo off
REM ═══════════════════════════════════════════════════════════════════════════════
REM SOULSLIKE FRAMEWORK - TEST RUNNER
REM ═══════════════════════════════════════════════════════════════════════════════
REM
REM Usage:
REM   run_tests.bat           - Run all tests
REM   run_tests.bat quick     - Run critical tests only
REM   run_tests.bat core      - Run core system tests
REM   run_tests.bat gameplay  - Run gameplay system tests
REM   run_tests.bat ui        - Run UI widget tests
REM   run_tests.bat ai        - Run AI system tests
REM   run_tests.bat world     - Run world actor tests
REM   run_tests.bat utility   - Run utility system tests
REM   run_tests.bat animation - Run animation system tests
REM

setlocal EnableDelayedExpansion

set UE_PATH=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set PROJECT=C:\scripts\SLFConversion\SLFConversion.uproject
set TEST_SCRIPT=C:\scripts\SLFConversion\tests\run_all_tests.py

echo.
echo ╔══════════════════════════════════════════════════════════════════════╗
echo ║              SOULSLIKE FRAMEWORK TEST RUNNER                         ║
echo ╚══════════════════════════════════════════════════════════════════════╝
echo.

if "%1"=="" (
    echo Running: FULL TEST SUITE
    set ARGS=
) else (
    echo Running: %1 tests
    set ARGS=%1
)

echo.
echo Executing tests...
echo.

"%UE_PATH%" "%PROJECT%" -run=pythonscript -script="%TEST_SCRIPT%" -ScriptArgs="%ARGS%" -stdout -unattended -nosplash 2>&1

echo.
echo Test run complete.
echo.

endlocal
