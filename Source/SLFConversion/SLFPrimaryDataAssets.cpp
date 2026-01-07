// SLFPrimaryDataAssets.cpp
// Implementation for Primary Data Asset classes

#include "SLFPrimaryDataAssets.h"
#include "Components/AC_ProgressManager.h"

void UPDA_Dialog::GetDialogTableBasedOnProgress(UActorComponent* ProgressManager, TSoftObjectPtr<UDataTable>& Table)
{
	// Default to the default dialog table
	Table = DefaultDialogTable;

	// Need a valid progress manager to check conditions
	if (!ProgressManager)
	{
		return;
	}

	// Try to cast to our progress manager type
	UAC_ProgressManager* PM = Cast<UAC_ProgressManager>(ProgressManager);
	if (!PM)
	{
		return;
	}

	// Iterate through Requirements (For Each Loop with Break)
	for (const FSLFDialogRequirement& Req : Requirement)
	{
		bool bAllConditionsMet = true;

		// For each DialogProgress in the Container
		for (const FSLFDialogProgress& Progress : Req.Container)
		{
			// Break the GameplayTagContainer to get individual tags
			TArray<FGameplayTag> Tags;
			Progress.ProgressContainer.GetGameplayTagArray(Tags);

			// Check each tag's progress state
			for (const FGameplayTag& Tag : Tags)
			{
				bool bFound = false;
				ESLFProgress CurrentState = ESLFProgress::NotStarted;

				// Call GetProgress on the ProgressManager
				PM->GetProgress(Tag, bFound, CurrentState);

				// Check if the state matches what we're looking for
				if (!bFound || CurrentState != Progress.State)
				{
					bAllConditionsMet = false;
					break;
				}
			}

			if (!bAllConditionsMet)
			{
				break;
			}
		}

		// If all conditions met, use this requirement's dialog table and break
		if (bAllConditionsMet && Req.RelatedDialogTable.IsValid())
		{
			Table = Req.RelatedDialogTable;
			return; // Break out of outer loop
		}
	}

	// If no matching requirement found, Table remains as DefaultDialogTable
}

void UPDA_Vendor::ReduceItemStock(UObject* Item, int32 Delta)
{
	// Find the item in CachedItems and reduce its stock
	for (int32 i = 0; i < CachedItems.Num(); ++i)
	{
		if (CachedItems[i].Item == Item)
		{
			// Don't reduce if infinite stock
			if (!CachedItems[i].bInfiniteStock)
			{
				CachedItems[i].StockAmount = FMath::Max(0, CachedItems[i].StockAmount - Delta);

				// If stock is now 0, remove the item
				if (CachedItems[i].StockAmount <= 0)
				{
					CachedItems.RemoveAt(i);
				}
			}

			// Broadcast the update event
			OnItemStockUpdated.Broadcast();
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// UPDA_Item Implementation
//////////////////////////////////////////////////////////////////////////

TMap<UPDA_Item*, int32> UPDA_Item::GetRequiredCraftingItems()
{
	TMap<UPDA_Item*, int32> Result;

	// Check if this item can be crafted
	if (!ItemInformation.CraftingDetails.bCanBeCrafted)
	{
		return Result; // Return empty map if can't be crafted
	}

	// Note: The Blueprint stores RequiredItems as TMap<FGameplayTag, int32>
	// But returns TMap<UPDA_Item*, int32> after looking up items by tag
	// For now, return empty map - the actual lookup would require asset registry
	// The Blueprint version does this lookup at runtime

	return Result;
}

void UPDA_Item::UnlockCrafting()
{
	// Only unlock if the item can be crafted
	if (ItemInformation.CraftingDetails.bCanBeCrafted)
	{
		// Set crafting as unlocked
		bCrafingUnlocked = true;

		// Broadcast the event
		OnCraftingUnlocked.Broadcast();
	}
}
