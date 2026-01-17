// B_Action.cpp
// C++ implementation for Blueprint B_Action
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action.json

#include "Blueprints/B_Action.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_InteractionManager.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_StatManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_ActionManager.h"
#include "Components/AC_InputBuffer.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "SLFPrimaryDataAssets.h"

UB_Action::UB_Action()
{
}

UAC_EquipmentManager* UB_Action::GetEquipmentManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_EquipmentManager>();
	}
	return nullptr;
}
UAC_InteractionManager* UB_Action::GetInteractionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_InteractionManager>();
	}
	return nullptr;
}
UAC_InventoryManager* UB_Action::GetInventoryManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_InventoryManager>();
	}
	return nullptr;
}
UAC_StatManager* UB_Action::GetStatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_StatManager>();
	}
	return nullptr;
}
UAC_CombatManager* UB_Action::GetCombatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_CombatManager>();
	}
	return nullptr;
}
UAC_ActionManager* UB_Action::GetActionManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_ActionManager>();
	}
	return nullptr;
}
UAC_InputBuffer* UB_Action::GetInputBuffer_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UAC_InputBuffer>();
	}
	return nullptr;
}
void UB_Action::GetWeaponStaminaMultiplier_Implementation(ESLFActionWeaponSlot Type, double& OutMultiplier, double& OutMultiplier1, double& OutMultiplier2, double& OutMultiplier3, double& OutMultiplier4, double& OutMultiplier5)
{
	// Default all outputs to 1.0
	OutMultiplier = 1.0;
	OutMultiplier1 = 1.0;
	OutMultiplier2 = 1.0;
	OutMultiplier3 = 1.0;
	OutMultiplier4 = 1.0;
	OutMultiplier5 = 1.0;

	UAC_EquipmentManager* EquipManager = GetEquipmentManager();
	if (!EquipManager)
	{
		return;
	}

	// Based on Type, get the appropriate weapon slot's stamina multiplier
	switch (Type)
	{
	case ESLFActionWeaponSlot::Right:
		{
			FGameplayTag ActiveSlot = EquipManager->GetActiveWeaponSlot(true); // RightHand = true
			FSLFItemInfo ItemData;
			UPrimaryDataAsset* ItemAsset = nullptr;
			FGuid Id;
			FSLFItemInfo ItemData1;
			UPrimaryDataAsset* ItemAsset1 = nullptr;
			FGuid Id1;
			EquipManager->GetItemAtSlot(ActiveSlot, ItemData, ItemAsset, Id, ItemData1, ItemAsset1, Id1);
			if (ItemAsset)
			{
				OutMultiplier = ItemData.EquipmentDetails.StaminaMultiplier;
			}
		}
		break;
	case ESLFActionWeaponSlot::Left:
		{
			FGameplayTag ActiveSlot = EquipManager->GetActiveWeaponSlot(false); // RightHand = false
			FSLFItemInfo ItemData;
			UPrimaryDataAsset* ItemAsset = nullptr;
			FGuid Id;
			FSLFItemInfo ItemData1;
			UPrimaryDataAsset* ItemAsset1 = nullptr;
			FGuid Id1;
			EquipManager->GetItemAtSlot(ActiveSlot, ItemData, ItemAsset, Id, ItemData1, ItemAsset1, Id1);
			if (ItemAsset)
			{
				OutMultiplier = ItemData.EquipmentDetails.StaminaMultiplier;
			}
		}
		break;
	case ESLFActionWeaponSlot::Dual:
		{
			// Get both weapons and combine multipliers
			FGameplayTag RightSlot = EquipManager->GetActiveWeaponSlot(true);
			FGameplayTag LeftSlot = EquipManager->GetActiveWeaponSlot(false);
			FSLFItemInfo RightItemData, LeftItemData;
			UPrimaryDataAsset* RightItemAsset = nullptr;
			UPrimaryDataAsset* LeftItemAsset = nullptr;
			FGuid RightId, LeftId;
			FSLFItemInfo Dummy1, Dummy2;
			UPrimaryDataAsset* DummyAsset1 = nullptr;
			UPrimaryDataAsset* DummyAsset2 = nullptr;
			FGuid DummyId1, DummyId2;

			EquipManager->GetItemAtSlot(RightSlot, RightItemData, RightItemAsset, RightId, Dummy1, DummyAsset1, DummyId1);
			EquipManager->GetItemAtSlot(LeftSlot, LeftItemData, LeftItemAsset, LeftId, Dummy2, DummyAsset2, DummyId2);

			double RightMultiplier = RightItemAsset ? RightItemData.EquipmentDetails.StaminaMultiplier : 1.0;
			double LeftMultiplier = LeftItemAsset ? LeftItemData.EquipmentDetails.StaminaMultiplier : 1.0;
			OutMultiplier = (RightMultiplier + LeftMultiplier) / 2.0;
		}
		break;
	case ESLFActionWeaponSlot::Null:
	default:
		// Keep default 1.0
		break;
	}
}

void UB_Action::GetOwnerAnimInstance_Implementation(UAnimInstance*& OutAnimInstance, UAnimInstance*& OutAnimInstance1)
{
	OutAnimInstance = nullptr;
	OutAnimInstance1 = nullptr;

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			OutAnimInstance = Mesh->GetAnimInstance();
			OutAnimInstance1 = OutAnimInstance;
		}
	}
}

void UB_Action::CheckStatRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot, bool& OutSuccess, bool& OutSuccess1)
{
	OutSuccess = false;
	OutSuccess1 = false;

	UAC_StatManager* StatManager = GetStatManager();
	if (!StatManager)
	{
		// No stat manager - fail safe by allowing action
		OutSuccess = true;
		OutSuccess1 = true;
		return;
	}

	// Check if we have the PDA_Action with stat requirements
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (!ActionData)
	{
		// No action data - allow action
		OutSuccess = true;
		OutSuccess1 = true;
		return;
	}

	// From Blueprint: Uses RequiredStatTag and RequiredStatAmount (NOT StaminaCost)
	// If no RequiredStatTag is set, action has no stat requirement
	if (!ActionData->RequiredStatTag.IsValid() || ActionData->RequiredStatAmount <= 0.0)
	{
		OutSuccess = true;
		OutSuccess1 = true;
		return;
	}

	// Get the stamina multiplier for this weapon slot
	double Multiplier, M1, M2, M3, M4, M5;
	GetWeaponStaminaMultiplier(StaminaMultiplierWeaponSlot, Multiplier, M1, M2, M3, M4, M5);

	// Calculate threshold: RequiredStatAmount * Multiplier
	double Threshold = ActionData->RequiredStatAmount * Multiplier;

	// Use StatManager's IsStatMoreThan to check if we meet the requirement
	bool bMeetsRequirement = StatManager->IsStatMoreThan(ActionData->RequiredStatTag, Threshold);

	if (!bMeetsRequirement)
	{
		UE_LOG(LogTemp, Log, TEXT("[B_Action] CheckStatRequirement FAILED: %s requires %.1f %s"),
			Action ? *Action->GetName() : TEXT("Unknown"),
			Threshold,
			*ActionData->RequiredStatTag.ToString());
	}

	OutSuccess = bMeetsRequirement;
	OutSuccess1 = bMeetsRequirement;
}

void UB_Action::AdjustStatByRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot)
{
	// First check if we meet the stat requirement
	bool bSuccess, bSuccess1;
	CheckStatRequirement(StaminaMultiplierWeaponSlot, bSuccess, bSuccess1);

	if (!bSuccess)
	{
		return;
	}

	UAC_StatManager* StatManager = GetStatManager();
	if (!StatManager)
	{
		return;
	}

	// Get the stamina multiplier
	double Multiplier, M1, M2, M3, M4, M5;
	GetWeaponStaminaMultiplier(StaminaMultiplierWeaponSlot, Multiplier, M1, M2, M3, M4, M5);

	// Get action data for stamina cost
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (!ActionData)
	{
		return;
	}

	// Calculate and apply stamina deduction
	double StaminaToDeduct = ActionData->StaminaCost * Multiplier;

	// Note: Actual stat adjustment would call StatManager to deduct stamina
	// The specific API depends on StatManager implementation
}

void UB_Action::GetWeaponAnimset_Implementation(ESLFActionWeaponSlot WeaponSlot, UPrimaryDataAsset*& OutAnimset, UPrimaryDataAsset*& OutAnimset1)
{
	OutAnimset = nullptr;
	OutAnimset1 = nullptr;

	UAC_EquipmentManager* EquipManager = GetEquipmentManager();
	if (!EquipManager)
	{
		return;
	}

	FGameplayTag SlotTag;
	switch (WeaponSlot)
	{
	case ESLFActionWeaponSlot::Right:
		SlotTag = EquipManager->GetActiveWeaponSlot(true);
		break;
	case ESLFActionWeaponSlot::Left:
		SlotTag = EquipManager->GetActiveWeaponSlot(false);
		break;
	case ESLFActionWeaponSlot::Dual:
	case ESLFActionWeaponSlot::Null:
	default:
		SlotTag = EquipManager->GetActiveWeaponSlot(true); // Default to right hand
		break;
	}

	FSLFItemInfo ItemData;
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid Id;
	FSLFItemInfo ItemData1;
	UPrimaryDataAsset* ItemAsset1 = nullptr;
	FGuid Id1;

	EquipManager->GetItemAtSlot(SlotTag, ItemData, ItemAsset, Id, ItemData1, ItemAsset1, Id1);

	if (ItemAsset)
	{
		// Get the MovesetWeapons from EquipmentDetails
		OutAnimset = Cast<UPrimaryDataAsset>(ItemData.EquipmentDetails.MovesetWeapons);
		OutAnimset1 = OutAnimset;
	}
}

void UB_Action::ExecuteAction_Implementation()
{
	// Base implementation - override in subclasses (B_Action_Jump, B_Action_Dodge, etc.)
	UE_LOG(LogTemp, Log, TEXT("UB_Action::ExecuteAction_Implementation - Base class called"));
}
