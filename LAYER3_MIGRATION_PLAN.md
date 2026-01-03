# Layer 3 Migration Plan - B_Stat Reparenting and TMap Types

## Status: IN PROGRESS

## Prerequisites

- [x] Layer 0 complete (SLFStatTypes.h with all enum/struct definitions)
- [x] USLFStatObject C++ class exists (minimal, no conflicting properties)
- [ ] B_Stat reparented to USLFStatObject
- [ ] C++ TMaps updated to use USLFStatObject types
- [ ] Full migration script tested

---

## Problem Statement

Previous migration attempts (v11, v12) failed because:

1. **v11**: Used `TSubclassOf<USLFStatObject>` but B_Stat was NOT reparented yet
   - Error: "Array of Stat/ Attribute Object Object References is not compatible with Array of SLFStat Object Object References"

2. **v12**: Used `TSubclassOf<UObject>` hoping for generic compatibility
   - Error: "Array of Stat/ Attribute Object Object References is not compatible with Array of Object References"

**Root Cause**: Unreal's Blueprint type system requires exact type matches. Even if B_Stat could theoretically be assigned to UObject*, Blueprint pins that expect B_Stat_C won't accept UObject*.

---

## Solution: Two-Phase Migration

### Phase A: B_Stat Reparenting

**Goal**: Make B_Stat inherit from USLFStatObject so that B_Stat_C IS-A USLFStatObject

**Script**: `reparent_b_stat.py`

**Steps**:
1. Load B_Stat Blueprint
2. Reparent to USLFStatObject C++ class
3. Save B_Stat

**Key Insight**: B_Stat keeps ALL its Blueprint variables (including FStatInfo). We're only changing the parent class, not migrating any data.

### Phase B: C++ TMap Type Update

**Goal**: Update SoulslikeStatComponent TMaps to use USLFStatObject types

**File**: `SoulslikeStatComponent.h`

**Changes**:
```cpp
// Before (v12 - UObject types)
TMap<TSubclassOf<UObject>, FGameplayTag> Stats;
TMap<FGameplayTag, UObject*> ActiveStats;

// After (v13 - USLFStatObject types)
TMap<TSubclassOf<USLFStatObject>, FGameplayTag> Stats;
TMap<FGameplayTag, USLFStatObject*> ActiveStats;
```

**Why This Works Now**:
- After Phase A, B_Stat inherits from USLFStatObject
- B_Stat_C IS-A USLFStatObject (inheritance relationship)
- Blueprint pins expecting B_Stat_C can accept USLFStatObject* because of inheritance

### Phase C: AC_StatManager Migration

**Script**: `run_fix_all.py` (v13)

**Steps**:
1. Rename IsAiComponent -> IsAiComponent_BP
2. Reparent AC_StatManager to SoulslikeStatComponent
3. Copy default values
4. Migrate ALL variable references (including TMap vars)
5. Delete old BP variables

---

## Execution Order

```
1. Restore clean Blueprint content
2. Open Unreal Editor
3. Run: exec(open(r"C:\scripts\SLFConversion\reparent_b_stat.py").read())
4. Verify B_Stat inherits from USLFStatObject
5. Close Unreal Editor
6. Update SoulslikeStatComponent.h TMaps to use USLFStatObject
7. Compile C++: Build.bat command
8. Open Unreal Editor
9. Run: exec(open(r"C:\scripts\SLFConversion\run_fix_all.py").read())
10. Verify all migrations successful
11. Compile all Blueprints
12. Test in Play mode
```

---

## Files to Modify

### SoulslikeStatComponent.h

```cpp
// Include USLFStatObject
#include "SLFStatObject.h"

// Update TMap types
TMap<TSubclassOf<USLFStatObject>, FGameplayTag> Stats;
TMap<FGameplayTag, USLFStatObject*> ActiveStats;
```

### run_fix_all.py (v13)

- Add Phase 0: Check that B_Stat is already reparented
- Keep all other phases the same

---

## Validation Checklist

- [ ] B_Stat shows "Parent Class: SLFStatObject" in editor
- [ ] C++ compiles with USLFStatObject TMap types
- [ ] No errors when opening AC_StatManager after migration
- [ ] All BP variable references migrated successfully
- [ ] Play mode works without crashes

---

## Rollback Plan

If migration fails:
1. Close Unreal Editor
2. Restore clean Blueprint content from backup
3. Keep C++ changes (Layer 0 types, USLFStatObject) - they don't affect Blueprints

```powershell
# Restore command
Remove-Item -Path "C:\scripts\SLFConversion\Content" -Recurse -Force
Copy-Item -Path "C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content" -Destination "C:\scripts\SLFConversion\Content" -Recurse
```
