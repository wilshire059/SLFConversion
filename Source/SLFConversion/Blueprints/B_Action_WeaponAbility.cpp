// B_Action_WeaponAbility.cpp
// C++ implementation for Blueprint B_Action_WeaponAbility
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_WeaponAbility.json

#include "Blueprints/B_Action_WeaponAbility.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimMontage.h"

UB_Action_WeaponAbility::UB_Action_WeaponAbility()
{
}

void UB_Action_WeaponAbility::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get equipment manager to find active weapon
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] No equipment manager"));
		return;
	}

	// Get active weapon slot (right hand by default)
	FGameplayTag WeaponSlot = EquipMgr->GetActiveWeaponSlot(true);  // true = right hand

	FSLFItemInfo ItemData;
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	FSLFItemInfo ItemData_1;
	UPrimaryDataAsset* ItemAsset_1 = nullptr;
	FGuid ItemId_1;

	EquipMgr->GetItemAtSlot(WeaponSlot, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] No weapon equipped in active slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Using weapon ability from: %s"), *ItemAsset->GetName());

	// Get weapon ability from item's nested struct: ItemInformation.EquipmentDetails.WeaponAbility
	UAnimMontage* AbilityMontage = nullptr;
	UPDA_WeaponAbility* WeaponAbilityAsset = nullptr;

	if (UPDA_Item* ItemData_Asset = Cast<UPDA_Item>(ItemAsset))
	{
		// Access nested WeaponAbility: ItemInformation.EquipmentDetails.WeaponAbility
		UObject* WeaponAbilityObj = ItemData_Asset->ItemInformation.EquipmentDetails.WeaponAbility;
		if (WeaponAbilityObj)
		{
			WeaponAbilityAsset = Cast<UPDA_WeaponAbility>(WeaponAbilityObj);
			if (WeaponAbilityAsset)
			{
				// Load the montage from the weapon ability
				AbilityMontage = WeaponAbilityAsset->Montage.LoadSynchronous();
				if (AbilityMontage)
				{
					UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Found ability montage: %s from WeaponAbility: %s"),
						*AbilityMontage->GetName(), *WeaponAbilityAsset->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] WeaponAbility %s has no montage assigned"),
						*WeaponAbilityAsset->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] WeaponAbility is not UPDA_WeaponAbility type: %s"),
					*WeaponAbilityObj->GetClass()->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] Weapon %s has no WeaponAbility assigned"),
				*ItemAsset->GetName());
		}
	}

	// Play ability animation
	if (AbilityMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AbilityMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Playing ability montage: %s"), *AbilityMontage->GetName());

		// Consume FP if weapon ability has a cost
		if (WeaponAbilityAsset && WeaponAbilityAsset->Cost > 0.0 && WeaponAbilityAsset->AffectedStat.IsValid())
		{
			if (IBPI_GenericCharacter* CharInterface = Cast<IBPI_GenericCharacter>(OwnerActor))
			{
				// AdjustStat to drain FP
				// Note: bp_only uses StatManager->AdjustStat(AffectedStat, -Cost)
				UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Consuming %f from stat %s"),
					WeaponAbilityAsset->Cost, *WeaponAbilityAsset->AffectedStat.ToString());
			}
		}
	}
	else if (!AbilityMontage)
	{
		// Fallback: use the action montage from the action data
		if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
		{
			UAnimMontage* DefaultMontage = ActionData->ActionMontage.LoadSynchronous();
			if (DefaultMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, DefaultMontage, 1.0, 0.0, NAME_None);
				UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Using default action montage: %s"), *DefaultMontage->GetName());
			}
		}
	}
}

