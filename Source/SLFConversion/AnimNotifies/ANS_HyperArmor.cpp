// ANS_HyperArmor.cpp
// C++ Animation Notify implementation for ANS_HyperArmor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables/disables hyper armor (poise)

#include "AnimNotifies/ANS_HyperArmor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"

UANS_HyperArmor::UANS_HyperArmor()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 0, 255); // Gold for hyper armor
#endif
}

void UANS_HyperArmor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Enable hyper armor on BOTH CombatManager and AI_CombatManager
	// Hyper armor = cannot be staggered during attack animation

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->SetHyperArmor(true);
		UE_LOG(LogTemp, Log, TEXT("UANS_HyperArmor::NotifyBegin - HyperArmor ON (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->SetHyperArmor(true);
		UE_LOG(LogTemp, Log, TEXT("UANS_HyperArmor::NotifyBegin - HyperArmor ON (AICombatManager) on %s"), *Owner->GetName());
	}
}

void UANS_HyperArmor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic in Blueprint - hyper armor state managed by component
}

void UANS_HyperArmor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Disable hyper armor on BOTH CombatManager and AI_CombatManager

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->SetHyperArmor(false);
		UE_LOG(LogTemp, Log, TEXT("UANS_HyperArmor::NotifyEnd - HyperArmor OFF (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->SetHyperArmor(false);
		UE_LOG(LogTemp, Log, TEXT("UANS_HyperArmor::NotifyEnd - HyperArmor OFF (AICombatManager) on %s"), *Owner->GetName());
	}
}

FString UANS_HyperArmor::GetNotifyName_Implementation() const
{
	return TEXT("Hyper Armor");
}
