@echo off
REM restore_and_migrate_animbps.bat
REM AAA-Quality AnimBP Migration: Restore from backup then migrate to C++
REM
REM This script:
REM 1. Restores AnimBP assets from bp_only backup (clean state)
REM 2. Runs migration to reparent to C++ AnimInstance classes
REM 3. C++ NativeUpdateAnimation() then handles all variable updates

echo ============================================================
echo AAA-QUALITY ANIMBP MIGRATION
echo ============================================================
echo.

REM Source and destination paths
set BACKUP_DIR=C:\scripts\bp_only\Content\SoulslikeFramework\Demo\_Animations\Locomotion\AnimBP
set TARGET_DIR=C:\scripts\SLFConversion\Content\SoulslikeFramework\Demo\_Animations\Locomotion\AnimBP

echo Step 1: Restoring AnimBP assets from backup...
echo.

REM Restore each AnimBP
echo   Restoring ABP_SoulslikeEnemy.uasset...
copy /Y "%BACKUP_DIR%\ABP_SoulslikeEnemy.uasset" "%TARGET_DIR%\" >nul
if %errorlevel% neq 0 (
    echo   FAILED: Could not copy ABP_SoulslikeEnemy
) else (
    echo   OK
)

echo   Restoring ABP_SoulslikeNPC.uasset...
copy /Y "%BACKUP_DIR%\ABP_SoulslikeNPC.uasset" "%TARGET_DIR%\" >nul
if %errorlevel% neq 0 (
    echo   FAILED: Could not copy ABP_SoulslikeNPC
) else (
    echo   OK
)

echo   Restoring ABP_SoulslikeBossNew.uasset...
copy /Y "%BACKUP_DIR%\ABP_SoulslikeBossNew.uasset" "%TARGET_DIR%\" >nul
if %errorlevel% neq 0 (
    echo   FAILED: Could not copy ABP_SoulslikeBossNew
) else (
    echo   OK
)

echo.
echo Step 2: Running AnimBP migration to C++...
echo.

REM Run the migration script
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" ^
  "C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -run=pythonscript -script="C:\scripts\SLFConversion\migrate_animbp_aaa.py" ^
  -stdout -unattended -nosplash

echo.
echo ============================================================
echo MIGRATION COMPLETE
echo ============================================================
echo.
echo If successful, the AnimBPs are now parented to C++ AnimInstance classes.
echo C++ NativeUpdateAnimation() handles all animation variable updates.
echo.
echo You can now remove the reflection workaround from SLFSoulslikeEnemy.cpp.
echo.
pause
