# Known Issues and Solutions

Reference document for migration issues encountered and their solutions.

---

## 1. "?" Suffix Pin Problem

Blueprint pins can have names like "Buffer Open?" but C++ cannot use "?".

**Solution:** Clear the calling Blueprint's logic (don't try to match pin names).

---

## 2. BindWidget Errors

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

---

## 3. Cascade Loading Crash

If migration crashes with "Cast of nullptr to Function failed":
1. Add Blueprint to SKIP_LIST in run_migration.py
2. Or add to priority processing (PRIORITY_ANIM_NOTIFY_STATES)

---

## 4. Blueprint Struct Property Access

**NEVER use reflection to access Blueprint struct properties with GUID-suffixed names.**

**ALWAYS use C++ property migration instead:**
1. Add equivalent UPROPERTY to C++ parent class
2. Reparent Blueprint to C++ class
3. Migrate data from Blueprint to C++ properties
4. Access via direct Cast<>

See: `.claude/skills/cpp-property-migration.md` for complete workflow.

---

## 5. Blueprint CDO Overrides C++ Constructor Defaults (CRITICAL)

After reparenting a Blueprint to a C++ class, the Blueprint's **serialized CDO values override C++ constructor defaults**.

**Symptom:** Stats show 0/0 values, empty display names, or invalid tags even though C++ constructor sets them.

**Root Cause:** Blueprint CDOs have serialized struct data (e.g., `FStatInfo`) that was empty/default before reparenting. This serialized data takes precedence over C++ constructor initialization.

**Solution:** When creating objects from Blueprint classes, check if values are default/empty and copy from C++ parent CDO:
```cpp
USLFStatBase* NewStat = NewObject<USLFStatBase>(this, BlueprintClass);
bool bNeedsParentDefaults = !NewStat->StatInfo.Tag.IsValid() ||
    (NewStat->StatInfo.CurrentValue == 0.0 && NewStat->StatInfo.MaxValue == 0.0);
if (bNeedsParentDefaults)
{
    UClass* ParentClass = BlueprintClass->GetSuperClass();
    while (ParentClass)
    {
        if (!ParentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
        {
            USLFStatBase* ParentCDO = Cast<USLFStatBase>(ParentClass->GetDefaultObject());
            if (ParentCDO && ParentCDO->StatInfo.Tag.IsValid())
            {
                NewStat->StatInfo = ParentCDO->StatInfo;
                break;
            }
        }
        ParentClass = ParentClass->GetSuperClass();
    }
}
```

---

## 6. Multiple Class Hierarchies (UB_Stat vs USLFStatBase)

Two separate stat class hierarchies:
- `UB_Stat` (older, deprecated in `Blueprints/B_Stat.h`)
- `USLFStatBase` (correct, in `Blueprints/SLFStatBase.h`)

**Critical:** Widget code must use `USLFStatBase`, never `UB_Stat`.

---

## 7. Widget Name Mismatches

Widget names in C++ must **exactly match** Blueprint UMG designer names.

**Symptom:** `GetWidgetFromName()` returns nullptr.

**Solution:** Check Blueprint JSON export for exact widget names:
```
Find correct names in: Exports/BlueprintDNA_v2/WidgetBlueprint/*.json
Search for: "Class": "VerticalBox" or similar, check "Name": field.
```

---

## 8. Widget Visual Feedback (Highlighting/Selection)

Blueprints often toggle **visibility** of highlight borders rather than changing brush colors.

**Right approach (matches Blueprint pattern):**
```cpp
HighlightBorder->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
```

Check Blueprint JSON for `Set Visibility` vs `Set Brush Color` calls.

---

## 9. Widget Typos in Blueprint Names

Blueprint widget names may contain typos that C++ must match exactly.

**Example:** `HightlightBorder` (typo) instead of `HighlightBorder`

Always check actual Blueprint JSON for exact spelling.

---

## 10. Component Mesh Data Lost During Migration (Weapon Mesh Pattern)

When Blueprint actors have components created via Blueprint SCS, mesh/asset assignments are lost during reparenting to C++.

**Solution: TSoftObjectPtr Property Pattern**

1. Add serializable property to C++ class:
```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
TSoftObjectPtr<UStaticMesh> DefaultWeaponMesh;
```

2. Apply property in BeginPlay:
```cpp
if (WeaponMesh && !DefaultWeaponMesh.IsNull())
{
    UStaticMesh* MeshToApply = DefaultWeaponMesh.LoadSynchronous();
    if (MeshToApply) WeaponMesh->SetStaticMesh(MeshToApply);
}
```

3. Set via Python script after migration: `apply_weapon_meshes.py`

**Weapons Updated:** SM_ExampleSword01, SM_Greatsword, SM_Katana, SM_Shield

---

## 11. Armor Mesh Data Lost During Migration (Character-Keyed Mesh Pattern)

Armor uses `TMap<TSoftObjectPtr<UPrimaryDataAsset>, TSoftObjectPtr<USkeletalMesh>>` keyed by character type.

**Solution:** Extract before migration, apply after:
- Extract: `extract_armor_data.py` (run on bp_only)
- Apply: `apply_armor_data.py` (run on SLFConversion after migration)
- Cache: `migration_cache/armor_mesh_data.json`

---

## Widget Debugging Strategies

### View Structure
Equipment screens use `UWidgetSwitcher`:
- Index 0: Equipment slots grid
- Index 1: Item selection list

### Prevent Pointer Invalidation
Delegate handlers that recreate widgets invalidate pointers. Update ONLY the specific slot, not all.

### Equipment Slot Matching
| Slot Type | Category | SubCategory |
|-----------|----------|-------------|
| Head | Armor (8) | Head (6) |
| Chest | Armor (8) | Chest (7) |
| Arms | Armor (8) | Arms (8) |
| Legs | Armor (8) | Legs (9) |
| Right Hand | Weapons (6) | Sword/Katana/etc. |
| Left Hand | Shields (7) | Any |
| Trinket | Bolstering (3) | Talismans (10) |

### Debugging Checklist
1. Widget not appearing? Check `GetWidgetFromName()` returns non-null
2. Highlighting not working? Check `SetVisibility` vs `SetBrushColor`
3. Items not matching slots? Log Category/SubCategory values
4. Pointers invalid? Check if delegate handlers recreate widgets
5. View not switching? Verify `WidgetSwitcher->SetActiveWidgetIndex()`

---

## Stat System Architecture

### Stat Initialization Flow
1. DataTable -> StatManagerComponent creates USLFStatBase objects
2. C++ constructor defaults (fallback)
3. Character Class BaseStats: `PDA_BaseCharacterInfo::BaseStats` TMap
4. Level-Up: `AdjustStat()` modifies from base values

### Key Files
- `SLFStatBase.h/cpp` - Base stat class
- `StatManagerComponent.h/cpp` - Creates/manages stats
- `SLFPrimaryDataAssets.h` - `UPDA_BaseCharacterInfo` with BaseStats

---

## C++ Property Migration Pattern

**NEVER use reflection for Blueprint struct properties. ALWAYS migrate to C++ properties.**

### Workflow
1. Extract data from Blueprint using `export_text()` + regex
2. Reparent Blueprint via migration script
3. Apply data to C++ properties via `set_editor_property()`
4. Access directly via `Cast<CppClass>()`

### Python Naming Conventions
- C++ `UPDA_Item` -> Python path `/Script/SLFConversion.PDA_Item` (no U prefix)
- C++ `WorldNiagaraSystem` -> Python `world_niagara_system` (snake_case)
