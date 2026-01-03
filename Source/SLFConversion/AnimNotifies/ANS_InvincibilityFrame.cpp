// ANS_InvincibilityFrame.cpp
// C++ Animation Notify implementation for ANS_InvincibilityFrame
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables/disables invincibility frames

#include "AnimNotifies/ANS_InvincibilityFrame.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"

UANS_InvincibilityFrame::UANS_InvincibilityFrame()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 255, 255, 255); // Cyan for i-frames
#endif
}

void UANS_InvincibilityFrame::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Enable invincibility on BOTH CombatManager and AI_CombatManager
	// Invincibility = cannot take damage during dodge/roll animation

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->SetInvincibility(true);
		UE_LOG(LogTemp, Log, TEXT("UANS_InvincibilityFrame::NotifyBegin - Invincibility ON (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->SetInvincibility(true);
		UE_LOG(LogTemp, Log, TEXT("UANS_InvincibilityFrame::NotifyBegin - Invincibility ON (AICombatManager) on %s"), *Owner->GetName());
	}
}

void UANS_InvincibilityFrame::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic in Blueprint - invincibility state managed by component
}

void UANS_InvincibilityFrame::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Disable invincibility on BOTH CombatManager and AI_CombatManager

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->SetInvincibility(false);
		UE_LOG(LogTemp, Log, TEXT("UANS_InvincibilityFrame::NotifyEnd - Invincibility OFF (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->SetInvincibility(false);
		UE_LOG(LogTemp, Log, TEXT("UANS_InvincibilityFrame::NotifyEnd - Invincibility OFF (AICombatManager) on %s"), *Owner->GetName());
	}
}

FString UANS_InvincibilityFrame::GetNotifyName_Implementation() const
{
	return TEXT("Invincibility Frame");
}
