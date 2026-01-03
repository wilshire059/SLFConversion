// ANS_ToggleChaosField.cpp
// C++ Animation Notify implementation for ANS_ToggleChaosField
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - calls BPI_Player::TriggerChaosField

#include "AnimNotifies/ANS_ToggleChaosField.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_Player.h"

UANS_ToggleChaosField::UANS_ToggleChaosField()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 150, 50, 255);
#endif
}

void UANS_ToggleChaosField::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Call TriggerChaosField(true) on owner via BPI_Player interface
	// Enables chaos destruction field on player
	// Uses interface message call - works whether target implements in BP or C++
	IBPI_Player::Execute_TriggerChaosField(Owner, true);
	UE_LOG(LogTemp, Log, TEXT("UANS_ToggleChaosField::NotifyBegin - Enabled chaos field on %s"), *Owner->GetName());
}

void UANS_ToggleChaosField::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	// No tick logic in Blueprint - intentionally empty
}

void UANS_ToggleChaosField::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Call TriggerChaosField(false) on owner via BPI_Player interface
	// Disables chaos destruction field on player
	// Uses interface message call - works whether target implements in BP or C++
	IBPI_Player::Execute_TriggerChaosField(Owner, false);
	UE_LOG(LogTemp, Log, TEXT("UANS_ToggleChaosField::NotifyEnd - Disabled chaos field on %s"), *Owner->GetName());
}

FString UANS_ToggleChaosField::GetNotifyName_Implementation() const
{
	return TEXT("Toggle Chaos Destruction Field");
}

