// SLFActionThrowProjectile.cpp
// Logic: Get active throwing item from tool slot, play throw animation,
// spawn projectile actor at appropriate time (via anim notify or here)
#include "SLFActionThrowProjectile.h"
#include "AC_EquipmentManager.h"
#include "AC_InventoryManager.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Character.h"

USLFActionThrowProjectile::USLFActionThrowProjectile()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Initialized"));
}

void USLFActionThrowProjectile::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] ExecuteAction - Throw consumable projectile"));

	if (!OwnerActor) return;

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
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	EquipMgr->GetItemAtSlotSimple(ActiveToolSlot, ItemAsset, ItemId);

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

	// Play throw animation - projectile spawn is typically handled by animation notify
	if (ThrowMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, ThrowMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Playing throw montage: %s"), *ThrowMontage->GetName());
	}

	// Note: The actual projectile spawn is handled by AN_SpawnProjectile animation notify
	// which reads the projectile class from the item data and spawns it at the appropriate socket
	// The notify is placed at the release point in the throw animation

	// Get item data to extract projectile class for reference
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
	{
		// Projectile class would be in ItemInformation.ConsumableDetails.ProjectileClass
		// or similar nested struct - the actual spawn is done by animation notify
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Item: %s will spawn projectile via animation notify"),
			*ItemData->GetName());
	}

	// Consume the item from inventory after throw
	UInventoryManagerComponent* InvMgr = GetInventoryManager();
	if (InvMgr)
	{
		InvMgr->RemoveItem(ItemAsset, 1);
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Consumed 1x %s from inventory"), *ItemAsset->GetName());
	}
}
