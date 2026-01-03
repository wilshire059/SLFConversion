# E_ValueType Enum Migration Tracker

## Status: IN PROGRESS - Investigating Python API Approach

## Problem Statement

Blueprint UserDefinedEnum values are stored as **strings** ("NewEnumerator0") in .uasset files.
C++ UENUM values are stored as **numeric** values (0).
CoreRedirects only redirect TYPE references, not VALUE serialization.

When Blueprint enum is deleted:
- Error: "Expected a valid unsigned number for a byte property"
- All Blueprints referencing the enum fail to compile

## Solution: Option B - Python Script to Fix Blueprint Nodes

### Phase 1: Analysis (COMPLETE)

**Enum Definition:**
```
NewEnumerator0 = 0 (DisplayName: "Current Value")
NewEnumerator1 = 1 (DisplayName: "Max Value")
```

**Storage Locations Identified:**

| Location Type | JSON Path | Example Value |
|--------------|-----------|---------------|
| Function Parameters | `Functions[].Inputs[].DefaultValue` | "NewEnumerator0" |
| Event Dispatcher Params | `EventDispatchers[].Parameters[].DefaultValue` | "NewEnumerator0" |
| Node Pin DefaultValue | `Nodes[].Pins[].DefaultValue` | "NewEnumerator0" |
| Switch Output Pin Names | `Nodes[].Pins[].Name` | "NewEnumerator0" |
| LinkedTo References | `Pins[].LinkedTo[].PinName` | "NewEnumerator0" |
| Struct Member Defaults | `Struct.Members[].DefaultValue` | "NewEnumerator0" |

**Affected Files (26 total):**
1. AC_StatManager.json - 15+ occurrences
2. B_Stat.json - Switch nodes + pins
3. AC_ActionManager.json
4. AC_AI_Boss.json
5. AC_AI_CombatManager.json
6. AC_CombatManager.json
7. AC_EquipmentManager.json
8. AN_AdjustStat.json
9. B_Action.json
10. B_Action_Dodge.json
11. B_Action_DrinkFlask_HP.json
12. B_Buff_AttackPower.json
13. B_ShowcaseEnemy_StatDisplay.json
14. B_StatusEffect.json
15. B_StatusEffect_Frostbite.json
16. B_StatusEffect_Plague.json (if exists)
17. FLevelChangeData.json (Struct)
18. FStatChange.json (Struct)
19. FStatChangePercent.json (Struct)
20. W_Boss_Healthbar.json
21. W_HUD.json
22. W_LevelUp.json
23. W_StatEntry.json
24. W_StatEntry_LevelUp.json
25. W_StatEntry_Status.json

### Phase 2: Python API Investigation (IN PROGRESS)

**Questions to Answer:**
1. Can Unreal Python access Blueprint graph nodes?
2. Can we modify pin default values via Python?
3. Can we access K2Node_SwitchEnum and modify output pin names?
4. What's the API for modifying struct member defaults?

**Potential APIs:**
- `unreal.BlueprintEditorLibrary`
- `unreal.EditorAssetLibrary`
- `unreal.load_asset()` + Blueprint reflection
- Direct property access via `get_editor_property()` / `set_editor_property()`

### Phase 3: Script Development (PENDING)

**Script Requirements:**
1. Load each affected Blueprint
2. Find all nodes/pins using E_ValueType
3. Convert string values to numeric values
4. Handle Switch node output pins specially
5. Save modified Blueprints
6. Comprehensive logging/debugging
7. Dry-run mode for testing
8. Reusable for other enum migrations

### Phase 4: Testing (PENDING)

**Test Procedure:**
1. Restore from Phase 1 backup
2. Run migration script in dry-run mode
3. Review logs for issues
4. Run migration script for real
5. Delete Blueprint E_ValueType enum
6. Compile all Blueprints
7. Test in Play mode

### Phase 5: Verification (PENDING)

- [ ] All 26 files compile without errors
- [ ] Switch on E_ValueType nodes work correctly
- [ ] AdjustStat functions work
- [ ] Damage/stamina/magic systems functional
- [ ] No runtime errors in Output Log

---

## Attempt Log

### Attempt 1: CoreRedirects (FAILED)
- Added EnumRedirects to DefaultEngine.ini
- Deleted Blueprint E_ValueType.uasset
- Result: "Expected a valid unsigned number for a byte property"
- Reason: CoreRedirects don't convert value serialization format

### Attempt 2: CoreRedirects with ValueChanges (FAILED)
- Added ValueChanges parameter to EnumRedirects
- Result: Same error
- Reason: ValueChanges maps old→new names, doesn't convert string→numeric

### Attempt 3: Python API Approach (IN PROGRESS)
- Investigating available APIs
- Need to determine if Blueprint graph manipulation is possible via Python

---

## Backup Reference

| Backup | Location | State |
|--------|----------|-------|
| Phase 1 Complete | `C:/scripts/SLFConversion_Migration/Backups/phase1_complete_20251229_110210` | Clean, working |
| Enum Migration | `C:/scripts/SLFConversion_Migration/Backups/enum_migration_backup` | Failed attempts |

**Restore Command:**
```powershell
rm -rf "C:/scripts/SLFConversion/Content" && cp -r "C:/scripts/SLFConversion_Migration/Backups/phase1_complete_20251229_110210/Content" "C:/scripts/SLFConversion/"
```

---

## Value Mapping

| String Name | Numeric Value | Display Name |
|-------------|---------------|--------------|
| NewEnumerator0 | 0 | Current Value |
| NewEnumerator1 | 1 | Max Value |

---

## Notes

- JSON exports are for ANALYSIS only - actual data is in binary .uasset files
- Must use Unreal Editor Python API to modify Blueprint data
- Switch node output pins are NAMED after enum values - this is complex to fix
- Consider alternative: Instead of numeric, maybe we can point to C++ enum values?

---

## Alternative Tool: UAssetAPI/UAssetGUI

**Discovered:** External .NET library for low-level .uasset manipulation

**Repository:** https://github.com/atenfyr/UAssetAPI

**Capabilities:**
- Read/write .uasset files at binary level (UE 4.13 to 5.3)
- Support for 100+ property types including enums
- JSON export/import with binary equality
- EnumProperty handling fixes in recent versions

**Relevant Fix from Releases:**
> "Fixed a bug where EnumProperties backed by integer types would not properly store the first enum entry when serialized as zero."

**Potential Approach:**
1. Export affected .uasset files to UAssetAPI JSON
2. Programmatically modify enum values from string to numeric
3. Re-import to .uasset
4. Test in Unreal Editor

**Pros:**
- Direct binary access - full control
- Can modify any property in the asset
- C#/.NET - well-documented API

**Cons:**
- External tool dependency
- Need to ensure UE 5.7 compatibility
- Risk of asset corruption if format changes

---

## Diagnostic Scripts Created

| Script | Purpose | Run Command |
|--------|---------|-------------|
| `diagnose_blueprint_api.py` | Test available Python APIs | `exec(open(r"C:\scripts\SLFConversion\diagnose_blueprint_api.py").read())` |
| `diagnose_enum_pins.py` | Test enum pin access | `exec(open(r"C:\scripts\SLFConversion\diagnose_enum_pins.py").read())` |

---

## Approaches Under Investigation

### Approach A: Custom C++ BlueprintFixerLibrary (CHOSEN)
- Extend existing BlueprintFixerLibrary with enum migration functions
- C++ can access EdGraphPin which Python cannot
- Status: **IMPLEMENTED** - Ready for testing

**New C++ Functions Added:**
| Function | Purpose |
|----------|---------|
| `FindEnumPins` | Find all pins referencing a specific enum (diagnostic) |
| `MigrateEnumReferences` | Update pin types from Blueprint enum to C++ enum |
| `FixSwitchOnEnumNodes` | Fix Switch on Enum nodes to use C++ enum |
| `MigrateEnumFull` | Complete migration (combines above functions) |

**Key Insight:** The existing `BlueprintFixerLibrary` from AC_InputBuffer migration already has C++ access to `UEdGraphPin`. We extended it with enum-specific functions.

### Approach B: UAssetAPI Binary Manipulation
- Use external .NET tool to modify .uasset files
- Full control over serialized data
- Status: **IDENTIFIED** - Need to test UE 5.7 compatibility

### Approach C: Text-Based .uasset Modification
- Treat .uasset as binary, find and replace patterns
- Extremely risky, no type safety
- Status: **LAST RESORT**

### Approach D: Hybrid - C++ Plugin + Python
- Create C++ plugin exposing pin modification APIs to Python
- Best of both worlds
- Status: **FALLBACK** - If Approaches A/B fail
