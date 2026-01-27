// W_CraftingEntry.cpp
// C++ Widget implementation for W_CraftingEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_CraftingEntry.h"
#include "Components/AC_InventoryManager.h"
#include "Components/InventoryManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UW_CraftingEntry::UW_CraftingEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_CraftingEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntry::NativeConstruct"));
}

void UW_CraftingEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntry::NativeDestruct"));
}

void UW_CraftingEntry::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

int32 UW_CraftingEntry::GetOwnedAmount_Implementation()
{
	// Return the amount of this item currently owned by the player
	if (!IsValid(RequiredItem))
	{
		return 0;
	}

	// Get inventory manager from player controller's pawn
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return 0;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return 0;
	}

	// Try AC_InventoryManager first (on Pawn)
	UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();
	if (InventoryManager)
	{
		// Get item tag from the required item
		UPDA_Item* ItemData = Cast<UPDA_Item>(RequiredItem);
		if (ItemData && ItemData->ItemInformation.ItemTag.IsValid())
		{
			int32 Amount = 0;
			bool bSuccess = false;
			InventoryManager->GetAmountOfItemWithTag(ItemData->ItemInformation.ItemTag, Amount, bSuccess);
			if (bSuccess)
			{
				OwnedAmount = Amount;
				return Amount;
			}
		}

		// Fallback: query by asset directly
		int32 Amount = 0;
		bool bSuccess = false;
		InventoryManager->GetAmountOfItem(RequiredItem, Amount, bSuccess);
		OwnedAmount = Amount;
		return Amount;
	}

	// Fallback: Try InventoryManagerComponent (slot-based with stacks)
	UInventoryManagerComponent* InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
	if (InvManagerComp)
	{
		// Get item tag from the required item
		UPDA_Item* ItemData = Cast<UPDA_Item>(RequiredItem);
		if (ItemData && ItemData->ItemInformation.ItemTag.IsValid())
		{
			int32 Amount = 0;
			bool bSuccess = false;
			InvManagerComp->GetAmountOfItemWithTag(ItemData->ItemInformation.ItemTag, Amount, bSuccess);
			if (bSuccess)
			{
				OwnedAmount = Amount;
				return Amount;
			}
		}

		// Fallback: query by asset directly
		int32 Amount = 0;
		bool bSuccess = false;
		InvManagerComp->GetAmountOfItem(RequiredItem, Amount, bSuccess);
		OwnedAmount = Amount;
		return Amount;
	}

	return 0;
}
void UW_CraftingEntry::EventRefreshAmounts_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CraftingEntry::EventRefreshAmounts_Implementation"));
}
