# Option 2: Full AC_StatManager to C++ Migration Tracker

## Source: AC_StatManager.json
Path: `/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager`
Parent: `UActorComponent`

---

## VARIABLES (11 total)

| Name | Type | Category | InstanceEditable | Notes |
|------|------|----------|------------------|-------|
| ActiveStats | TMap<FGameplayTag, B_Stat*> | Runtime | No | Runtime stat instances |
| IsAiComponent | bool | Config | Yes | Is this for AI? |
| SpeedAsset | PDA_MovementSpeedData_C* | Config | No | Movement speed data asset |
| StatTable | UDataTable* | Config | Yes | Stat configuration table |
| Stats | TMap<TSubclassOf<B_Stat>, FGameplayTag> | Config | Yes | Stat class to tag mapping |
| Level | int | Runtime | No | Current character level |
| SelectedClassAsset | PDA_BaseCharacterInfo_C* | Runtime | No | Character class info |
| StatOverrides | TMap<FGameplayTag, FStatOverride> | Config | Yes | Per-instance stat overrides |
| OnStatsInitialized | Delegate | - | - | Event dispatcher (no params) |
| OnLevelUpdated | Delegate | - | - | Event dispatcher (NewLevel: int) |
| OnSaveRequested | Delegate | - | - | Event dispatcher (SaveGameTag, DataToSave) |

---

## EVENT DISPATCHERS (3)

### OnStatsInitialized
- Parameters: None
- Called when stats are fully initialized

### OnLevelUpdated
- Parameters: `NewLevel` (int)
- Called when level changes

### OnSaveRequested
- Parameters:
  - `SaveGameTag` (FGameplayTag)
  - `DataToSave` (TArray<FInstancedStruct>)
- Called when save is requested

---

## FUNCTIONS (12)

### 1. GetStat
```
Input:  StatTag (FGameplayTag)
Output: FoundStat (B_Stat*), StatInfo (FStatInfo)
```
Implementation: Look up StatTag in ActiveStats map, return stat and its info

### 2. GetAllStats
```
Input:  None
Output: StatObjects (TArray<B_Stat*>), StatClassesAndCategories (TMap<TSubclassOf<B_Stat>, FGameplayTag>)
```
Implementation: Return all active stats and the Stats config map

### 3. ResetStat
```
Input:  StatTag (FGameplayTag)
Output: None
```
Implementation: Reset stat to its default/max value

### 4. AdjustStat
```
Input:  StatTag (FGameplayTag), ValueType (E_ValueType), Change (double), LevelUp (bool), TriggerRegen (bool)
Output: None
```
Implementation: Modify stat value, optionally level up and trigger regen

### 5. AdjustAffected
```
Input:  StatTag (FGameplayTag), ValueType (E_ValueType), Change (double)
Output: None
```
Implementation: Adjust stats that are affected by the given stat

### 6. AdjustAffectedStats
```
Input:  Stat (B_Stat*), Change (double), ValueType (E_ValueType)
Output: None
```
Implementation: Adjust stats based on a stat object's modifiers

### 7. SerializeStatsData
```
Input:  None
Output: None
Local:  LOCAL_StatsToSave (TArray<FStatInfo>), LOCAL_InstancedStats (TArray<FInstancedStruct>)
```
Implementation: Serialize current stats for saving

### 8. InitializeLoadedStats
```
Input:  LoadedStats (TArray<FStatInfo>)
Output: None
```
Implementation: Initialize stats from loaded save data

### 9. GetStatsForCategory
```
Input:  StatCategory (FGameplayTag)
Output: Stats (FGameplayTagContainer)
Local:  LOCAL_Stats (TArray<FGameplayTag>), LOCAL_StatsContainer (FGameplayTagContainer)
```
Implementation: Get all stats that belong to a category

### 10. AdjustLevel
```
Input:  Delta (int)
Output: None
```
Implementation: Adjust character level and broadcast OnLevelUpdated

### 11. IsStatMoreThan
```
Input:  StatTag (FGameplayTag), Threshold (double)
Output: ReturnValue (bool)
```
Implementation: Check if stat current value exceeds threshold

### 12. ToggleRegenForStat
```
Input:  StatTag (FGameplayTag), Stop (bool)
Output: None
```
Implementation: Enable/disable regeneration for a stat

---

## C++ TYPE MAPPINGS

| Blueprint Type | C++ Type |
|----------------|----------|
| B_Stat* | UObject* (or UB_Stat* if we create base class) |
| B_Stat_C | UObject |
| TSubclassOf<B_Stat> | TSubclassOf<UObject> |
| FStatInfo | FStatInfo (C++ struct in SLFStatTypes.h) |
| FStatOverride | FStatOverride (C++ struct in SLFStatTypes.h) |
| E_ValueType | E_ValueType (C++ enum in SLFStatTypes.h) |
| FGameplayTag | FGameplayTag |
| FGameplayTagContainer | FGameplayTagContainer |
| UDataTable* | UDataTable* |
| FInstancedStruct | FInstancedStruct |
| PDA_MovementSpeedData_C* | UDataAsset* |
| PDA_BaseCharacterInfo_C* | UDataAsset* |

---

## DEPENDENCIES

### External Blueprint Dependencies:
- B_Stat (Blueprint class) - stat object with StatInfo property
- BFL_Helper (Blueprint Function Library) - GetSaveLoadComponent
- AC_SaveLoadManager (Blueprint component) - OnDataLoaded event
- PDA_MovementSpeedData (Data Asset)
- PDA_BaseCharacterInfo (Data Asset)

### C++ Dependencies Already Created:
- FStatInfo, FStatOverride, FRegen, FStatBehavior (SLFStatTypes.h)
- E_ValueType (SLFStatTypes.h)

---

## MIGRATION STATUS

### Phase 1: Initial Attempt (FAILED)
- [x] Full C++ with properties + functions created
- [x] Compiled successfully
- [x] Migration script created
- [x] **FAILED**: Function name conflicts with Blueprint functions
  - Error: "The function name in node GetStat is already used"
  - Error: "Cannot override '::GetStat' at GetStat which was declared in a parent with a different signature"

### Phase 2: Properties-Only Approach (FAILED)
- [x] Restored Content from backup
- [x] Rewrote SoulslikeStatComponent.h - PROPERTIES ONLY
- [x] Rewrote SoulslikeStatComponent.cpp - minimal implementation
- [x] Compiled successfully (Dec 28, 2025)
- [x] **FAILED**: TMap type incompatibility
  - Error: "Only exactly matching structures are considered compatible"
  - Blueprint UserDefinedStruct `FStatOverride` != C++ USTRUCT `FStatOverride`
  - Blueprint `B_Stat*` != C++ `UObject*`

### Phase 3: Simple Properties Only (ABANDONED)
- Gave up too easily on TMap properties

### Phase 4: Full C++ with CoreRedirects (CURRENT)
- [x] Added CoreRedirects to DefaultEngine.ini for struct/enum type mapping
- [x] Restored ALL TMap properties in SoulslikeStatComponent.h
- [x] Two-phase migration script (B_Stat first, then AC_StatManager)
- [x] Compiled successfully (Dec 28, 2025)
- [ ] Run migration script
- [ ] Compile all Blueprints
- [ ] Runtime tested

### Key Insights
1. **C++ functions conflict with Blueprint functions** of the same name when reparenting
2. **CoreRedirects solve struct/enum type mismatches** - redirect Blueprint types to C++ types
3. **B_Stat must inherit from USLFStatObject** for TMap<TSubclassOf<USLFStatObject>, ...> compatibility
4. **Two-phase migration required**: B_Stat first, then AC_StatManager

### CoreRedirects Added (DefaultEngine.ini)
```ini
[CoreRedirects]
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FRegen.FRegen",NewName="/Script/SLFConversion.FRegen")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatBehavior.FStatBehavior",NewName="/Script/SLFConversion.FStatBehavior")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo",NewName="/Script/SLFConversion.FStatInfo")
+StructRedirects=(OldName="/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride",NewName="/Script/SLFConversion.FStatOverride")
+EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType",NewName="/Script/SLFConversion.E_ValueType")
```

Solution: Full C++ migration with CoreRedirects + two-phase reparenting.

## NEXT STEPS

1. Open Unreal Editor
2. Run migration script:
   ```python
   exec(open(r"C:\scripts\SLFConversion\migrate_statmanager_to_cpp.py").read())
   ```
3. This will reparent AC_StatManager to inherit from USoulslikeStatComponent
4. C++ properties will shadow Blueprint variables
5. Blueprint functions remain intact
6. Compile all Blueprints
7. Test in Play mode
