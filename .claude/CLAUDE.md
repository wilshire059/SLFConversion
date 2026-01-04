# SLFConversion Project - Claude Code Context

---

## ⚠️ CONTEXT COMPACTION RECOVERY - READ FIRST ⚠️

**If your context was just compacted, do this:**

1. Read `C:/scripts/SLFConversion/migration_tracker.md` for current status
2. Check the "Priority Queue" for the current item to work on
3. Use `/slf-migration [BLUEPRINT_NAME]` to extract and implement logic
4. Complete 20-pass validation for each item
5. Update migration_tracker.md before ending session

---

## CRITICAL MIGRATION RULES (NON-NEGOTIABLE)

1. **ALL Blueprint logic MUST be in C++** - NO stubs, NO TODOs, NO "implement later"
2. **Use `/slf-migration` skill** - This extracts actual Blueprint graph logic
3. **Migrate dependencies first** - Check migration_tracker.md for order
4. **Preserve ALL defaults** - Mesh assignments, skeleton, component settings
5. **AnimGraphs are PRESERVED** - Only clear EventGraph in AnimBPs

---

## Quick Status (Update Before Ending Session)

- **Completed**: AC_InteractionManager ✓
- **Current**: AC_CombatManager
- **Next**: AC_InputBuffer
- **Blocked**: B_Soulslike_Character (waiting on 5 remaining AC_* components)
- **Tracker**: `C:/scripts/SLFConversion/migration_tracker.md`

---

## CURRENT MIGRATION STATUS (Updated 2026-01-02)

| Metric | Value |
|--------|-------|
| **Blueprints Reparented** | 306 |
| **AnimBPs Migrated** | 3 (ABP_SoulslikeNPC, ABP_SoulslikeEnemy, ABP_SoulslikeBossNew) |
| **NPC Characters Migrated** | 3 |
| **Widgets Migrated** | 100+ |
| **Components Migrated** | 20+ |
| **Migration Script** | `run_migration.py` with multi-phase support |

---

## MULTI-PHASE MIGRATION (CRITICAL - READ THIS FIRST)

The migration script uses **multi-phase processing** to handle cascade loading issues. Assets must be processed in a specific order.

### The Problem: Cascade Loading
When loading a Blueprint, UE5 loads all dependencies and tries to compile them. If a dependency has pin mismatches, the entire load fails with crashes like "Cast of nullptr to Function failed".

### The Solution: Multi-Phase Processing

```
Phase 0A: Clear Priority AnimNotifyStates FIRST
  - ANS_InputBuffer, ANS_RegisterAttackSequence
  - ANS_InvincibilityFrame, ANS_HyperArmor
  - These call functions with "?" suffix pins
  - MUST be cleared before anything else loads them

Phase 0A2: Save Priority AnimNotifyStates immediately

Phase 0B: Clear AnimBlueprints
  - ABP_SoulslikeNPC, ABP_SoulslikeEnemy, ABP_SoulslikeBossNew
  - Keep variables (AnimGraph reads them)
  - Clear EventGraph only

Phase 0B2: Save AnimBlueprints immediately

Phase 0C: Clear NPC Characters
  - B_Soulslike_NPC, ShowcaseGuide, ShowcaseVendor
  - Now safe because AnimBPs are already cleared

Phase 1+: Process all other Blueprints normally
```

### Why This Order?
1. AnimNotifyStates call C++ functions with "?" suffix pins (e.g., "Buffer Open?")
2. C++ cannot use "?" in identifiers
3. UPARAM(DisplayName) changes display but NOT the FName used for pin matching
4. By clearing these assets FIRST and saving, later loads get the cleared versions
5. No pin mismatch errors because the function calls are gone

---

## RUNNING THE MIGRATION

### Fresh Migration (Recommended)
```bash
# 1. Restore backup
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"

# 2. Run migration
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1
```

### Build C++ First (if needed)
```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```

### Expected Output
- 465 pre-existing errors at startup (from backup content - this is normal)
- "Successfully reparented X to Y" messages
- ~306 successful reparents total
- Migration completes in ~15 seconds

---

## CRITICAL FILES

| File | Purpose |
|------|---------|
| `run_migration.py` | Multi-phase migration script (main entry point) |
| `Exports/BlueprintDNA_v2/*.json` | Source of truth for Blueprint logic |
| `Source/SLFConversion/` | C++ implementation |
| `C:\scripts\SLFConversion_Migration\Backups\blueprint_only\` | Clean backup content |

### Key Sections in run_migration.py

```python
PRIORITY_ANIM_NOTIFY_STATES = {...}  # Cleared FIRST
ANIM_BP_MAP = {...}                   # Cleared SECOND
NPC_CHARACTER_MAP = {...}             # Cleared THIRD
MIGRATION_MAP = {...}                 # All other Blueprints
```

---

## KNOWN ISSUES AND SOLUTIONS

### 1. "?" Suffix Pin Problem
Blueprint pins can have names like "Buffer Open?" but C++ cannot use "?".

**Solution:** Clear the calling Blueprint's logic (don't try to match pin names).

### 2. BindWidget Errors
Some widgets have `meta = (BindWidget)` for runtime-set variables.

**Solution:** Change to regular UPROPERTY:
```cpp
// WRONG
UPROPERTY(meta = (BindWidget))
UW_GenericButton* ActiveBtn;

// RIGHT
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
UW_GenericButton* ActiveBtn;
```

### 3. Cascade Loading Crash
If migration crashes with "Cast of nullptr to Function failed":
1. Add Blueprint to SKIP_LIST in run_migration.py
2. Or add to priority processing (PRIORITY_ANIM_NOTIFY_STATES)

### 4. Blueprint Struct Property Access
**NEVER use reflection to access Blueprint struct properties with GUID-suffixed names.**

**ALWAYS use C++ property migration instead:**
1. Add equivalent UPROPERTY to C++ parent class
2. Reparent Blueprint to C++ class
3. Migrate data from Blueprint to C++ properties
4. Access via direct Cast<>

See: `.claude/skills/cpp-property-migration.md` for complete workflow.

---

## C++ PROPERTY MIGRATION (CRITICAL PATTERN)

When C++ code needs data from Blueprint struct properties (e.g., `ItemInformation.WorldNiagaraSystem`):

### ❌ WRONG: Reflection (DO NOT USE)
```cpp
// NEVER DO THIS - Complex, fragile, slow
FProperty* Prop = Class->FindPropertyByName(TEXT("ItemInformation"));
// ...150 lines of nested struct traversal with GUID matching...
```

### ✅ RIGHT: C++ Property Migration
```cpp
// Add UPROPERTY to C++ class
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

// Direct access after reparenting + data migration
if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
{
    UNiagaraSystem* System = ItemData->WorldNiagaraSystem.LoadSynchronous();
}
```

### Migration Workflow
1. **Extract data** from Blueprint using `export_text()` + regex
2. **Reparent** Blueprint via `BlueprintEditorLibrary.reparent_blueprint()`
3. **Apply data** to C++ properties via `set_editor_property()`
4. **Access directly** via `Cast<CppClass>()`

### Python Naming Conventions
```python
# C++ class UPDA_Item → Python path (no U prefix)
unreal.load_class(None, "/Script/SLFConversion.PDA_Item")

# C++ property WorldNiagaraSystem → Python (snake_case)
asset.set_editor_property('world_niagara_system', value)
```

### Verified Working: PDA_Item Migration
- 21 items migrated from Blueprint `ItemInformation.WorldNiagaraSystem` to C++ `UPDA_Item::WorldNiagaraSystem`
- Scripts: `extract_niagara_paths.py`, `reparent_pda_item.py`, `apply_niagara_paths.py`
- Result: Direct C++ access, no reflection fallback

---

## THE MISSION

Migrate ALL SoulslikeFramework Blueprint logic to native C++. Every variable, function, graph, macro, interface function, and event dispatcher must be implemented in C++ with full 20-pass validation.

### What "DONE" Means
An item is DONE when:
1. ALL logic is in C++ (no Blueprint execution nodes)
2. All 20 validation passes complete
3. C++ compiles (pass 16)
4. Blueprint callers compile (pass 18)
5. PIE test passes (pass 19)
6. Tracker updated with notes (pass 20)

---

## NON-NEGOTIABLE RULES

### NEVER DO THESE:
- Leave logic in Blueprint "because it's complex"
- Skip migration "because it's not possible"
- Make assumptions without verifying from JSON exports
- Write stub implementations with `// TODO`
- Give up on type compatibility issues
- **Use reflection to access Blueprint struct properties** - Always migrate to C++ properties instead

### ALWAYS DO THESE:
- Read the JSON export BEFORE writing any code
- Complete ALL 20 passes for EVERY item
- Verify branch logic (TRUE/FALSE) matches exactly
- Find a way when something seems impossible

---

## CRITICAL BUG WATCH

### #1 Bug: Inverted Logic
```cpp
// WRONG - inverted logic
if (bBufferOpen) { ProcessNow(); }

// RIGHT - matches Blueprint
if (bBufferOpen) { QueueForLater(); }
else { ProcessNow(); }
```

### #2 Bug: Name Mismatch
```cpp
// Blueprint uses "IsSprinting" not "bIsSprinting"
bool IsSprinting;  // CORRECT
bool bIsSprinting; // WRONG - will break callers
```

### #3 Bug: Signature Mismatch
```cpp
// Callers expect (Item, Count, bTriggerLootUi)
void AddItem(UDataAsset* Item, int32 Count, bool bTriggerLootUi); // CORRECT
void AddItem(UDataAsset* Item, int32 Amount);                      // WRONG
```

---

## ANIMATION BLUEPRINT STRATEGY

**For AnimBPs:** Keep variables, clear EventGraph only, reparent to C++ AnimInstance.

### What Happens
1. **EventGraph cleared** - All logic nodes removed
2. **Variables kept** - AnimGraph reads these via property access
3. **Reparented to C++ AnimInstance** - NativeUpdateAnimation() sets the variables
4. **AnimGraph unchanged** - Continues to read from the (now C++) UPROPERTY variables

### C++ AnimInstance Pattern
```cpp
UCLASS()
class USLFNPCAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    float Speed;

    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    FVector LookAtLocation;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
```

---

## ERROR RESPONSE MINDSET

### Pin Errors = Migration Needed
```
ERROR: "In use pin 'SomeProperty' no longer exists"
```
**RIGHT Response:** "This caller Blueprint needs migration. Clear its logic."

### Function Not Found = Migration Needed
```
ERROR: "Cannot find function 'GetSomeValue'"
```
**RIGHT Response:** "Clear the calling Blueprint's logic to remove this call."

**THE GOAL IS TO REMOVE ALL BLUEPRINT LOGIC, NOT FIX PINS.**

---

## THE 20-PASS VALIDATION PROTOCOL

### Phase 1: JSON Analysis (Passes 1-5)
| Pass | Focus |
|------|-------|
| 1 | Extract ALL items from JSON |
| 2 | Document EXACT types (Blueprint names) |
| 3 | Map ALL dependencies |
| 4 | Trace logic flow node-by-node |
| 5 | Identify edge cases |

### Phase 2: Implementation (Passes 6-10)
| Pass | Focus |
|------|-------|
| 6 | Write UPROPERTY declarations |
| 7 | Write UFUNCTION declarations |
| 8 | Write function implementations |
| 9 | Write event dispatchers |
| 10 | Add debug logging |

### Phase 3: Verification (Passes 11-15)
| Pass | Focus |
|------|-------|
| 11 | Re-read JSON, compare node-by-node |
| 12 | Verify ALL branches (TRUE/FALSE) |
| 13 | Verify ALL types |
| 14 | Verify ALL names |
| 15 | Verify ALL signatures |

### Phase 4: Testing (Passes 16-20)
| Pass | Focus |
|------|-------|
| 16 | Compile C++ |
| 17 | Run migration script |
| 18 | Compile Blueprints |
| 19 | PIE test |
| 20 | Final review |

---

## SCOPE: ALL LOGIC-BEARING ITEMS

| Item Type | JSON Location | C++ Implementation |
|-----------|---------------|-------------------|
| **Variables** | `Variables.List` | `UPROPERTY` declarations |
| **Functions** | `Functions`, `Graphs` | `UFUNCTION` + `_Implementation` |
| **Event Dispatchers** | `EventDispatchers.List` | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` |
| **Interface Functions** | `Interfaces` | `UINTERFACE` + `I<Name>` class |
| **Macros** | `MacroGraphs` | Inline C++ or helper functions |
| **Event Graphs** | `Graphs[EventGraph]` | `BeginPlay`, `Tick`, handlers |
| **Function Graphs** | `Graphs[FunctionName]` | `_Implementation` bodies |
| **Animation Graphs** | AnimBP | C++ AnimInstance with UPROPERTY variables |

---

## HEADLESS EXECUTION

**ALWAYS use headless mode - never ask user to open editor!**

All migration and testing is done via command line:
- `UnrealEditor-Cmd.exe` with `-run=pythonscript`
- Build via `Build.bat`
- Tests via `run_tests.bat`
