# AC_InputBuffer Migration Post-Mortem

## Gold Standard Reference for Blueprint-to-C++ Migration

**Date**: December 29, 2025
**Component**: AC_InputBuffer → UInputBufferComponent
**Status**: SUCCESSFUL after extensive troubleshooting

---

## Executive Summary

The AC_InputBuffer migration took significantly longer than expected due to a fundamental logic inversion bug that was only discovered through runtime debugging. This document captures all lessons learned to prevent similar issues in future migrations.

---

## What Worked

### 1. Minimal C++ Interface Design
- Only 4 functions exposed: `QueueAction`, `ToggleBuffer`, `ConsumeInputBuffer`, `ExecuteActionImmediately`
- Only 1 event dispatcher: `OnInputBufferConsumed`
- No conflicting function names with existing Blueprint functions
- Used `BlueprintNativeEvent` pattern for overridable functions

### 2. Debug Logging from the Start
- Added `UE_LOG` statements to all key functions
- Logged entry points, variable states, and decision branches
- This was CRITICAL for discovering the root cause

### 3. Parameter Name Matching
- Used `UPARAM(DisplayName="Buffer Open?")` to match Blueprint pin names
- Ensured parameter names matched what dependents expected

### 4. Automated Migration Script
- Python script handled reparenting, node reconstruction, and dependent fixing
- `BlueprintFixerLibrary` C++ class provided Blueprint manipulation utilities

---

## What Didn't Work

### 1. Assuming Logic Without Verification (CRITICAL FAILURE)
**The Bug**: C++ implementation had INVERTED logic compared to Blueprint

**Blueprint QueueAction Logic**:
```
if (BufferOpen?) {
    QueueNext()  // Buffer OPEN = queue for later
} else {
    ConsumeInputBuffer()  // Buffer CLOSED = execute immediately
}
```

**Incorrect C++ Implementation**:
```cpp
if (bBufferOpen) {
    ProcessAction()  // WRONG: processed when open
}
// Else: stayed queued - WRONG: should have consumed
```

**Impact**: Combat actions (attack, dodge, jump) were silently failing. Actions were being queued when they should have executed immediately.

### 2. Initial Parameter Name Mismatch
- C++ used `bOpen` but Blueprint expected `BufferOpen`
- Caused ANS_InputBuffer nodes to break
- User had to manually fix nodes in Blueprint Editor

### 3. CoreRedirects Pollution
- Stale `[CoreRedirects]` entries in DefaultEngine.ini caused `E_ValueType` enum errors
- These were leftover from previous migration attempts
- Had to manually remove the entire section

### 4. Not Reading JSON Export First
- Should have read `Component/AC_InputBuffer.json` BEFORE writing any C++ code
- The JSON export contains exact function graph logic including branch conditions
- Would have immediately revealed the correct `BufferOpen` semantics

---

## Troubleshooting Timeline

| Step | Action | Result |
|------|--------|--------|
| 1 | Initial migration | Compiled but combat didn't work |
| 2 | Added debug logging | Saw QueueAction called but buffer always "closed" |
| 3 | Checked ANS_InputBuffer | ToggleBuffer nodes had broken pins |
| 4 | Fixed parameter name | Nodes reconnected but still broken |
| 5 | User manually fixed nodes | Still no combat |
| 6 | Added print statements | ANS_InputBuffer fires for ladder, not combat |
| 7 | Tested ladder interaction | Saw ToggleBuffer called, action processed |
| 8 | **Realized logic inversion** | Buffer semantics were backwards |
| 9 | Read JSON export | Confirmed original Blueprint logic |
| 10 | Fixed C++ logic | **WORKING** |

---

## Root Cause Analysis

### Why Was the Logic Inverted?

1. **Assumed "BufferOpen" meant "ready to process"**
   - Intuitive interpretation: open = accepting/processing
   - Actual semantics: open = holding/queuing (buffer window during animation)

2. **Didn't trace the original Blueprint logic**
   - The JSON export clearly shows the branch conditions
   - "If buffer is closed, consume immediately" comment was in the Blueprint

3. **Variable naming was misleading**
   - `BufferOpen?` sounds like "is ready to receive"
   - Actually means "is in buffering mode"

### The Correct Mental Model

```
NORMAL GAMEPLAY (BufferOpen = false):
  Player presses attack → Execute immediately

DURING ANIMATION (BufferOpen = true via ANS_InputBuffer):
  Player presses attack → Queue for later
  Animation ends → ANS_InputBuffer sets BufferOpen = false
  ToggleBuffer detects transition → Consume queued action
```

---

## What Should Have Been Done Differently

### 1. ALWAYS Read JSON Export First
**Before writing ANY C++ code:**
```
1. Open Component/AC_InputBuffer.json
2. Find the function graph (e.g., "GraphName": "QueueAction")
3. Trace EVERY branch condition
4. Document the logic in comments BEFORE implementing
```

### 2. Document Semantics Before Coding
**Create a logic document like this:**
```
QueueAction(ActionTag):
  - Store IncomingActionTag = ActionTag
  - IF BufferOpen? THEN
      → Action stays queued (QueueNext event)
  - ELSE
      → Execute immediately (ConsumeInputBuffer)
```

### 3. Add Debug Logging Immediately
**Every function should start with:**
```cpp
UE_LOG(LogTemp, Warning, TEXT("FunctionName: param=%s, state=%s"),
    *Param.ToString(), bState ? TEXT("true") : TEXT("false"));
```

### 4. Test Incrementally
**Don't run full migration then debug. Instead:**
1. Reparent
2. Compile
3. Test ONE function in isolation
4. Confirm behavior matches original
5. Proceed to next function

---

## Definition of Done

A migration is complete when:

### Functional Requirements
- [ ] All C++ functions compile without errors
- [ ] Blueprint compiles without errors
- [ ] All dependent Blueprints compile without errors
- [ ] Functionality works identically to pre-migration behavior
- [ ] Debug logging confirms expected execution paths

### Documentation Requirements
- [ ] JSON export was read and logic was documented BEFORE coding
- [ ] All function semantics are documented in code comments
- [ ] Parameter names match original Blueprint exactly
- [ ] Any deviations from original behavior are intentional and documented

### Testing Requirements
- [ ] Tested in Play-In-Editor (PIE)
- [ ] Each migrated function was exercised
- [ ] Debug logs show expected flow
- [ ] No regression in dependent systems

---

## Checklist for Future Migrations

### Pre-Implementation
- [ ] Read JSON export for target component
- [ ] Document ALL function logic from graph nodes
- [ ] Identify all branch conditions and their semantics
- [ ] List all parameter names exactly as they appear in Blueprint
- [ ] Check for conflicting function names with intended C++ interface
- [ ] Backup current state before any changes

### Implementation
- [ ] Add UE_LOG to every function from the start
- [ ] Match parameter names EXACTLY (use UPARAM if needed)
- [ ] Use BlueprintNativeEvent for overridable functions
- [ ] Don't create C++ functions that conflict with existing BP functions
- [ ] Keep C++ interface minimal - only expose what's needed

### Post-Implementation
- [ ] Run migration script
- [ ] Check compiler warnings for parameter/signature mismatches
- [ ] Test in PIE immediately
- [ ] Verify debug logs show expected behavior
- [ ] Compare behavior to pre-migration state

---

## Files Created During This Migration

| File | Purpose |
|------|---------|
| `InputBufferComponent.h` | C++ header with BlueprintNativeEvent functions |
| `InputBufferComponent.cpp` | Implementation with debug logging |
| `BlueprintFixerLibrary.h/.cpp` | C++ utilities for Blueprint manipulation |
| `migrate_inputbuffer_automated.py` | Main migration script |
| `fix_remaining_dependents.py` | Fix dependent Blueprint nodes |
| `debug_inputbuffer.py` | Diagnostic script |
| `fix_character_bindings.py` | Fix character event bindings |
| `fix_ans_inputbuffer.py` | Fix ANS_InputBuffer nodes |

---

## Key Takeaways

1. **"Read JSON First" is non-negotiable** - The JSON export is the source of truth for Blueprint logic.

2. **Variable semantics can be counter-intuitive** - Don't assume based on names. Verify.

3. **Debug logging is not optional** - It's the fastest way to diagnose runtime issues.

4. **Minimal interfaces are safer** - The more functions you add, the more chances for conflicts.

5. **Test incrementally** - Don't batch debugging at the end.

---

## Appendix: Correct AC_InputBuffer Logic

```cpp
// QueueAction: Called when player presses an action button
void QueueAction(FGameplayTag ActionTag)
{
    IncomingActionTag = ActionTag;

    // BufferOpen = true means we're in an animation's buffer window
    // BufferOpen = false means normal gameplay - execute immediately
    if (!bBufferOpen)  // Buffer CLOSED = execute now
    {
        ConsumeInputBuffer();
    }
    // If buffer is OPEN, action stays queued until buffer closes
}

// ToggleBuffer: Called by ANS_InputBuffer notify state
void ToggleBuffer(bool BufferOpen)
{
    bool bWasOpen = bBufferOpen;
    bBufferOpen = BufferOpen;

    // When buffer CLOSES, consume any queued action
    if (bWasOpen && !BufferOpen && IncomingActionTag.IsValid())
    {
        ConsumeInputBuffer();
    }
}
```
