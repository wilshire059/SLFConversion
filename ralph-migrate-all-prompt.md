# RALPH PROMPT: Migrate All SoulslikeFramework Blueprints to C++

## INSTRUCTIONS

You are tasked with systematically migrating EVERY Blueprint in the SoulslikeFramework project to C++. Work through them ONE BY ONE until ALL are complete.

---

## STEP 1: GET CURRENT TARGET

Read the file `C:/scripts/SLFConversion/BLUEPRINT_MIGRATION_CHECKLIST.md` and find the FIRST Blueprint marked with `[ ]` (not migrated).

Skip any marked:
- `[x]` = already migrated
- `[~]` = partial migration (intentional)
- `[!]` = excluded (crashes/special handling)

Once you find the first `[ ]` item, that is your current migration target.

---

## STEP 2: CHECK DEPENDENCIES

Before migrating the target, read its JSON export from:
`C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/[Category]/[BLUEPRINT_NAME].json`

Check the "ParentClass" and "Interfaces" sections. If ANY dependency is also marked `[ ]` in the checklist, migrate THAT dependency FIRST (recursive).

Dependency order must be:
1. Interfaces (BPI_*)
2. Base Components (AC_*)
3. Base Data Assets (PDA_*, B_Stat, B_Action, B_StatusEffect)
4. Derived Data Assets (B_HP, B_Action_Dodge, etc.)
5. Base Actors (B_Item, B_Interactable, B_BaseProjectile)
6. Derived Actors
7. Base Characters (B_BaseCharacter)
8. Derived Characters (B_Soulslike_*)
9. Widgets (W_*)
10. AnimBPs (ABP_*)
11. AnimNotifies (AN_*, ANS_*)
12. AI Tasks/Services (BTT_*, BTS_*)

---

## STEP 3: INVOKE THE MIGRATION SKILL

Run:
```
/slf-migration [BLUEPRINT_NAME]
```

This will perform the 20-pass validation protocol:
- Phase 0: Data Flow Analysis (trace callers/callees, identify runtime setup)
- Passes 1-5: Extract ALL variables, functions, dispatchers from JSON
- Passes 6-10: Implement in C++ (EXACT signatures, NO stubs, NO TODOs)
- Passes 11-15: Verify implementation matches JSON node-by-node
- Passes 16-20: Build, migrate, PIE test

---

## STEP 4: CRITICAL RULES TO FOLLOW

### 4.1 NEVER Use Reflection for Blueprint Struct Properties
```cpp
// WRONG - Complex reflection code
FProperty* Prop = Class->FindPropertyByName(TEXT("ItemInformation"));
void* StructPtr = Prop->ContainerPtrToValuePtr<void>(Object);
// ... 150 lines of nested traversal ...

// RIGHT - Add property to C++ class, reparent Blueprint, access directly
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

// Then just cast and access:
if (UPDA_Item* Item = Cast<UPDA_Item>(DataAsset))
{
    UNiagaraSystem* System = Item->WorldNiagaraSystem.LoadSynchronous();
}
```

### 4.2 NEVER Leave Stubs or TODOs
```cpp
// WRONG
void MyFunction_Implementation()
{
    // TODO: Implement later
}

// RIGHT - Complete implementation matching JSON graph
void UMyClass::MyFunction_Implementation()
{
    ACharacter* Owner = Cast<ACharacter>(GetOuter());
    if (!Owner) return;

    UActionManagerComponent* ActionMgr = Owner->FindComponentByClass<UActionManagerComponent>();
    if (!ActionMgr) return;

    // ... complete logic matching every JSON node ...
}
```

### 4.3 Names MUST Match EXACTLY
```cpp
// JSON says: "IsSprinting"
bool IsSprinting;     // CORRECT
bool bIsSprinting;    // WRONG - breaks all Blueprint callers!

// JSON says: "Hit Location" (with space)
UPROPERTY(meta = (DisplayName = "Hit Location"))
FVector HitLocation;  // Use DisplayName for editor, but property name has no space
```

### 4.4 ALL Branch Logic Must Match
```cpp
// If JSON shows: Branch node with TRUE -> DoA(), FALSE -> DoB()

// WRONG - Missing FALSE path
if (condition) { DoA(); }

// RIGHT - Both paths implemented
if (condition) { DoA(); }
else { DoB(); }
```

### 4.5 Preserve ALL Default Values
```cpp
// JSON: "DefaultValue": 100.0
UPROPERTY(EditAnywhere, BlueprintReadWrite)
double MaxHealth = 100.0;  // MUST match JSON default exactly
```

### 4.6 Match EXACT Function Signatures
```cpp
// JSON: {"Name": "CanExecuteAction", "ReturnType": "bool",
//        "Parameters": [{"Name": "OwnerActor", "Type": "AActor*"}],
//        "Specifiers": ["BlueprintNativeEvent", "BlueprintCallable"]}

// C++ must be EXACTLY:
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
bool CanExecuteAction(AActor* OwnerActor);
virtual bool CanExecuteAction_Implementation(AActor* OwnerActor);
```

---

## STEP 5: BUILD AND TEST

After implementing the C++ code:

### 5.1 Build C++
```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```
**MUST pass with 0 errors.**

### 5.2 Run Migration Script
```bash
# First restore clean backup
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"

# Then run migration
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1
```
**MUST show 0 errors on second pass.**

### 5.3 PIE Test
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/Source/SLFConversion/Testing/SLFPIETestRunner.py" ^
  -stdout -unattended -nosplash 2>&1
```
**MUST pass with 0 errors.**

---

## STEP 6: UPDATE CHECKLIST

Edit `BLUEPRINT_MIGRATION_CHECKLIST.md` and change the target's status from `[ ]` to `[x]`:

```markdown
| [x] | BLUEPRINT_NAME | OriginalParent | CppClassName |
```

---

## STEP 7: COMMIT AND CONTINUE

```bash
git add -A
git commit -m "Migrate [BLUEPRINT_NAME] to C++

- Variables: X
- Functions: Y
- Event Dispatchers: Z
- All 20 passes validated
- PIE test: PASS

🤖 Generated with Claude Code"
```

Then **RETURN TO STEP 1** and get the next `[ ]` item.

---

## SPECIAL HANDLING PATTERNS

Different Blueprints need different handling based on their role:

| Blueprint Type | Map in run_migration.py | What Happens |
|----------------|-------------------------|--------------|
| Normal | MIGRATION_MAP | Reparent to C++ + clear EventGraph |
| AnimBP-Referenced | KEEP_VARS_MAP | Reparent + KEEP variables (AnimBP PropertyAccess reads them) |
| External Callers | REPARENT_ONLY_MAP | Reparent + KEEP EventGraph (Level BP calls custom events) |
| AnimBPs | ANIM_BP_MAP | NO reparent, just clear EventGraph |
| Data Instances | DA_* | NO migration needed (data, not logic) |

---

## ERROR RECOVERY

| Error | Solution |
|-------|----------|
| "Undefined identifier" | Add #include for the header or forward declaration |
| "No matching function" | Check JSON for exact signature (parameters, return type, const) |
| "Pin mismatch" | Clear the calling Blueprint's EventGraph |
| "Cast failed" | Verify MIGRATION_MAP entry has correct C++ class path |
| "Class not found" | Check path format (use `PDA_Item` not `UPDA_Item` for script paths) |

---

## SUCCESS CRITERIA FOR EACH BLUEPRINT

A Blueprint is FULLY MIGRATED when ALL of these are true:

- [ ] All variables in C++ with correct types and defaults
- [ ] All functions in C++ with exact signatures
- [ ] All event dispatchers declared
- [ ] All graph logic implemented (no stubs, no TODOs)
- [ ] C++ compiles with 0 errors
- [ ] Migration script runs with 0 errors
- [ ] PIE test passes with 0 errors
- [ ] Checklist updated to [x]
- [ ] Changes committed

---

## TRACKING PROGRESS

Use TodoWrite to track your current position:

```
- [x] Completed: AC_StatManager
- [x] Completed: AC_InputBuffer
- [ ] Current: BPI_Interactable (migrating now)
- [ ] Next: BPI_Item
- [ ] Remaining: ~48 items
```

---

## REFERENCE FILES

| File | Purpose |
|------|---------|
| `BLUEPRINT_MIGRATION_CHECKLIST.md` | Master checklist of all Blueprints |
| `Exports/BlueprintDNA_v2/` | JSON exports (source of truth for variables/functions) |
| `run_migration.py` | Migration script with all maps |
| `.claude/skills/slf-migration/SKILL.md` | 20-pass validation protocol |
| `.claude/skills/slf-migration/DATA_FLOW_ANALYSIS.md` | Data flow patterns |
| `.claude/skills/cpp-property-migration.md` | C++ property migration pattern |

---

## BEGIN

Start by reading `BLUEPRINT_MIGRATION_CHECKLIST.md` and finding the first `[ ]` item. Then follow steps 2-7 for that item. When complete, return to step 1 for the next item. Continue until ALL items are `[x]`.

**DO NOT STOP until the entire checklist shows NO remaining `[ ]` items.**
