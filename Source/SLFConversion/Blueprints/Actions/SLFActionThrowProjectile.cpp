// SLFActionThrowProjectile.cpp
// Logic: Get active throwing item from tool slot, play throw animation,
// spawn projectile actor at appropriate time (via anim notify or here)
//
// bp_only Logic: Uses RelevantData (FInstancedStruct containing FSLFMontage) and PlaySoftMontageReplicated
#include "SLFActionThrowProjectile.h"
#include "AC_EquipmentManager.h"
#include "AC_InventoryManager.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
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

	// Get throw montage from action's RelevantData (FInstancedStruct containing FSLFMontage)
	// bp_only: Get Action.RelevantData → GetInstancedStructValue → extract FMontage → Break FMontage → get AnimMontage
	TSoftObjectPtr<UAnimMontage> SoftMontage;
	bool bFoundMontage = false;

	if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
	{
		// First try to get FSLFMontage from RelevantData
		const FSLFMontage* MontageStruct = ActionData->RelevantData.GetPtr<FSLFMontage>();
		if (MontageStruct && !MontageStruct->AnimMontage.IsNull())
		{
			SoftMontage = MontageStruct->AnimMontage;
			bFoundMontage = true;
			UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Got montage from RelevantData: %s"), *SoftMontage.ToString());
		}
		else
		{
			// Fallback to ActionMontage if RelevantData doesn't have it
			if (!ActionData->ActionMontage.IsNull())
			{
				SoftMontage = ActionData->ActionMontage;
				bFoundMontage = true;
				UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Fallback to ActionMontage: %s"), *SoftMontage.ToString());
			}
		}
	}

	// Play throw animation using PlaySoftMontageReplicated (matches bp_only behavior)
	// Projectile spawn is handled by AN_SpawnProjectile animation notify
	if (bFoundMontage && !SoftMontage.IsNull() && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		// Cast to UObject soft ptr for interface call
		TSoftObjectPtr<UObject> SoftMontageAsObject;
		SoftMontageAsObject = TSoftObjectPtr<UObject>(SoftMontage.ToSoftObjectPath());

		IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(OwnerActor, SoftMontageAsObject, 1.0, 0.0, NAME_None, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Playing throw montage via PlaySoftMontageReplicated: %s"), *SoftMontage.ToString());
	}
	else if (!bFoundMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionThrowProjectile] No throw montage found in RelevantData or ActionMontage!"));
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
