// B_Action_ComboHeavy.cpp
// C++ implementation for Blueprint B_Action_ComboHeavy
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ComboHeavy.json

#include "Blueprints/B_Action_ComboHeavy.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_ComboHeavy::UB_Action_ComboHeavy()
{
}

void UB_Action_ComboHeavy::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No OwnerActor"));
		return;
	}

	// Get equipment manager to check stance
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No EquipmentManager found"));
		return;
	}

	// Check if two-hand stance is active
	bool bOutLeftHand = false;
	bool bOutRightHand = false;
	bool bOutLeftHand_1 = false;
	bool bOutRightHand_1 = false;
	bool bOutLeftHand_2 = false;
	bool bOutRightHand_2 = false;
	EquipMgr->IsTwoHandStanceActive(bOutLeftHand, bOutRightHand, bOutLeftHand_1, bOutRightHand_1, bOutLeftHand_2, bOutRightHand_2);
	bool bIsTwoHanded = bOutLeftHand || bOutRightHand;

	// Get weapon animset for right hand (heavy attack uses right hand)
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Right, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed (heavy attack uses right hand)
	UAnimMontage* Montage = nullptr;
	if (bIsTwoHanded)
	{
		Montage = WeaponAnimset->TwoH_HeavyComboMontage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 2h_HeavyComboMontage"));
	}
	else
	{
		Montage = WeaponAnimset->OneH_HeavyComboMontage_R.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 1h_HeavyComboMontage_R"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] Montage is null (TwoHanded=%s)"), bIsTwoHanded ? TEXT("true") : TEXT("false"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Playing montage: %s"), *Montage->GetName());
	}
}

