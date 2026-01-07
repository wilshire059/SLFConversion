// B_Action_Jump.cpp
// C++ implementation for Blueprint B_Action_Jump
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_Jump.json

#include "Blueprints/B_Action_Jump.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/BPI_GenericCharacter.h"

UB_Action_Jump::UB_Action_Jump()
{
}

void UB_Action_Jump::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJump] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJump] No OwnerActor"));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJump] OwnerActor is not a Character"));
		return;
	}

	// Get skeletal mesh and anim instance
	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (MeshComp)
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
		{
			// Stop current montage with 0.2s blend out (HermiteCubic)
			UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
			if (CurrentMontage)
			{
				FAlphaBlendArgs BlendArgs;
				BlendArgs.BlendTime = 0.2f;
				BlendArgs.BlendOption = EAlphaBlendOption::HermiteCubic;
				AnimInstance->Montage_StopWithBlendOut(BlendArgs, CurrentMontage);
				UE_LOG(LogTemp, Log, TEXT("[ActionJump] Stopped montage: %s"), *CurrentMontage->GetName());
			}
		}
	}

	// Call JumpReplicated via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_JumpReplicated(OwnerActor);
		UE_LOG(LogTemp, Log, TEXT("[ActionJump] Called JumpReplicated"));
	}
	else
	{
		// Fallback: direct jump
		Character->Jump();
		UE_LOG(LogTemp, Log, TEXT("[ActionJump] Called Jump() directly"));
	}
}
