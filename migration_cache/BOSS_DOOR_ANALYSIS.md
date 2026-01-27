# Boss Door Analysis - Issues and Fixes

## Summary

The boss door has two issues:
1. **Door scale appears wrong** (user report)
2. **Player doesn't automatically move through the door** (animation/movement missing)

## Root Cause Analysis

### Issue 1: Player Movement Through Door Not Working

The original Blueprint B_Door flow:
1. Player interacts with door
2. Door's `OnInteract` calls player's `OpenDoor` via BPI_GenericCharacter interface
3. Player's `OpenDoor` plays montage AND moves player to door's `MoveTo` location
4. Door rotates/opens via DoorTimeline

**Current C++ B_Door::OnInteract:**
- Only calls `OpenDoor()` on the door itself (starts rotation animation)
- Does NOT call `OpenDoor` on the player via interface
- Missing `MoveTo` BillboardComponent

**Current C++ SLFBaseCharacter::OpenDoor_Implementation:**
- Only plays the montage
- Does NOT move player to the door's MoveTo location
- Creates circular call: OpenDoor calls OnInteract on door

### Issue 2: Door Scale

Debug logs show door has correct scale (3.25, 0.1, 4.25) but:
- Scale is in units (325cm x 10cm x 425cm)
- This is for a cube mesh (100x100x100 units by default)
- Final visual size: ~6.5m wide x 8.5m tall
- May not match the actual doorway in the level

## Blueprint B_Door Variables (from JSON export)

```
Variables:
- MoveToDistance (double): Distance to teleport player through door
- MoveTo (BillboardComponent): Target location marker
- InteractMontage_RH, InteractMontage_LH: Player montages
- YawRotationAmount: Door rotation angle
- PlayRate: Door animation speed
- DoorTimeline: Timeline component for door animation
```

## Required Fixes

### Fix 1: Add MoveTo Component to B_Door

```cpp
// In B_Door.h
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Components")
UBillboardComponent* MoveTo;
```

### Fix 2: Update B_Door::OnInteract

```cpp
void AB_Door::OnInteract_Implementation(AActor* InteractingActor)
{
    // ... existing validation ...

    // Calculate MoveTo position based on player direction
    FVector DoorForward = GetActorForwardVector();
    FVector ToPlayer = InteractingActor->GetActorLocation() - GetActorLocation();
    ToPlayer.Normalize();
    float DotProduct = FVector::DotProduct(DoorForward, ToPlayer);
    OpenForwards = (DotProduct >= 0.0f);

    // Update MoveTo location
    if (MoveTo)
    {
        FVector TargetOffset = OpenForwards ?
            (DoorForward * MoveToDistance) :
            (-DoorForward * MoveToDistance);
        MoveTo->SetRelativeLocation(TargetOffset);
    }

    // Select montage
    SelectedMontage = OpenForwards ?
        InteractMontage_RH.LoadSynchronous() :
        InteractMontage_LH.LoadSynchronous();

    // CRITICAL: Call OpenDoor on the PLAYER (not the door)
    if (InteractingActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
    {
        IBPI_GenericCharacter::Execute_OpenDoor(InteractingActor, SelectedMontage, this);
    }

    // Start door animation
    OpenDoor();
}
```

### Fix 3: Update SLFBaseCharacter::OpenDoor_Implementation

```cpp
void ASLFBaseCharacter::OpenDoor_Implementation(UAnimMontage* Montage, AActor* Door)
{
    // Play montage
    if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
    {
        GetMesh()->GetAnimInstance()->Montage_Play(Montage);
    }

    // Move player to door's MoveTo location
    if (AB_Door* DoorActor = Cast<AB_Door>(Door))
    {
        if (UBillboardComponent* MoveToComp = DoorActor->MoveTo)
        {
            FVector TargetLocation = MoveToComp->GetComponentLocation();
            TargetLocation.Z = GetActorLocation().Z; // Keep player Z

            // Use smooth interpolation over montage duration
            // Or simple teleport at end of montage
            SetActorLocation(TargetLocation);
        }
    }
}
```

## Class Relationships

```
ASLFBossDoor (in Blueprints/Actors/) - Standalone implementation with FogGateMesh
    └── Inherits: ASLFDoorBase

AB_BossDoor (in Blueprints/) - Used by B_BossDoor Blueprint
    └── Inherits: AB_Door
        └── Inherits: AB_Interactable

B_BossDoor_C (Blueprint)
    └── Inherits: AB_BossDoor
```

The level uses `B_BossDoor_C` (the Blueprint class inheriting from AB_BossDoor).

## Next Steps

1. Add MoveTo component to AB_Door constructor
2. Fix AB_Door::OnInteract to call player's OpenDoor
3. Fix ASLFBaseCharacter::OpenDoor to move player
4. Test in PIE
