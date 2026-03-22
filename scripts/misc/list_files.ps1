# List files
Write-Host "=== EXPORTS ==="
Get-ChildItem "C:\scripts\SLFConversion\Exports" -Recurse -File -ErrorAction SilentlyContinue | Select-Object -First 30 -ExpandProperty FullName

Write-Host "`n=== BP_ONLY ==="
Test-Path "C:\scripts\bp_only"

Write-Host "`n=== BACKUP ITEMS ==="
Get-ChildItem "C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content\SoulslikeFramework\Data\Items" -ErrorAction SilentlyContinue | Select-Object -First 30 -ExpandProperty Name

Write-Host "`n=== PDA_ITEM JSON ==="
Get-ChildItem "C:\scripts\SLFConversion\Exports" -Recurse -Filter "*Item*.json" -ErrorAction SilentlyContinue | Select-Object -First 10 -ExpandProperty FullName
