# Custom Animation Nodes - Strategy for AnimBP Migration

## THE STRATEGY

**For Animation Blueprints:** "Create custom K2 anim graph nodes that call C++ functions for ALL logic"

**CRITICAL:** Both EventGraph AND AnimGraph logic move to C++. This avoids variable conflicts between C++ and Blueprint that cause reparenting issues.

### What This Means:
- **EventGraph** → `NativeUpdateAnimation()` in C++ AnimInstance
- **AnimGraph nodes** → Custom `FAnimNode_*` classes in C++
- **State Machines** → Can be `FAnimNode_StateMachine` or custom implementation
- **Blend logic** → Custom `FAnimNode_*` that handles blending in C++
- **AnimBP becomes empty shell** → Just references the C++ node tree

---

## Architecture Overview

### Two Classes Required

| Class Type | Base Class | Purpose | Module |
|------------|------------|---------|--------|
| **Anim Graph Node** | `UAnimGraphNode_Base` (UObject) | Editor visualization, tooltips, pins | Editor module (not shipped) |
| **Anim Behavior Node** | `FAnimNode_Base` (UStruct) | Runtime work, actual blending | Runtime module |

### Key Relationship
```cpp
// Graph Node KNOWS about Behavior Node
class UAnimGraphNode_SequencePlayer : public UAnimGraphNode_Base
{
    GENERATED_UCLASS_BODY()

    UPROPERTY(EditAnywhere, Category=Settings)
    FAnimNode_SequencePlayer Node;  // Contains the behavior node
};

// Behavior Node does NOT know about Graph Node
struct ENGINE_API FAnimNode_SequencePlayer : public FAnimNode_Base
{
    // Runtime behavior only
};
```

**Critical:** Graph nodes exist only in editor. Behavior nodes exist at runtime. This separation is for performance - spawning characters doesn't create expensive graph nodes.

---

## Anim Behavior Node Core Functions

### FAnimNode_Base Interface

```cpp
struct ENGINE_API FAnimNode_Base
{
    // Called when initializing/reinitializing (e.g., mesh change)
    virtual void Initialize(const FAnimationInitializeContext& Context) {}

    // Called to update state (play time, blend weights)
    // DeltaTime and blend weight available in Context
    virtual void Update(const FAnimationUpdateContext& Context) {}

    // Called to generate pose (list of bone transforms)
    virtual void Evaluate(FPoseContext& Output) { check(false); }

    // Called to refresh bone indices referenced by node
    virtual void CacheBones(const FAnimationCacheBonesContext& Context) {}

    // Called for "ShowDebug Animation" data
    virtual void GatherDebugData(FNodeDebugData& DebugData) {}
};
```

### Evaluate Example (Sequence Evaluator)
```cpp
void FAnimNode_SequenceEvaluator::Evaluate(FPoseContext& Output)
{
    if ((Sequence != NULL) &&
        (Output.AnimInstance->CurrentSkeleton->IsCompatible(Sequence->GetSkeleton())))
    {
        Output.AnimInstance->SequenceEvaluatePose(Sequence, Output.Pose,
            FAnimExtractContext(ExplicitTime));
    }
    else
    {
        Output.ResetToRefPose();
    }
}
```

---

## FPoseLink - Critical for Connections

### Declaration Creates Visual Pin
```cpp
UPROPERTY(Category=Links)
FPoseLink BasePose;  // Shows as pose input pin in editor
```

### Must Call Children's Functions
```cpp
// In your Update, call child's Update
BasePose->Update(Context);

// In your CacheBones, call child's CacheBones
BasePose->CacheBones(Context);
```

### CacheBones Example (Blend List)
```cpp
void FAnimNode_BlendListBase::CacheBones(const FAnimationCacheBonesContext& Context)
{
    for(int32 ChildIndex = 0; ChildIndex < BlendPose.Num(); ChildIndex++)
    {
        BlendPose[ChildIndex].CacheBones(Context);
    }
}
```

---

## Skeletal Control Nodes

### Different Base Class
```cpp
// Works in Component space
struct ENGINE_API FAnimNode_SkeletalControlBase : public FAnimNode_Base
{
    virtual void Initialize(const FAnimationInitializeContext& Context) override;
    virtual void CacheBones(const FAnimationCacheBonesContext& Context) override;
    virtual void Update(const FAnimationUpdateContext& Context) override;
    virtual void EvaluateComponentSpace(FComponentSpacePoseContext& Output) override;
};
```

### EvaluateComponentSpace Example (Copy Bone)
```cpp
void FAnimNode_CopyBone::EvaluateBoneTransforms(
    USkeletalMeshComponent* SkelComp,
    FCSPose<FCompactPose>& MeshBases,
    TArray<FBoneTransform>& OutBoneTransforms)
{
    check(OutBoneTransforms.Num() == 0);

    if (!bCopyTranslation && !bCopyRotation && !bCopyScale)
        return;

    const FBoneContainer& BoneContainer = MeshBases.GetPose().GetBoneContainer();
    FCompactPoseBoneIndex TargetBoneIndex = TargetBone.GetCompactPoseIndex(BoneContainer);

    const FTransform& SourceBoneTM = MeshBases.GetComponentSpaceTransform(
        SourceBone.GetCompactPoseIndex(BoneContainer));
    FTransform CurrentBoneTM = MeshBases.GetComponentSpaceTransform(TargetBoneIndex);

    if (bCopyTranslation)
        CurrentBoneTM.SetTranslation(SourceBoneTM.GetTranslation());
    if (bCopyRotation)
        CurrentBoneTM.SetRotation(SourceBoneTM.GetRotation());
    if (bCopyScale)
        CurrentBoneTM.SetScale3D(SourceBoneTM.GetScale3D());

    // Return bone transforms (preserve parent->child order!)
    OutBoneTransforms.Add(FBoneTransform(TargetBoneIndex, CurrentBoneTM));
}
```

---

## How to Apply to SLF Migration

### For AnimBP EventGraphs

1. **Identify EventGraph logic** - Variables set, functions called
2. **Create C++ AnimInstance base class** - With those variables/functions
3. **Create custom FAnimNode** - That calls C++ functions
4. **Create UAnimGraphNode** - For editor visualization
5. **Replace EventGraph nodes** - With custom node calls

### Example: Direction Calculation

**Before (Blueprint EventGraph):**
```
Event Blueprint Update Animation
→ Get Velocity
→ Calculate Direction
→ Set Direction Variable
```

**After (C++ Custom Node):**
```cpp
// In AnimInstance (NativeUpdateAnimation override)
void USLFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (APawn* Owner = TryGetPawnOwner())
    {
        FVector Velocity = Owner->GetVelocity();
        Direction = UKismetAnimationLibrary::CalculateDirection(
            Velocity, Owner->GetActorRotation());
    }
}
```

---

## FAnimationRuntime

Use `FAnimationRuntime` for common operations:
- Local ↔ Component space conversion
- Bone transform manipulation
- Pose blending utilities

---

## Key Reminders

1. **Graph Node = Editor only** - Lives in editor module
2. **Behavior Node = Runtime** - Lives in game module
3. **FPoseLink requires child calls** - Always propagate Update/CacheBones
4. **Skeleton Control uses Component Space** - Different evaluation function
5. **OutBoneTransforms order matters** - Parent before children

---

## Source

Epic Games - "Creating Custom Animation Nodes" (October 2015)
https://www.unrealengine.com/en-US/blog/creating-custom-animation-nodes

---

## Full AnimGraph Migration to C++

### Why Move AnimGraph Too?

When reparenting AnimBPs to C++ base classes, variable conflicts occur:
- Blueprint variables clash with C++ UPROPERTY
- Pin connections break
- State machine references become invalid

**Solution:** Move ALL AnimGraph logic to C++ custom nodes. AnimBP becomes an empty shell.

### AnimGraph Node Types → C++ Equivalents

| Blueprint Node | C++ Implementation |
|----------------|-------------------|
| Sequence Player | `FAnimNode_SequencePlayer` (built-in) |
| Blend by Bool | `FAnimNode_BlendListByBool` (built-in) |
| Blend by Int | `FAnimNode_BlendListByInt` (built-in) |
| Two Bone IK | `FAnimNode_TwoBoneIK` (built-in) |
| State Machine | `FAnimNode_StateMachine` or custom |
| Blend Space | `FAnimNode_BlendSpacePlayer` (built-in) |
| Custom Logic | Custom `FAnimNode_YourLogic` |

### Creating a Custom AnimGraph Node

**1. Runtime Behavior Node (FAnimNode_*):**
```cpp
// In runtime module (e.g., SLFConversion)
USTRUCT(BlueprintInternalUseOnly)
struct SLFCONVERSION_API FAnimNode_SLFLocomotion : public FAnimNode_Base
{
    GENERATED_BODY()

    // Input pose link
    UPROPERTY(EditAnywhere, Category = Links)
    FPoseLink BasePose;

    // Parameters from C++ (no Blueprint variables!)
    UPROPERTY(EditAnywhere, Category = Settings)
    float Speed;

    UPROPERTY(EditAnywhere, Category = Settings)
    float Direction;

    // Runtime functions
    virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
    virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
    virtual void Evaluate_AnyThread(FPoseContext& Output) override;
    virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
};
```

**2. Editor Graph Node (UAnimGraphNode_*):**
```cpp
// In EDITOR module (e.g., SLFConversionEditor)
UCLASS()
class SLFCONVERSIONEDITOR_API UAnimGraphNode_SLFLocomotion : public UAnimGraphNode_Base
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Settings)
    FAnimNode_SLFLocomotion Node;

    // Editor display
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FString GetNodeCategory() const override;
};
```

### State Machine in C++

State machines can be implemented using:

**Option 1: Use built-in FAnimNode_StateMachine**
```cpp
UPROPERTY()
FAnimNode_StateMachine StateMachine;

// Configure states and transitions in C++
```

**Option 2: Custom state logic**
```cpp
USTRUCT()
struct FAnimNode_SLFStateMachine : public FAnimNode_Base
{
    UPROPERTY()
    TArray<FPoseLink> StatePoses;

    UPROPERTY()
    int32 CurrentState;

    // Custom transition logic in C++
    virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override
    {
        // Check conditions, change CurrentState
        // Blend between poses
    }
};
```

### Blend Spaces from C++

```cpp
USTRUCT()
struct FAnimNode_SLFBlendSpace : public FAnimNode_BlendSpacePlayer
{
    GENERATED_BODY()

    // Override to feed C++ values instead of Blueprint variables
    virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
    {
        // Get speed/direction from C++ AnimInstance, not BP variables
        USLFAnimInstance* AnimInstance = Cast<USLFAnimInstance>(
            Context.AnimInstanceProxy->GetAnimInstanceObject());
        if (AnimInstance)
        {
            X = AnimInstance->Speed;
            Y = AnimInstance->Direction;
        }
        Super::UpdateInternal(Context);
    }
};
```

### The Empty Shell AnimBP

After migration, the AnimBP contains:
- Reference to C++ AnimInstance class (parent)
- Skeleton reference
- Preview mesh (editor only)
- **NO variables**
- **NO logic nodes**
- **AnimGraph instantiates C++ nodes only**

### Migration Steps for AnimBP

1. **Analyze AnimBP JSON export** - List all variables, EventGraph logic, AnimGraph nodes
2. **Create C++ AnimInstance** - `USLFAnimInstance : public UAnimInstance`
3. **Move EventGraph to NativeUpdateAnimation()** - All variable updates
4. **Create custom FAnimNode_* for complex logic** - State machines, blends
5. **Create UAnimGraphNode_* for editor** - In editor module
6. **Reparent AnimBP to C++ AnimInstance** - Becomes empty shell
7. **Replace AnimGraph nodes** - With C++ node references
8. **Validate** - All animation behavior preserved
