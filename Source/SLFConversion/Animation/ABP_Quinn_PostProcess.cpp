// ABP_Quinn_PostProcess.cpp
// C++ AnimInstance implementation for ABP_Quinn_PostProcess
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Animation/ABP_Quinn_PostProcess.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UABP_Quinn_PostProcess::UABP_Quinn_PostProcess()
{
	OwnerCharacter = nullptr;
}

void UABP_Quinn_PostProcess::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner reference
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	UE_LOG(LogTemp, Log, TEXT("UABP_Quinn_PostProcess::NativeInitializeAnimation - Owner: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

void UABP_Quinn_PostProcess::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}

	// PostProcess AnimBP has no EventGraph logic - AnimGraph handles procedural animation directly
	// The Blueprint EventGraph nodes are disabled with comment: "This node is disabled and will not be called"
}

FVector UABP_Quinn_PostProcess::GetOwnerVelocity() const
{
	return OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
}

FRotator UABP_Quinn_PostProcess::GetOwnerRotation() const
{
	return OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
}

// AnimGraph function removed - conflicts with UE's internal AnimGraph function name
