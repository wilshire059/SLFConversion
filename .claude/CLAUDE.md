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
6. **SURGICAL MIGRATION ONLY** - See warning below

---

## ⚠️ SURGICAL vs BROAD MIGRATION - CRITICAL ⚠️

**NEVER do a full Content/ restore and migration.** This causes regressions in working Blueprints.

### ❌ WRONG: Broad Migration (DO NOT DO THIS)
```bash
# DO NOT restore entire Content folder
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' ...

# DO NOT run full migration script on everything
python run_migration.py  # Touches ALL Blueprints
```

### ✅ RIGHT: Surgical Migration
```bash
# 1. Copy ONLY the specific assets you're migrating
cp "C:/scripts/bp_only/Content/.../DA_ExampleArmor.uasset" "C:/scripts/SLFConversion/Content/.../"

# 2. Run targeted migration script for ONLY those assets
python migrate_armor_only.py  # Only touches armor data assets
```

### Surgical Migration Pattern

1. **Make C++ changes** - Add properties, functions, update structs
2. **Extract data from backup** - Cache mesh/icon/stat data BEFORE restore
3. **Copy ONLY affected assets** - From backup to working project
4. **Reparent ONLY those assets** - Targeted Python script
5. **Apply cached data** - Restore mesh/icon/stat values
6. **Test the specific feature** - Don't break unrelated features

### Example: Armor Mesh Migration (Surgical)

```bash
# Step 1: C++ changes (SLFGameTypes.h, AC_EquipmentManager.h/cpp)
# Step 2: Extract was already done (migration_cache/armor_mesh_data.json)

# Step 3: Copy ONLY armor assets from backup
cp "C:/scripts/bp_only/Content/SoulslikeFramework/Data/Items/DA_ExampleArmor.uasset" \
   "C:/scripts/SLFConversion/Content/SoulslikeFramework/Data/Items/"
# ... repeat for each armor asset

# Step 4-5: Run surgical migration script
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/migrate_armor_only.py" ^
  -stdout -unattended -nosplash
```

**Result:** Only armor assets are touched. Widgets, characters, weapons, etc. remain untouched and working.

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

## RUNNING THE MIGRATION (RESILIENT 4-STEP WORKFLOW)

> **⚠️ WARNING: This section describes the INITIAL FULL migration.**
> **For ongoing changes, use SURGICAL MIGRATION instead (see above).**
> **Only use full migration when starting fresh or recovering from major issues.**

The migration process preserves Blueprint data (icons, Niagara effects, montages) that would otherwise be lost during reparenting. **The cache survives restores**, so data only needs to be extracted once.

### Step 1a: Extract Item Data FROM BACKUP (One-time or if data changed)

**IMPORTANT:** Extract BEFORE restoring, while the backup has valid Blueprint data.

```bash
# Extract item icons, niagara systems, and other Blueprint data to cache
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_data.py" ^
  -stdout -unattended -nosplash 2>&1
```

Cache location: `C:/scripts/SLFConversion/migration_cache/item_data.json`

### Step 1b: Extract BaseStats FROM BACKUP (One-time or if data changed)

```bash
# Extract character class BaseStats (stat values per character class)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_base_stats.py" ^
  -stdout -unattended -nosplash 2>&1
```

Cache location: `C:/scripts/SLFConversion/migration_cache/base_stats.json`

### Step 1c: Extract Stat Defaults FROM BACKUP (One-time)

**IMPORTANT:** This extracts the original Blueprint CDO stat values. The C++ SLFAutomationLibrary must be copied to the backup project first.

```bash
# First, copy SLFAutomationLibrary to backup and build it
cp "C:/scripts/SLFConversion/Source/SLFConversion/SLFAutomationLibrary.h" "C:/scripts/bp_only/Source/bp_only/"
cp "C:/scripts/SLFConversion/Source/SLFConversion/SLFAutomationLibrary.cpp" "C:/scripts/bp_only/Source/bp_only/"
# Update Build.cs to add GameplayTags, UnrealEd, Kismet, BlueprintGraph dependencies
# Build backup project

# Then extract stat defaults
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_stat_defaults_cpp.py" ^
  -stdout -unattended -nosplash 2>&1
```

Cache location: `C:/scripts/SLFConversion/migration_cache/stat_defaults.json`

### Step 2: Restore Backup

```bash
powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"
```

### Step 3: Run Reparenting Migration

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/run_migration.py" ^
  -stdout -unattended -nosplash 2>&1
```

### Step 4: Apply Cached Data (ALL SCRIPTS REQUIRED)

```bash
# 4a. Apply icons to items
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_icons_fixed.py" ^
  -stdout -unattended -nosplash 2>&1

# 4b. Apply niagara and dodge montages
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_remaining_data.py" ^
  -stdout -unattended -nosplash 2>&1

# 4c. Apply item descriptions, names, and data (CRITICAL for inventory display)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_data.py" ^
  -stdout -unattended -nosplash 2>&1

# 4d. Apply item categories
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_categories.py" ^
  -stdout -unattended -nosplash 2>&1

# 4e. Apply BaseStats to character class data assets
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_base_stats.py" ^
  -stdout -unattended -nosplash 2>&1

# 4f. Apply armor mesh data (character-keyed skeletal meshes)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_armor_data.py" ^
  -stdout -unattended -nosplash 2>&1
```

### Build C++ First (if needed)

```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```

### Why This Workflow?

| Problem | Solution |
|---------|----------|
| Reparenting clears Blueprint struct data | Cache data BEFORE migration |
| Icons/Niagara lost after restore | Apply from cache AFTER migration |
| Dodge montages wiped | Apply to CDO AFTER migration |
| Repeated restores lose data | Cache survives restores (outside Content/) |

### Migration Scripts

| Script | Purpose |
|--------|---------|
| `extract_item_data.py` | Extract icons, niagara from backup (run on bp_only) |
| `extract_base_stats.py` | Extract character class BaseStats (run on bp_only) |
| `extract_stat_defaults_cpp.py` | Extract stat CDO defaults using C++ (run on bp_only) |
| `extract_armor_data.py` | Extract armor mesh data per character (run on bp_only) |
| `run_migration.py` | Multi-phase reparenting |
| `apply_icons_fixed.py` | Apply item icons from cache |
| `apply_remaining_data.py` | Apply niagara + dodge montages |
| `apply_item_data.py` | Apply item descriptions, names, display data |
| `apply_item_categories.py` | Apply item categories (Weapons, Armor, Tools, etc.) |
| `apply_base_stats.py` | Apply BaseStats to character class data assets |
| `apply_armor_data.py` | Apply armor mesh data (character-keyed SkeletalMeshInfo) |
| `apply_weapon_meshes.py` | Apply weapon mesh assignments to Blueprint CDOs |
| `full_migration.py` | All-in-one workflow (extract + migrate + apply) |

### Expected Output
- Extract: "Saved 21 items to migration_cache/item_data.json"
- Migration: ~306 successful reparents, 0 errors
- Apply icons: "Applied icons to 21 items"
- Apply remaining: "Applied Niagara to 21 items", "Saved B_Action_Dodge"
- Apply item data: "21 succeeded, 0 failed"
- Apply categories: "OK: DA_Sword01" with category assignments

---

## CRITICAL FILES

| File | Purpose |
|------|---------|
| `run_migration.py` | Multi-phase reparenting script |
| `migration_cache/item_data.json` | Cached icons, niagara, etc. (survives restores) |
| `apply_icons_fixed.py` | Apply item icons from cache |
| `apply_remaining_data.py` | Apply niagara + dodge montages |
| `Exports/BlueprintDNA_v2/*.json` | Source of truth for Blueprint logic |
| `Source/SLFConversion/` | C++ implementation |
| `C:\scripts\bp_only\` | Clean backup content (Blueprint-only, pre-migration) |

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

### 5. Blueprint CDO Overrides C++ Constructor Defaults (CRITICAL)
After reparenting a Blueprint to a C++ class, the Blueprint's **serialized CDO values override C++ constructor defaults**.

**Symptom:** Stats show 0/0 values, empty display names, or invalid tags even though C++ constructor sets them.

**Root Cause:** Blueprint CDOs have serialized struct data (e.g., `FStatInfo`) that was empty/default before reparenting. This serialized data takes precedence over C++ constructor initialization.

**Solution:** When creating objects from Blueprint classes, check if values are default/empty and copy from C++ parent CDO:
```cpp
USLFStatBase* NewStat = NewObject<USLFStatBase>(this, BlueprintClass);

// Check if Blueprint CDO has empty/default values
bool bNeedsParentDefaults = !NewStat->StatInfo.Tag.IsValid() ||
    (NewStat->StatInfo.CurrentValue == 0.0 && NewStat->StatInfo.MaxValue == 0.0);

if (bNeedsParentDefaults)
{
    // Walk up to find native C++ parent
    UClass* ParentClass = BlueprintClass->GetSuperClass();
    while (ParentClass)
    {
        if (!ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
        {
            USLFStatBase* ParentCDO = Cast<USLFStatBase>(ParentClass->GetDefaultObject());
            if (ParentCDO && ParentCDO->StatInfo.Tag.IsValid())
            {
                // Copy FULL struct from C++ parent
                NewStat->StatInfo = ParentCDO->StatInfo;
                break;
            }
        }
        ParentClass = ParentClass->GetSuperClass();
    }
}
```

### 6. Multiple Class Hierarchies (UB_Stat vs USLFStatBase)
This project has **two separate stat class hierarchies**:
- `UB_Stat` (older, in `Blueprints/B_Stat.h`)
- `USLFStatBase` (newer, in `Blueprints/SLFStatBase.h`)

Both inherit from `UObject` and are **NOT related**. Blueprints are parented to `USLFStatBase` hierarchy.

**Critical:** Widget code must use the **correct hierarchy**. If widgets cast to `UB_Stat*` but objects are `USLFStatBase*`, the cast returns nullptr.

**Solution:** Check which class hierarchy the StatManager creates, then update all widget code to match:
```cpp
// WRONG - widgets using old hierarchy
UB_Stat* Stat = Cast<UB_Stat>(StatObj);  // Returns nullptr!

// RIGHT - widgets using correct hierarchy
USLFStatBase* Stat = Cast<USLFStatBase>(StatObj);  // Works!
```

### 7. Widget Name Mismatches
Widget names in C++ must **exactly match** Blueprint UMG designer names.

**Symptom:** `GetWidgetFromName()` returns nullptr, widgets don't populate.

**Solution:** Check Blueprint JSON export for exact widget names:
```cpp
// WRONG - guessed name
StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatsBox")));

// RIGHT - exact name from Blueprint
StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatBox")));
```

Find correct names in: `Exports/BlueprintDNA_v2/WidgetBlueprint/*.json`
Search for: `"Class": "VerticalBox"` or similar, check `"Name":` field.

### 8. Widget Visual Feedback (Highlighting/Selection)

Blueprints often toggle **visibility** of highlight borders rather than changing brush colors.

**Symptom:** Selection/highlighting doesn't appear even though code runs.

**Wrong approach (won't work if border is designed as overlay):**
```cpp
SlotBorder->SetBrushColor(FLinearColor::Gold);  // May not be visible
```

**Right approach (matches Blueprint pattern):**
```cpp
HighlightBorder->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
```

**How to verify:** Check Blueprint JSON for `Set Visibility` vs `Set Brush Color` calls:
```bash
grep -i "visibility\|brush.*color" Exports/BlueprintDNA_v2/WidgetBlueprint/W_InventorySlot.json
```

### 9. Widget Typos in Blueprint Names

Blueprint widget names may contain typos that C++ must match exactly.

**Example:** `HightlightBorder` (typo) instead of `HighlightBorder`

**Solution:** Always check the actual Blueprint JSON for exact spelling:
```bash
grep -i "border\|highlight" Exports/BlueprintDNA_v2/WidgetBlueprint/W_EquipmentSlot.json
```

### 10. Component Mesh Data Lost During Migration (Weapon Mesh Pattern)

When Blueprint actors have components (StaticMeshComponent, NiagaraComponent) created via Blueprint SCS, the mesh/asset assignments are lost during reparenting to C++.

**Symptom:** Equipped weapons appear invisible - components exist but have no mesh assigned.

**Root Cause:**
1. Blueprint SCS (SimpleConstructionScript) stores component hierarchy and property values
2. When reparenting to C++ parent with `CreateDefaultSubobject`, SCS data is lost
3. C++ components are created fresh with no mesh assignment

**Solution: TSoftObjectPtr Property Pattern**

1. **Add serializable property to C++ class:**
```cpp
// In SLFWeaponBase.h
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
TSoftObjectPtr<UStaticMesh> DefaultWeaponMesh;
```

2. **Apply property in BeginPlay:**
```cpp
// In SLFWeaponBase.cpp BeginPlay()
if (WeaponMesh && !DefaultWeaponMesh.IsNull())
{
    UStaticMesh* MeshToApply = DefaultWeaponMesh.LoadSynchronous();
    if (MeshToApply)
    {
        WeaponMesh->SetStaticMesh(MeshToApply);
    }
}
```

3. **Set property via Python script (persists to Blueprint CDO):**
```python
# apply_weapon_meshes.py
cdo = unreal.get_default_object(gen_class)
cdo.set_editor_property("default_weapon_mesh", mesh)
unreal.EditorAssetLibrary.save_asset(bp_path)
```

**Why This Works:**
- `TSoftObjectPtr<UStaticMesh>` is a UPROPERTY that serializes with the Blueprint
- Setting via `set_editor_property()` persists to the Blueprint CDO
- At runtime, `LoadSynchronous()` loads the mesh and applies it

**Script:** `apply_weapon_meshes.py` - Run after migration to restore weapon meshes

**Weapons Updated:**
| Blueprint | Mesh |
|-----------|------|
| B_Item_Weapon_SwordExample01 | SM_ExampleSword01 |
| B_Item_Weapon_Greatsword | SM_Greatsword |
| B_Item_Weapon_Katana | SM_Katana |
| B_Item_Weapon_Shield | SM_Shield |

### 11. Armor Mesh Data Lost During Migration (Character-Keyed Mesh Pattern)

Armor items use a **different pattern than weapons**: Instead of spawning actor components, armor swaps skeletal meshes on character body slots. The mesh data is stored in a TMap keyed by character type.

**Key Differences: Weapons vs Armor**

| Aspect | Weapons | Armor |
|--------|---------|-------|
| Blueprint Type | Actor (ASLFWeaponBase) | Data Asset (UPDA_Item) |
| Mesh Type | StaticMeshComponent | SkeletalMesh (mesh swap) |
| Display Method | Spawn actor, attach to socket | Swap character mesh components |
| Storage | `DefaultWeaponMesh` property | `ItemInformation.EquipmentDetails.SkeletalMeshInfo` |
| Key Type | N/A (single mesh) | Character data asset (DA_Quinn, DA_Manny) |

**Symptom:** Equipped armor has no visual effect - character appearance doesn't change.

**Root Cause:**
1. Armor data is stored in nested struct `FSLFEquipmentInfo.SkeletalMeshInfo`
2. This is a `TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>`
3. Keys are character info assets (DA_Quinn, DA_Manny) to support different meshes per body type
4. During Blueprint reparenting, nested struct data is cleared

**Solution: Extract and Restore via Python Scripts**

1. **Extract data FROM BACKUP before restore:**
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_armor_data.py" ^
  -stdout -unattended -nosplash
```

2. **Apply data AFTER migration:**
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_armor_data.py" ^
  -stdout -unattended -nosplash
```

**Cache Location:** `migration_cache/armor_mesh_data.json`

**How AC_EquipmentManager Uses This Data:**
```cpp
// Get character info from GameInstance
UObject* SelectedClass = GetSelectedBaseClassFromGameInstance();
TSoftObjectPtr<UPrimaryDataAsset> CharacterInfoKey(Cast<UPrimaryDataAsset>(SelectedClass));

// Look up mesh for this character
const TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>& MeshMap =
    ArmorItem->ItemInformation.EquipmentDetails.SkeletalMeshInfo;

if (const TSoftObjectPtr<USkeletalMesh>* MeshPtr = MeshMap.Find(CharacterInfoKey))
{
    // Apply mesh via ISLFPlayerInterface (ChangeArmor, ChangeHeadpiece, etc.)
    PlayerInterface->ChangeArmor(*MeshPtr);
}
```

**Armor Items Updated:**
| Data Asset | Characters | Meshes |
|------------|------------|--------|
| DA_ExampleArmor | Quinn, Manny | SKM_Quinn_Armor01, SKM_MannyArmor_01 |
| DA_ExampleArmor02 | Quinn, Manny | SKM_Quinn_Armor02, SKM_MannyArmor_02 |
| DA_ExampleHelmet | Quinn, Manny | SKM_QuinnHelm, SKM_MannyHelmet |
| DA_ExampleHat | Quinn, Manny | SKM_QuinnHat, SKM_MannyHat |
| DA_ExampleBracers | Quinn, Manny | SKM_Quinn_Gloves, SKM_MannyGloves |
| DA_ExampleGreaves | Quinn, Manny | SKM_Quinn_Greaves, SKM_MannyGreaves |

---

## WIDGET DEBUGGING STRATEGIES (W_Equipment Case Study)

When debugging complex widget systems like equipment screens, follow this systematic approach:

### Phase 1: Identify the View Structure

Equipment screens typically use `UWidgetSwitcher` to toggle between views:
- Index 0: Equipment slots grid (showing equipped items)
- Index 1: Item selection list (showing available items to equip)

**Check for switcher in Blueprint JSON:**
```bash
grep -i "WidgetSwitcher\|switcher" Exports/BlueprintDNA_v2/WidgetBlueprint/W_Equipment.json
```

### Phase 2: Track Navigation State

Navigation functions must know which view is active:
```cpp
void EventNavigateDown_Implementation()
{
    // Check if in item selection view
    if (EquipmentInventorySlots.Num() > 0)
    {
        // Navigate through items
        ItemNavigationIndex++;
        ActiveItemSlot = EquipmentInventorySlots[ItemNavigationIndex];
    }
    else
    {
        // Navigate through equipment slots
        EquipmentSlotNavigationIndex++;
        SelectedSlot = EquipmentSlots[EquipmentSlotNavigationIndex];
    }
}
```

### Phase 3: Prevent Pointer Invalidation

**Critical Bug:** Delegate handlers that recreate widgets invalidate pointers held elsewhere.

**Wrong:**
```cpp
void HandleItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot)
{
    // THIS DESTROYS ALL SLOTS - including ActiveEquipmentSlot pointer!
    PopulateEquipmentSlots();
    BindEquipmentSlotEvents();
}
```

**Right:**
```cpp
void HandleItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot)
{
    // Update ONLY the specific slot
    UW_EquipmentSlot* TargetEquipSlot = GetEquipmentSlotByTag(TargetSlot);
    if (TargetEquipSlot && ItemData.ItemAsset)
    {
        TargetEquipSlot->EventOccupyEquipmentSlot(Cast<UPDA_Item>(ItemData.ItemAsset));
    }
}
```

### Phase 4: Auto-Return to Previous View

After completing an action, return to the appropriate view:
```cpp
void EventNavigateOk_Implementation()
{
    if (ActiveItemSlot && ActiveItemSlot->IsOccupied)
    {
        EquipItemAtSlot(ActiveItemSlot);

        // After equipping, switch back to equipment slots view
        EquipmentInventorySlots.Empty();
        UniformEquipmentItemsGrid->ClearChildren();
        EquipmentSwitcher->SetActiveWidgetIndex(0);
    }
}
```

### Phase 5: Data Asset SubCategory Matching

Equipment slots match items by Category AND SubCategory:

| Slot Type | Category | SubCategory |
|-----------|----------|-------------|
| Head | Armor (8) | Head (6) |
| Chest | Armor (8) | Chest (7) |
| Arms/Gloves | Armor (8) | Arms (8) |
| Legs/Greaves | Armor (8) | Legs (9) |
| Right Hand | Weapons (6) | Sword/Katana/etc. |
| Left Hand | Shields (7) | Any |
| Trinket | Bolstering (3) | Talismans (10) |
| Arrow/Bullet | Tools (1) | Projectiles (2) |

**Fix missing subcategories with Python script:**
```python
# apply_item_subcategories.py
ITEM_SUBCATEGORY_DATA = {
    "DA_ExampleHelmet": SLFItemSubCategory.HEAD,
    "DA_ExampleArmor": SLFItemSubCategory.CHEST,
    "DA_ExampleBracers": SLFItemSubCategory.ARMS,
    "DA_ExampleGreaves": SLFItemSubCategory.LEGS,
}
```

### Debugging Checklist

1. **Widget not appearing?** Check `GetWidgetFromName()` returns non-null, verify exact name from JSON
2. **Highlighting not working?** Check if Blueprint uses `SetVisibility` vs `SetBrushColor`
3. **Items not matching slots?** Add logging to show Category/SubCategory values
4. **Pointers becoming invalid?** Check if delegate handlers recreate widgets mid-operation
5. **View not switching?** Verify `WidgetSwitcher->SetActiveWidgetIndex()` is called
6. **Navigation not working?** Check if navigation function knows which view is active

---

## STAT SYSTEM ARCHITECTURE

The stat system uses a multi-layered approach for character stats (Health, Stamina, Vigor, etc.).

### Class Hierarchies

**USLFStatBase** (the correct hierarchy):
- `USLFStatBase` - Base class with `FStatInfo` struct
- `USLFStatHP`, `USLFStatVigor`, etc. - Specific stat types
- Blueprint stats (B_Vigor, B_HP, etc.) are reparented to these C++ classes

**UB_Stat** (older, deprecated):
- Separate hierarchy, NOT related to USLFStatBase
- Do NOT mix these in widget code

### Stat Initialization Flow

1. **DataTable → Stat Objects**
   - `StatManagerComponent` reads `DT_ExampleStatTable`
   - Creates `USLFStatBase` objects from Blueprint classes in the table

2. **C++ Constructor Defaults**
   - Each `USLFStat*` class sets defaults in constructor
   - These are FALLBACK values if no character class is selected

3. **Character Class BaseStats**
   - `PDA_BaseCharacterInfo` assets (DA_Manny, DA_Quinn) have `BaseStats` TMap
   - Maps `UClass* → double` (stat class → base value)
   - `StatManagerComponent::ApplyBaseStatsFromCharacterClass()` applies these at runtime

4. **Level-Up System**
   - `AdjustStat()` modifies stats from their base values
   - Fully compatible with character class system

### Key Files

| File | Purpose |
|------|---------|
| `SLFStatBase.h/cpp` | Base stat class with FStatInfo |
| `StatManagerComponent.h/cpp` | Creates and manages stats, applies BaseStats |
| `SLFPrimaryDataAssets.h` | `UPDA_BaseCharacterInfo` with `BaseStats` property |

### UPDA_BaseCharacterInfo Structure

```cpp
UCLASS(Blueprintable, BlueprintType)
class UPDA_BaseCharacterInfo : public UPDA_Base
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    FText CharacterClassName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    TMap<UClass*, double> BaseStats;  // Stat class -> base value

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    UDataAsset* DefaultMeshAsset;
};
```

### C++ Automation Functions (SLFAutomationLibrary)

```cpp
// Extract BaseStats from character class data assets
static bool ExtractBaseStats(const FString& CharacterClassPath, const FString& OutputPath);
static int32 ExtractAllBaseStats(const FString& OutputPath);

// Apply BaseStats from JSON cache
static int32 ApplyBaseStatsFromCache(const FString& InputPath);
```

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
