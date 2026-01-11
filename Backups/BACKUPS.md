# SLFConversion Backups Index

## How to Use This File
When restoring a backup, check this index to find the right backup for your needs.

---

## animbp_native_complete/

**Date:** 2026-01-10
**Purpose:** AnimBP after successful C++ native migration (no reflection)

### What's Inside
| File | Description |
|------|-------------|
| `ABP_SoulslikeCharacter_Additive.uasset` | AnimBP with 16 functions cleared, only AnimGraph remains |
| `ABP_SoulslikeCharacter_Additive.h` | C++ AnimInstance header |
| `ABP_SoulslikeCharacter_Additive.cpp` | C++ implementation (no reflection) |
| `clear_animbp_functions.py` | Script to clear redundant functions |
| `fix_blend_bindings.py` | Script to fix BlendListByEnum bindings |
| `rename_overlay_vars.py` | Script to rename _0 suffix variables |
| `test_animbp_native.py` | Validation script |

### State
- Blueprint variables renamed: `LeftHandOverlayState_0` → `LeftHandOverlayState`
- BlendListByEnum reads directly from C++ UPROPERTY
- 0 compile errors, 0 warnings
- Functions remaining: AnimGraph only

### Restore Commands
```bash
# Restore AnimBP asset
cp "C:/scripts/SLFConversion/backups/animbp_native_complete/ABP_SoulslikeCharacter_Additive.uasset" \
   "C:/scripts/SLFConversion/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/"

# Restore C++ files
cp "C:/scripts/SLFConversion/backups/animbp_native_complete/ABP_SoulslikeCharacter_Additive.h" \
   "C:/scripts/SLFConversion/Source/SLFConversion/Animation/"
cp "C:/scripts/SLFConversion/backups/animbp_native_complete/ABP_SoulslikeCharacter_Additive.cpp" \
   "C:/scripts/SLFConversion/Source/SLFConversion/Animation/"

# Rebuild C++
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" \
  SLFConversionEditor Win64 Development \
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" \
  -WaitMutex -FromMsBuild
```

---

## bp_only (Primary Backup - External)

**Location:** `C:/scripts/bp_only/`
**Purpose:** Original Blueprint-only project before any C++ migration

### ⚠️ CRITICAL: SURGICAL RESTORES ONLY ⚠️

**NEVER do this:**
```bash
# ❌ WRONG - NEVER overwrite entire Content folder
Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force
Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse
```

**ALWAYS do this:**
```bash
# ✅ RIGHT - Copy ONLY specific assets you need
cp "C:/scripts/bp_only/Content/SoulslikeFramework/Data/Items/DA_ExampleArmor.uasset" \
   "C:/scripts/SLFConversion/Content/SoulslikeFramework/Data/Items/"
```

### Why?
Full Content/ overwrites cause **regressions in working Blueprints**. Many Blueprints have already been reparented to C++ and have correct configurations. Overwriting them loses that work.

### When to Use
- Extracting original Blueprint logic (read-only reference)
- Restoring **specific** assets that need re-migration
- Comparing against migrated code
- Running extraction scripts (extract_*.py) to cache data

### Surgical Restore Pattern
1. Identify the specific asset(s) you need
2. Copy ONLY those assets from `bp_only`
3. Run targeted migration script for ONLY those assets
4. Verify the specific feature works
5. Don't touch unrelated assets

### Note
This is the primary backup - a separate project, not inside the backups folder.
