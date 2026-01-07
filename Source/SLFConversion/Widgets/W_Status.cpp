// W_Status.cpp
// C++ Widget implementation for W_Status
//
// 20-PASS VALIDATION: 2026-01-05
// NO REFLECTION - direct property access for all child widgets

#include "Widgets/W_Status.h"
#include "Widgets/W_Status_LevelCurrencyBlock.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_StatManager.h"

UW_Status::UW_Status(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, W_Status_LevelCurrencyBlock(nullptr)
	, InventoryComponent(nullptr)
	, CurrentPlayerCurrency(0)
	, StatManagerComponent(nullptr)
	, CurrentPlayerLevel(1)
{
}

void UW_Status::NativeConstruct()
{
	Super::NativeConstruct();

	// StatManagerComponent is on PAWN
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		StatManagerComponent = Pawn->FindComponentByClass<UAC_StatManager>();
		if (StatManagerComponent)
		{
			// Bind to OnLevelUpdated event
			StatManagerComponent->OnLevelUpdated.AddDynamic(this, &UW_Status::EventOnLevelUpdated);

			// Initialize CurrentPlayerLevel from component
			CurrentPlayerLevel = StatManagerComponent->Level;

			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found StatManagerComponent on Pawn, Level=%d"), CurrentPlayerLevel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Status] StatManagerComponent not found on pawn"));
		}
	}

	// InventoryComponent is on PLAYERCONTROLLER (not Pawn!)
	if (APlayerController* PC = GetOwningPlayer())
	{
		InventoryComponent = PC->FindComponentByClass<UAC_InventoryManager>();
		if (InventoryComponent)
		{
			// Bind to OnCurrencyUpdated event
			InventoryComponent->OnCurrencyUpdated.AddDynamic(this, &UW_Status::EventOnCurrencyUpdated);

			// Initialize CurrentPlayerCurrency from component
			CurrentPlayerCurrency = InventoryComponent->Currency;

			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found InventoryComponent on PlayerController, Currency=%d"), CurrentPlayerCurrency);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Status] InventoryComponent not found on PlayerController"));
		}
	}

	// Initialize the level/currency block with current values
	InitializeLevelCurrencyBlock();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] NativeConstruct complete - Direct C++ bindings (NO REFLECTION)"));
}

void UW_Status::NativeDestruct()
{
	// Unbind from component events
	if (StatManagerComponent)
	{
		StatManagerComponent->OnLevelUpdated.RemoveAll(this);
	}
	if (InventoryComponent)
	{
		InventoryComponent->OnCurrencyUpdated.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] NativeDestruct"));
}

void UW_Status::InitializeLevelCurrencyBlock()
{
	// Initialize W_Status_LevelCurrencyBlock with current level and currency
	// From Blueprint: W_Status_LevelCurrencyBlock.EventInitializeLevelAndCurrency(CurrentPlayerLevel, CurrentPlayerCurrency)
	if (W_Status_LevelCurrencyBlock)
	{
		W_Status_LevelCurrencyBlock->EventInitializeLevelAndCurrency(CurrentPlayerLevel, CurrentPlayerCurrency);
		UE_LOG(LogTemp, Log, TEXT("[W_Status] InitializeLevelCurrencyBlock - Level=%d, Currency=%d"), CurrentPlayerLevel, CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status] W_Status_LevelCurrencyBlock is NULL - cannot initialize"));
	}
}

void UW_Status::EventNavigateCancel_Implementation()
{
	// Broadcast OnStatusClosed - W_HUD will handle returning to game menu
	OnStatusClosed.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[W_Status] EventNavigateCancel - Broadcasting OnStatusClosed"));
}

void UW_Status::EventOnCurrencyUpdated_Implementation(int32 NewCurrency)
{
	// Update local currency value
	CurrentPlayerCurrency = NewCurrency;

	// Re-initialize the level/currency block to show updated values
	InitializeLevelCurrencyBlock();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] EventOnCurrencyUpdated - NewCurrency=%d"), NewCurrency);
}

void UW_Status::EventOnLevelUpdated_Implementation(int32 NewLevel)
{
	// Update local level value
	CurrentPlayerLevel = NewLevel;

	// Re-initialize the level/currency block to show updated values
	InitializeLevelCurrencyBlock();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] EventOnLevelUpdated - NewLevel=%d"), NewLevel);
}

void UW_Status::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	// When this widget becomes visible, play fade in animation and initialize level/currency
	// From Blueprint comment: "When this widget becomes visible, play fade in animation and initialize level/currency in W_Status_LevelCurrencyBlock"

	if (InVisibility == ESlateVisibility::Visible)
	{
		// Refresh level/currency from components in case they changed while hidden
		if (StatManagerComponent)
		{
			CurrentPlayerLevel = StatManagerComponent->Level;
		}
		if (InventoryComponent)
		{
			CurrentPlayerCurrency = InventoryComponent->Currency;
		}

		// Initialize the level/currency block
		InitializeLevelCurrencyBlock();

		UE_LOG(LogTemp, Log, TEXT("[W_Status] Visibility changed to Visible - Refreshed Level=%d, Currency=%d"), CurrentPlayerLevel, CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Status] Visibility changed to %d"), static_cast<int32>(InVisibility));
	}
}
