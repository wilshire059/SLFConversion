// B_Action_ThrowProjectile.cpp
// C++ implementation for Blueprint B_Action_ThrowProjectile
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ThrowProjectile.json

#include "Blueprints/B_Action_ThrowProjectile.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_InventoryManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimMontage.h"

UB_Action_ThrowProjectile::UB_Action_ThrowProjectile()
{
}

void UB_Action_ThrowProjectile::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] ExecuteAction"));

	if (!OwnerActor)
	{
		return;
	}

	// Get equipment manager to find active tool slot
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionThrowProjectile] No equipment manager"));
		return;
	}

	// Get active tool slot (throwables are equipped as tools)
	FGameplayTag ActiveToolSlot = EquipMgr->GetActiveToolSlot();
	if (!ActiveToolSlot.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] No active tool slot"));
		return;
	}

	// Get item at that slot
	FSLFItemInfo ItemData;
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	FSLFItemInfo ItemData_1;
	UPrimaryDataAsset* ItemAsset_1 = nullptr;
	FGuid ItemId_1;

	EquipMgr->GetItemAtSlot(ActiveToolSlot, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] No item at active tool slot"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Throwing item: %s"), *ItemAsset->GetName());

	// Get throw montage from action data
	UAnimMontage* ThrowMontage = nullptr;
	if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
	{
		ThrowMontage = ActionData->ActionMontage.LoadSynchronous();
	}

	// Play throw animation - projectile spawn is handled by animation notify
	if (ThrowMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, ThrowMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Playing throw montage: %s"), *ThrowMontage->GetName());
	}

	// Consume the item from inventory after throw
	UAC_InventoryManager* InvMgr = GetInventoryManager();
	if (InvMgr)
	{
		InvMgr->RemoveItem(ItemAsset, 1);
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Consumed 1x %s from inventory"), *ItemAsset->GetName());
	}
}

