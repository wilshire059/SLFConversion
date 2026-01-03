// AN_InterruptMontage.cpp
// C++ Animation Notify implementation for AN_InterruptMontage
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - stops current montage if movement input detected

#include "AnimNotifies/AN_InterruptMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Pawn.h"

UAN_InterruptMontage::UAN_InterruptMontage()
	: BlendOutDuration(0.25)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 0, 255); // Yellow for interrupt
#endif
}

void UAN_InterruptMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		return;
	}

	// Check if there's movement input
	FVector MovementInput = OwnerPawn->GetLastMovementInputVector();
	bool bHasMovementInput = !MovementInput.IsNearlyZero();

	if (!bHasMovementInput)
	{
		// No movement input, don't interrupt
		return;
	}

	// Get anim instance and stop current montage
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		if (CurrentMontage)
		{
			AnimInstance->Montage_Stop(BlendOutDuration, CurrentMontage);
			UE_LOG(LogTemp, Log, TEXT("UAN_InterruptMontage::Notify - Interrupted montage %s on %s due to movement input"),
				*CurrentMontage->GetName(), *Owner->GetName());
		}
	}
}

FString UAN_InterruptMontage::GetNotifyName_Implementation() const
{
	return TEXT("Interrupt on Movement");
}
