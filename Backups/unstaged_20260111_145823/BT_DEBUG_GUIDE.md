# Behavior Tree Debug Guide

## What's Been Added

### 1. AIController BT Debug Logging (AIC_SoulslikeFramework.cpp)
When an enemy is in Combat state, logs every 2 seconds:
- `[AIC_BT_DEBUG]` - BT State information
- CurrentTree name
- ActiveNode (currently executing node)

### 2. Enhanced Blackboard Logging (AIBehaviorManagerComponent.cpp)
Every 0.5 seconds logs:
- State value (0=Idle, 4=Combat)
- InCombat boolean
- Target actor
- **NEW:** CanAttack boolean
- **NEW:** CanStrafe boolean

### 3. Custom BTD_StateEquals Decorator (BTD_StateEquals.h/cpp)
A replacement for BTDecorator_Blackboard that logs every evaluation:
- Current state value
- Required state value  
- PASS/FAIL result

### 4. BTS_DebugLog Service (BTS_DebugLog.h/cpp)
A BTService that can be added to any BT node to log:
- Service tick
- Blackboard state at that point

## What to Look For in PIE

### Expected Log Pattern When Working Correctly:
```
[AIBehaviorManager] TICK DEBUG on B_Soulslike_Enemy_Demo_C_1:
  Blackboard State=4 (Combat=4), InCombat=TRUE, Target=B_Soulslike_Character_C_0
  CanAttack=FALSE, CanStrafe=FALSE

[AIC_BT_DEBUG] B_Soulslike_Enemy_Demo_C_1 - BT State:
  IsRunning: YES, IsPaused: NO
  CurrentTree: BT_Enemy
  ActiveNode: BTTask_SimpleMoveTo - Simply Move To   <-- THIS IS THE KEY!

>>> BTT_SimpleMoveTo::ExecuteTask CALLED <<<   <-- Movement task executing
```

### Problem Pattern (What We're Seeing):
```
[AIBehaviorManager] TICK DEBUG on B_Soulslike_Enemy_Demo_C_1:
  Blackboard State=4 (Combat=4), InCombat=TRUE, Target=B_Soulslike_Character_C_0
  CanAttack=???, CanStrafe=???   <-- Check these values!

[AIC_BT_DEBUG] B_Soulslike_Enemy_Demo_C_1 - BT State:
  IsRunning: YES, IsPaused: NO
  CurrentTree: BT_Enemy
  ActiveNode: ???   <-- What node is active?

(No BTT_SimpleMoveTo log appears)   <-- Task never executes
```

## BT_Combat Flow Analysis

For BTT_SimpleMoveTo to execute, this path must be taken:
1. BT_Combat root decorators PASS:
   - Target Is Set ✓
   - State == Combat ✓

2. Inside BT_Combat Selector:
   - Child[0] (InCombat NOT Set) → FAIL (because InCombat=TRUE)
   - Child[1] (InCombat IS Set) → PASS → Enter IN COMBAT

3. Inside IN COMBAT Selector:
   - Child[0] (CanAttack IS Set) → Must FAIL for movement
   - Child[1] (CanAttack NOT Set) → PASS → Enter MOVEMENT

4. Inside MOVEMENT Selector:
   - Child[0] (CanStrafe IS Set) → Must FAIL for SimpleMoveTo
   - Child[1] (no decorator) → Execute MOVE TO ATTACK RADIUS

5. MOVE TO ATTACK RADIUS Sequence:
   - [0] BTT_ToggleFocus → Execute → Succeed
   - [1] BTT_SimpleMoveTo → Execute → IN PROGRESS (enemy moves!)
   - [2] BTT_SetKey → Execute when move completes

## Troubleshooting Checklist

1. **Check CanAttack value** - If TRUE, BT will try attacks instead of movement
2. **Check CanStrafe value** - If TRUE, BT will strafe instead of direct movement
3. **Check ActiveNode** - What node is the BT currently on?
4. **Check if BTT_ToggleFocus executes** - It runs before BTT_SimpleMoveTo
5. **Check CurrentTree** - Should be BT_Enemy or BT_Combat (subtree)
