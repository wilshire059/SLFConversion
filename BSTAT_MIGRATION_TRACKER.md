# B_Stat / AC_StatManager Full C++ Migration Tracker

## Overview

This tracker follows the incremental migration of Blueprint types to C++ for the stats system.
Each type is migrated one at a time with Editor verification between steps.

**Start Date:** December 29, 2025
**Base Backup:** `C:/scripts/backups/SLFConversion_InputBuffer_Complete_20251229`
**Pre-Enum Backup:** `C:/scripts/backups/SLFConversion_PreEnumMigration_*`

---

## Critical Discovery: E_ValueType Dependencies (JSON Analysis Complete)

The E_ValueType enum is used by **3 structs** as a member type:
- FStatChange (5 members including ValueType)
- FStatChangePercent (4 members including ValueType)
- FLevelChangeData (2 members including ValueType)

**These must be migrated TOGETHER with E_ValueType** or they will break.

### Comprehensive JSON Analysis Results:

**22 unique Blueprint files** reference E_ValueType with **121 pin instances**:

| File | E_ValueType Refs | Category |
|------|------------------|----------|
| AC_StatManager | 23 | Component |
| B_Stat | 21 | Stats |
| W_LevelUp | 18 | Widget |
| AC_CombatManager | 12 | Component |
| W_HUD | 7 | Widget |
| AC_AI_CombatManager | 6 | Component |
| B_StatusEffect | 6 | Effects |
| B_Action_DrinkFlask_HP | 4 | Actions |
| AN_AdjustStat | 3 | Animation |
| B_Action | 2 | Actions |
| B_Action_Dodge | 2 | Actions |
| B_Buff_AttackPower | 2 | Buffs |
| B_StatusEffect_Frostbite | 2 | Effects |
| B_StatusEffect_Plague | 2 | Effects |
| W_Boss_Healthbar | 1 | Widget |
| AC_EquipmentManager | 1 | Component |
| W_StatEntry_Status | 1 | Widget |
| AC_AI_Boss | 1 | Component |
| W_StatEntry_LevelUp | 1 | Widget |
| W_StatEntry | 1 | Widget |
| AC_ActionManager | 1 | Component |
| B_ShowcaseEnemy_StatDisplay | 1 | Stats |

**IMPORTANT**: Many other enums (E_WorldMeshStyle, E_VendorType, E_TraceType, etc.) also use
"NewEnumerator0/1" naming. The fix script ONLY modifies pins where SubCategoryObject = "E_ValueType".

---

## Revised Migration Steps

### Step 1: E_ValueType + Dependent Structs (BATCH)

| Type | Asset Path | Status |
|------|------------|--------|
| E_ValueType | Enums/E_ValueType.uasset | [~] IN PROGRESS |
| FStatChange | Structures/Stats/FStatChange.uasset | [~] IN PROGRESS |
| FStatChangePercent | Structures/Stats/FStatChangePercent.uasset | [~] IN PROGRESS |
| FLevelChangeData | Structures/Stats/FLevelChangeData.uasset | [~] IN PROGRESS |

**Affected Blueprints (27 files, 91 pin instances):**
- Animation/AN_AdjustStat
- Blueprint/B_Action, B_Action_Dodge, B_Action_DrinkFlask_HP
- Blueprint/B_Buff_AttackPower, B_ShowcaseEnemy_StatDisplay
- Blueprint/B_Stat, B_StatusEffect, B_StatusEffect_Frostbite, B_StatusEffect_Plague
- Component/AC_ActionManager, AC_AI_Boss, AC_AI_CombatManager
- Component/AC_CombatManager, AC_EquipmentManager, AC_StatManager
- WidgetBlueprint/W_Boss_Healthbar, W_HUD, W_LevelUp
- WidgetBlueprint/W_StatEntry, W_StatEntry_LevelUp, W_StatEntry_Status

### Step 2-7: Remaining Structs

| Step | Type | Status | Notes |
|------|------|--------|-------|
| 2 | FRegen | [ ] NOT STARTED | Used by FStatInfo, FStatOverride |
| 3 | FAffectedStat | [ ] NOT STARTED | Used by FAffectedStats |
| 4 | FAffectedStats | [ ] NOT STARTED | Used by FStatBehavior |
| 5 | FStatBehavior | [ ] NOT STARTED | Used by FStatInfo |
| 6 | FStatInfo | [ ] NOT STARTED | Main stat struct |
| 7 | FStatOverride | [ ] NOT STARTED | Used by AC_StatManager |

### Step 8-9: Class Migration

| Step | Type | Status | Notes |
|------|------|--------|-------|
| 8 | B_Stat | [ ] NOT STARTED | Reparent to UStatObject |
| 9 | AC_StatManager | [ ] NOT STARTED | Reparent to UStatManagerComponent |

---

## C++ Types Reference

All C++ types in: `Source/SLFConversion/SLFStatTypes.h`

| Blueprint Type | C++ Type |
|----------------|----------|
| E_ValueType | ESLFValueType |
| FStatChange | FStatChange |
| FStatChangePercent | FStatChangePercent |
| FLevelChangeData | FLevelChangeData |
| FRegen | FRegen |
| FAffectedStat | FAffectedStat |
| FAffectedStats | FAffectedStats |
| FStatBehavior | FStatBehavior |
| FStatInfo | FStatInfo |
| FStatOverride | FStatOverride |

---

## Step 1 Migration Process

### Phase A: Run Comprehensive Python Fix Script (in Editor)
```python
exec(open(r"C:\scripts\SLFConversion\fix_enum_comprehensive.py").read())
```

This script will:
1. Process all 22 affected Blueprints
2. Find pins where SubCategoryObject = "E_ValueType"
3. Convert "NewEnumerator0" -> "CurrentValue"
4. Convert "NewEnumerator1" -> "MaxValue"
5. Compile and save each modified Blueprint
6. Report detailed statistics

### Phase B: Save All and Close Editor
1. File -> Save All
2. Close Unreal Editor completely

### Phase C: Delete Blueprint Type Assets (from disk)
```
Content/SoulslikeFramework/Enums/E_ValueType.uasset
Content/SoulslikeFramework/Structures/Stats/FStatChange.uasset
Content/SoulslikeFramework/Structures/Stats/FStatChangePercent.uasset
Content/SoulslikeFramework/Structures/Stats/FLevelChangeData.uasset
```

### Phase D: Add CoreRedirects (before reopening Editor)
Add to Config/DefaultEngine.ini:
```ini
[CoreRedirects]
+EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType",NewName="/Script/SLFConversion.ESLFValueType")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatChange.FStatChange",NewName="/Script/SLFConversion.FStatChange")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatChangePercent.FStatChangePercent",NewName="/Script/SLFConversion.FStatChangePercent")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FLevelChangeData.FLevelChangeData",NewName="/Script/SLFConversion.FLevelChangeData")
```

### Phase E: Open Editor and Fix Remaining Issues
- Compile all Blueprints
- Fix any remaining type mismatches manually

---

## Error Log

### Step 1 Errors
```
(will be logged here during migration)
```

---

## Rollback Instructions

If migration fails:
```powershell
# Close Unreal Editor first!
cd C:/scripts/SLFConversion
rm -rf Content Config
cp -r "C:/scripts/backups/SLFConversion_PreEnumMigration_*/Content" .
cp -r "C:/scripts/backups/SLFConversion_InputBuffer_Complete_20251229/Config" .
```
