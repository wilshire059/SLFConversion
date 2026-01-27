# Fix Animation and Equipment Regression

## COMMAND: /fix-animation-equipment

Holistically investigate and fix the animation and equipment regressions that occurred after Blueprint-to-C++ migration.

---

## CURRENT STATUS

### Equipment: FIXED (via git restore)
Equipment menu now works after restoring Content assets from git commit `c892084`.

**What was done to fix equipment:**
1. Content folder assets had been overwritten from backup, losing migrated data
2. Restored all item data assets (DA_*.uasset) - had DisplayName, icons, descriptions
3. Restored all weapon Blueprints - had DefaultWeaponMesh assignments
4. Restored all stat assets - had correct migration
5. Restored all AnimBP assets - had correct parent classes
6. Restored all widget assets - had correct bindings
7. Restored all character Blueprints - had correct parent classes

```bash
# Commands used to restore:
git checkout HEAD -- Content/SoulslikeFramework/Data/Items/*.uasset
git checkout HEAD -- "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/"*.uasset
git checkout HEAD -- "Content/SoulslikeFramework/Data/Stats/"*.uasset
git checkout HEAD -- "Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/"*.uasset
git checkout HEAD -- "Content/SoulslikeFramework/Widgets/"*.uasset
git checkout HEAD -- "Content/SoulslikeFramework/Blueprints/_Characters/"*.uasset
```

### Animations: BROKEN
Character is frozen in T-pose. Sword is visible and attached, but no animations play.
Enemy NPCs also frozen - they move around but no animations.

---

## KNOWN ERRORS FROM debug-log.txt

### Critical AnimBP Errors

1. **ABP_SoulslikeCharacter_Additive** (Player):
```
[Compiler] Invalid field 'IsGuarding?' found in property path for Property Access
```
- AnimGraph has Property Access node looking for `IsGuarding?` (with question mark)
- C++ property is `IsGuarding` (without question mark)
- This breaks the animation state machine

2. **ABP_SoulslikeEnemy** (Enemies):
```
PIE: Error: Blueprint Runtime Error: "Accessed None trying to read (real) property PoiseBreakAsset"
Function: ExecuteUbergraph_ABP_SoulslikeEnemy
```
- EventGraph code trying to access property that doesn't exist
- EventGraph should have been cleared but still has logic

3. **ABP_SoulslikeBossNew** (Boss):
```
PIE: Error: Blueprint Runtime Error: "Accessed None trying to read (real) property AC AI Combat Manager"
Function: ExecuteUbergraph_ABP_SoulslikeBossNew
```
- Same pattern - EventGraph accessing null component reference

### EventGraph Status Check
When running `clear_animbp_only.py`, it reported:
```
Cleared 0 nodes from EventGraph
```
This means EventGraphs appear empty, but `ExecuteUbergraph` errors indicate compiled code still exists.

---

## RELEVANT COMMITS

### Working Equipment Commits
| Commit | Description | What Worked |
|--------|-------------|-------------|
| `8e1bb49` | Fix armor visual equipping with mesh swap system | Armor equipping, mesh changes |
| `3e34143` | Fix weapon meshes lost during Blueprint migration | Weapon visibility |
| `635a740` | Fix equipment spawn to use character pawn as Instigator | Equipment spawning |
| `366d271` | Migrate item data (icons, names, descriptions) for 21 items | Item display |

### Animation-Related Commits
| Commit | Description | Status |
|--------|-------------|--------|
| `c892084` | Fix component lookup and add overlay state sync | Latest - equipment works, animations broken |
| `9816126` | Enhance AC_EquipmentManager overlay tag handling | Overlay states for weapons |
| `0e381b9` | Add AAA-quality AnimBP diagnostic functions | Diagnostic tools |
| `307e7e0` | Fix crouch animation without reflection | Crouch animation fix |
| `fac8108` | Fix crouch animation by initializing AnimBP component references | Early crouch fix |

### Key Insight
The `IsGuarding?` Property Access error was identified but NOT confirmed fixed because equipment wasn't working at the time. Now equipment works, but animations are still broken.

---

## ROOT CAUSE ANALYSIS NEEDED

### Question 1: Why are EventGraphs reporting 0 nodes but still executing?
- `SLFAutomationLibrary.clear_graphs_keep_variables_no_compile()` claims to clear
- But runtime still hits `ExecuteUbergraph` errors
- Possible: Compiled bytecode not invalidated after clear

### Question 2: Why is Property Access looking for `IsGuarding?`?
- AnimGraph Property Access paths are baked into the asset
- Path was set when AnimBP read from Blueprint component with `IsGuarding?` display name
- C++ property is `IsGuarding` (no question mark)
- Need to fix the Property Access path in the AnimGraph

### Question 3: What provides animation data to the AnimGraph?
- Original: EventGraph set component references, Property Access read from them
- After migration: EventGraph cleared, but Property Access paths still point to old structure
- C++ AnimInstance has NativeUpdateAnimation but AnimBP may not be reparented to it

---

## INVESTIGATION STEPS

### Step 1: Verify AnimBP Parent Classes
```python
# Check if AnimBPs are reparented to C++ AnimInstance
for abp in ["ABP_SoulslikeEnemy", "ABP_SoulslikeBossNew", "ABP_SoulslikeCharacter_Additive"]:
    # Get parent class
    # If parent is Blueprint class, NativeUpdateAnimation won't run
    # If parent is C++ class, check if variables are being set
```

### Step 2: Check Property Access Paths
```python
# Export Property Access node configurations
# Identify paths with "?" suffix that don't match C++ properties
# List all broken paths that need fixing
```

### Step 3: Verify C++ Variable Updates
```cpp
// Check if NativeUpdateAnimation is being called
// Add logging to confirm:
UE_LOG(LogTemp, Warning, TEXT("[AnimBP] NativeUpdateAnimation called, Speed=%.1f"), Speed);
```

### Step 4: Compare with Backup
```bash
# Diff AnimBP assets between backup and current
# Look for what changed in AnimGraph structure
```

---

## FIX STRATEGIES

### Strategy A: Fix Property Access Paths (Preferred)
1. Identify all Property Access nodes with broken paths
2. Update paths to match C++ property names
3. Requires C++ automation function or manual editor fix

### Strategy B: Force Recompile AnimBPs
1. Mark AnimBPs as modified
2. Force full recompile to regenerate bytecode
3. May fix ExecuteUbergraph issues

### Strategy C: Restore AnimBPs from Pre-Migration Backup
1. Copy AnimBP assets from `C:\scripts\bp_only\`
2. Re-run targeted migration on just AnimBPs
3. Ensure EventGraphs are properly cleared this time

### Strategy D: Set Variables via Tick (Workaround)
1. If AnimBP not reparented to C++, NativeUpdateAnimation won't run
2. Use character Tick to set AnimBP variables via reflection
3. Already partially implemented in SLFSoulslikeCharacter::UpdateAnimInstanceOverlayStates()

---

## C++ FILES TO CHECK

| File | What to Check |
|------|---------------|
| `ABP_SoulslikeCharacter_Additive.h/cpp` | NativeUpdateAnimation implementation |
| `SLFSoulslikeCharacter.cpp` | UpdateAnimInstanceOverlayStates() |
| `SLFAutomationLibrary.cpp` | clear_graphs_keep_variables_no_compile() |
| `AC_CombatManager.h` | IsGuarding property (not IsGuarding?) |

## ANIMGRAPH PROPERTY ACCESS EXPECTED PATHS

| AnimBP | Property Access Path | Expected C++ Property |
|--------|---------------------|----------------------|
| ABP_SoulslikeCharacter_Additive | `?.IsGuarding?` | `CombatManager->IsGuarding` |
| ABP_SoulslikeCharacter_Additive | `ActionManager.IsCrouched` | `ActionManager->IsCrouched` |
| ABP_SoulslikeEnemy | `?.PoiseBreakAsset` | Should be removed or use C++ |
| ABP_SoulslikeBossNew | `AC AI Combat Manager.?` | Should be removed or use C++ |

---

## ACCEPTANCE CRITERIA

Output `<promise>ANIMATION_EQUIPMENT_FIX_COMPLETE</promise>` ONLY when:

1. **C++ builds with 0 errors**
2. **Equipment menu opens and displays items** ✅ (Already working)
3. **Equipping weapons shows weapon visually** ✅ (Already working)
4. **Player character animates** (locomotion, idle, crouch)
5. **Guard/block animation plays when blocking**
6. **Enemy NPCs animate** (not frozen)
7. **Boss animates** (not frozen)
8. **No AnimBP runtime errors in log**
9. **Property Access warnings resolved**

---

## AAA GAME STUDIO STANDARDS

- NO fragile reflection hacks for struct access
- NO hardcoded property GUIDs
- PROPER use of UE5 C++ APIs (Cast<>, typed accessors)
- PIN-LEVEL structural validation where needed
- COMPILE-TIME error extraction and fixing
- SURGICAL changes only - don't break working equipment

---

## START

1. Read debug-log.txt for full error context
2. Check AnimBP parent classes
3. Identify all broken Property Access paths
4. Implement fix strategy
5. Test in PIE
6. Verify all acceptance criteria
7. Output promise when complete
