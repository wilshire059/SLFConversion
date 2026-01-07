// SLFEnemyAnimInstance.cpp
// C++ AnimInstance implementation for enemy characters

#include "SLFEnemyAnimInstance.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"  // For CalculateDirection - matches Blueprint exactly
#include "Blueprints/SLFSoulslikeEnemy.h"

USLFEnemyAnimInstance::USLFEnemyAnimInstance()
{
	// Initialize all properties
	SoulslikeCharacter = nullptr;
	SoulslikeEnemy = nullptr;
	MovementComponent = nullptr;
	AC_AI_CombatManager = nullptr;
	EquipmentComponent = nullptr;

	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0;
	IsFalling = false;
	Direction = 0.0f;
	LocomotionType = 0;

	PoiseBroken = false;
	HitLocation = FVector::ZeroVector;
	PoiseBreakAsset = nullptr;

	PhysicsWeight = 0.0f;
}

void USLFEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheReferences();

	UE_LOG(LogTemp, Log, TEXT("[SLFEnemyAnimInstance] Initialized - Owner: %s, CombatMgr: %s"),
		SoulslikeEnemy ? *SoulslikeEnemy->GetName() : TEXT("NULL"),
		AC_AI_CombatManager ? TEXT("Found") : TEXT("NULL"));
}

void USLFEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Ensure references are valid
	if (!SoulslikeEnemy)
	{
		CacheReferences();
		if (!SoulslikeEnemy)
		{
			return;
		}
	}

	UpdateAnimationProperties(DeltaSeconds);
}

void USLFEnemyAnimInstance::CacheReferences()
{
	// Get owning actor (the enemy character)
	SoulslikeEnemy = Cast<ASLFSoulslikeEnemy>(GetOwningActor());
	SoulslikeCharacter = SoulslikeEnemy; // Same reference, different name
	if (!SoulslikeEnemy)
	{
		return;
	}

	// Cache movement component
	ACharacter* Character = Cast<ACharacter>(SoulslikeEnemy);
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}

	// Cache AI Combat Manager component
	AC_AI_CombatManager = SoulslikeEnemy->FindComponentByClass<UAICombatManagerComponent>();

	// Cache Equipment Manager component
	EquipmentComponent = SoulslikeEnemy->FindComponentByClass<UEquipmentManagerComponent>();
}

void USLFEnemyAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	// ═══════════════════════════════════════════════════════════════════
	// MOVEMENT PROPERTIES (from JSON: EventGraph Branch A)
	// Matches Blueprint exactly: Velocity -> GroundSpeed -> IsFalling -> Direction
	// ═══════════════════════════════════════════════════════════════════
	if (MovementComponent)
	{
		// JSON: Get MovementComponent.Velocity -> Set Velocity
		Velocity = MovementComponent->Velocity;

		// JSON: Vector Length XY -> Set GroundSpeed
		GroundSpeed = Velocity.Size2D();

		// JSON: Is Falling -> Set IsFalling
		IsFalling = MovementComponent->IsFalling();

		// JSON: Calculate Direction(Velocity, GetActorRotation) -> Set Direction
		// MUST use UKismetAnimationLibrary::CalculateDirection to match Blueprint!
		// Returns [-180, 180] degrees - feeds directional blendspaces
		FRotator ActorRotation = SoulslikeEnemy->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
	}

	// ═══════════════════════════════════════════════════════════════════
	// COMBAT PROPERTIES (from JSON: EventGraph Branch B)
	// Matches Blueprint exactly: IkWeight -> PhysicsWeight, CurrentHitNormal -> Hit Location, etc.
	// ═══════════════════════════════════════════════════════════════════
	if (AC_AI_CombatManager)
	{
		// JSON: Get IkWeight -> Set PhysicsWeight
		// THIS WAS MISSING! AnimGraph reads PhysicsWeight for IK blend
		PhysicsWeight = AC_AI_CombatManager->IkWeight;

		// JSON: Get CurrentHitNormal -> Set Hit Location
		HitLocation = AC_AI_CombatManager->CurrentHitNormal;

		// JSON: Get PoiseBroken -> Set PoiseBroken
		PoiseBroken = AC_AI_CombatManager->bPoiseBroken;

		// JSON: Get PoiseBreakAsset -> Set PoiseBreakAsset (done in init, but refresh here too)
		PoiseBreakAsset = Cast<UPDA_PoiseBreakAnimData>(AC_AI_CombatManager->PoiseBreakAsset);
	}
}
