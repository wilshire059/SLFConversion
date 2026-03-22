# List all Niagara systems in the project
Write-Host "=== NIAGARA SYSTEMS ==="
Get-ChildItem "C:\scripts\SLFConversion\Content\SoulslikeFramework\VFX" -Recurse -Filter "*.uasset" -ErrorAction SilentlyContinue | ForEach-Object { $_.FullName }

Write-Host "`n=== CHECKING BACKUP PDA_Item ==="
$backupPda = "C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content\SoulslikeFramework\Data\PDA_Item.uasset"
if (Test-Path $backupPda) {
    Write-Host "PDA_Item backup exists at: $backupPda"
    Write-Host "Size: $((Get-Item $backupPda).Length) bytes"
} else {
    Write-Host "PDA_Item backup NOT found"
}

Write-Host "`n=== BACKUP DA_* ITEMS ==="
Get-ChildItem "C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content\SoulslikeFramework\Data\Items" -Filter "DA_*.uasset" -ErrorAction SilentlyContinue | ForEach-Object { $_.Name }
