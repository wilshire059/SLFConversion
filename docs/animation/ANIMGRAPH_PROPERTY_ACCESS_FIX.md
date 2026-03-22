# AnimGraph Property Access Fix Required

## Problem
The AnimBP `ABP_SoulslikeCharacter_Additive` has a Property Access node in the AnimGraph that's looking for a property named `IsGuarding?` (with question mark). C++ cannot have `?` in identifiers, so the property is named `IsGuarding` (no `?`).

**Result**: The Property Access fails to resolve, always returns `false`, and guard animations may not play.

## Warning Message
```
[Compiler] Invalid field 'IsGuarding?' found in property path for Property Access
```

## Why This Happened
1. Original Blueprint AnimBP had a variable `IsGuarding?` in CombatManager
2. AnimGraph uses Property Access nodes to read this variable
3. After migrating CombatManager to C++, the property became `IsGuarding` (no `?`)
4. Property Access uses exact FName matching - `IsGuarding?` ≠ `IsGuarding`
5. CoreRedirects don't help because Property Access has its own resolution system

## Fix (Manual - Editor Required)

1. Open `Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive` in Unreal Editor

2. Find the AnimGraph's Property Access node that reads `IsGuarding?`
   - It might be in a state machine transition
   - Or in a blend node condition

3. Change the Property Access path from:
   - `CombatManager.IsGuarding?`
   - to `bIsBlocking` (local variable set by C++ NativeUpdateAnimation)

4. Compile and save the AnimBP

## Alternative C++ Workaround (Already Implemented)
The C++ AnimInstance `UABP_SoulslikeCharacter_Additive` already has:
```cpp
UPROPERTY(BlueprintReadWrite)
bool bIsBlocking;

void NativeUpdateAnimation(float DeltaSeconds) {
    bIsBlocking = CombatManager->IsGuarding;
}
```

The AnimGraph just needs to read `bIsBlocking` instead of `CombatManager.IsGuarding?`.

## Impact If Not Fixed
- Guard animations may not play
- Character may not visually enter guard stance
- Gameplay logic still works (C++ handles guard state)
- Only visual/animation feedback is affected
