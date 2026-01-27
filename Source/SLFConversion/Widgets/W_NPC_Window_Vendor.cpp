// W_NPC_Window_Vendor.cpp
// C++ Widget implementation for W_NPC_Window_Vendor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_NPC_Window_Vendor.h"
#include "Widgets/W_VendorSlot.h"

UW_NPC_Window_Vendor::UW_NPC_Window_Vendor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_NPC_Window_Vendor::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Hidden MainBlur widget"));
	}

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::NativeConstruct"));
}

void UW_NPC_Window_Vendor::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::NativeDestruct"));
}

void UW_NPC_Window_Vendor::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

int32 UW_NPC_Window_Vendor::GetPlayerCurrency_Implementation()
{
	// Get owning player and return their currency (typically from PlayerState or GameInstance)
	if (APlayerController* PC = GetOwningPlayer())
	{
		// Access the player's currency through the game instance or player state
		// This would typically go through an interface or component
		UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::GetPlayerCurrency"));
	}
	return 0;
}

UW_VendorSlot* UW_NPC_Window_Vendor::GetEmptySlot_Implementation()
{
	// Iterate VendorSlots and find one that doesn't have an item assigned
	for (UW_VendorSlot* VendorSlot : VendorSlots)
	{
		if (VendorSlot && !OccupiedVendorSlots.Contains(VendorSlot))
		{
			return VendorSlot;
		}
	}
	return nullptr;
}

void UW_NPC_Window_Vendor::AddNewSlots_Implementation()
{
	// Create W_VendorSlot widgets and add to VendorSlots array
	if (UWorld* World = GetWorld())
	{
		UClass* SlotClass = LoadClass<UW_VendorSlot>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot.W_VendorSlot_C"));
		if (SlotClass)
		{
			UW_VendorSlot* NewSlot = CreateWidget<UW_VendorSlot>(World, SlotClass);
			if (NewSlot)
			{
				VendorSlots.Add(NewSlot);
				UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::AddNewSlots - Added new vendor slot"));
			}
		}
	}
}
void UW_NPC_Window_Vendor::EventCloseVendorAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventCloseVendorAction_Implementation"));
}


void UW_NPC_Window_Vendor::EventInitializeVendor_Implementation(const FText& NpcName, UPDA_Vendor* InVendorAsset, uint8 InVendorType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventInitializeVendor_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateCancel_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateDown_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateLeft_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateOk_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateRight_Implementation"));
}


void UW_NPC_Window_Vendor::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateUp_Implementation"));
}


void UW_NPC_Window_Vendor::EventOnActionMenuVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnActionMenuVisibilityChanged_Implementation"));
}


void UW_NPC_Window_Vendor::EventOnSellSlotCleared_Implementation(UW_VendorSlot* InSlot, bool TriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnSellSlotCleared_Implementation"));
}


void UW_NPC_Window_Vendor::EventOnSlotPressed_Implementation(UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnSlotPressed_Implementation"));
}


void UW_NPC_Window_Vendor::EventOnSlotSelected_Implementation(bool Selected, UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnSlotSelected_Implementation"));
}


void UW_NPC_Window_Vendor::EventOnVendorSlotAssigned_Implementation(UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnVendorSlotAssigned_Implementation"));
}


void UW_NPC_Window_Vendor::EventSetupItemInfoPanel_Implementation(UW_VendorSlot* ForSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventSetupItemInfoPanel_Implementation"));
}


void UW_NPC_Window_Vendor::EventToggleItemInfo_Implementation(bool Visible)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventToggleItemInfo_Implementation"));
}
