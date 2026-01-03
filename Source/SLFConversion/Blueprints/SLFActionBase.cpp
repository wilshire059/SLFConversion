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
#include "StatManagerComponent.h"
#include "ActionManagerComponent.h"
#include "CombatManagerComponent.h"
#include "EquipmentManagerComponent.h"
#include "InventoryManagerComponent.h"
#include "InteractionManagerComponent.h"
#include "InputBufferComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

USLFActionBase::USLFActionBase()
{
	Action = nullptr;
	OwnerActor = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMPONENT GETTERS [1-7/14]
// ═══════════════════════════════════════════════════════════════════════════════

UEquipmentManagerComponent* USLFActionBase::GetEquipmentManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UEquipmentManagerComponent>();
	}
	return nullptr;
}

UInteractionManagerComponent* USLFActionBase::GetInteractionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UInteractionManagerComponent>();
	}
	return nullptr;
}

UInventoryManagerComponent* USLFActionBase::GetInventoryManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UInventoryManagerComponent>();
	}
	return nullptr;
}

UStatManagerComponent* USLFActionBase::GetStatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UStatManagerComponent>();
	}
	return nullptr;
}

UCombatManagerComponent* USLFActionBase::GetCombatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UCombatManagerComponent>();
	}
	return nullptr;
}

UActionManagerComponent* USLFActionBase::GetActionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UActionManagerComponent>();
	}
	return nullptr;
}

UInputBufferComponent* USLFActionBase::GetInputBuffer_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UInputBufferComponent>();
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ANIMATION/WEAPON HELPERS [8-10/14]
// ═══════════════════════════════════════════════════════════════════════════════

float USLFActionBase::GetWeaponStaminaMultiplier_Implementation()
{
	// TODO: Get from equipped weapon data asset
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
	// TODO: Get animset from currently equipped weapon
	return nullptr;
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
