# Resilient Restore & Migration Guide

## The Problem

When restoring from backup and running migration, Blueprint struct data (icons, mesh references, niagara systems, etc.) is lost because:
1. Original Blueprint structs (like `FItemInfo`, `FSkeletalMeshData`) contain asset references
2. After reparenting to C++ classes, these references don't automatically transfer
3. Running Python extraction AFTER restore extracts empty/default values

## The Solution

**Always extract data BEFORE restoring.** The cache should survive restores.

## Workflow

### Option 1: Full Resilient Migration (Recommended)

```bash
# 1. If you have good content, extract data FIRST
UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript ^
  -script="C:/scripts/SLFConversion/resilient_migration.py" ^
  -stdout -unattended -nosplash

# This does:
# - Phase 1: Extract data to migration_cache/
# - Phase 2: Run reparenting
# - Phase 3: Apply cached data
```

### Option 2: Manual Steps

```bash
# 1. With ORIGINAL content (before any restore), extract data:
UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript ^
  -script="C:/scripts/SLFConversion/apply_item_icons.py" ^
  -stdout -unattended -nosplash

# 2. Restore backup
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"

# 3. Run migration
UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript ^
  -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash

# 4. Apply cached data
UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript ^
  -script="C:/scripts/SLFConversion/apply_item_icons.py" ^
  -stdout -unattended -nosplash
```

## Cache Files

Located at `C:/scripts/SLFConversion/migration_cache/`:

| File | Contents |
|------|----------|
| `item_data.json` | IconSmall, IconLargeOptional, WorldNiagaraSystem for all DA_* items |
| `default_mesh_data.json` | Mesh references for PDA_DefaultMeshData |
| `physics_data.json` | Physics asset reference |

## Hard-Coded Fallbacks

The C++ code now includes hard-coded fallbacks for critical assets:

### Character Mesh (SLFSoulslikeCharacter.cpp)
If no mesh data is available, automatically loads:
- `/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyHead`
- `/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyUpperBody`
- `/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyArms`
- `/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyLowerBody`

## Troubleshooting

### Icons showing as white squares
- Check `migration_cache/item_data.json` has data
- Run `apply_item_icons.py` after migration

### Character invisible
- C++ fallback should load Manny meshes automatically
- Check output log for "FALLBACK: Loaded" messages
- Verify SKM_Manny* meshes exist at `/Game/SoulslikeFramework/Meshes/SKM/`

### Cache is empty
- Extract was run AFTER reparenting (data was already lost)
- Delete cache, restore backup, re-extract, then migrate

## Key Insight

The migration_cache should be treated as **precious data** that survives restores. Only delete it when you intentionally want to re-extract from fresh Blueprint content.
