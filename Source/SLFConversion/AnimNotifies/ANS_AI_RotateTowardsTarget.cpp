// ANS_AI_RotateTowardsTarget.cpp
// C++ Animation Notify implementation for ANS_AI_RotateTowardsTarget
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables AI rotation towards target during attack animations

#include "AnimNotifies/ANS_AI_RotateTowardsTarget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_Enemy.h"

UANS_AI_RotateTowardsTarget::UANS_AI_RotateTowardsTarget()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 100, 255); // Orange for rotation
#endif
}

void UANS_AI_RotateTowardsTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Call RotateTowardsTarget via BPI_Enemy interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
	{
		IBPI_Enemy::Execute_RotateTowardsTarget(Owner, 0.0); // Duration 0 = instant
		UE_LOG(LogTemp, Log, TEXT("UANS_AI_RotateTowardsTarget::NotifyBegin - RotateTowardsTarget ON on %s"), *Owner->GetName());
	}
}

void UANS_AI_RotateTowardsTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// Rotation handled by BPI_Enemy implementation
}

void UANS_AI_RotateTowardsTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Call StopRotateTowardsTarget via BPI_Enemy interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
	{
		IBPI_Enemy::Execute_StopRotateTowardsTarget(Owner);
		UE_LOG(LogTemp, Log, TEXT("UANS_AI_RotateTowardsTarget::NotifyEnd - RotateTowardsTarget OFF on %s"), *Owner->GetName());
	}
}

FString UANS_AI_RotateTowardsTarget::GetNotifyName_Implementation() const
{
	return TEXT("AI Rotate Towards Target");
}
