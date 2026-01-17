// ABP_SoulslikeEnemy.cpp
// C++ AnimInstance implementation for ABP_SoulslikeEnemy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-11 - Added reflection helpers for Blueprint variables with spaces

#include "Animation/ABP_SoulslikeEnemy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/PropertyAccessUtil.h"
#include "Components/ActorComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "SLFPrimaryDataAssets.h"

UABP_SoulslikeEnemy::UABP_SoulslikeEnemy()
{
	OwnerCharacter = nullptr;
	CachedCombatManager = nullptr;
	IkWeight = 1.0f;
	PhysicsWeight = 1.0f;
	PoiseBroken = false;
	PoiseBreakAsset = nullptr;
}

void UABP_SoulslikeEnemy::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner reference
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeEnemy::NativeInitializeAnimation - Owner: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

void UABP_SoulslikeEnemy::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}

	// ═══════════════════════════════════════════════════════════════════════
	// C++ PROPERTIES (direct assignment - AnimGraph reads these by matching name)
	// ═══════════════════════════════════════════════════════════════════════

	// Update SoulslikeEnemy/SoulslikeCharacter reference (owning actor)
	SoulslikeEnemy = OwnerCharacter;
	SoulslikeCharacter = OwnerCharacter;

	// Get movement component reference
	MovementComponent = OwnerCharacter->GetCharacterMovement();

	// Update velocity from owner
	Velocity = OwnerCharacter->GetVelocity();

	// Calculate ground speed (2D velocity magnitude)
	GroundSpeed = Velocity.Size2D();

	// IsFalling from movement component
	IsFalling = MovementComponent ? MovementComponent->IsFalling() : false;

	// Calculate direction relative to actor rotation
	Direction = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

	// Get AI Combat Manager component if not cached
	if (!CachedCombatManager)
	{
		// Find specifically by type - this is the correct way
		CachedCombatManager = OwnerCharacter->FindComponentByClass<UAICombatManagerComponent>();
	}

	// Update component reference
	ACAICombatManager = CachedCombatManager;

	// Get properties from combat manager for AnimGraph
	if (UAICombatManagerComponent* AICombatMgr = Cast<UAICombatManagerComponent>(CachedCombatManager))
	{
		// Get PoiseBreakAsset from combat manager
		if (AICombatMgr->PoiseBreakAsset)
		{
			PoiseBreakAsset = Cast<UPDA_PoiseBreakAnimData>(AICombatMgr->PoiseBreakAsset);
		}
		// Get PoiseBroken state
		PoiseBroken = AICombatMgr->bPoiseBroken;
		// Get IkWeight from combat manager - THIS DRIVES THE IK FLINCH
		IkWeight = AICombatMgr->IkWeight;
		// CRITICAL: Copy IkWeight to PhysicsWeight - AnimGraph reads PhysicsWeight!
		PhysicsWeight = AICombatMgr->IkWeight;
		// Get CurrentHitNormal from combat manager
		CurrentHitNormal = AICombatMgr->CurrentHitNormal;
		HitLocation = AICombatMgr->CurrentHitNormal;
	}
	else
	{
		// Default values when no combat manager
		PhysicsWeight = 0.0f;
		IkWeight = 0.0f;
	}

	// Get equipment component
	for (UActorComponent* Comp : OwnerCharacter->GetComponents())
	{
		if (Comp && Comp->GetClass()->GetName().Contains(TEXT("Equipment")))
		{
			EquipmentComponent = Comp;
			break;
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// BLUEPRINT VARIABLES WITH SPACES (set via reflection)
	// AnimGraph nodes reference these by FName: "AC AI Combat Manager", "Hit Location"
	// ═══════════════════════════════════════════════════════════════════════

	// Set "AC AI Combat Manager" Blueprint variable
	SetBlueprintObjectVariable(FName(TEXT("AC AI Combat Manager")), CachedCombatManager);

	// Set "Hit Location" Blueprint variable (for hit reactions)
	SetBlueprintVectorVariable(FName(TEXT("Hit Location")), HitLocation);

	// Also set CurrentHitNormal (C++ property that might be referenced)
	CurrentHitNormal = HitLocation;

	// DEBUG: Log animation state periodically
	static int32 LogCounter = 0;
	if (LogCounter++ % 120 == 0) // Log every ~2 seconds at 60fps
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABP_SoulslikeEnemy] %s: GroundSpeed=%.1f, Velocity=%s, IsFalling=%d, Direction=%.1f, CombatMgr=%s"),
			*OwnerCharacter->GetName(), GroundSpeed, *Velocity.ToString(), IsFalling ? 1 : 0, Direction,
			CachedCombatManager ? *CachedCombatManager->GetName() : TEXT("None"));
	}
}

FVector UABP_SoulslikeEnemy::GetOwnerVelocity() const
{
	return OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
}

FRotator UABP_SoulslikeEnemy::GetOwnerRotation() const
{
	return OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
}

// ═══════════════════════════════════════════════════════════════════════════════
// REFLECTION HELPERS
// These allow setting Blueprint variables by FName, including those with spaces
// Used to set variables like "AC AI Combat Manager" and "Hit Location"
// ═══════════════════════════════════════════════════════════════════════════════

void UABP_SoulslikeEnemy::SetBlueprintObjectVariable(const FName& VarName, UObject* Value)
{
	// Get the Blueprint generated class
	UClass* MyClass = GetClass();
	if (!MyClass) return;

	// Find the property by name (FName can contain spaces)
	FProperty* Property = MyClass->FindPropertyByName(VarName);
	if (!Property)
	{
		// Property might be in parent class or not exist
		return;
	}

	// Verify it's an object property
	FObjectProperty* ObjProp = CastField<FObjectProperty>(Property);
	if (!ObjProp)
	{
		return;
	}

	// Set the value
	void* PropertyAddr = ObjProp->ContainerPtrToValuePtr<void>(this);
	ObjProp->SetObjectPropertyValue(PropertyAddr, Value);
}

void UABP_SoulslikeEnemy::SetBlueprintVectorVariable(const FName& VarName, const FVector& Value)
{
	// Get the Blueprint generated class
	UClass* MyClass = GetClass();
	if (!MyClass) return;

	// Find the property by name (FName can contain spaces)
	FProperty* Property = MyClass->FindPropertyByName(VarName);
	if (!Property)
	{
		// Property might be in parent class or not exist
		return;
	}

	// Verify it's a struct property (FVector is a struct)
	FStructProperty* StructProp = CastField<FStructProperty>(Property);
	if (!StructProp)
	{
		return;
	}

	// Verify the struct type is FVector
	if (StructProp->Struct != TBaseStructure<FVector>::Get())
	{
		return;
	}

	// Set the value
	void* PropertyAddr = StructProp->ContainerPtrToValuePtr<void>(this);
	FMemory::Memcpy(PropertyAddr, &Value, sizeof(FVector));
}

// AnimGraph function removed - conflicts with UE's internal AnimGraph function name
