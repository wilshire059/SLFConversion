# Migration Workflow - Detailed 4-Step Process

> **For ongoing changes, use SURGICAL MIGRATION (see CLAUDE.md).**
> Only use full migration when starting fresh or recovering from major issues.

---

## Step 1: Extract Data FROM BACKUP (One-time)

Extract BEFORE restoring, while backup has valid Blueprint data.

### 1a: Extract Item Data
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_data.py" ^
  -stdout -unattended -nosplash 2>&1
```
Cache: `migration_cache/item_data.json`

### 1b: Extract BaseStats
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_base_stats.py" ^
  -stdout -unattended -nosplash 2>&1
```
Cache: `migration_cache/base_stats.json`

### 1c: Extract Stat Defaults (requires SLFAutomationLibrary in backup)
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_stat_defaults_cpp.py" ^
  -stdout -unattended -nosplash 2>&1
```
Cache: `migration_cache/stat_defaults.json`

---

## Step 2: Restore Backup (FULL MIGRATION ONLY)

```bash
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"
```

---

## Step 3: Run Reparenting Migration

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1
```

---

## Step 4: Apply Cached Data (ALL SCRIPTS REQUIRED)

| Step | Script | Purpose |
|------|--------|---------|
| 4a | `apply_icons_fixed.py` | Apply item icons from cache |
| 4b | `apply_remaining_data.py` | Apply niagara + dodge montages |
| 4c | `apply_item_data.py` | Apply item descriptions, names, display data |
| 4d | `apply_item_categories.py` | Apply item categories |
| 4e | `apply_base_stats.py` | Apply BaseStats to character class data assets |
| 4f | `apply_armor_data.py` | Apply armor mesh data |
| 4g | `apply_weapon_overlay.py` | Apply weapon overlay tags |

Each script runs with:
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/[SCRIPT_NAME]" ^
  -stdout -unattended -nosplash 2>&1
```

---

## Build C++ (if needed)

```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```

---

## Migration Scripts Reference

| Script | Purpose | Run On |
|--------|---------|--------|
| `extract_item_data.py` | Extract icons, niagara from backup | bp_only |
| `extract_base_stats.py` | Extract character class BaseStats | bp_only |
| `extract_stat_defaults_cpp.py` | Extract stat CDO defaults using C++ | bp_only |
| `extract_armor_data.py` | Extract armor mesh data per character | bp_only |
| `run_migration.py` | Multi-phase reparenting | SLFConversion |
| `apply_icons_fixed.py` | Apply item icons from cache | SLFConversion |
| `apply_remaining_data.py` | Apply niagara + dodge montages | SLFConversion |
| `apply_item_data.py` | Apply item descriptions, names, display data | SLFConversion |
| `apply_item_categories.py` | Apply item categories | SLFConversion |
| `apply_base_stats.py` | Apply BaseStats | SLFConversion |
| `apply_armor_data.py` | Apply armor mesh data | SLFConversion |
| `apply_weapon_meshes.py` | Apply weapon mesh assignments | SLFConversion |
| `extract_weapon_overlay.py` | Extract WeaponOverlay tags | bp_only |
| `apply_weapon_overlay.py` | Apply WeaponOverlay tags | SLFConversion |
| `full_migration.py` | All-in-one workflow | SLFConversion |

---

## Expected Output

- Extract: "Saved 21 items to migration_cache/item_data.json"
- Migration: ~306 successful reparents, 0 errors
- Apply icons: "Applied icons to 21 items"
- Apply remaining: "Applied Niagara to 21 items", "Saved B_Action_Dodge"
- Apply item data: "21 succeeded, 0 failed"
- Apply categories: "OK: DA_Sword01" with category assignments
