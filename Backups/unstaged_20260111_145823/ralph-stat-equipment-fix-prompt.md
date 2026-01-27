# Ralph Loop: Fix Stat Display and Equipment Regression

## Mission

Investigate and fix the regression where:
1. **Character stats are not displaying** in inventory and equipment menus
2. **Equipment functionality is broken** (items not equipping properly)

**Known Good Commit:** `8e1bb49` - "Fix armor visual equipping with mesh swap system" - This commit had working stats and equipment.

---

## AAA Game Studio Methodology

### Principles

1. **No fragile reflection hacks** - Use proper UE5 C++ APIs (`Cast<>`, typed accessors)
2. **Surgical changes only** - Don't restore entire Content/ folder, identify specific broken files
3. **Node-by-node comparison** - Export Blueprint state and diff against backup
4. **Pin-by-pin validation** - Ensure all connections match original
5. **Property-level diff** - Compare struct properties between backup and current
6. **Cache extraction BEFORE migration** - Never lose data

### Reference Projects

- **Backup (ground truth):** `C:\scripts\bp_only\` - Original Blueprint-only project
- **Current (working on):** `C:\scripts\SLFConversion\` - Migrated project with C++

---

## Phase 1: Diagnosis

### 1.1 Identify Affected Widgets

Check these widgets for stat display issues:
```
/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory
/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment
/Game/SoulslikeFramework/Widgets/HUD/W_HUD (if stat bars exist)
```

### 1.2 Identify Affected Components

Check these components for equipment issues:
```
AC_StatManager - Creates and manages stats
AC_EquipmentManager - Handles equipment slots and equipping
AC_InventoryManager - Manages inventory items
```

### 1.3 Compare Working Commit vs Current

```bash
# Show what changed since stats were working
git diff 8e1bb49..HEAD --stat

# Show specific file changes
git diff 8e1bb49..HEAD -- Source/SLFConversion/Components/AC_StatManager.cpp
git diff 8e1bb49..HEAD -- Source/SLFConversion/Components/AC_EquipmentManager.cpp
git diff 8e1bb49..HEAD -- Source/SLFConversion/Widgets/
```

### 1.4 Export Blueprint State for Diff

Create Python scripts to export current vs backup widget states:

```python
# export_widget_state.py
import unreal
import json

WIDGETS = [
    "/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory",
    "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment",
]

def export_widget_state(widget_path):
    """Export widget bindings, variables, and function calls"""
    asset = unreal.EditorAssetLibrary.load_asset(widget_path)
    if not asset:
        return None

    # Use SLFAutomationLibrary to export detailed state
    state = unreal.SLFAutomationLibrary.export_blueprint_state(asset)
    return state
```

---

## Phase 2: Root Cause Analysis

### 2.1 Stat Display Issues

Check for these common problems:

1. **Wrong stat class hierarchy** - Widgets may cast to `UB_Stat*` when objects are `USLFStatBase*`
2. **Widget name mismatches** - `GetWidgetFromName()` returns nullptr if name doesn't match exactly
3. **Empty stat values** - Blueprint CDO overrides C++ constructor defaults
4. **Missing DataTable binding** - StatManager may not be reading stats correctly

### 2.2 Equipment Issues

Check for these common problems:

1. **Equipment slot tag mismatches** - Category/SubCategory not matching
2. **Delegate binding broken** - Events not firing to update UI
3. **Item data lost during reparenting** - Nested struct data cleared
4. **Character reference issues** - OwnerCharacter or PlayerCharacter null

### 2.3 Key Functions to Verify

**AC_StatManager:**
```cpp
InitializeStatComponent() - Creates stat objects
GetStatByTag() - Returns stat object by tag
GetAllStats() - Returns all stats for UI population
```

**AC_EquipmentManager:**
```cpp
EquipItemToSlot() / EquipWeaponToSlot() / EquipArmorToSlot()
GetEquippedItemForSlot() - Returns item at slot
OnItemEquippedToSlot - Delegate for UI updates
```

**Widgets:**
```cpp
PopulateStats() / RefreshStats() - Fill stat display
PopulateEquipmentSlots() - Fill equipment grid
HandleItemEquippedToSlot() - React to equip events
```

---

## Phase 3: Fix Implementation

### 3.1 Surgical Fix Pattern

```bash
# 1. Identify broken file(s) from diff
git diff 8e1bb49..HEAD -- Source/SLFConversion/Components/AC_StatManager.cpp

# 2. If C++ regression: Revert specific changes
git checkout 8e1bb49 -- Source/SLFConversion/Components/AC_StatManager.cpp

# 3. If Blueprint regression: Restore from backup
cp "C:/scripts/bp_only/Content/.../W_Inventory.uasset" \
   "C:/scripts/SLFConversion/Content/.../"

# 4. If data migration issue: Re-run targeted apply script
python apply_stat_data.py  # Only touches stat-related assets
```

### 3.2 Property Migration Pattern

If struct properties are missing after reparenting:

1. **Extract data from backup:**
   ```python
   # Run on bp_only
   def extract_stat_info():
       for stat_path in STAT_ASSETS:
           asset = load_asset(stat_path)
           stat_info = asset.get_editor_property("stat_info")
           cache[stat_path] = {
               "tag": str(stat_info.tag),
               "current_value": stat_info.current_value,
               "max_value": stat_info.max_value,
               # ...
           }
   ```

2. **Apply data after migration:**
   ```python
   # Run on SLFConversion
   def apply_stat_info():
       for stat_path, data in cache.items():
           asset = load_asset(stat_path)
           stat_info = asset.get_editor_property("stat_info")
           stat_info.tag = GameplayTag.request_gameplay_tag(data["tag"])
           stat_info.current_value = data["current_value"]
           # ...
           asset.set_editor_property("stat_info", stat_info)
           save_asset(stat_path)
   ```

---

## Phase 4: Verification

### 4.1 Automated Tests

Create test script to verify fixes:

```python
# test_stat_equipment.py
def test_stat_display():
    """Verify stats populate in widgets"""
    # Load W_Inventory
    # Check GetStatByTag returns valid objects
    # Verify stat values are non-zero
    pass

def test_equipment_slots():
    """Verify equipment can be equipped"""
    # Get EquipmentManager
    # Try equipping item
    # Verify OnItemEquippedToSlot fires
    pass
```

### 4.2 Visual Verification Checklist

- [ ] Open inventory menu - stats should display with names and values
- [ ] Open equipment menu - stats should display
- [ ] Select equipment slot - should highlight
- [ ] Equip weapon to slot - weapon should appear
- [ ] Equip armor to slot - character mesh should change
- [ ] Unequip item - slot should clear

### 4.3 Node-Level Diff

Use SLFAutomationLibrary to diff widget graphs:

```python
# diff_widget_states.py
backup_state = export_from_backup("W_Inventory")
current_state = export_from_current("W_Inventory")

# Compare node counts
# Compare function calls
# Compare variable bindings
# Report any [CRITICAL] differences
```

---

## Acceptance Criteria

1. **C++ builds with 0 errors**
2. **Inventory widget displays stats** with correct names and values
3. **Equipment widget displays stats** with correct names and values
4. **Equipment slots highlight** when selected
5. **Weapons equip** and display visually
6. **Armor equips** and changes character mesh
7. **No regression** from commit `8e1bb49` functionality

---

## Files to Investigate

### C++ Components
| File | What to Check |
|------|---------------|
| `AC_StatManager.h/cpp` | Stat creation, GetStatByTag, GetAllStats |
| `AC_EquipmentManager.h/cpp` | Equip functions, delegates, slot management |
| `SLFStatBase.h/cpp` | Stat class hierarchy, FStatInfo struct |
| `SLFPrimaryDataAssets.h` | UPDA_Item, equipment info structs |

### Widget Blueprints
| File | What to Check |
|------|---------------|
| `W_Inventory.uasset` | Stat display bindings, PopulateStats calls |
| `W_Equipment.uasset` | Stat display, slot population, equip handlers |
| `W_EquipmentSlot.uasset` | Slot selection, highlighting |
| `W_InventorySlot.uasset` | Item display |

### JSON Exports (Ground Truth)
| File | What to Check |
|------|---------------|
| `Exports/BlueprintDNA_v2/WidgetBlueprint/W_Inventory.json` | Original widget logic |
| `Exports/BlueprintDNA_v2/WidgetBlueprint/W_Equipment.json` | Original widget logic |
| `Exports/BlueprintDNA_v2/Component/AC_StatManager.json` | Original stat functions |

---

## Common Regression Patterns

### Pattern 1: Stat Class Cast Failure

**Symptom:** Stat values show 0/0 or empty
**Cause:** Widget casts to wrong class (`UB_Stat` vs `USLFStatBase`)
**Fix:** Update widget code to use correct class hierarchy

### Pattern 2: Widget Binding Name Mismatch

**Symptom:** Widget elements don't populate
**Cause:** `GetWidgetFromName("StatBox")` returns nullptr because actual name is "StatsBox"
**Fix:** Check JSON export for exact widget names

### Pattern 3: Struct Data Lost

**Symptom:** Items have no icons, stats have no tags
**Cause:** Reparenting cleared nested struct data
**Fix:** Run data migration scripts (extract from backup, apply after migration)

### Pattern 4: Delegate Not Bound

**Symptom:** Equipping works but UI doesn't update
**Cause:** `OnItemEquippedToSlot.AddDynamic()` not called in NativeConstruct
**Fix:** Ensure delegate binding in widget initialization

---

## Output

When complete, output:

```
<promise>STAT_EQUIPMENT_FIX_COMPLETE</promise>
```

With report including:
- Root cause identified
- Files modified
- Tests passed
- Visual verification complete
