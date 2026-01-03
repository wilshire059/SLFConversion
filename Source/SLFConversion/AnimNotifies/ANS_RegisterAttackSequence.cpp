// ANS_RegisterAttackSequence.cpp
// C++ Animation Notify implementation for ANS_RegisterAttackSequence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - registers attack combo sequence for combo system

#include "AnimNotifies/ANS_RegisterAttackSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"

UANS_RegisterAttackSequence::UANS_RegisterAttackSequence()
	: QueuedSection(ESLFMontageSection::Light_01)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 0, 255); // Yellow for attack sequence
#endif
}

void UANS_RegisterAttackSequence::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get CombatManager and register next combo
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->RegisterNextCombo();
		UE_LOG(LogTemp, Log, TEXT("UANS_RegisterAttackSequence::NotifyBegin - Registered combo on %s"), *Owner->GetName());
		return;
	}

	// AI characters don't use combo system in the same way
	UE_LOG(LogTemp, Log, TEXT("UANS_RegisterAttackSequence::NotifyBegin - No CombatManager on %s"), *Owner->GetName());
}

void UANS_RegisterAttackSequence::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic - combo registration lasts during notify window
}

void UANS_RegisterAttackSequence::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get CombatManager and reset attack combo
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->ResetAttackCombo();
		UE_LOG(LogTemp, Log, TEXT("UANS_RegisterAttackSequence::NotifyEnd - Reset combo on %s"), *Owner->GetName());
	}
}

FString UANS_RegisterAttackSequence::GetNotifyName_Implementation() const
{
	return TEXT("Register Attack Sequence");
}
