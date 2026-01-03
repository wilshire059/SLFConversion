@echo off
REM ═══════════════════════════════════════════════════════════════════════════════
REM SOULSLIKE FRAMEWORK - FULL MIGRATION WORKFLOW
REM ═══════════════════════════════════════════════════════════════════════════════
REM
REM This script runs the complete migration workflow:
REM   1. Restore backup content
REM   2. Extract CDO values from original Blueprints
REM   3. Run migration (clear logic, reparent to C++)
REM   4. Restore CDO values to migrated Blueprints
REM   5. Run tests
REM

setlocal EnableDelayedExpansion

set UE_PATH=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set PROJECT=C:\scripts\SLFConversion\SLFConversion.uproject
set BACKUP_CONTENT=C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content
set PROJECT_CONTENT=C:\scripts\SLFConversion\Content

echo.
echo ╔══════════════════════════════════════════════════════════════════════╗
echo ║           SOULSLIKE FRAMEWORK - FULL MIGRATION WORKFLOW              ║
echo ╚══════════════════════════════════════════════════════════════════════╝
echo.

REM ═══════════════════════════════════════════════════════════════════════════════
REM PHASE 1: Restore backup content
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo PHASE 1: Restoring backup content
echo ═══════════════════════════════════════════════════════════════════════════════
echo.

if exist "%PROJECT_CONTENT%" (
    echo Removing existing Content folder...
    rmdir /S /Q "%PROJECT_CONTENT%"
)

echo Copying backup content...
xcopy "%BACKUP_CONTENT%" "%PROJECT_CONTENT%" /E /I /Q

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to restore backup content
    exit /b 1
)
echo Content restored successfully.

REM ═══════════════════════════════════════════════════════════════════════════════
REM PHASE 2: Extract CDO values from original Blueprints
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo PHASE 2: Extracting CDO values from original Blueprints
echo ═══════════════════════════════════════════════════════════════════════════════
echo.

"%UE_PATH%" "%PROJECT%" -run=pythonscript -script="C:/scripts/SLFConversion/extract_blueprint_values.py" -stdout -unattended -nosplash 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Value extraction may have had issues
)

REM Check if values were extracted
if not exist "C:\scripts\SLFConversion\blueprint_cdo_values.json" (
    echo ERROR: CDO values file was not created
    exit /b 1
)
echo CDO values extracted successfully.

REM ═══════════════════════════════════════════════════════════════════════════════
REM PHASE 3: Run migration
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo PHASE 3: Running Blueprint migration
echo ═══════════════════════════════════════════════════════════════════════════════
echo.

"%UE_PATH%" "%PROJECT%" -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" -stdout -unattended -nosplash 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Migration may have had issues
)
echo Migration complete.

REM ═══════════════════════════════════════════════════════════════════════════════
REM PHASE 4: Restore CDO values to migrated Blueprints
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo PHASE 4: Restoring CDO values to migrated Blueprints
echo ═══════════════════════════════════════════════════════════════════════════════
echo.

"%UE_PATH%" "%PROJECT%" -run=pythonscript -script="C:/scripts/SLFConversion/restore_blueprint_values.py" -stdout -unattended -nosplash 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Value restoration may have had issues
)
echo Values restored.

REM ═══════════════════════════════════════════════════════════════════════════════
REM PHASE 5: Run tests (optional)
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo PHASE 5: Running validation tests
echo ═══════════════════════════════════════════════════════════════════════════════
echo.

if "%1"=="--skip-tests" (
    echo Tests skipped.
) else (
    "%UE_PATH%" "%PROJECT%" -run=pythonscript -script="C:/scripts/SLFConversion/tests/run_all_tests.py" -stdout -unattended -nosplash 2>&1
)

REM ═══════════════════════════════════════════════════════════════════════════════
REM COMPLETE
REM ═══════════════════════════════════════════════════════════════════════════════
echo.
echo ═══════════════════════════════════════════════════════════════════════════════
echo MIGRATION WORKFLOW COMPLETE
echo ═══════════════════════════════════════════════════════════════════════════════
echo.
echo Next steps:
echo   1. Open project in Unreal Editor
echo   2. Fix any remaining Blueprint compilation errors
echo   3. Run PIE test
echo.

endlocal
