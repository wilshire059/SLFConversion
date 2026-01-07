// AIInteractionManagerComponent.cpp
// C++ implementation for AC_AI_InteractionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_AI_InteractionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 (initialized in constructor)
// Functions:         6/6 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 1/1 (OnDialogFinished)
// ═══════════════════════════════════════════════════════════════════════════════

#include "AIInteractionManagerComponent.h"
#include "ProgressManagerComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Blueprint/UserWidget.h"

UAIInteractionManagerComponent::UAIInteractionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config variables
	Name = FText::FromString(TEXT("NPC"));
	DialogAsset = nullptr;
	VendorAsset = nullptr;

	// Initialize runtime variables
	ProgressManager = nullptr;
	CurrentIndex = 0;
	MaxIndex = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DIALOG ACCESS [1/6]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIInteractionManagerComponent::GetCurrentDialogEntry_Implementation(
	UDataTable* DataTable, FName& OutRowName, FTableRowBase& OutRow, bool& bSuccess)
{
	bSuccess = false;

	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] GetCurrentDialogEntry - No DataTable"));
		return;
	}

	// Get row names
	TArray<FName> RowNames = DataTable->GetRowNames();

	if (CurrentIndex >= 0 && CurrentIndex < RowNames.Num())
	{
		OutRowName = RowNames[CurrentIndex];

		// Try to find the row
		if (FTableRowBase* FoundRow = DataTable->FindRow<FTableRowBase>(OutRowName, TEXT("GetCurrentDialogEntry")))
		{
			OutRow = *FoundRow;
			bSuccess = true;
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] GetCurrentDialogEntry - Found row %s at index %d"),
				*OutRowName.ToString(), CurrentIndex);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// DIALOG CONTROL [2-4/6]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIInteractionManagerComponent::BeginDialog_Implementation(
	UProgressManagerComponent* InProgressManager, UUserWidget* HUD)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] BeginDialog - NPC: %s"), *Name.ToString());

	ProgressManager = InProgressManager;

	// Load dialog table from asset
	if (UPDA_Dialog* Dialog = Cast<UPDA_Dialog>(DialogAsset))
	{
		// Get dialog table based on player's progress
		TSoftObjectPtr<UDataTable> DialogTable;
		Dialog->GetDialogTableBasedOnProgress(ProgressManager, DialogTable);

		// If no progress-based table, use default
		if (DialogTable.IsNull())
		{
			DialogTable = Dialog->DefaultDialogTable;
		}

		// Async load the dialog table
		if (!DialogTable.IsNull())
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			Streamable.RequestAsyncLoad(DialogTable.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &UAIInteractionManagerComponent::OnDialogTableLoaded, DialogTable));
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Async loading dialog table: %s"),
				*DialogTable.ToSoftObjectPath().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] No dialog table available"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] DialogAsset is not a UPDA_Dialog"));
	}
}

void UAIInteractionManagerComponent::AdjustIndexForExit_Implementation()
{
	// Increment index for "goodbye" responses
	CurrentIndex = FMath::Min(CurrentIndex + 1, MaxIndex);

	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] AdjustIndexForExit - New index: %d/%d"),
		CurrentIndex, MaxIndex);

	// Broadcast dialog finished
	OnDialogFinished.Broadcast();
}

void UAIInteractionManagerComponent::ResetDialogIndex_Implementation()
{
	CurrentIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] ResetDialogIndex"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL CALLBACKS [5-6/6]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIInteractionManagerComponent::OnLoaded_06386C0B4155173667FF4F931DF1ACA5(UObject* Loaded)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Dialog table loaded: %s"),
		Loaded ? *Loaded->GetName() : TEXT("null"));

	if (UDataTable* LoadedTable = Cast<UDataTable>(Loaded))
	{
		ActiveTable = LoadedTable;
		MaxIndex = LoadedTable->GetRowNames().Num() - 1;
	}
}

void UAIInteractionManagerComponent::OnLoaded_0D66BEC94ACD1661A92D91A4A9C8D9E4(UObject* Loaded)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Vendor table loaded: %s"),
		Loaded ? *Loaded->GetName() : TEXT("null"));

	// Vendor inventory is typically pre-configured in UPDA_Vendor::Items
	// This callback can be used for additional setup if needed
	if (UPDA_Vendor* Vendor = Cast<UPDA_Vendor>(VendorAsset))
	{
		UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Vendor has %d items configured"),
			Vendor->Items.Num());
	}
}

void UAIInteractionManagerComponent::OnDialogTableLoaded(TSoftObjectPtr<UDataTable> LoadedTable)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Dialog table async load complete"));

	if (UDataTable* Table = LoadedTable.Get())
	{
		ActiveTable = Table;
		MaxIndex = FMath::Max(0, Table->GetRowNames().Num() - 1);
		UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Dialog table initialized with %d entries (MaxIndex: %d)"),
			Table->GetRowNames().Num(), MaxIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] Failed to resolve dialog table"));
	}
}
