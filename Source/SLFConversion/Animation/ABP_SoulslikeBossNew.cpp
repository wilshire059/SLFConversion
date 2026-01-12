// ABP_SoulslikeBossNew.cpp
// C++ AnimInstance implementation for ABP_SoulslikeBossNew
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-11 - Added reflection helpers for Blueprint variables with spaces

#include "Animation/ABP_SoulslikeBossNew.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ActorComponent.h"

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
}

void UABP_SoulslikeBossNew::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
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

	// Default physics/IK weights
	PhysicsWeight = 1.0f;
	IkWeight = 1.0f;

	// Get AI Combat Manager component if not cached
	if (!CachedCombatManager)
	{
		for (UActorComponent* Comp : OwnerCharacter->GetComponents())
		{
			if (Comp && Comp->GetClass()->GetName().Contains(TEXT("AI_CombatManager")))
			{
				CachedCombatManager = Comp;
				ACAICombatManager = Comp;
				break;
			}
		}
	}
	ACAICombatManager = CachedCombatManager;

	// Copy HitLocation to CurrentHitNormal
	CurrentHitNormal = HitLocation;

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

	FProperty* Property = MyClass->FindPropertyByName(VarName);
	if (!Property) return;

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
