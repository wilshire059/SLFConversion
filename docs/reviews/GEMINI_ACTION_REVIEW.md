# Action System Investigation - Peer Review Request

## Context
We're investigating why dodge/jump/sprint can be activated while in the air in the C++ migration, but NOT in the original bp_only Blueprint project.

## CRITICAL FINDINGS

### Issue 1: Missing Stamina Check in C++ Input Handlers

**Original Blueprint flow (B_Soulslike_Character.json lines 64230-64340):**
```
1. IA_Sprint_Dodge input triggered
2. Check ElapsedSeconds <= 0.2 (tap for dodge, hold for sprint)
3. DoOnce gate -> IsStatMoreThan(StaminaTag, Threshold) on StatManager
4. Branch on IsStatMoreThan result:
   - TRUE (has stamina): Check IsCrouched, then QueueAction(Dodge) or QueueAction(Crouch)
   - FALSE (no stamina): ExecuteActionImmediately (out-of-stamina feedback)
```

**C++ Implementation (SLFSoulslikeCharacter.cpp line 528-532):**
```cpp
void ASLFSoulslikeCharacter::HandleDodge()
{
    // NO STAMINA CHECK!
    QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Dodge")));
}
```

**Missing:** The IsStatMoreThan check before queuing the dodge action.

### Issue 2: Input Binding Timing Wrong

**Original Blueprint:**
- Dodge triggers on `IA_Sprint_Dodge` **Completed** (when button released)
- Checks `ElapsedSeconds <= 0.2` to distinguish tap (dodge) from hold (sprint)

**C++ Implementation:**
```cpp
// Lines 417-425 in SLFSoulslikeCharacter.cpp
if (IA_Sprint)
{
    EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleSprintStarted);
    EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleSprintCompleted);
}
if (IA_Dodge)
{
    EnhancedInput->BindAction(IA_Dodge, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleDodge);
}
```

**Problem:** Dodge is bound to Started, not Completed. And both IA_Sprint and IA_Dodge are the SAME input action asset!

### Issue 3: CheckStatRequirement is Stubbed

**B_Action.cpp lines 174-205:**
```cpp
void UB_Action::CheckStatRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot, bool& OutSuccess, bool& OutSuccess1)
{
    // ... gets stamina multiplier and action data ...
    double RequiredStamina = ActionData->StaminaCost * Multiplier;  // WRONG PROPERTY!

    // For now, assume success if stat manager exists - actual implementation depends on StatManager API
    OutSuccess = true;   // ALWAYS TRUE - NO ACTUAL CHECK!
    OutSuccess1 = true;
}
```

**Original Blueprint (B_Action.json):**
- Uses `ActionData->RequiredStatTag` and `ActionData->RequiredStatAmount`
- Calls `StatManager->IsStatMoreThan(RequiredStatTag, RequiredStatAmount * Multiplier)`
- Returns actual result

## Blocking Conditions Summary

### Original Blueprint Has These Checks:
1. **IsDead?** from CombatManager (in B_Soulslike_Character OnInputBufferConsumed)
2. **IsOnLadder** from ActionManager (in AC_ActionManager PerformAction)
3. **Action in AvailableActions map** (in AC_ActionManager PerformAction)
4. **IsStatMoreThan stamina check** (in B_Soulslike_Character before QueueAction)

### C++ Implementation Missing:
1. **Stamina check before queuing dodge** - HandleDodge() has no check
2. **Proper CheckStatRequirement implementation** - Always returns true
3. **Input timing logic** - Dodge triggers on Started instead of Completed with elapsed time check

## Stamina Requirements from Data Assets

| Action | RequiredStatTag | RequiredStatAmount |
|--------|-----------------|-------------------|
| Dodge | Stamina | 5.0 |
| Jump | Stamina | 5.0 |
| Sprint | (empty) | 0.0 |
| Light Attack | Stamina | 5.0 |
| Heavy Attack | Stamina | 0.0 |

## Why "Dodge While Falling" Works in C++

The most likely explanation is that the **input buffer** behavior combined with **missing stamina checks** allows immediate action execution:

1. In bp_only: If stamina is low (drained by falling animation or recent action), IsStatMoreThan fails, blocking dodge
2. In C++: No stamina check, so dodge always queues
3. If buffer is closed (not in animation), dodge executes immediately

Additionally, there may be animation notifies keeping the buffer open during fall that aren't working in C++.

## Questions for Gemini Review

1. Is my analysis of the original Blueprint flow correct?
2. Are there other blocking conditions I might have missed?
3. What's the correct way to implement the stamina check in the C++ input handlers?
4. Should we add an explicit IsFalling check, or is the stamina system sufficient?

## Files Analyzed

### Blueprint JSON Exports:
- B_Soulslike_Character.json - SPRINT & DODGES section (lines 64230+)
- AC_ActionManager.json - Event PerformAction function
- AC_InputBuffer.json - Queue and consume logic
- B_Action.json - CheckStatRequirement function
- DA_Action_Dodge.json, DA_Action_Jump.json - RequiredStatAmount values

### C++ Files Needing Fixes:
- `SLFSoulslikeCharacter.cpp` - HandleDodge, HandleJump need stamina checks
- `B_Action.cpp` - CheckStatRequirement_Implementation needs proper logic
- Potentially add IsFalling checks if stamina alone isn't sufficient

## Recommended Fixes

### Fix 1: Add Stamina Check to HandleDodge
```cpp
void ASLFSoulslikeCharacter::HandleDodge()
{
    // Check stamina before queuing
    if (CachedStatManager)
    {
        FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
        if (CachedStatManager->IsStatMoreThan(StaminaTag, 5.0))  // RequiredStatAmount from DA_Action_Dodge
        {
            QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Dodge")));
        }
        else
        {
            // Out of stamina - could play feedback here
            UE_LOG(LogTemp, Warning, TEXT("Dodge blocked - not enough stamina"));
        }
    }
}
```

### Fix 2: Fix Input Binding Timing
```cpp
// In SetupPlayerInputComponent - sprint/dodge use SAME input action
if (IA_Sprint)  // IA_Sprint == IA_Dodge == IA_Sprint_Dodge
{
    EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Ongoing, this, &ASLFSoulslikeCharacter::HandleSprintOngoing);
    EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleSprintDodgeCompleted);
}

// HandleSprintDodgeCompleted checks elapsed time to decide dodge vs sprint stop
```

### Fix 3: Fix CheckStatRequirement Implementation
```cpp
void UB_Action::CheckStatRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot, bool& OutSuccess, bool& OutSuccess1)
{
    // ... get stat manager and action data ...

    // Use RequiredStatTag and RequiredStatAmount, not StaminaCost
    FGameplayTag RequiredStatTag = ActionData->RequiredStatTag;
    double RequiredAmount = ActionData->RequiredStatAmount;
    double Multiplier = GetWeaponStaminaMultiplier(StaminaMultiplierWeaponSlot);
    double Threshold = RequiredAmount * Multiplier;

    // Call actual stat check
    OutSuccess = StatManager->IsStatMoreThan(RequiredStatTag, Threshold);
    OutSuccess1 = OutSuccess;
}
```

Please review and provide feedback on this analysis.
