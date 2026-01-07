# Migrate All Blueprints Command

## COMMAND: /migrate-all

Systematically migrate every Blueprint in `BLUEPRINT_MIGRATION_CHECKLIST.md` to C++.

---

## EXECUTION PROTOCOL

### Step 0: Load Current State
```
1. Read BLUEPRINT_MIGRATION_CHECKLIST.md
2. Find the FIRST item marked [ ] (not migrated)
3. That is your current target
4. Update TodoWrite with: "Migrating [BLUEPRINT_NAME]"
```

### Step 1: Invoke Migration Skill
```
/slf-migration [BLUEPRINT_NAME]
```

This skill performs:
- Phase 0: Data Flow Analysis (trace all callers/callees)
- 20-pass validation
- JSON extraction from Exports/BlueprintDNA_v2/
- Full C++ implementation

### Step 2: Dependency Check (BEFORE writing any code)

```python
# Check if dependencies are migrated first
dependencies = extract_dependencies_from_json(blueprint_json)
for dep in dependencies:
    if dep not in MIGRATION_MAP and dep not in KEEP_VARS_MAP:
        print(f"BLOCKED: Must migrate {dep} first")
        # Add dep to front of queue and migrate it first
```

**Dependency Types to Check:**
- Parent class (must be migrated or engine class)
- Components used (AC_* must be in C++)
- Interfaces implemented (need C++ UINTERFACE)
- Data assets referenced (PDA_*, DA_*)
- Other Blueprints called via interface

### Step 3: JSON Analysis (20-Pass Protocol)

**Passes 1-5: Extraction**
```
1. Read: Exports/BlueprintDNA_v2/[Category]/[BLUEPRINT_NAME].json
2. Extract ALL variables with EXACT types
3. Extract ALL functions with EXACT signatures
4. Extract ALL event dispatchers
5. Map ALL graph nodes (EventGraph, FunctionGraphs)
```

**Critical Checks:**
- Variable names MUST match exactly (IsSprinting not bIsSprinting)
- Function signatures MUST match exactly (all parameters, return types)
- Default values MUST be preserved
- UPROPERTY specifiers MUST match (EditAnywhere, BlueprintReadWrite, etc.)

### Step 4: C++ Implementation

**NEVER use reflection to access Blueprint struct properties.**

**Pattern: C++ Property Migration (from B_PickupItem example)**
```cpp
// 1. Add UPROPERTY to C++ class
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

// 2. Access directly after reparenting
if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
{
    UNiagaraSystem* System = ItemData->WorldNiagaraSystem.LoadSynchronous();
}
```

**Pattern: Data Table Migration**
```cpp
// Define row struct in C++
USTRUCT(BlueprintType)
struct FMyTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag Tag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UAnimMontage> Montage;
};
```

**Pattern: Struct Property Access**
```cpp
// WRONG - Reflection (DO NOT USE)
FProperty* Prop = Class->FindPropertyByName(TEXT("MyStruct"));
// 150 lines of fragile reflection code...

// RIGHT - Direct C++ property
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FMyStruct MyStruct;

// Direct access
MyStruct.SomeValue = NewValue;
```

### Step 5: Function Migration (from B_Action_Dodge example)

**BlueprintNativeEvent Pattern:**
```cpp
// Header
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
bool CanExecuteAction(AActor* OwnerActor);
virtual bool CanExecuteAction_Implementation(AActor* OwnerActor);

UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
void ExecuteAction(AActor* OwnerActor);
virtual void ExecuteAction_Implementation(AActor* OwnerActor);

// Implementation - MUST match JSON graph logic exactly
bool UMyAction::CanExecuteAction_Implementation(AActor* OwnerActor)
{
    // Trace JSON nodes step by step
    // Node 1: Get ActionManager component
    // Node 2: Check IsActionLocked
    // Node 3: Check stamina cost
    // ... match EVERY branch (TRUE and FALSE paths)
}
```

### Step 6: Build & Compile

```bash
# Build C++
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild

# Must pass with 0 errors
```

### Step 7: Run Migration Script

```bash
# Restore clean backup first
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"

# Run migration
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1
```

**Expected Result:** 0 errors on second pass

### Step 8: PIE Test

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/Source/SLFConversion/Testing/SLFPIETestRunner.py" ^
  -stdout -unattended -nosplash 2>&1
```

**Expected Result:** 0 errors

### Step 9: Verify JSON Match

```python
# Compare C++ implementation against JSON
for func in json_functions:
    cpp_func = find_cpp_function(func.name)
    assert cpp_func.return_type == func.return_type
    assert cpp_func.parameters == func.parameters
    assert cpp_func.specifiers == func.specifiers  # BlueprintCallable, etc.

for var in json_variables:
    cpp_var = find_cpp_variable(var.name)
    assert cpp_var.type == var.type
    assert cpp_var.default_value == var.default_value
    assert cpp_var.specifiers == var.specifiers  # EditAnywhere, etc.
```

### Step 10: Update Checklist

```markdown
# In BLUEPRINT_MIGRATION_CHECKLIST.md, change:
| [ ] | BLUEPRINT_NAME | OriginalParent | - |
# To:
| [x] | BLUEPRINT_NAME | OriginalParent | CppClassName |
```

### Step 11: Commit & Continue

```bash
git add -A
git commit -m "Migrate BLUEPRINT_NAME to C++

- Variables: X
- Functions: Y
- Event Dispatchers: Z
- All 20 passes validated
- PIE test: PASS

🤖 Generated with Claude Code"
```

**Then return to Step 0 and get the next [ ] item.**

---

## CRITICAL RULES (NON-NEGOTIABLE)

### 1. NO Reflection for Struct Properties
```cpp
// NEVER DO THIS
FProperty* Prop = FindPropertyByName(TEXT("ItemInformation"));
void* StructPtr = Prop->ContainerPtrToValuePtr<void>(Asset);
// ... complex nested reflection ...

// ALWAYS DO THIS
// Add property to C++ class, reparent Blueprint, access directly
```

### 2. NO Stubs or TODOs
```cpp
// NEVER DO THIS
void MyFunction_Implementation()
{
    // TODO: Implement later
}

// ALWAYS DO THIS
void MyFunction_Implementation()
{
    // Full implementation matching JSON graph
    ACharacter* Owner = Cast<ACharacter>(GetOuter());
    if (!Owner) return;

    UActionManagerComponent* ActionMgr = Owner->FindComponentByClass<UActionManagerComponent>();
    if (!ActionMgr) return;

    // ... complete logic ...
}
```

### 3. NO Assumptions - Verify from JSON
```cpp
// NEVER assume a function signature
// ALWAYS read the JSON first:
// {
//   "Name": "GetStatPercent",
//   "ReturnType": "double",
//   "Parameters": [{"Name": "StatInfo", "Type": "FStatInfo"}],
//   "Specifiers": ["BlueprintCallable", "BlueprintPure"]
// }

// Then implement EXACTLY:
UFUNCTION(BlueprintCallable, BlueprintPure)
double GetStatPercent(const FStatInfo& StatInfo);
```

### 4. Branch Logic MUST Match
```cpp
// JSON shows: Branch node with TRUE -> DoA(), FALSE -> DoB()
// WRONG:
if (condition) { DoA(); }  // Missing FALSE path!

// RIGHT:
if (condition) { DoA(); }
else { DoB(); }
```

### 5. Preserve ALL Defaults
```cpp
// JSON: "DefaultValue": 100.0
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double MaxHealth = 100.0;  // MUST match JSON default
```

### 6. Name Matching is EXACT
```cpp
// JSON: "IsSprinting"
bool IsSprinting;     // CORRECT
bool bIsSprinting;    // WRONG - breaks Blueprint callers

// JSON: "Hit Location" (with space)
// Can't have space in C++ - use DisplayName:
UPROPERTY(meta = (DisplayName = "Hit Location"))
FVector HitLocation;
```

---

## MIGRATION ORDER (Hierarchy First)

```
1. Engine Base Classes (already done - GameMode, etc.)
2. Interfaces (BPI_*)
3. Base Components (AC_*)
4. Base Data Assets (PDA_*, B_Stat, B_Action, B_StatusEffect)
5. Derived Data Assets (B_HP, B_Action_Dodge, etc.)
6. Base Actors (B_Item, B_Interactable, B_BaseProjectile)
7. Derived Actors (B_Item_Weapon, B_Door, etc.)
8. Base Characters (B_BaseCharacter)
9. Derived Characters (B_Soulslike_Character, B_Soulslike_Enemy, etc.)
10. Widgets (W_*)
11. AnimBPs (ABP_*)
12. AnimNotifies (AN_*, ANS_*)
13. AI Tasks/Services (BTT_*, BTS_*)
```

---

## HANDLING SPECIAL CASES

### AnimBPs (EventGraph Clear Only)
```python
# Don't reparent - just clear EventGraph
# Variables are kept for AnimGraph PropertyAccess
ANIM_BP_MAP = {
    "ABP_Name": None,  # None = clear only, no reparent
}
```

### KEEP_VARS_MAP (Preserve Variables for AnimBP)
```python
# Reparent but keep variables
# These components are read by AnimBP PropertyAccess
KEEP_VARS_MAP = {
    "AC_ActionManager": "/Script/SLFConversion.AC_ActionManager",
}
```

### REPARENT_ONLY_MAP (Keep EventGraph)
```python
# Reparent but DON'T clear EventGraph
# External Blueprints call custom events with spaces in names
REPARENT_ONLY_MAP = {
    "W_Dialog": "/Script/SLFConversion.W_Dialog",
}
```

### Data Asset Instances (DA_*)
```
DA_* assets are DATA, not LOGIC.
- They inherit from PDA_* (the class)
- They contain property values, not functions
- Migration = reparent PDA_* class, DA_* instances auto-update
```

---

## RECOVERY FROM ERRORS

### Compile Error: "Undefined identifier"
```
1. Check if dependency is migrated
2. Add #include for the header
3. Add forward declaration if circular
```

### Compile Error: "No matching function"
```
1. Check JSON for exact signature
2. Verify parameter types match
3. Verify const-correctness
```

### Runtime Error: "Cast failed"
```
1. Verify Blueprint is reparented to C++ class
2. Check class hierarchy matches
3. Verify MIGRATION_MAP entry is correct
```

### Migration Error: "Pin mismatch"
```
1. Blueprint calls function with special characters (?, !, space)
2. Clear the calling Blueprint's EventGraph
3. Or add to PRIORITY processing list
```

---

## SUCCESS CRITERIA

A Blueprint is FULLY MIGRATED when:

1. ✅ All variables in C++ with correct types and defaults
2. ✅ All functions in C++ with exact signatures
3. ✅ All event dispatchers declared
4. ✅ All graph logic implemented (no stubs)
5. ✅ C++ compiles with 0 errors
6. ✅ Migration script runs with 0 errors
7. ✅ PIE test passes
8. ✅ Checklist updated to [x]
9. ✅ Changes committed

---

## START COMMAND

To begin the migration process:

```
Read BLUEPRINT_MIGRATION_CHECKLIST.md and find the first [ ] item.
Then run: /slf-migration [BLUEPRINT_NAME]
Follow the 20-pass validation protocol.
Update checklist when complete.
Continue to next [ ] item.
```
