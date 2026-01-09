---
name: slf-migration
description: Migrate SoulslikeFramework Blueprints to C++ with 20-pass validation. Use when migrating Blueprint components, interfaces, or classes to C++. Triggers on "migrate", "migration", "Blueprint to C++", "write C++ for", or any SLF component name. (project)
---

# SoulslikeFramework Blueprint-to-C++ Migration

## CURRENT STATUS (Updated 2026-01-02)

| Metric | Value |
|--------|-------|
| **Blueprints Reparented** | 306 |
| **AnimBPs Migrated** | 3 (ABP_SoulslikeNPC, ABP_SoulslikeEnemy, ABP_SoulslikeBossNew) |
| **NPC Characters Migrated** | 3 (B_Soulslike_NPC, ShowcaseGuide, ShowcaseVendor) |
| **Widgets Migrated** | 100+ |
| **Components Migrated** | 20+ |
| **Migration Script** | `run_migration.py` with multi-phase support |

---

## ABSOLUTE REQUIREMENTS - NO EXCEPTIONS

1. **ALL LOGIC MOVES TO C++** - Blueprints become empty shells
2. **20-PASS VALIDATION** - Every variable, function, graph, macro, interface function
3. **NO SHORTCUTS** - Complete all passes, no skipping
4. **NO ASSUMPTIONS** - Always verify from JSON exports
5. **NO STUB IMPLEMENTATIONS** - Full logic required
6. **DATA FLOW ANALYSIS FIRST** - Identify runtime setup and reflection needs BEFORE coding

**Reference:** `C:\scripts\SLFConversion\DEFINITION_OF_DONE.md`

---

## Phase 0: Data Flow Analysis (MANDATORY)

**Perform this analysis BEFORE writing any C++ implementation code.**

See `DATA_FLOW_ANALYSIS.md` for detailed patterns and examples.

### 0A. Component Preservation Check

For each SCS component in the Blueprint:
- [ ] Is the component a visual element (Mesh, Niagara, Material)?
- [ ] Is the asset set in component defaults? (design-time)
- [ ] Is the asset set in EventGraph? (runtime)

**Decision:**
- Design-time defaults → `MIGRATION_MAP` (clear logic, keep defaults)
- Runtime setup → `KEEP_VARS_MAP` (preserve SCS) + reproduce in C++ `BeginPlay()`

### 0B. Runtime Setup Detection

Scan EventGraph JSON for these function calls:
- `SetAsset` (Niagara)
- `SetStaticMesh`
- `SetSkeletalMesh`
- `SetMaterial` / `SetMaterialByName`
- `SetBrushFromTexture`

For each found:
- [ ] Document the target component
- [ ] Trace the data source (where does the asset come from?)
- [ ] Flag for C++ reproduction

### 0C. Data Asset Dependency Analysis

For each `UDataAsset*` variable:
- [ ] What data asset class? (C++ `UPDA_*` or Blueprint `PDA_*`?)
- [ ] What struct properties does it contain?
- [ ] Are structs C++ (`FSLFItemInfo`) or Blueprint (`FItemInfo`)?
- [ ] Map the complete property path (Item → Info → SubInfo → Value)

### 0D. Reflection Requirements

If Blueprint structs detected:
- [ ] Document property names (may have GUID suffixes like `WorldPickupInfo_50_...`)
- [ ] Identify traversal order (outer → inner)
- [ ] Note which properties need prefix matching
- [ ] Reference `BLUEPRINT_STRUCT_EXTRACTION.md` for extraction patterns

---

## MULTI-PHASE MIGRATION (CRITICAL)

The migration script uses multi-phase processing to handle cascade loading issues. Assets must be processed in a specific order to avoid compilation errors during loading.

### The Problem: Cascade Loading
When loading a Blueprint, the engine loads all its dependencies and tries to compile them. If a dependency has pin mismatches (e.g., calling a function with changed signature), the entire load fails.

### The Solution: Multi-Phase Processing

```
Phase 0A: Clear Priority AnimNotifyStates FIRST
  - ANS_InputBuffer, ANS_RegisterAttackSequence
  - ANS_InvincibilityFrame, ANS_HyperArmor
  - These call functions with "?" suffix pins
  - Must be cleared BEFORE anything else loads them

Phase 0A2: Save Priority AnimNotifyStates
  - Persist changes immediately

Phase 0B: Clear AnimBlueprints
  - ABP_SoulslikeNPC, ABP_SoulslikeEnemy, ABP_SoulslikeBossNew
  - Keep variables (for AnimGraph compatibility)
  - Clear EventGraph only

Phase 0B2: Save AnimBlueprints

Phase 0C: Clear NPC Characters
  - B_Soulslike_NPC, ShowcaseGuide, ShowcaseVendor
  - Now safe because AnimBPs are already cleared

Phase 1+: Process all other Blueprints normally
```

### Why This Order?
1. AnimNotifyStates call C++ functions with "?" suffix pins (e.g., "Buffer Open?", "Is Invincible?")
2. C++ cannot use "?" in identifiers
3. UPARAM(DisplayName) changes display but NOT the FName used for pin matching
4. By clearing these assets FIRST and saving them, when AnimBPs load later they get the cleared versions
5. No pin mismatch errors because the function calls are gone

---

## KNOWN PIN NAMING ISSUES

### "?" Suffix Problem
Blueprint pins can have names like "Buffer Open?" or "Is Invincible?" but C++ parameters cannot contain "?".

**WRONG - Trying to match with DisplayName:**
```cpp
// UPARAM(DisplayName) changes display but NOT FName for pin matching
void ToggleBuffer(UPARAM(DisplayName = "Buffer Open?") bool BufferOpen);
// Pin matching still uses "BufferOpen", not "Buffer Open?"
```

**RIGHT - Clear the calling Blueprint's logic:**
The solution is to clear the logic from Blueprints that call these functions, not to try to match the pin names in C++.

### Solutions Applied
1. **Phase 0A Priority Processing** - Clear AnimNotifyStates before they're loaded
2. **Immediate Saves** - Persist cleared assets before processing dependencies
3. **MIGRATION_MAP ordering** - Critical assets processed first

---

## CRITICAL FILES

| Location | Purpose |
|----------|---------|
| `C:/scripts/SLFConversion/run_migration.py` | Multi-phase migration script |
| `C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/` | Blueprint JSON Exports (SOURCE OF TRUTH) |
| `C:/scripts/SLFConversion/MASTER_MIGRATION_TRACKER.md` | Progress tracking |
| `C:/scripts/SLFConversion/DEFINITION_OF_DONE.md` | Definition of Done + Example |
| `C:/scripts/SLFConversion/Source/SLFConversion/` | C++ Source |
| `C:/scripts/bp_only/` | Clean backup content (Blueprint-only, pre-migration) |

### Skill Guides

| Guide | Purpose |
|-------|---------|
| `EXTRACTION_GUIDE.md` | How to extract data from Blueprint JSON exports |
| `BLUEPRINT_STRUCT_EXTRACTION.md` | How to extract Blueprint struct data from FInstancedStruct at runtime |

### Key Sections in run_migration.py

```python
# Priority AnimNotifyStates - must be cleared FIRST
PRIORITY_ANIM_NOTIFY_STATES = {
    "ANS_InputBuffer": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_RegisterAttackSequence": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_InvincibilityFrame": "/Script/SLFConversion.SLFAnimNotifyStateInvincibility",
    "ANS_HyperArmor": "/Script/SLFConversion.SLFAnimNotifyStateHyperArmor",
}

# AnimBlueprints - clear EventGraph, keep variables for AnimGraph
ANIM_BP_MAP = {
    "ABP_SoulslikeNPC": "/Script/SLFConversion.SLFNPCAnimInstance",
    "ABP_SoulslikeEnemy": "/Script/SLFConversion.SLFEnemyAnimInstance",
    "ABP_SoulslikeBossNew": "/Script/SLFConversion.SLFBossAnimInstance",
}

# NPC Characters - must be processed AFTER AnimBPs
NPC_CHARACTER_MAP = {
    "B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",
}
```

---

## RUNNING THE MIGRATION (RESILIENT 4-STEP WORKFLOW)

The migration preserves Blueprint data (icons, Niagara, montages) that would otherwise be lost. **The cache survives restores.**

### Step 1: Extract Data FROM BACKUP (one-time)

Extract BEFORE restoring, while backup has valid data:

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_data.py" ^
  -stdout -unattended -nosplash 2>&1
```

### Step 2: SURGICAL Restore (NOT full backup)

**⚠️ DO NOT restore entire Content folder. Copy only the specific assets being migrated:**

```bash
# Copy ONLY the specific Blueprint(s) being migrated
cp "C:/scripts/bp_only/Content/[path]/[BLUEPRINT].uasset" \
   "C:/scripts/SLFConversion/Content/[path]/"
```

### Step 3: Run TARGETED Reparenting Migration

**Create a targeted migration script for only the assets being migrated:**

```bash
# Example: migrate_[category]_only.py
# See migrate_armor_only.py for pattern

"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/migrate_[category]_only.py" ^
  -stdout -unattended -nosplash 2>&1
```

**Result:** Only the specified assets are reparented. All other assets remain untouched.

### Build C++ First (if needed)

```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```

### Expected Output
- Migration: ~306 successful reparents, 0 errors
- Apply icons: "Applied icons to 21 items"
- Apply remaining: "Applied Niagara to 21 items", "Saved B_Action_Dodge"

---

## WIDGET BINDWIDGET ISSUES

Some widgets have `UPROPERTY(meta = (BindWidget))` for variables that were actually runtime-set in the original Blueprint.

### The Problem
```cpp
// BindWidget expects a designer-placed widget in the UMG hierarchy
UPROPERTY(meta = (BindWidget))
UW_GenericButton* ActiveQuitButton;
// ERROR: "A required widget binding 'ActiveQuitButton' was not found"
```

### The Solution
Change from BindWidget to regular UPROPERTY:
```cpp
// Runtime reference set by Blueprint logic - NOT a BindWidget
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
UW_GenericButton* ActiveQuitButton;
```

### Widgets Fixed
- `W_Settings.h` - ActiveQuitButton
- `W_Settings_QuitConfirmation.h` - ActiveBtn

---

## COMPLETE SCOPE - All Item Types

| Item Type | JSON Location | C++ Implementation |
|-----------|---------------|-------------------|
| **Variables** | `Variables.List` | `UPROPERTY` declarations |
| **Functions** | `Functions`, `Graphs` | `UFUNCTION` + `_Implementation` |
| **Event Dispatchers** | `EventDispatchers.List` | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` |
| **Interface Functions** | `Interfaces`, `ImplementedInterfaces` | `UINTERFACE` + `I<Name>` class |
| **Macros** | `MacroGraphs` | Inline C++ or helper functions |
| **Event Graphs** | `Graphs[EventGraph]` | `BeginPlay`, `Tick`, event handlers |
| **Function Graphs** | `Graphs[FunctionName]` | `_Implementation` function bodies |
| **Animation Graphs** | AnimBP `AnimGraph` | C++ AnimInstance with UPROPERTY variables |

---

## The 20-Pass Validation Protocol

### Phase 1: JSON Analysis (Passes 1-5)
| Pass | Focus |
|------|-------|
| 1 | Extract ALL items from JSON |
| 2 | Document EXACT types (Blueprint names) |
| 3 | Map ALL dependencies + **DATA ASSET SOURCES** (see Phase 0C) |
| 4 | Trace logic flow node-by-node + **IDENTIFY RUNTIME SETUP NODES** (see Phase 0B) |
| 5 | Identify edge cases + **FLAG REFLECTION REQUIREMENTS** (see Phase 0D) |

**IMPORTANT:** Passes 3-5 now include data flow analysis:
- Pass 3: For each data asset variable, trace where the data comes from and what struct types are involved
- Pass 4: Look for `SetAsset`, `SetMesh`, `SetMaterial` calls that indicate runtime component configuration
- Pass 5: Flag any Blueprint struct access that will require reflection (GUID suffix property names)

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

## Animation Blueprint Strategy

**For AnimBPs:** Keep variables, clear EventGraph only.

### What Happens
1. **EventGraph cleared** - All logic nodes removed
2. **Variables kept** - AnimGraph reads these via property access
3. **Reparented to C++ AnimInstance** - NativeUpdateAnimation() sets the variables
4. **AnimGraph unchanged** - Reads from the C++ UPROPERTY variables

### C++ AnimInstance Pattern
```cpp
UCLASS()
class USLFNPCAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:
    // Variables that AnimGraph reads
    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    float Speed;

    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    FVector LookAtLocation;

    // Called every frame - sets the variables
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
```

---

## Failure Modes (MUST CHECK)

### 1. Inverted Logic
```cpp
// WRONG
if (bBufferOpen) { ProcessNow(); }
// RIGHT
if (bBufferOpen) { QueueForLater(); }
else { ProcessNow(); }
```

### 2. Name Mismatch
```cpp
// Blueprint uses "IsSprinting" not "bIsSprinting"
bool IsSprinting;  // CORRECT
```

### 3. Cascade Loading Crash
If migration crashes with "Cast of nullptr to Function failed":
1. Check SKIP_LIST in run_migration.py
2. Add problematic Blueprint to skip list
3. Process it manually later

---

## Non-Negotiable Rules

1. **NO STUB IMPLEMENTATIONS** - `// TODO` is not acceptable
2. **NO ASSUMPTIONS** - Read JSON again if unsure
3. **NO SHORTCUTS** - All 20 passes for every item
4. **NO LEAVING LOGIC IN BLUEPRINT** - Everything moves to C++
5. **NO GIVING UP** - Find a way, it's always possible
6. **NEVER REVERT** - Don't fall back to "leave in Blueprint because complex"

---

## CRITICAL: Error Response Mindset

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

## Case Study: B_PickupItem (Data Flow Analysis Example)

This example demonstrates what Phase 0 analysis should have caught BEFORE implementation.

### The Problem

B_PickupItem had a World Niagara component that wasn't showing after migration.

**Initial (Wrong) Approach:**
1. Assumed `ItemInfo` struct had the Niagara system reference
2. Wrote C++ that read from empty struct
3. Niagara component remained unconfigured

**Why It Failed:**
The `ItemInfo` member variable was a C++ struct (`FSLFItemInfo`), but the actual data was in the `Item` data asset (Blueprint struct `FItemInfo`). These are DIFFERENT types.

### Correct Phase 0 Analysis

**Phase 0A - Component Preservation Check:**
```
SCS Component: "World Niagara" (UNiagaraComponent)
Asset in defaults: NO (null)
Asset set in EventGraph: YES ("Set Niagara System Asset" node)
→ Use KEEP_VARS_MAP + reproduce in BeginPlay
```

**Phase 0B - Runtime Setup Detection:**
```
Found: K2Node_CallFunction "SetAsset" targeting Niagara component
Source traced: Item → ItemInformation → WorldPickupInfo → WorldNiagaraSystem
```

**Phase 0C - Data Asset Analysis:**
```
Variable: Item (UDataAsset*)
  └─ Class: PDA_Item_C (Blueprint data asset - path has /Game/)
  └─ Property: ItemInformation
       └─ Type: FItemInfo (Blueprint struct - not C++ FSLFItemInfo!)
       └─ Nested: WorldPickupInfo (FWorldMeshInfo)
            └─ Nested: WorldNiagaraSystem (TSoftObjectPtr<UNiagaraSystem>)
```

**Phase 0D - Reflection Requirements:**
```
⚠️ REFLECTION REQUIRED
  - PDA_Item is Blueprint-derived
  - FItemInfo is Blueprint struct (property names have GUID suffixes)
  - Must use TFieldIterator + prefix matching
  - Property path: ItemInformation.WorldPickupInfo*.WorldNiagaraSystem*
```

### The Correct Implementation

See `DATA_FLOW_ANALYSIS.md` for the complete C++ code pattern.

The key insight: **Always trace data to its SOURCE before writing code.**
