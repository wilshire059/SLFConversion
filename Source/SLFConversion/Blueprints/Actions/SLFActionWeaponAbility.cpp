// SLFActionWeaponAbility.cpp
// Logic: Get currently equipped weapon, extract its WeaponAbility data,
// play the ability animation and apply any associated effects
#include "SLFActionWeaponAbility.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/StatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "UObject/UnrealType.h"

USLFActionWeaponAbility::USLFActionWeaponAbility()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Initialized"));
}

void USLFActionWeaponAbility::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] ExecuteAction - Weapon skill/art"));

	if (!OwnerActor) return;

	// Get equipment manager to find active weapon
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] No equipment manager"));
		return;
	}

	// Get active weapon slot (right hand by default)
	FGameplayTag WeaponSlot = EquipMgr->GetActiveWeaponSlot(true); // true = right hand
	UDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	EquipMgr->GetItemAtSlot(WeaponSlot, ItemAsset, ItemId);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] No weapon equipped in active slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Using weapon ability from: %s"), *ItemAsset->GetName());

	// Extract weapon ability data from item
	// In UPDA_Item, this would be in ItemInformation.EquipmentDetails.WeaponAbility
	UAnimMontage* AbilityMontage = nullptr;
	float FPCost = 0.0f;

	if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
	{
		// Look for WeaponAbility property in ItemInformation.EquipmentDetails
		// This contains the PDA_WeaponAbility reference with the ability montage and FP cost
		UClass* ItemClass = ItemData->GetClass();

		// Try to get WeaponAbility from EquipmentDetails
		for (TFieldIterator<FProperty> PropIt(ItemClass); PropIt; ++PropIt)
		{
			FProperty* Prop = *PropIt;
			FString PropName = Prop->GetName();

			// Look for ability montage properties
			if (PropName.Contains(TEXT("WeaponAbility")) || PropName.Contains(TEXT("AbilityMontage")))
			{
				if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
				{
					void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(ItemData);
					TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
					if (SoftPtr)
					{
						AbilityMontage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
						UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Found ability montage from: %s"), *PropName);
					}
				}
				break;
			}
		}
	}

	// Check if we have enough FP to use the ability
	if (FPCost > 0.0f)
	{
		UStatManagerComponent* StatMgr = GetStatManager();
		if (StatMgr)
		{
			FGameplayTag FPTag = FGameplayTag::RequestGameplayTag(FName("Stat.FP"), false);
			if (!CheckStatRequirement(FPTag, FPCost))
			{
				UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Insufficient FP for ability"));
				return;
			}
			// Consume FP
			AdjustStatByRequirement(FPTag, FPCost);
		}
	}

	// Play ability animation
	if (AbilityMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AbilityMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Playing ability montage: %s"), *AbilityMontage->GetName());
	}
	else if (!AbilityMontage)
	{
		// Fallback: use the action montage from the action data if no weapon-specific ability
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
