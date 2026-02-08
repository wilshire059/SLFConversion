// SLFPrimaryDataAssets.cpp
// Implementation for Primary Data Asset classes

#include "SLFPrimaryDataAssets.h"
#include "Components/ProgressManagerComponent.h"
#include "Components/AC_ProgressManager.h"

// Helper lambda to get progress from either manager type
static bool GetProgressFromManager(UActorComponent* Manager, const FGameplayTag& Tag, ESLFProgress& OutState, bool& bFound)
{
	bFound = false;
	OutState = ESLFProgress::NotStarted;

	if (UProgressManagerComponent* PMC = Cast<UProgressManagerComponent>(Manager))
	{
		PMC->GetProgress(Tag, OutState, bFound);
		return true;
	}
	else if (UAC_ProgressManager* ACPM = Cast<UAC_ProgressManager>(Manager))
	{
		ACPM->GetProgress(Tag, bFound, OutState);
		return true;
	}
	return false;
}

void UPDA_Dialog::GetDialogTableBasedOnProgress(UActorComponent* ProgressManager, TSoftObjectPtr<UDataTable>& Table)
{
	// Default to the default dialog table
	Table = DefaultDialogTable;

	// Need a valid progress manager to check conditions
	if (!ProgressManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] GetDialogTableBasedOnProgress - No ProgressManager, using default table"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] GetDialogTableBasedOnProgress - ProgressManager type: %s, Checking %d requirements"),
		*ProgressManager->GetClass()->GetName(), Requirement.Num());

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

				// Call GetProgress on the ProgressManager (supports both class types)
				if (!GetProgressFromManager(ProgressManager, Tag, CurrentState, bFound))
				{
					UE_LOG(LogTemp, Warning, TEXT("[PDA_Dialog] Unknown ProgressManager type: %s"),
						*ProgressManager->GetClass()->GetName());
					bAllConditionsMet = false;
					break;
				}

				UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] Checking tag %s: Found=%s, State=%d (Required=%d)"),
					*Tag.ToString(), bFound ? TEXT("true") : TEXT("false"),
					static_cast<int32>(CurrentState), static_cast<int32>(Progress.State));

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
		UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] After checking: bAllConditionsMet=%s, TableIsNull=%s, TableIsValid=%s, TablePath=%s"),
			bAllConditionsMet ? TEXT("true") : TEXT("false"),
			Req.RelatedDialogTable.IsNull() ? TEXT("true") : TEXT("false"),
			Req.RelatedDialogTable.IsValid() ? TEXT("true") : TEXT("false"),
			*Req.RelatedDialogTable.ToString());

		if (bAllConditionsMet && !Req.RelatedDialogTable.IsNull())
		{
			UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] All conditions met, using table: %s"),
				*Req.RelatedDialogTable.GetAssetName());
			Table = Req.RelatedDialogTable;
			return; // Break out of outer loop
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] No matching requirement, using default table"));
	// If no matching requirement found, Table remains as DefaultDialogTable
}

bool UPDA_Dialog::GetCompletionProgressTag(FGameplayTag& OutProgressTag)
{
	// Find the first Requirement that checks for Completed state
	// This is the progress tag that should be set when dialog completes
	for (const FSLFDialogRequirement& Req : Requirement)
	{
		for (const FSLFDialogProgress& Progress : Req.Container)
		{
			// Look for requirements that check for Completed state
			if (Progress.State == ESLFProgress::Completed)
			{
				// Get the first tag from the container
				TArray<FGameplayTag> Tags;
				Progress.ProgressContainer.GetGameplayTagArray(Tags);
				if (Tags.Num() > 0)
				{
					OutProgressTag = Tags[0];
					UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] GetCompletionProgressTag: Found tag %s"),
						*OutProgressTag.ToString());
					return true;
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[PDA_Dialog] GetCompletionProgressTag: No completion tag found in Requirements"));
	return false;
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
// UPDA_DayNight Implementation
//////////////////////////////////////////////////////////////////////////

FSLFDayNightInfo UPDA_DayNight::GetEntryForTime(double Time) const
{
	// Find the entry that contains this time
	for (const FSLFDayNightInfo& Entry : Entries)
	{
		// Check if Time is between FromTime and ToTime
		if (Time >= Entry.FromTime && Time < Entry.ToTime)
		{
			return Entry;
		}
	}

	// Return first entry as default if no match (or empty if no entries)
	if (Entries.Num() > 0)
	{
		return Entries[0];
	}

	return FSLFDayNightInfo();
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
