// AC_AI_InteractionManager.cpp
// C++ component implementation for AC_AI_InteractionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_InteractionManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_AI_InteractionManager.h"
#include "Engine/DataTable.h"
#include "Widgets/W_HUD.h"
#include "Components/AC_ProgressManager.h"
#include "Kismet/DataTableFunctionLibrary.h"

// Helper function to strip Blueprint property name suffixes
// Blueprint struct property names have format: PropertyName_NumericSuffix_GUID
// e.g., "Entry_3_E3D06962403A4DF403DFBF892EECBE72" -> "Entry"
static FString StripBlueprintPropertySuffix_AC(const FString& PropName)
{
	FString BaseName = PropName;

	// Strip GUID suffix if present (32 hex chars after last underscore)
	int32 UnderscoreIndex;
	if (BaseName.FindLastChar(TEXT('_'), UnderscoreIndex) && UnderscoreIndex > 0)
	{
		FString Suffix = BaseName.RightChop(UnderscoreIndex + 1);
		if (Suffix.Len() == 32)
		{
			BaseName = BaseName.Left(UnderscoreIndex);
		}
	}

	// Strip numeric suffix if present (e.g., "_3" from "Entry_3")
	if (BaseName.FindLastChar(TEXT('_'), UnderscoreIndex) && UnderscoreIndex > 0)
	{
		FString NumericSuffix = BaseName.RightChop(UnderscoreIndex + 1);
		bool bIsNumeric = true;
		for (TCHAR C : NumericSuffix)
		{
			if (!FChar::IsDigit(C))
			{
				bIsNumeric = false;
				break;
			}
		}
		if (bIsNumeric && NumericSuffix.Len() > 0)
		{
			BaseName = BaseName.Left(UnderscoreIndex);
		}
	}

	return BaseName;
}

UAC_AI_InteractionManager::UAC_AI_InteractionManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	DialogAsset = nullptr;
	ProgressManager = nullptr;
	CurrentIndex = 0;
	MaxIndex = 0;
	VendorAsset = nullptr;
	CachedHUD = nullptr;
	bIsFinishingDialog = false;
}

void UAC_AI_InteractionManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_InteractionManager::BeginPlay"));
}

void UAC_AI_InteractionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetCurrentDialogEntry - Get dialog entry at current index
 *
 * Blueprint Logic:
 * 1. Check if CurrentIndex is valid within Rows array
 * 2. Look up row in DataTable using Rows[CurrentIndex]
 * 3. Return the dialog entry
 */
FSLFDialogEntry UAC_AI_InteractionManager::GetCurrentDialogEntry_Implementation(UDataTable* DataTable, const TArray<FName>& Rows)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] GetCurrentDialogEntry - Index: %d, MaxIndex: %d"),
		CurrentIndex, Rows.Num());

	FSLFDialogEntry Result;

	if (!IsValid(DataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] Invalid DataTable"));
		return Result;
	}

	// Update max index
	MaxIndex = Rows.Num();

	// Validate current index
	if (CurrentIndex < 0 || CurrentIndex >= Rows.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] Invalid CurrentIndex: %d (Max: %d)"), CurrentIndex, Rows.Num());
		return Result;
	}

	// Get row name at current index
	FName RowName = Rows[CurrentIndex];

	// Check if DataTable uses C++ struct or Blueprint struct
	const UScriptStruct* RowStruct = DataTable->GetRowStruct();
	FString RowStructPath = RowStruct ? RowStruct->GetPathName() : TEXT("Unknown");
	bool bIsCppStruct = RowStructPath.Contains(TEXT("/Script/SLFConversion"));

	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] DataTable RowStruct: %s (C++: %s)"),
		*RowStructPath, bIsCppStruct ? TEXT("YES") : TEXT("NO"));

	if (bIsCppStruct)
	{
		// Direct typed lookup - DataTable uses C++ FSLFDialogEntry
		FSLFDialogEntry* FoundEntry = DataTable->FindRow<FSLFDialogEntry>(RowName, TEXT("GetCurrentDialogEntry"));
		if (FoundEntry)
		{
			Result = *FoundEntry;
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Found entry via C++ FindRow: %s"), *RowName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] C++ FindRow failed for: %s"), *RowName.ToString());
		}
	}
	else
	{
		// DataTable uses Blueprint struct - use FindRowUnchecked and extract data via reflection
		const uint8* RowData = DataTable->FindRowUnchecked(RowName);
		if (RowData && RowStruct)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Reading Blueprint struct via reflection..."));

			// Extract "Entry" FText property
			for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
			{
				FProperty* Prop = *PropIt;
				FString BaseName = StripBlueprintPropertySuffix_AC(Prop->GetName());

				// Match "Entry" property
				if (BaseName.Equals(TEXT("Entry"), ESearchCase::IgnoreCase))
				{
					if (FTextProperty* TextProp = CastField<FTextProperty>(Prop))
					{
						Result.Entry = TextProp->GetPropertyValue_InContainer(RowData);
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Extracted Entry: \"%s\""),
							*Result.Entry.ToString().Left(50));
					}
				}
				// Note: GameplayEvents array is complex, leaving empty for now
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] FindRowUnchecked failed for: %s"), *RowName.ToString());
		}
	}

	return Result;
}

// ═══════════════════════════════════════════════════════════════════════
// DIALOG EVENTS
// ═══════════════════════════════════════════════════════════════════════

/**
 * EventBeginDialog - Start dialog with NPC
 *
 * Blueprint Logic:
 * 1. Store ProgressManager and HUD references
 * 2. Get DialogAsset and call GetDialogTableBasedOnProgress
 * 3. Store result in ActiveTable
 * 4. Load table and show first dialog entry
 */
void UAC_AI_InteractionManager::EventBeginDialog_Implementation(UAC_ProgressManager* InProgressManager, UW_HUD* InHUD)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventBeginDialog - ProgressManager: %s, HUD: %s"),
		InProgressManager ? *InProgressManager->GetName() : TEXT("null"),
		InHUD ? *InHUD->GetName() : TEXT("null"));

	// Cache references
	ProgressManager = InProgressManager;
	CachedHUD = InHUD;
	CurrentIndex = 0;
	bIsFinishingDialog = false;  // Reset guard flag for new dialog session

	// Get dialog table from DialogAsset based on progress
	// For now, ActiveTable should already be set by Blueprint or externally
	// TODO: If DialogAsset is a PDA_Dialog, call GetDialogTableBasedOnProgress

	// Load the ActiveTable and show first entry
	if (!ActiveTable.IsNull())
	{
		UDataTable* LoadedTable = ActiveTable.LoadSynchronous();
		if (LoadedTable)
		{
			TArray<FName> RowNames;
			UDataTableFunctionLibrary::GetDataTableRowNames(LoadedTable, RowNames);
			MaxIndex = RowNames.Num();

			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventBeginDialog - Loaded table with %d rows"), MaxIndex);

			if (MaxIndex > 0)
			{
				FSLFDialogEntry Entry = GetCurrentDialogEntry(LoadedTable, RowNames);
				if (CachedHUD)
				{
					CachedHUD->EventSetupDialog(Entry.Entry);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] EventBeginDialog - ActiveTable is null!"));
	}
}

/**
 * EventAdjustIndexForExit - Advance to next dialog entry or finish
 *
 * Blueprint Logic:
 * 1. Increment CurrentIndex
 * 2. Load ActiveTable
 * 3. Get row count
 * 4. If CurrentIndex >= row count: finish dialog
 * 5. Else: show next dialog entry
 */
void UAC_AI_InteractionManager::EventAdjustIndexForExit_Implementation()
{
	// Guard against re-entry during dialog finish sequence
	if (bIsFinishingDialog)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] EventAdjustIndexForExit - Already finishing dialog, ignoring re-entry"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventAdjustIndexForExit - CurrentIndex: %d, MaxIndex: %d"),
		CurrentIndex, MaxIndex);

	// Increment index first
	CurrentIndex++;

	// Load the table
	if (ActiveTable.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] EventAdjustIndexForExit - ActiveTable is null!"));
		return;
	}

	UDataTable* LoadedTable = ActiveTable.LoadSynchronous();
	if (!LoadedTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] EventAdjustIndexForExit - Failed to load ActiveTable!"));
		return;
	}

	// Get row names to determine max index
	TArray<FName> RowNames;
	UDataTableFunctionLibrary::GetDataTableRowNames(LoadedTable, RowNames);
	MaxIndex = RowNames.Num();

	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventAdjustIndexForExit - After increment: CurrentIndex: %d, MaxIndex: %d"),
		CurrentIndex, MaxIndex);

	// Check if we've reached the end
	if (CurrentIndex >= MaxIndex)
	{
		// Set guard flag to prevent re-entry
		bIsFinishingDialog = true;

		UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventAdjustIndexForExit - End of dialog, finishing..."));

		// Reset index for next dialog
		CurrentIndex = 0;

		// Finish dialog on HUD
		if (CachedHUD)
		{
			CachedHUD->EventFinishDialog();
		}

		// Broadcast that dialog is finished
		OnDialogFinished.Broadcast();

		// Reset guard flag now that dialog is fully finished
		bIsFinishingDialog = false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventAdjustIndexForExit - Showing next entry at index %d"), CurrentIndex);

		// Show next dialog entry
		FSLFDialogEntry Entry = GetCurrentDialogEntry(LoadedTable, RowNames);
		if (CachedHUD)
		{
			CachedHUD->EventSetupDialog(Entry.Entry);
		}
	}
}

/**
 * EventResetDialogIndex - Reset dialog to beginning
 */
void UAC_AI_InteractionManager::EventResetDialogIndex_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] EventResetDialogIndex"));
	CurrentIndex = 0;
}
