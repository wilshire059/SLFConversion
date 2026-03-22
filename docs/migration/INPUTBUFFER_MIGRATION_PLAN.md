# AC_InputBuffer Migration Plan

## Overview

**Target:** AC_InputBuffer (Blueprint ActorComponent)
**Approach:** BlueprintNativeEvent - C++ provides implementation, Blueprint becomes thin wrapper
**Complexity:** LOW (4 variables, 4 functions, 1 event dispatcher)
**Priority:** HIGH (directly affects input feel/responsiveness)

---

## Why AC_InputBuffer First?

| Factor | Value |
|--------|-------|
| File Size | 98KB (smallest component) |
| Variables | 4 (simple types) |
| Functions | 4 (straightforward logic) |
| Dependencies | 8 files (manageable) |
| Performance Impact | HIGH (input latency) |
| Risk | LOW (limited blast radius) |

---

## Current Blueprint Structure

### Variables (4)
| Name | Type | Category | Notes |
|------|------|----------|-------|
| `IncomingActionTag` | FGameplayTag | Runtime | Stores queued action |
| `IgnoreNextOfActions` | FGameplayTagContainer | Config | Actions to skip (Instance Editable) |
| `BufferOpen?` | bool | Runtime | Buffer accepts input |
| `OnInputBufferConsumed` | MulticastDelegate | Runtime | Event dispatcher |

### Functions (4)
| Name | Parameters | Returns | Logic |
|------|------------|---------|-------|
| `QueueAction` | QueuedActionTag: FGameplayTag | void | Set IncomingActionTag, fire event if buffer open |
| `ConsumeInputBuffer` | none | void | Fire OnInputBufferConsumed, clear IncomingActionTag |
| `ToggleBuffer` | BufferOpen?: bool | void | Set BufferOpen? variable |
| `ExecuteActionImmediately` | Action: FGameplayTag | void | Bypass buffer, fire event directly |

### Event Dispatchers (1)
| Name | Parameters |
|------|------------|
| `OnInputBufferConsumed` | Action: FGameplayTag |

### Custom Events (1)
| Name | Parameters | Notes |
|------|------------|-------|
| `Event QueueNext` | ActionTag: FGameplayTag | Internal event, chains to check buffer |

---

## Dependencies (8 files)

### Direct Users (Must Test After Migration)

| File | Type | How It Uses AC_InputBuffer |
|------|------|----------------------------|
| `ANS_InputBuffer` | AnimNotifyState | Calls `ToggleBuffer()` - opens/closes buffer during animation |
| `AN_TryGuard` | AnimNotify | Uses buffer for guard input |
| `B_Action` | Blueprint | Has `GetInputBuffer()` function, calls various functions |
| `B_Soulslike_Character` | Blueprint | Owns the component instance |

### Indirect/Debug Users (Lower Priority)
| File | Type | Usage |
|------|------|-------|
| `AC_DebugCentral` | Component | Debug display |
| `AC_InventoryManager` | Component | May queue actions |
| `W_DebugWindow` | Widget | Debug UI |
| `W_Debug_ComponentTooltip` | Widget | Debug UI |

---

## Migration Order

```
Phase 1: C++ Foundation
├── 1. Create UInputBufferComponent.h
├── 2. Create UInputBufferComponent.cpp
└── 3. Compile C++

Phase 2: Blueprint Migration (Python Automated)
├── 4. Reparent AC_InputBuffer → UInputBufferComponent
├── 5. Delete BP function implementations (C++ takes over)
└── 6. Compile Blueprints

Phase 3: Verification
├── 7. Test ANS_InputBuffer (animation notify)
├── 8. Test B_Action (action execution)
├── 9. Test B_Soulslike_Character (play test)
└── 10. Test input feel in combat
```

---

## C++ Implementation

### Header: `Source/SLFConversion/InputBufferComponent.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InputBufferComponent.generated.h"

// Event dispatcher signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBufferConsumed, FGameplayTag, Action);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UInputBufferComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputBufferComponent();

    //~ Variables (match BP names exactly)

    UPROPERTY(BlueprintReadWrite, Category = "Runtime")
    FGameplayTag IncomingActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    FGameplayTagContainer IgnoreNextOfActions;

    UPROPERTY(BlueprintReadWrite, Category = "Runtime")
    bool bBufferOpen = false;  // Note: BP name is "BufferOpen?" but C++ can't use ?

    //~ Event Dispatcher

    UPROPERTY(BlueprintAssignable, Category = "Runtime")
    FOnInputBufferConsumed OnInputBufferConsumed;

    //~ Functions (BlueprintNativeEvent = C++ default, BP can override)

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
    void QueueAction(FGameplayTag QueuedActionTag);
    virtual void QueueAction_Implementation(FGameplayTag QueuedActionTag);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
    void ConsumeInputBuffer();
    virtual void ConsumeInputBuffer_Implementation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
    void ToggleBuffer(bool bOpen);
    virtual void ToggleBuffer_Implementation(bool bOpen);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input Buffer")
    void ExecuteActionImmediately(FGameplayTag Action);
    virtual void ExecuteActionImmediately_Implementation(FGameplayTag Action);

protected:
    // Internal: Called when action should be processed
    void ProcessAction(FGameplayTag Action);

    // Check if action should be ignored
    bool ShouldIgnoreAction(FGameplayTag Action) const;
};
```

### Source: `Source/SLFConversion/InputBufferComponent.cpp`

```cpp
#include "InputBufferComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInputBufferComponent::QueueAction_Implementation(FGameplayTag QueuedActionTag)
{
    // Store the incoming action
    IncomingActionTag = QueuedActionTag;

    // If buffer is open and action isn't ignored, process immediately
    if (bBufferOpen && !ShouldIgnoreAction(QueuedActionTag))
    {
        ProcessAction(QueuedActionTag);
    }
}

void UInputBufferComponent::ConsumeInputBuffer_Implementation()
{
    if (IncomingActionTag.IsValid())
    {
        ProcessAction(IncomingActionTag);
        IncomingActionTag = FGameplayTag();  // Clear after consuming
    }
}

void UInputBufferComponent::ToggleBuffer_Implementation(bool bOpen)
{
    bBufferOpen = bOpen;

    // If opening buffer and we have a queued action, process it
    if (bOpen && IncomingActionTag.IsValid() && !ShouldIgnoreAction(IncomingActionTag))
    {
        ProcessAction(IncomingActionTag);
    }
}

void UInputBufferComponent::ExecuteActionImmediately_Implementation(FGameplayTag Action)
{
    // Bypass buffer entirely
    ProcessAction(Action);
}

void UInputBufferComponent::ProcessAction(FGameplayTag Action)
{
    if (Action.IsValid())
    {
        OnInputBufferConsumed.Broadcast(Action);
    }
}

bool UInputBufferComponent::ShouldIgnoreAction(FGameplayTag Action) const
{
    return IgnoreNextOfActions.HasTag(Action);
}
```

---

## Python Automation Script

### File: `migrate_inputbuffer.py`

The Python script will:
1. Find AC_InputBuffer Blueprint
2. Reparent it to UInputBufferComponent
3. Delete the Blueprint function graphs (C++ takes over)
4. Save the Blueprint

```python
import unreal

def migrate_ac_inputbuffer():
    """
    Migrate AC_InputBuffer to inherit from UInputBufferComponent.
    C++ provides the function implementations.
    """

    # Find the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
    bp = unreal.load_asset(bp_path)

    if not bp:
        print(f"[ERROR] Could not load Blueprint: {bp_path}")
        return False

    # Get the C++ class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.InputBufferComponent")

    if not cpp_class:
        print("[ERROR] Could not find UInputBufferComponent C++ class")
        print("Make sure you compiled the C++ code first!")
        return False

    # Get Blueprint library
    bp_lib = unreal.BlueprintEditorLibrary

    # Reparent the Blueprint
    print(f"[INFO] Reparenting AC_InputBuffer to UInputBufferComponent...")
    success = bp_lib.reparent_blueprint(bp, cpp_class)

    if not success:
        print("[ERROR] Failed to reparent Blueprint")
        return False

    print("[SUCCESS] AC_InputBuffer now inherits from UInputBufferComponent")

    # Compile the Blueprint
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Save the Blueprint
    unreal.EditorAssetLibrary.save_asset(bp_path)

    print("[DONE] Migration complete!")
    print("")
    print("Next steps:")
    print("1. Open AC_InputBuffer in Blueprint Editor")
    print("2. Delete the function implementations (graphs)")
    print("3. C++ implementations will be used instead")
    print("4. Test in-game")

    return True

# Run migration
if __name__ == "__main__":
    migrate_ac_inputbuffer()
```

---

## Manual Steps (After Running Script)

1. **Open AC_InputBuffer** in Blueprint Editor
2. **Delete function graphs** for:
   - QueueAction
   - ConsumeInputBuffer
   - ToggleBuffer
   - ExecuteActionImmediately
3. **Keep Event QueueNext** (or convert to C++ later)
4. **Compile** and Save

---

## Verification Checklist

### Compilation
- [ ] C++ compiles without errors
- [ ] AC_InputBuffer compiles without errors
- [ ] All dependent Blueprints compile without errors

### Functionality
- [ ] ANS_InputBuffer still toggles buffer during animations
- [ ] B_Action can still get and use InputBuffer
- [ ] Input buffering works in combat
- [ ] Actions execute with proper timing

### Performance (The Whole Point!)
- [ ] Input feels more responsive
- [ ] No dropped inputs during combat
- [ ] Smooth action chaining

---

## Rollback Plan

If migration fails:
```powershell
# Restore AC_InputBuffer from backup
cp "C:/scripts/SLFConversion_Migration/Backups/blueprint_only/Content/SoulslikeFramework/Blueprints/Components/AC_InputBuffer.uasset" "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/Components/"
```

---

## Future Improvements (Post-Migration)

Once basic migration is verified working:

1. **Add Timestamp Tracking**
   - Track when each action was queued
   - Allow "early out" for animation interrupts

2. **Circular Buffer**
   - Queue multiple actions (TCircularQueue)
   - Process in order

3. **TickGroup Optimization**
   - Set component to tick in PrePhysics
   - Ensures input processed before physics

4. **Input Prediction**
   - For networked games, predict input on client
   - Server validates

---

## Timeline

| Step | Action |
|------|--------|
| 1 | Create C++ files (5 min) |
| 2 | Compile C++ (2 min) |
| 3 | Run Python script (1 min) |
| 4 | Delete BP function graphs (2 min) |
| 5 | Test in editor (5 min) |
| **Total** | **~15 minutes** |
