// SLFNPCAnimInstance.cpp
// C++ AnimInstance implementation for NPC characters

#include "SLFNPCAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprints/SLFSoulslikeNPC.h"

// Already includes SLFSoulslikeNPC.h which provides proper type

USLFNPCAnimInstance::USLFNPCAnimInstance()
{
	// Initialize all properties
	SoulslikeCharacter = nullptr;
	SoulslikeNpc = nullptr;
	MovementComponent = nullptr;
	AC_AI_CombatManager = nullptr;
	EquipmentComponent = nullptr;

	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0;
	ShouldMove = false;
	IsFalling = false;
	Direction = 0.0f;

	HasLookAtTarget = false;
	LookAtLocation = FVector::ZeroVector;
	DistanceToLookAtTarget = 0.0;
	MaxDistance = 500.0f;
}

void USLFNPCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheReferences();

	UE_LOG(LogTemp, Log, TEXT("[SLFNPCAnimInstance] Initialized - Owner: %s"),
		SoulslikeNpc ? *SoulslikeNpc->GetName() : TEXT("NULL"));
}

void USLFNPCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Ensure references are valid
	if (!SoulslikeNpc)
	{
		CacheReferences();
		if (!SoulslikeNpc)
		{
			return;
		}
	}

	UpdateAnimationProperties(DeltaSeconds);
}

void USLFNPCAnimInstance::CacheReferences()
{
	// Get owning actor (the NPC character)
	SoulslikeNpc = Cast<ASLFSoulslikeNPC>(GetOwningActor());
	SoulslikeCharacter = SoulslikeNpc; // Same reference, different name
	if (!SoulslikeNpc)
	{
		return;
	}

	// Cache movement component
	ACharacter* Character = Cast<ACharacter>(SoulslikeNpc);
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}

	// Cache AI Combat Manager component
	AC_AI_CombatManager = SoulslikeNpc->FindComponentByClass<UAICombatManagerComponent>();

	// Cache Equipment Manager component
	EquipmentComponent = SoulslikeNpc->FindComponentByClass<UEquipmentManagerComponent>();
}

void USLFNPCAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	// Update movement properties
	if (MovementComponent)
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();
		IsFalling = MovementComponent->IsFalling();
		ShouldMove = GroundSpeed > 10.0; // Threshold for movement

		// Calculate movement direction relative to actor forward
		if (GroundSpeed > 0.01f)
		{
			FRotator ActorRotation = SoulslikeNpc->GetActorRotation();
			Direction = UKismetMathLibrary::NormalizedDeltaRotator(
				Velocity.ToOrientationRotator(),
				ActorRotation
			).Yaw;
		}
		else
		{
			Direction = 0.0f;
		}
	}

	// Update look-at properties by calling GetLookAtLocation on NPC
	if (ASLFSoulslikeNPC* NPC = Cast<ASLFSoulslikeNPC>(SoulslikeNpc))
	{
		// Call the NPC's function to get look-at location
		NPC->GetLookAtLocation(LookAtLocation);

		// Determine if we have a valid look-at target
		HasLookAtTarget = !LookAtLocation.IsNearlyZero();

		// Calculate distance to look-at target
		if (HasLookAtTarget)
		{
			DistanceToLookAtTarget = FVector::Dist(NPC->GetActorLocation(), LookAtLocation);
		}
		else
		{
			DistanceToLookAtTarget = 0.0;
		}
	}
}
