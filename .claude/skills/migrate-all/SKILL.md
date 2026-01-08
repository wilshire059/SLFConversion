---
name: migrate-all
description: Iterate through BLUEPRINT_MIGRATION_CHECKLIST.md and migrate every Blueprint to C++ one by one. Uses /slf-migration skill, follows 20-pass validation, handles dependencies, no reflection for structs. Triggers on "migrate all", "continue migration", "next blueprint". (project)
---

# Migrate All Blueprints - Iterative Migration Skill

## PURPOSE

Systematically migrate EVERY Blueprint in the project to C++, one by one, following the established 20-pass validation protocol.

---

## EXECUTION PROTOCOL

### Step 0: Load Current State
```
1. Read C:/scripts/SLFConversion/BLUEPRINT_MIGRATION_CHECKLIST.md
2. Find the FIRST item marked [ ] (not migrated)
3. Skip items marked [x] (done), [~] (partial), [!] (excluded)
4. Update TodoWrite with: "Migrating [BLUEPRINT_NAME]"
```

### Step 1: Check Dependencies FIRST

Before migrating the target Blueprint:

```
1. Read its JSON from Exports/BlueprintDNA_v2/[Category]/[NAME].json
2. Extract: parent class, interfaces, components used, data assets referenced
3. Verify ALL dependencies are already migrated (marked [x])
4. If any dependency is [ ], migrate IT first (recursive)
```

**Dependency Types:**
- Parent class (must be migrated or engine class)
- Components used (AC_* must be in C++)
- Interfaces implemented (need C++ UINTERFACE)
- Data assets referenced (PDA_*, DA_*)
- Other Blueprints called via interface

### Step 2: Invoke /slf-migration Skill
```
/slf-migration [BLUEPRINT_NAME]
```

This performs:
- Phase 0: Data Flow Analysis (trace all callers/callees)
- Passes 1-5: JSON extraction (exact types, signatures)
- Passes 6-10: C++ implementation (no stubs, no TODOs)
- Passes 11-15: Verification (match JSON node-by-node)
- Passes 16-20: Testing (compile, migrate, PIE)

### Step 3: Build & Compile

```bash
# Build C++
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild

# Must pass with 0 errors
```

### Step 4: Run Migration Script (RESILIENT 4-STEP)

The migration preserves Blueprint data. **Cache survives restores.**

```bash
# Step 4a: Extract data FROM BACKUP (one-time, if cache doesn't exist)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_data.py" ^
  -stdout -unattended -nosplash 2>&1

# Step 4b: Restore clean backup
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"

# Step 4c: Run reparenting migration
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1

# Step 4d: Apply cached data (icons, niagara, montages)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_icons_fixed.py" ^
  -stdout -unattended -nosplash 2>&1

"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_remaining_data.py" ^
  -stdout -unattended -nosplash 2>&1
```

**Expected Result:** 0 errors, icons/niagara/montages restored

### Step 5: PIE Test

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/Source/SLFConversion/Testing/SLFPIETestRunner.py" ^
  -stdout -unattended -nosplash 2>&1
```

**Expected Result:** 0 errors

### Step 6: Verify JSON Match

For each function in JSON:
- Verify C++ has same name, return type, parameters, specifiers
- Verify const-correctness matches
- Verify BlueprintCallable/BlueprintPure/BlueprintNativeEvent

For each variable in JSON:
- Verify C++ has same name, type, default value
- Verify UPROPERTY specifiers match (EditAnywhere, BlueprintReadWrite, etc.)

### Step 7: Update Checklist

In `BLUEPRINT_MIGRATION_CHECKLIST.md`, change:
```
| [ ] | BLUEPRINT_NAME | OriginalParent | - |
```
To:
```
| [x] | BLUEPRINT_NAME | OriginalParent | CppClassName |
```

### Step 8: Commit & Continue

```bash
git add -A
git commit -m "Migrate BLUEPRINT_NAME to C++

- Variables: X
- Functions: Y
- Event Dispatchers: Z
- All 20 passes validated
- PIE test: PASS"
```

Then return to Step 0 and get the next [ ] item.

---

## CRITICAL RULES (NON-NEGOTIABLE)

### 1. NO Reflection for Struct Properties
```cpp
// NEVER DO THIS
FProperty* Prop = FindPropertyByName(TEXT("ItemInformation"));
void* StructPtr = Prop->ContainerPtrToValuePtr<void>(Asset);

// ALWAYS DO THIS - Add property to C++ class
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

// Then reparent Blueprint and access directly
```

### 2. NO Stubs or TODOs
```cpp
// NEVER DO THIS
void MyFunction_Implementation()
{
    // TODO: Implement later
}

// ALWAYS - Full implementation matching JSON graph
```

### 3. Names Must Match EXACTLY
```cpp
// JSON: "IsSprinting"
bool IsSprinting;     // CORRECT
bool bIsSprinting;    // WRONG - breaks Blueprint callers
```

### 4. All Branch Logic Must Match
```cpp
// JSON: Branch with TRUE -> DoA(), FALSE -> DoB()
// WRONG:
if (condition) { DoA(); }  // Missing FALSE path!

// RIGHT:
if (condition) { DoA(); }
else { DoB(); }
```

### 5. Preserve ALL Defaults
```cpp
// JSON: "DefaultValue": 100.0
UPROPERTY(...)
double MaxHealth = 100.0;  // MUST match JSON
```

---

## MIGRATION ORDER (Hierarchy First)

```
1. Interfaces (BPI_*) - Need UINTERFACE definitions
2. Base Components (AC_*) - Character dependencies
3. Base Data Assets (PDA_*, B_Stat, B_Action, B_StatusEffect, B_Buff)
4. Derived Data Assets (B_HP, B_Action_Dodge, etc.)
5. Base Actors (B_Item, B_Interactable, B_BaseProjectile)
6. Derived Actors (B_Item_Weapon, B_Door, etc.)
7. Base Characters (B_BaseCharacter)
8. Derived Characters (B_Soulslike_Character, B_Soulslike_Enemy, etc.)
9. Widgets (W_*)
10. AnimBPs (ABP_*) - EventGraph clear only
11. AnimNotifies (AN_*, ANS_*)
12. AI Tasks/Services (BTT_*, BTS_*)
```

---

## SPECIAL HANDLING PATTERNS

| Pattern | Map | Behavior |
|---------|-----|----------|
| Full Migration | MIGRATION_MAP | Reparent + clear EventGraph |
| Keep Variables | KEEP_VARS_MAP | Reparent + keep vars (AnimBP PropertyAccess) |
| Reparent Only | REPARENT_ONLY_MAP | Reparent + keep EventGraph (external callers) |
| AnimBP | ANIM_BP_MAP | Clear EventGraph only (no reparent) |
| Data Instances | DA_* | No migration needed (data, not logic) |

---

## REFERENCE EXAMPLES

### B_PickupItem Pattern (Data Flow + Niagara)
- See: `DATA_FLOW_ANALYSIS.md`
- Pattern: KEEP_VARS_MAP + BeginPlay Niagara setup via reflection

### B_Action_Dodge Pattern (Function Migration)
- See: `Source/SLFConversion/Blueprints/B_Action_Dodge.cpp`
- Pattern: BlueprintNativeEvent with full logic implementation

### PDA_Item Pattern (C++ Property Migration)
- See: `cpp-property-migration.md`
- Pattern: Add UPROPERTY to C++, reparent, access directly

---

## ERROR RECOVERY

| Error | Solution |
|-------|----------|
| "Undefined identifier" | Add #include or forward declaration |
| "No matching function" | Check JSON for exact signature |
| "Pin mismatch" | Clear calling Blueprint's EventGraph |
| "Cast failed" | Verify MIGRATION_MAP entry |
| "Class not found" | Check path (no U prefix in script path) |

---

## SUCCESS CRITERIA

A Blueprint is FULLY MIGRATED when:

1. ✅ All variables in C++ with correct types and defaults
2. ✅ All functions in C++ with exact signatures
3. ✅ All event dispatchers declared
4. ✅ All graph logic implemented (no stubs)
5. ✅ C++ compiles with 0 errors
6. ✅ Migration script runs with 0 errors
7. ✅ PIE test passes with 0 errors
8. ✅ Checklist updated to [x]
9. ✅ Changes committed

---

## INVOCATION

```
/migrate-all
```

Or natural language:
- "Continue migrating blueprints"
- "What's the next blueprint to migrate?"
- "Migrate the remaining blueprints"
- "Start the migration process"
