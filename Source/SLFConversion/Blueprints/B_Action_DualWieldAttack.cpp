// B_Action_DualWieldAttack.cpp
// C++ implementation for Blueprint B_Action_DualWieldAttack
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_DualWieldAttack.json

#include "Blueprints/B_Action_DualWieldAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_DualWieldAttack::UB_Action_DualWieldAttack()
{
}

void UB_Action_DualWieldAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get weapon animset for dual (uses right hand as base)
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Dual, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDualWieldAttack] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDualWieldAttack] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->LightDualWieldMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDualWieldAttack] No LightDualWieldMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] Playing montage: %s"), *Montage->GetName());
	}
}

