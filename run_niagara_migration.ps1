# run_niagara_migration.ps1
# Full workflow to migrate Niagara data from Blueprint to C++ properties
#
# Steps:
# 1. Copy backup items to Content (restores Blueprint data)
# 2. Copy backup PDA_Item to Content (restores Blueprint class)
# 3. Run extraction script (reads Blueprint data, saves to JSON)
# 4. Run migration script (reparents PDA_Item to UPDA_Item)
# 5. Run apply script (sets C++ properties from JSON)

param(
    [switch]$SkipRestore,
    [switch]$SkipExtract,
    [switch]$SkipMigrate,
    [switch]$SkipApply
)

$ErrorActionPreference = "Stop"

$ProjectRoot = "C:\scripts\SLFConversion"
$BackupRoot = "C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content\SoulslikeFramework"
$ContentRoot = "$ProjectRoot\Content\SoulslikeFramework"
$UEEditor = "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$UEProject = "$ProjectRoot\SLFConversion.uproject"

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "NIAGARA DATA MIGRATION WORKFLOW" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# Step 1: Restore backup items
if (-not $SkipRestore) {
    Write-Host "STEP 1: Restoring backup items to Content folder..." -ForegroundColor Yellow
    Write-Host ""

    # Copy DA_* items
    $backupItems = "$BackupRoot\Data\Items"
    $currentItems = "$ContentRoot\Data\Items"

    Write-Host "  Source: $backupItems"
    Write-Host "  Dest:   $currentItems"

    if (Test-Path $backupItems) {
        $files = Get-ChildItem $backupItems -Filter "DA_*.uasset"
        Write-Host "  Found $($files.Count) DA_* files to restore"

        foreach ($file in $files) {
            Copy-Item $file.FullName -Destination $currentItems -Force
            Write-Host "    Copied: $($file.Name)" -ForegroundColor Green
        }
    } else {
        Write-Host "  [ERROR] Backup items folder not found!" -ForegroundColor Red
        exit 1
    }

    # Copy PDA_Item
    $backupPDA = "$BackupRoot\Data\PDA_Item.uasset"
    $currentPDA = "$ContentRoot\Data\PDA_Item.uasset"

    Write-Host ""
    Write-Host "  Restoring PDA_Item..."
    if (Test-Path $backupPDA) {
        Copy-Item $backupPDA -Destination $currentPDA -Force
        Write-Host "    Copied: PDA_Item.uasset" -ForegroundColor Green
    } else {
        Write-Host "  [ERROR] Backup PDA_Item not found!" -ForegroundColor Red
        exit 1
    }

    Write-Host ""
    Write-Host "  [OK] Backup restored" -ForegroundColor Green
} else {
    Write-Host "STEP 1: Skipped (restore)" -ForegroundColor DarkGray
}

# Step 2: Run extraction script
if (-not $SkipExtract) {
    Write-Host ""
    Write-Host "STEP 2: Running Niagara path extraction..." -ForegroundColor Yellow
    Write-Host ""

    $extractScript = "$ProjectRoot\extract_niagara_paths.py"

    Write-Host "  Running: $extractScript"
    Write-Host ""

    & $UEEditor $UEProject -run=pythonscript -script="$extractScript" -stdout -unattended -nosplash 2>&1 | Tee-Object -Variable extractOutput

    # Check if JSON was created
    $jsonFile = "$ProjectRoot\niagara_paths.json"
    if (Test-Path $jsonFile) {
        Write-Host ""
        Write-Host "  [OK] Extraction complete" -ForegroundColor Green
        Write-Host "  Output: $jsonFile"
        Get-Content $jsonFile | Write-Host
    } else {
        Write-Host ""
        Write-Host "  [WARNING] niagara_paths.json not created" -ForegroundColor Yellow
        Write-Host "  Check extraction output above for errors"
    }
} else {
    Write-Host "STEP 2: Skipped (extract)" -ForegroundColor DarkGray
}

# Step 3: Run migration script (reparent PDA_Item)
if (-not $SkipMigrate) {
    Write-Host ""
    Write-Host "STEP 3: Running migration (reparent PDA_Item to UPDA_Item)..." -ForegroundColor Yellow
    Write-Host ""

    $migrateScript = "$ProjectRoot\run_migration.py"

    Write-Host "  Running: $migrateScript"
    Write-Host ""

    & $UEEditor $UEProject -run=pythonscript -script="$migrateScript" -stdout -unattended -nosplash 2>&1 | Tee-Object -Variable migrateOutput

    Write-Host ""
    Write-Host "  [OK] Migration complete" -ForegroundColor Green
} else {
    Write-Host "STEP 3: Skipped (migrate)" -ForegroundColor DarkGray
}

# Step 4: Run apply script
if (-not $SkipApply) {
    Write-Host ""
    Write-Host "STEP 4: Applying Niagara paths to C++ properties..." -ForegroundColor Yellow
    Write-Host ""

    $applyScript = "$ProjectRoot\apply_niagara_paths.py"

    Write-Host "  Running: $applyScript"
    Write-Host ""

    & $UEEditor $UEProject -run=pythonscript -script="$applyScript" -stdout -unattended -nosplash 2>&1 | Tee-Object -Variable applyOutput

    Write-Host ""
    Write-Host "  [OK] Apply complete" -ForegroundColor Green
} else {
    Write-Host "STEP 4: Skipped (apply)" -ForegroundColor DarkGray
}

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "WORKFLOW COMPLETE" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Open Unreal Editor"
Write-Host "  2. Play in Editor (PIE)"
Write-Host "  3. Check if Niagara effects appear on pickup items"
Write-Host ""
