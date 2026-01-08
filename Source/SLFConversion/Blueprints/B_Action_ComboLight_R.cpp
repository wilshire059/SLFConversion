// B_Action_ComboLight_R.cpp
// C++ implementation for Blueprint B_Action_ComboLight_R
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ComboLight_R.json

#include "Blueprints/B_Action_ComboLight_R.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_ComboLight_R::UB_Action_ComboLight_R()
{
}

void UB_Action_ComboLight_R::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No OwnerActor"));
		return;
	}

	// Get equipment manager to check stance
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No EquipmentManager found"));
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

	// Get weapon animset for right hand
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Right, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = nullptr;
	if (bIsTwoHanded)
	{
		Montage = WeaponAnimset->TwoH_LightComboMontage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Using 2h_LightComboMontage"));
	}
	else
	{
		Montage = WeaponAnimset->OneH_LightComboMontage_R.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Using 1h_LightComboMontage_R"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] Montage is null (TwoHanded=%s)"), bIsTwoHanded ? TEXT("true") : TEXT("false"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Playing montage: %s"), *Montage->GetName());
	}
}

