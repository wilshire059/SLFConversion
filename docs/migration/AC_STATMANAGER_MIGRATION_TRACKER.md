# AC_StatManager Migration Tracker

## Migration Status: IN PROGRESS
**Started**: December 29, 2025
**Last Updated**: December 29, 2025

---

## Pre-Migration Checklist (from INPUTBUFFER_MIGRATION_POSTMORTEM.md)

### Pre-Implementation
- [ ] Read JSON export for target component
- [ ] Document ALL function logic from graph nodes
- [ ] Identify all branch conditions and their semantics
- [ ] List all parameter names exactly as they appear in Blueprint
- [ ] Check for conflicting function names with intended C++ interface
- [ ] Backup current state before any changes

### Implementation
- [ ] Add UE_LOG to every function from the start
- [ ] Match parameter names EXACTLY (use UPARAM if needed)
- [ ] Use BlueprintNativeEvent for overridable functions
- [ ] Don't create C++ functions that conflict with existing BP functions
- [ ] Keep C++ interface minimal - only expose what's needed

### Post-Implementation
- [ ] Run migration script
- [ ] Check compiler warnings for parameter/signature mismatches
- [ ] Test in PIE immediately
- [ ] Verify debug logs show expected behavior
- [ ] Compare behavior to pre-migration state

---

## Dependency Chain

```
LAYER 0: Structs & Enums (migrate first)
├── FRegen
├── FStatBehavior
├── FStatInfo (depends on FRegen, FStatBehavior)
├── FStatOverride (depends on FRegen)
├── E_ValueType

LAYER 1: Data Objects
├── B_Stat (uses FStatInfo) - TBD: migrate or use UObject*

LAYER 2: Component
├── AC_StatManager (uses all above)
```

---

## Phase 1: Layer 0 Analysis

### E_ValueType Enum
**Source**: `/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Value | Display Name |
|-------|--------------|
| 0 | Current Value |
| 1 | Max Value |

### FRegen Struct
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| CanRegenerate? | bCanRegenerate | bool | false |
| RegenPercent | RegenPercent | float | 0.0 |
| RegenInterval | RegenInterval | float | 0.0 |

### FAffectedStat Struct
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FAffectedStat.FAffectedStat`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| FromLevel | FromLevel | int32 | 0 |
| UntilLevel | UntilLevel | int32 | 0 |
| AffectMaxValue? | bAffectMaxValue | bool | false |
| Modifier | Modifier | double | 0.0 |
| Calculation | Calculation | UObject* | nullptr |
| ChangebyCurveOptional | ChangeByCurve | UCurveFloat* | nullptr |

### FAffectedStats Struct (Container)
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FAffectedStats.FAffectedStats`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| SoftcapData | SoftcapData | TArray<FAffectedStat> | empty |

### FStatBehavior Struct
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| StatstoAffect | StatsToAffect | TMap<FGameplayTag, FAffectedStats> | empty |

### FStatInfo Struct
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| Tag | Tag | FGameplayTag | empty |
| DisplayName | DisplayName | FText | empty |
| Description | Description | FText | empty |
| DisplayasPercent? | bDisplayAsPercent | bool | false |
| CurrentValue | CurrentValue | double | 0.0 |
| MaxValue | MaxValue | double | 0.0 |
| ShowMaxValue? | bShowMaxValue | bool | false |
| RegenInfo | RegenInfo | FRegen | default |
| StatModifiers | StatModifiers | FStatBehavior | default |

### FStatOverride Struct
**Source**: `/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride`
**Status**: [x] Analyzed [ ] C++ Created [ ] Tested

| Blueprint Name | C++ Name | Type | Default |
|----------------|----------|------|---------|
| OverrideCurrentValue | OverrideCurrentValue | double | 0.0 |
| OverrideMaxValue | OverrideMaxValue | double | 0.0 |
| OverrideRegen? | bOverrideRegen | bool | false |
| OverrideRegenInfo | OverrideRegenInfo | FRegen | default |

---

## Phase 2: AC_StatManager Analysis

### Variables (11 total)
| Name | Type | Category | C++ Type |
|------|------|----------|----------|
| ActiveStats | TMap<FGameplayTag, B_Stat_C*> | Runtime | TBD |
| IsAiComponent | bool | Config | bool |
| SpeedAsset | PDA_MovementSpeedData_C* | Config | UDataAsset* |
| StatTable | UDataTable* | Config | UDataTable* |
| Stats | TMap<TSubclassOf<B_Stat_C>, FGameplayTag> | Config | TBD |
| Level | int | Runtime | int32 |
| SelectedClassAsset | PDA_BaseCharacterInfo_C* | Runtime | UDataAsset* |
| StatOverrides | TMap<FGameplayTag, FStatOverride> | Config | TMap<FGameplayTag, FStatOverride> |
| OnStatsInitialized | mcdelegate | Default | DECLARE_DYNAMIC_MULTICAST_DELEGATE |
| OnLevelUpdated | mcdelegate | Default | DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam |
| OnSaveRequested | mcdelegate | Default | DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams |

### Event Dispatchers (3 total)
| Name | Parameters |
|------|------------|
| OnStatsInitialized | none |
| OnLevelUpdated | int NewLevel |
| OnSaveRequested | FGameplayTag SaveGameTag, TArray<FInstancedStruct> DataToSave |

### Functions (12 total)
| Name | Inputs | Outputs | Status |
|------|--------|---------|--------|
| GetStat | FGameplayTag StatTag | B_Stat_C*, FStatInfo | [ ] Analyzed |
| GetAllStats | none | TArray<B_Stat_C*>, TMap | [ ] Analyzed |
| ResetStat | FGameplayTag StatTag | none | [ ] Analyzed |
| AdjustStat | FGameplayTag, E_ValueType, double, bool, bool | none | [ ] Analyzed |
| AdjustAffected | FGameplayTag, E_ValueType, double | none | [ ] Analyzed |
| AdjustAffectedStats | B_Stat_C*, double, E_ValueType | none | [ ] Analyzed |
| SerializeStatsData | TBD | TBD | [ ] Analyzed |
| InitializeLoadedStats | TBD | TBD | [ ] Analyzed |
| GetStatsForCategory | TBD | TBD | [ ] Analyzed |
| AdjustLevel | TBD | TBD | [ ] Analyzed |
| IsStatMoreThan | TBD | TBD | [ ] Analyzed |
| ToggleRegenForStat | TBD | TBD | [ ] Analyzed |

---

## Phase 2 Analysis Summary

### Function Logic Patterns

**GetStat(FGameplayTag StatTag)**:
1. Find StatTag in ActiveStats map
2. If found, get B_Stat_C* object
3. Call GetStatInfo() on the B_Stat object
4. Return B_Stat_C* and FStatInfo

**AdjustStat(StatTag, ValueType, Change, LevelUp?, TriggerRegen?)**:
1. Call GetStat(StatTag) to get B_Stat_C*
2. Validate stat exists
3. Call AdjustValue() on the B_Stat object

**EventGraph INITIALIZATION**:
1. BeginPlay → Event Initialize Stats
2. If NOT IsAiComponent → Bind to OnDataLoaded from AC_SaveLoadManager
3. Complex initialization with data tables and stat instantiation

### Key Insight
AC_StatManager is a COORDINATOR - it holds the TMap<FGameplayTag, B_Stat_C*> and orchestrates calls to B_Stat instances. The actual stat logic is in B_Stat class.

---

## Phase 3: C++ Implementation

### Files Created
- [x] `SLFStatTypes.h` - Structs and enums (EStatValueType, FSLFRegen, FSLFAffectedStat, FSLFAffectedStats, FSLFStatBehavior, FSLFStatInfo, FSLFStatOverride)
- [x] `StatManagerComponent.h` - C++ base class header with 12 BlueprintNativeEvent functions
- [x] `StatManagerComponent.cpp` - Implementation with debug logging
- [x] `migrate_statmanager_to_cpp.py` - Migration script

### C++ Design Decisions

**For B_Stat_C references:**
- Decision: Use `UObject*` in C++
- Rationale: B_Stat_C is a Blueprint class. C++ can store as UObject* and use reflection to call Blueprint functions (GetStatInfo, AdjustValue, etc.)

**For PDA_* references:**
- Decision: Use `UDataAsset*` base type
- Rationale: PDA classes are Blueprint DataAssets, C++ can use base type

**For function return types:**
- GetStat returns B_Stat_C* in Blueprint → C++ returns UObject*
- GetStatInfo struct output → C++ returns FSLFStatInfo (C++ struct)

**For E_ValueType:**
- Blueprint uses E_ValueType enum
- C++ uses EStatValueType enum
- Need CoreRedirect or matching names

---

## Troubleshooting Log

| Date | Issue | Resolution |
|------|-------|------------|
| | | |

---

## Backup Information

**Pre-Migration Backup**: `C:\scripts\SLFConversion_Migration\Backups\blueprint_only\`
**JSON Exports**: `C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA\`

### Restore Command
```powershell
# Close Unreal Editor first!
rm -rf "C:/scripts/SLFConversion/Content" && cp -r "C:/scripts/SLFConversion_Migration/Backups/blueprint_only/Content" "C:/scripts/SLFConversion/"
```

---

## Notes

- Follow INPUTBUFFER_MIGRATION_POSTMORTEM.md guidelines
- Always add debug logging FIRST
- Verify branch logic from JSON before implementing
- Test incrementally, not all at once
