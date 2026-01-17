// W_Status.cpp
// C++ Widget implementation for W_Status
//
// 20-PASS VALIDATION: 2026-01-05
// NO REFLECTION - direct property access for all child widgets

#include "Widgets/W_Status.h"
#include "Widgets/W_Status_LevelCurrencyBlock.h"
#include "Widgets/W_Status_StatBlock.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/StatManagerComponent.h"
#include "Blueprints/SLFStatBase.h"

UW_Status::UW_Status(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, W_Status_LevelCurrencyBlock(nullptr)
	, W_StatBlock_Status(nullptr)
	, W_StatBlock_Status_1(nullptr)
	, W_StatBlock_Status_3(nullptr)
	, W_StatBlock_Status_5(nullptr)
	, W_StatBlock_Status_7(nullptr)
	, W_StatBlock_Status_8(nullptr)
	, InventoryComponent(nullptr)
	, CurrentPlayerCurrency(0)
	, StatManagerComponent(nullptr)
	, CurrentPlayerLevel(1)
{
}

void UW_Status::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Status] Hidden MainBlur widget"));
	}

	// Components are on PlayerController based on debug logs
	// Blueprint uses GetOwningPlayer() which returns PC
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] NativeConstruct - PC: %s, Pawn: %s"),
		PC ? *PC->GetName() : TEXT("NULL"),
		Pawn ? *Pawn->GetName() : TEXT("NULL"));

	// Check PlayerController first (where components actually are)
	if (PC)
	{
		StatManagerComponent = PC->FindComponentByClass<UStatManagerComponent>();
		if (StatManagerComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found StatManagerComponent on PlayerController"));
		}

		InventoryComponent = PC->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found InventoryComponent on PlayerController"));
		}
	}

	// Fallback to Pawn if not found on PC
	if (!StatManagerComponent && Pawn)
	{
		StatManagerComponent = Pawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManagerComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found StatManagerComponent on Pawn (fallback)"));
		}
	}
	if (!InventoryComponent && Pawn)
	{
		InventoryComponent = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Found InventoryComponent on Pawn (fallback)"));
		}
	}

	// Now bind events and initialize values
	if (StatManagerComponent)
	{
		StatManagerComponent->OnLevelUpdated.AddDynamic(this, &UW_Status::EventOnLevelUpdated);
		StatManagerComponent->OnStatsInitialized.AddDynamic(this, &UW_Status::OnStatsInitializedHandler);
		CurrentPlayerLevel = StatManagerComponent->Level;
		UE_LOG(LogTemp, Log, TEXT("[W_Status] Bound to StatManagerComponent, Level=%d"), CurrentPlayerLevel);

		// If stats are already initialized (ActiveStats is populated), populate stat blocks now
		if (StatManagerComponent->ActiveStats.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Stats already initialized (%d stats), populating immediately"),
				StatManagerComponent->ActiveStats.Num());
			PopulateStatBlocks();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status] StatManagerComponent NOT FOUND on PC or Pawn!"));
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnCurrencyUpdated.AddDynamic(this, &UW_Status::EventOnCurrencyUpdated);
		CurrentPlayerCurrency = InventoryComponent->Currency;
		UE_LOG(LogTemp, Log, TEXT("[W_Status] Bound to InventoryComponent, Currency=%d"), CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status] InventoryComponent NOT FOUND on PC or Pawn!"));
	}

	// Make focusable for keyboard input
	SetIsFocusable(true);

	// Bind visibility changed
	OnVisibilityChanged.AddDynamic(this, &UW_Status::EventOnVisibilityChanged);

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
		StatManagerComponent->OnStatsInitialized.RemoveAll(this);
	}
	if (InventoryComponent)
	{
		InventoryComponent->OnCurrencyUpdated.RemoveAll(this);
	}

	OnVisibilityChanged.RemoveAll(this);

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] NativeDestruct"));
}

FReply UW_Status::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_Status] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Navigate Cancel/Back: Escape, Gamepad B, Tab
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Tab)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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

		// Set focus to this widget for keyboard input
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
			UE_LOG(LogTemp, Log, TEXT("[W_Status] Set user focus"));
		}

		UE_LOG(LogTemp, Log, TEXT("[W_Status] Visibility changed to Visible - Refreshed Level=%d, Currency=%d"), CurrentPlayerLevel, CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Status] Visibility changed to %d"), static_cast<int32>(InVisibility));
	}
}

void UW_Status::OnStatsInitializedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Status] OnStatsInitializedHandler - Stats are ready, populating stat blocks"));
	PopulateStatBlocks();
}

void UW_Status::PopulateStatBlocks()
{
	if (!StatManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Status] PopulateStatBlocks - No StatManagerComponent!"));
		return;
	}

	// Get all stats from StatManager
	TArray<UObject*> StatObjects;
	TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
	StatManagerComponent->GetAllStats(StatObjects, StatClassesAndCategories);

	UE_LOG(LogTemp, Log, TEXT("[W_Status] PopulateStatBlocks - Got %d stats from StatManager"), StatObjects.Num());

	// Convert to USLFStatBase array (StatManagerComponent creates USLFStatBase objects)
	TArray<USLFStatBase*> Stats;
	for (UObject* Obj : StatObjects)
	{
		if (USLFStatBase* StatObj = Cast<USLFStatBase>(Obj))
		{
			Stats.Add(StatObj);
			UE_LOG(LogTemp, Log, TEXT("[W_Status] - Stat: %s (Tag: %s, Current: %.0f, Max: %.0f)"),
				*StatObj->StatInfo.DisplayName.ToString(),
				*StatObj->StatInfo.Tag.ToString(),
				StatObj->StatInfo.CurrentValue,
				StatObj->StatInfo.MaxValue);
		}
	}

	// Convert TSubclassOf<UObject> map to TSubclassOf<USLFStatBase> map for SetupCurrentStats
	TMap<FGameplayTag, TSubclassOf<USLFStatBase>> StatTagToClass;
	for (const auto& Pair : StatClassesAndCategories)
	{
		if (Pair.Key && Pair.Key->IsChildOf(USLFStatBase::StaticClass()))
		{
			StatTagToClass.Add(Pair.Value, TSubclassOf<USLFStatBase>(Pair.Key));
		}
	}

	// Define correct category tags that match actual stat tags
	// Actual stat tags use: Primary, Secondary, AttackPower, DamageNegation, Resistance, Derived
	// Blueprint had wrong categories like Defense.Negation, Defense.Resistances, Secondary.AttackPower
	static const FGameplayTag PrimaryTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary"));
	static const FGameplayTag SecondaryTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary"));
	static const FGameplayTag AttackPowerTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.AttackPower"));
	static const FGameplayTag DamageNegationTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.DamageNegation"));
	static const FGameplayTag ResistanceTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Resistance"));
	static const FGameplayTag DerivedTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Derived"));

	// Map each stat block to correct category
	// W_StatBlock_Status = Primary stats (Vigor, Mind, Endurance, etc.)
	// W_StatBlock_Status_1 = Damage Negation stats
	// W_StatBlock_Status_3 = Resistance stats
	// W_StatBlock_Status_5 = Secondary stats (HP, FP, Stamina, etc.)
	// W_StatBlock_Status_7 = Derived/Misc stats
	// W_StatBlock_Status_8 = Attack Power stats
	struct FStatBlockConfig
	{
		UW_Status_StatBlock* Block;
		FGameplayTag CorrectCategory;
		const TCHAR* Name;
	};

	TArray<FStatBlockConfig> BlockConfigs = {
		{ W_StatBlock_Status, PrimaryTag, TEXT("Primary") },
		{ W_StatBlock_Status_1, DamageNegationTag, TEXT("DamageNegation") },
		{ W_StatBlock_Status_3, ResistanceTag, TEXT("Resistance") },
		{ W_StatBlock_Status_5, SecondaryTag, TEXT("Secondary") },
		{ W_StatBlock_Status_7, DerivedTag, TEXT("Derived") },
		{ W_StatBlock_Status_8, AttackPowerTag, TEXT("AttackPower") }
	};

	for (const FStatBlockConfig& Config : BlockConfigs)
	{
		if (Config.Block)
		{
			// Override the category with the correct value
			Config.Block->Category = Config.CorrectCategory;

			UE_LOG(LogTemp, Log, TEXT("[W_Status] Calling SetupCurrentStats on %s block with Category: %s"),
				Config.Name, *Config.CorrectCategory.ToString());
			Config.Block->SetupCurrentStats(Stats, StatTagToClass);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Status] %s stat block is NULL!"), Config.Name);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Status] PopulateStatBlocks complete"));
}
