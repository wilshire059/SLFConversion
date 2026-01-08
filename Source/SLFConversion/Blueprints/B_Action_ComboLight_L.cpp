// B_Action_ComboLight_L.cpp
// C++ implementation for Blueprint B_Action_ComboLight_L
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ComboLight_L.json

#include "Blueprints/B_Action_ComboLight_L.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_ComboLight_L::UB_Action_ComboLight_L()
{
}

void UB_Action_ComboLight_L::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_L] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_L] No OwnerActor"));
		return;
	}

	// Get equipment manager to check stance
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_L] No EquipmentManager found"));
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
	bool bIsTwoHanded = bOutLeftHand || bOutRightHand;  // Either hand in two-hand mode

	// Get weapon animset for left hand
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Left, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_L] No weapon animset found"));
		return;
	}

	// Cast to C++ type for direct property access
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_L] Animset is not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = nullptr;
	if (bIsTwoHanded)
	{
		Montage = WeaponAnimset->TwoH_LightComboMontage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_L] Using 2h_LightComboMontage"));
	}
	else
	{
		Montage = WeaponAnimset->OneH_LightComboMontage_L.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_L] Using 1h_LightComboMontage_L"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_L] Montage is null (TwoHanded=%s)"), bIsTwoHanded ? TEXT("true") : TEXT("false"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_L] Playing montage: %s"), *Montage->GetName());
	}
}

