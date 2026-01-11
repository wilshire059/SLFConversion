// ANS_RegisterAttackSequence.cpp
// C++ Animation Notify implementation for ANS_RegisterAttackSequence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - registers attack combo sequence for combo system

#include "AnimNotifies/ANS_RegisterAttackSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AC_CombatManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

UANS_RegisterAttackSequence::UANS_RegisterAttackSequence()
	: QueuedSection(NAME_None)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 0, 255); // Yellow for attack sequence
#endif
}

UAC_CombatManager* UANS_RegisterAttackSequence::GetCombatManager(AActor* Owner)
{
	if (!Owner) return nullptr;

	// First check on actor itself
	UAC_CombatManager* Result = Owner->FindComponentByClass<UAC_CombatManager>();
	if (Result) return Result;

	// If not found and owner is a pawn, check controller
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		if (AController* Controller = Pawn->GetController())
		{
			Result = Controller->FindComponentByClass<UAC_CombatManager>();
			if (Result) return Result;
		}
	}

	return nullptr;
}

void UANS_RegisterAttackSequence::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// Get CombatManager and register next combo section
	UAC_CombatManager* CombatManager = GetCombatManager(Owner);
	if (CombatManager)
	{
		// Use QueuedSection FName directly
		CombatManager->EventRegisterNextCombo(QueuedSection);
		UE_LOG(LogTemp, Log, TEXT("[ANS_RegisterAttackSequence] NotifyBegin - Registered combo section '%s' on %s"),
			*QueuedSection.ToString(), *Owner->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[ANS_RegisterAttackSequence] NotifyBegin - No CombatManager on %s"), *Owner->GetName());
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

	// Get CombatManager and reset attack combo
	UAC_CombatManager* CombatManager = GetCombatManager(Owner);
	if (CombatManager)
	{
		CombatManager->EventRegisterNextCombo(NAME_None);  // Clear combo section
		UE_LOG(LogTemp, Log, TEXT("[ANS_RegisterAttackSequence] NotifyEnd - Reset combo on %s"), *Owner->GetName());
	}
}

FString UANS_RegisterAttackSequence::GetNotifyName_Implementation() const
{
	return TEXT("Register Attack Sequence");
}
