// B_Action_JumpAttack.cpp
// C++ implementation for Blueprint B_Action_JumpAttack
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_JumpAttack.json

#include "Blueprints/B_Action_JumpAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_JumpAttack::UB_Action_JumpAttack()
{
}

void UB_Action_JumpAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get weapon animset for right hand
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Right, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->JumpAttackMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] No JumpAttackMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Playing montage: %s"), *Montage->GetName());
	}
}

