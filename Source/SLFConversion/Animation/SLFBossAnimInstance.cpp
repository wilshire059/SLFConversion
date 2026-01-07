// SLFBossAnimInstance.cpp
// C++ AnimInstance implementation for boss characters

#include "SLFBossAnimInstance.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"  // For CalculateDirection - matches Blueprint exactly
#include "Blueprints/SLFSoulslikeBoss.h"

USLFBossAnimInstance::USLFBossAnimInstance()
{
	// Initialize all properties
	SoulslikeBoss = nullptr;
	MovementComponent = nullptr;
	AC_AI_CombatManager = nullptr;

	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0;
	IsFalling = false;
	Direction = 0.0f;

	PoiseBroken = false;
	IkWeight = 0.0f;
	CurrentHitNormal = FVector::ZeroVector;
	HitLocation = FVector::ZeroVector;
	PoiseBreakAsset = nullptr;

	PhysicsWeight = 0.0f;
}

void USLFBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheReferences();

	UE_LOG(LogTemp, Log, TEXT("[SLFBossAnimInstance] Initialized - Owner: %s, CombatMgr: %s"),
		SoulslikeBoss ? *SoulslikeBoss->GetName() : TEXT("NULL"),
		AC_AI_CombatManager ? TEXT("Found") : TEXT("NULL"));
}

void USLFBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Ensure references are valid
	if (!SoulslikeBoss)
	{
		CacheReferences();
		if (!SoulslikeBoss)
		{
			return;
		}
	}

	UpdateAnimationProperties(DeltaSeconds);
}

void USLFBossAnimInstance::CacheReferences()
{
	// Get owning actor (the boss character)
	SoulslikeBoss = Cast<ASLFSoulslikeBoss>(GetOwningActor());
	if (!SoulslikeBoss)
	{
		return;
	}

	// Cache movement component
	ACharacter* Character = Cast<ACharacter>(SoulslikeBoss);
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}

	// Cache AI Combat Manager component
	AC_AI_CombatManager = SoulslikeBoss->FindComponentByClass<UAICombatManagerComponent>();
}

void USLFBossAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	// ═══════════════════════════════════════════════════════════════════
	// MOVEMENT PROPERTIES (from JSON: EventGraph)
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
		FRotator ActorRotation = SoulslikeBoss->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
	}

	// ═══════════════════════════════════════════════════════════════════
	// COMBAT PROPERTIES (from JSON: EventGraph)
	// JSON shows: Set PhysicsWeight = Get IkWeight (from AC_AI_CombatManager)
	// AnimGraph READS: PhysicsWeight, Hit Location, PoiseBroken
	// ═══════════════════════════════════════════════════════════════════
	if (AC_AI_CombatManager)
	{
		// JSON: Get IkWeight -> Set PhysicsWeight
		// CRITICAL: AnimGraph reads PhysicsWeight, not IkWeight!
		PhysicsWeight = AC_AI_CombatManager->IkWeight;
		IkWeight = AC_AI_CombatManager->IkWeight;  // Keep for compatibility

		// JSON: Get CurrentHitNormal -> Set Hit Location
		HitLocation = AC_AI_CombatManager->CurrentHitNormal;
		CurrentHitNormal = AC_AI_CombatManager->CurrentHitNormal;  // Keep for compatibility

		// JSON: Get PoiseBroken -> Set PoiseBroken
		PoiseBroken = AC_AI_CombatManager->bPoiseBroken;

		// PoiseBreakAsset for poise break state machine
		PoiseBreakAsset = Cast<UPDA_PoiseBreakAnimData>(AC_AI_CombatManager->PoiseBreakAsset);
	}
}
