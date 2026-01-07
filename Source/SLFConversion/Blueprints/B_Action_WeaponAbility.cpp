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

	// Extract weapon ability montage using reflection
	UAnimMontage* AbilityMontage = nullptr;

	if (UPDA_Item* ItemData_Asset = Cast<UPDA_Item>(ItemAsset))
	{
		// Look for WeaponAbility montage in item properties
		UClass* ItemClass = ItemData_Asset->GetClass();

		for (TFieldIterator<FProperty> PropIt(ItemClass); PropIt; ++PropIt)
		{
			FProperty* Prop = *PropIt;
			FString PropName = Prop->GetName();

			if (PropName.Contains(TEXT("WeaponAbility")) || PropName.Contains(TEXT("AbilityMontage")))
			{
				if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
				{
					void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(ItemData_Asset);
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

	// Play ability animation
	if (AbilityMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AbilityMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Playing ability montage: %s"), *AbilityMontage->GetName());
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

