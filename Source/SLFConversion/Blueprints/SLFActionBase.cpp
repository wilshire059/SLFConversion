// SLFActionBase.cpp
// C++ implementation for B_Action
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (initialized in constructor)
// Functions:         14/14 implemented (12 migrated + 2 lifecycle)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFActionBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Components/StatManagerComponent.h"  // Use UStatManagerComponent, NOT UAC_StatManager
#include "Components/InventoryManagerComponent.h"  // Use UInventoryManagerComponent, NOT UAC_InventoryManager
#include "AC_ActionManager.h"
#include "AC_CombatManager.h"
#include "AC_EquipmentManager.h"
#include "AC_InteractionManager.h"
#include "AC_InputBuffer.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "SLFPrimaryDataAssets.h"

USLFActionBase::USLFActionBase()
{
	Action = nullptr;
	OwnerActor = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMPONENT GETTERS [1-7/14]
// ═══════════════════════════════════════════════════════════════════════════════

UAC_EquipmentManager* USLFActionBase::GetEquipmentManager_Implementation()
{
	if (OwnerActor)
	{
		// First check on the character itself
		UAC_EquipmentManager* Result = OwnerActor->FindComponentByClass<UAC_EquipmentManager>();
		if (Result)
		{
			return Result;
		}

		// If not on character, check on the PlayerController
		if (APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			if (AController* Controller = Pawn->GetController())
			{
				Result = Controller->FindComponentByClass<UAC_EquipmentManager>();
				if (Result)
				{
					return Result;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[GetEquipmentManager] Not found on Character or Controller"));
	}
	return nullptr;
}

UAC_InteractionManager* USLFActionBase::GetInteractionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_InteractionManager>();
	}
	return nullptr;
}

UInventoryManagerComponent* USLFActionBase::GetInventoryManager_Implementation()
{
	if (OwnerActor)
	{
		UInventoryManagerComponent* Result = OwnerActor->FindComponentByClass<UInventoryManagerComponent>();
		if (Result) return Result;

		if (APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			if (AController* Controller = Pawn->GetController())
			{
				Result = Controller->FindComponentByClass<UInventoryManagerComponent>();
				if (Result) return Result;
			}
		}
	}
	return nullptr;
}

UStatManagerComponent* USLFActionBase::GetStatManager_Implementation()
{
	if (OwnerActor)
	{
		UStatManagerComponent* Result = OwnerActor->FindComponentByClass<UStatManagerComponent>();
		if (Result) return Result;

		if (APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			if (AController* Controller = Pawn->GetController())
			{
				Result = Controller->FindComponentByClass<UStatManagerComponent>();
				if (Result) return Result;
			}
		}
	}
	return nullptr;
}

UAC_CombatManager* USLFActionBase::GetCombatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_CombatManager>();
	}
	return nullptr;
}

UAC_ActionManager* USLFActionBase::GetActionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_ActionManager>();
	}
	return nullptr;
}

UAC_InputBuffer* USLFActionBase::GetInputBuffer_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_InputBuffer>();
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ANIMATION/WEAPON HELPERS [8-10/14]
// ═══════════════════════════════════════════════════════════════════════════════

float USLFActionBase::GetWeaponStaminaMultiplier_Implementation()
{
	// Get stamina multiplier from equipped weapon
	if (UAC_EquipmentManager* EquipmentManager = GetEquipmentManager())
	{
		// Get right hand weapon slot
		FGameplayTag WeaponSlot = EquipmentManager->GetActiveWeaponSlot(true);
		UPrimaryDataAsset* ItemAsset = nullptr;
		FGuid Id;
		EquipmentManager->GetItemAtSlotSimple(WeaponSlot, ItemAsset, Id);
		
		if (UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset))
		{
			return Item->ItemInformation.EquipmentDetails.StaminaMultiplier;
		}
	}
	return 1.0f;
}

UAnimInstance* USLFActionBase::GetOwnerAnimInstance_Implementation()
{
	if (OwnerActor)
	{
		if (USkeletalMeshComponent* Mesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			return Mesh->GetAnimInstance();
		}
	}
	return nullptr;
}


UDataAsset* USLFActionBase::GetWeaponAnimset_Implementation()
{
	// Get animset from equipped weapon
	UAC_EquipmentManager* EquipmentManager = GetEquipmentManager();
	if (!EquipmentManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] No EquipmentManager"));
		return nullptr;
	}

	// Get right hand weapon slot
	FGameplayTag WeaponSlot = EquipmentManager->GetActiveWeaponSlot(true);
	UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] WeaponSlot: %s"), *WeaponSlot.ToString());

	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid Id;
	EquipmentManager->GetItemAtSlotSimple(WeaponSlot, ItemAsset, Id);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] No item at weapon slot"));
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] ItemAsset: %s"), *ItemAsset->GetName());

	UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset);
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] ItemAsset is not UPDA_Item (class: %s)"), *ItemAsset->GetClass()->GetName());
		return nullptr;
	}

	UObject* MovesetWeapons = Item->ItemInformation.EquipmentDetails.MovesetWeapons;
	if (!MovesetWeapons)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] MovesetWeapons is NULL on item %s"), *Item->GetName());
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GetWeaponAnimset] MovesetWeapons: %s (Class: %s)"), *MovesetWeapons->GetName(), *MovesetWeapons->GetClass()->GetName());

	return Cast<UDataAsset>(MovesetWeapons);
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT REQUIREMENTS [11-12/14]
// ═══════════════════════════════════════════════════════════════════════════════

bool USLFActionBase::CheckStatRequirement_Implementation(FGameplayTag StatTag, float RequiredValue)
{
	if (UStatManagerComponent* StatManager = GetStatManager())
	{
		return StatManager->IsStatMoreThan(StatTag, RequiredValue);
	}
	return false;
}

void USLFActionBase::AdjustStatByRequirement_Implementation(FGameplayTag StatTag, float Amount)
{
	if (UStatManagerComponent* StatManager = GetStatManager())
	{
		// New signature: (StatTag, ValueType, Change, bLevelUp, bTriggerRegen)
		StatManager->AdjustStat(StatTag, ESLFValueType::CurrentValue, -Amount, false, true);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EXECUTION [13-14/14]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFActionBase::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[Action] ExecuteAction: %s"),
		Action ? *Action->GetName() : TEXT("null"));

	// Base implementation - override in child classes
}

bool USLFActionBase::CanExecuteAction_Implementation()
{
	// Base implementation - override in child classes for specific checks
	return true;
}
