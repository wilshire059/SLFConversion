# AI System Comparison Report: bp_only vs SLFConversion

## Executive Summary

**ROOT CAUSE IDENTIFIED:** The behavior tree structure and blackboard enum were modified during migration. The SLFConversion project has State-checking decorators that don't exist in the original bp_only project.

---

## Key Difference #1: Blackboard State Key Enum Type

### bp_only (Original)
```
Key: State (Type: BlackboardKeyType_Enum)
  EnumType: /Game/SoulslikeFramework/Enums/E_AI_States.E_AI_States
  Enum Values:
    [0] Value=0 Name=NewEnumerator0 Display=Idle
    [1] Value=1 Name=NewEnumerator4 Display=Random Roam
    [2] Value=2 Name=NewEnumerator1 Display=Patrolling
    [3] Value=3 Name=NewEnumerator5 Display=Investigating
    [4] Value=4 Name=NewEnumerator2 Display=Combat
    [5] Value=5 Name=NewEnumerator6 Display=Poise Broken
    [6] Value=6 Name=NewEnumerator7 Display=Uninterruptable
    [7] Value=7 Name=NewEnumerator3 Display=Out of Bounds
```

### SLFConversion (Current)
```
Key: State (Type: BlackboardKeyType_Enum)
  EnumType: /Script/SLFConversion.ESLFAIStates
  Enum Values:
    [0] Value=0 Name=ESLFAIStates::Idle Display=Idle
    [1] Value=1 Name=ESLFAIStates::RandomRoam Display=Random Roam
    [2] Value=2 Name=ESLFAIStates::Patrolling Display=Patrolling
    [3] Value=3 Name=ESLFAIStates::Investigating Display=Investigating
    [4] Value=4 Name=ESLFAIStates::Combat Display=Combat
    [5] Value=5 Name=ESLFAIStates::PoiseBroken Display=Poise Broken
    [6] Value=6 Name=ESLFAIStates::Uninterruptable Display=Uninterruptable
    [7] Value=7 Name=ESLFAIStates::OutOfBounds Display=Out of Bounds
```

**Analysis:**
- Both have the same values (0-7) with same display names
- bp_only uses Blueprint enum with scrambled internal names (NewEnumerator0, etc.)
- SLFConversion uses C++ enum with proper internal names

---

## Key Difference #2: BT_Combat Root Decorators (CRITICAL!)

### bp_only (Original)
```
--- Root Decorators ---
[Root][0] Blackboard Based Condition
    Key: InCombat
    IntValue: 0

Total Decorators: 5
State Key Decorators: 0
```

### SLFConversion (Current)
```
--- Root Decorators ---
[Root][0] Is Target Set? - Key: Target
    IntValue: 3
[Root][1] Is State == Combat? - Key: State
    IntValue: 4
    ^^^ STATE KEY DECORATOR - IntValue=4 ^^^

Total Decorators: 7
State Key Decorators: 1
```

**Analysis:**
- **bp_only**: BT_Combat only checks if `InCombat` boolean is set
- **SLFConversion**: BT_Combat checks BOTH `Target` is set AND `State == 4`
- The State==4 decorator was ADDED during migration - it doesn't exist in original!
- This added decorator may be blocking execution

---

## Key Difference #3: BT_Enemy Structure

### Both Projects (Same)
```
Node: Selector (BTComposite_Selector)
  NumChildren: 6
  [Branch 0] Child: POISE BROKEN MODE (BTTask_RunBehavior), Decorators: 0
  [Branch 1] Child: UNINTERRUPTABLE MODE (BTTask_RunBehavior), Decorators: 0
  [Branch 2] Child: OUT OF BOUNDS MODE (BTTask_RunBehavior), Decorators: 0
  [Branch 3] Child: COMBAT MODE (BTTask_RunBehavior), Decorators: 0
  [Branch 4] Child: INVESTIGATE MODE (BTTask_RunBehavior), Decorators: 0
  [Branch 5] Child: PASSIVE MODES (BTComposite_Selector), Decorators: 0

Total Decorators: 0
State Key Decorators: 0
```

**Analysis:** The main BT_Enemy is the same - no decorators at the top level.

---

## Key Difference #4: BT Task Parent Classes

### bp_only (Original)
Tasks like BTT_SimpleMoveTo_C, BTT_ToggleFocus_C are Blueprint classes parented to:
- `BTTask_BlueprintBase` (Unreal Engine base class)

### SLFConversion (Current)
Tasks are reparented to C++ classes:
- BTT_SimpleMoveTo_C → `/Script/SLFConversion.BTT_SimpleMoveTo`
- BTT_ToggleFocus_C → `/Script/SLFConversion.BTT_ToggleFocus`
- etc.

**Analysis:** This is expected from migration. C++ classes have full logic implemented.

---

## How the Original BT Works (bp_only)

1. **BT_Enemy** runs subtrees via `BTTask_RunBehavior` in order
2. Each subtree has its OWN condition decorators:
   - BT_Combat: Checks `InCombat` boolean
   - BT_Idle: Checks its conditions
   - etc.
3. **NO State enum checking at BT level** - State is used by code to set `InCombat` and other booleans
4. The subtrees select themselves based on boolean keys, not the State enum

---

## How Our BT Works (SLFConversion) - THE PROBLEM

1. **BT_Enemy** runs subtrees via `BTTask_RunBehavior` in order
2. **BT_Combat has ADDED decorators**:
   - `Is Target Set?` - Check if Target blackboard key has a value
   - `Is State == Combat?` - Check if State == 4
3. These decorators were ADDED during migration but may not be working correctly
4. The `Is Target Set?` check with `IntValue: 3` is suspicious

---

## Why Combat Doesn't Work

Based on debug logs:
```
[SLF_PERCEPTION_SENSED] Player detected on B_Soulslike_Enemy_Demo_C_1! Switching to Combat state
[AIBehaviorManager] SetTarget: B_Soulslike_Character_C_0
[AIBehaviorManager] Blackboard Target key updated
[AIBehaviorManager] SetState on B_Soulslike_Enemy_Demo_C_1: 0 -> 4
[AIBehaviorManager] Blackboard State key updated to 4
```

Then immediately:
```
[SLF_PERCEPTION] bCurrentlySensed=0, CurrentState=4
[SLF_PERCEPTION_LOST] Player lost! Switching to Investigating state
[AIBehaviorManager] SetState: 4 -> 3
```

**Sequence of events:**
1. Player detected → State set to 4 (Combat)
2. Target set in blackboard
3. Almost immediately, perception reports player "lost"
4. State switches to 3 (Investigating) before BT can execute Combat tasks

**The perception flickering is the immediate cause**, but the BT structure differences may be contributing.

---

## Recommended Fix Options

### Option 1: Restore Original BT Assets from bp_only
Copy the behavior tree assets from bp_only to SLFConversion:
- BT_Enemy.uasset
- All subtrees in Subtrees/

This restores the original working structure (no State decorators at subtree level).

### Option 2: Fix Perception Flickering
The perception system is rapidly detecting/losing the player. Fix the debounce or perception config.

### Option 3: Remove Added Decorators
Remove the State==4 and Target decorators that were added to BT_Combat, restoring original InCombat-based logic.

---

## Files to Compare

| Asset | bp_only Path | Status |
|-------|--------------|--------|
| BB_Standard | Same path | **DIFFERENT** - Enum type changed |
| BT_Enemy | Same path | Same structure |
| BT_Combat | Same path | **DIFFERENT** - Decorators added |
| BT_Idle | Same path | Needs comparison |
| E_AI_States | Same path | Exists in bp_only, NOT in SLFConversion |

---

## Next Steps

1. **Decide on approach**: Restore original BTs OR fix current implementation
2. **If restoring**: Copy BT assets from bp_only to SLFConversion
3. **If fixing**: Remove State decorators from subtrees, fix perception flickering
