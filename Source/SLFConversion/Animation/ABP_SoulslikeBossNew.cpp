// ABP_SoulslikeBossNew.cpp
// C++ AnimInstance implementation for ABP_SoulslikeBossNew
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-11 - Added reflection helpers for Blueprint variables with spaces

#include "Animation/ABP_SoulslikeBossNew.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Components/AC_AI_CombatManager.h"

UABP_SoulslikeBossNew::UABP_SoulslikeBossNew()
{
	OwnerCharacter = nullptr;
	CachedCombatManager = nullptr;
	IkWeight = 1.0f;
	PhysicsWeight = 1.0f;
	PoiseBroken = false;
}

void UABP_SoulslikeBossNew::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner reference
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeBossNew::NativeInitializeAnimation - Owner: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));

	// Initialize component references early to prevent null access in AnimGraph
	if (OwnerCharacter)
	{
		SoulslikeBoss = OwnerCharacter;
		MovementComponent = OwnerCharacter->GetCharacterMovement();

		// Find AC_AI_CombatManager component (the type AnimGraph property bindings expect)
		CachedCombatManager = OwnerCharacter->FindComponentByClass<UAC_AI_CombatManager>();
		ACAICombatManager = CachedCombatManager;

		// Also try to set the Blueprint variable with spaces if it exists
		SetBlueprintObjectVariable(FName(TEXT("AC AI Combat Manager")), CachedCombatManager);

		// Initialize properties from combat manager (UAC_AI_CombatManager has these properties)
		if (CachedCombatManager)
		{
			IkWeight = CachedCombatManager->IkWeight;
			PhysicsWeight = CachedCombatManager->IkWeight;
			CurrentHitNormal = CachedCombatManager->CurrentHitNormal;
			HitLocation = CachedCombatManager->CurrentHitNormal;
			PoiseBroken = CachedCombatManager->PoiseBroken;  // Note: PoiseBroken without 'b' prefix in UAC_AI_CombatManager
		}

		UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeBossNew::NativeInitializeAnimation - CombatMgr: %s"),
			CachedCombatManager ? *CachedCombatManager->GetName() : TEXT("None"));
	}
}

void UABP_SoulslikeBossNew::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}

	// Guard against PIE exit - don't update if world is tearing down
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		// Clear references during teardown to prevent stale access
		ACAICombatManager = nullptr;
		CachedCombatManager = nullptr;
		return;
	}

	// Check if owner is valid and not being destroyed
	if (!IsValid(OwnerCharacter) || OwnerCharacter->IsPendingKillPending())
	{
		ACAICombatManager = nullptr;
		CachedCombatManager = nullptr;
		return;
	}

	// ═══════════════════════════════════════════════════════════════════════
	// C++ PROPERTIES (direct assignment)
	// ═══════════════════════════════════════════════════════════════════════

	// Update SoulslikeBoss reference (owning actor)
	SoulslikeBoss = OwnerCharacter;

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

	// Get AC_AI_CombatManager component if not cached
	if (!CachedCombatManager || !IsValid(CachedCombatManager))
	{
		CachedCombatManager = OwnerCharacter->FindComponentByClass<UAC_AI_CombatManager>();
	}

	// Validate component is still valid (not being destroyed)
	if (!IsValid(CachedCombatManager))
	{
		ACAICombatManager = nullptr;
		CachedCombatManager = nullptr;
		return;
	}

	ACAICombatManager = CachedCombatManager;

	// Get properties from combat manager for AnimGraph (UAC_AI_CombatManager has these properties)
	if (CachedCombatManager)
	{
		// Get PoiseBroken state (PoiseBroken without 'b' prefix in UAC_AI_CombatManager)
		PoiseBroken = CachedCombatManager->PoiseBroken;
		// Get IkWeight from combat manager
		IkWeight = CachedCombatManager->IkWeight;
		PhysicsWeight = CachedCombatManager->IkWeight;
		// Get CurrentHitNormal from combat manager
		CurrentHitNormal = CachedCombatManager->CurrentHitNormal;
		HitLocation = CachedCombatManager->CurrentHitNormal;
	}

	// ═══════════════════════════════════════════════════════════════════════
	// BLUEPRINT VARIABLES WITH SPACES (set via reflection)
	// ═══════════════════════════════════════════════════════════════════════

	// Set "AC AI Combat Manager" Blueprint variable
	SetBlueprintObjectVariable(FName(TEXT("AC AI Combat Manager")), CachedCombatManager);

	// Set "Hit Location" Blueprint variable
	SetBlueprintVectorVariable(FName(TEXT("Hit Location")), HitLocation);
}

FVector UABP_SoulslikeBossNew::GetOwnerVelocity() const
{
	return OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
}

FRotator UABP_SoulslikeBossNew::GetOwnerRotation() const
{
	return OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
}

// ═══════════════════════════════════════════════════════════════════════════════
// REFLECTION HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void UABP_SoulslikeBossNew::SetBlueprintObjectVariable(const FName& VarName, UObject* Value)
{
	UClass* MyClass = GetClass();
	if (!MyClass) return;

	// Find the property by name (FName can contain spaces)
	FProperty* Property = MyClass->FindPropertyByName(VarName);
	if (!Property)
	{
		// Try to find in the class hierarchy
		for (UClass* SearchClass = MyClass; SearchClass; SearchClass = SearchClass->GetSuperClass())
		{
			Property = SearchClass->FindPropertyByName(VarName);
			if (Property)
			{
				break;
			}
		}

		if (!Property)
		{
			// DEBUG: Log that we couldn't find the property (only once per name)
			static TSet<FName> LoggedNames;
			if (!LoggedNames.Contains(VarName))
			{
				LoggedNames.Add(VarName);
				UE_LOG(LogTemp, Warning, TEXT("[ABP_SoulslikeBossNew] SetBlueprintObjectVariable: Property '%s' not found in class hierarchy"), *VarName.ToString());
			}
			return;
		}
	}

	FObjectProperty* ObjProp = CastField<FObjectProperty>(Property);
	if (!ObjProp) return;

	void* PropertyAddr = ObjProp->ContainerPtrToValuePtr<void>(this);
	ObjProp->SetObjectPropertyValue(PropertyAddr, Value);
}

void UABP_SoulslikeBossNew::SetBlueprintVectorVariable(const FName& VarName, const FVector& Value)
{
	UClass* MyClass = GetClass();
	if (!MyClass) return;

	FProperty* Property = MyClass->FindPropertyByName(VarName);
	if (!Property) return;

	FStructProperty* StructProp = CastField<FStructProperty>(Property);
	if (!StructProp) return;

	if (StructProp->Struct != TBaseStructure<FVector>::Get()) return;

	void* PropertyAddr = StructProp->ContainerPtrToValuePtr<void>(this);
	FMemory::Memcpy(PropertyAddr, &Value, sizeof(FVector));
}

// AnimGraph function removed - conflicts with UE's internal AnimGraph function name

void UABP_SoulslikeBossNew::NativeUninitializeAnimation()
{
	// Clear all references before destruction to prevent AnimGraph from accessing stale pointers
	ACAICombatManager = nullptr;
	CachedCombatManager = nullptr;
	OwnerCharacter = nullptr;
	SoulslikeBoss = nullptr;
	MovementComponent = nullptr;

	Super::NativeUninitializeAnimation();
}
