// B_Action_SprintAttack.cpp
// C++ implementation for Blueprint B_Action_SprintAttack
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_SprintAttack.json

#include "Blueprints/B_Action_SprintAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_SprintAttack::UB_Action_SprintAttack()
{
}

void UB_Action_SprintAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] ExecuteAction"));

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
		UE_LOG(LogTemp, Warning, TEXT("[ActionSprintAttack] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionSprintAttack] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->SprintAttackMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionSprintAttack] No SprintAttackMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] Playing montage: %s"), *Montage->GetName());
	}
}

