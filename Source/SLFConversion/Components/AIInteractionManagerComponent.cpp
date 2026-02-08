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
#include "AC_ProgressManager.h"
#include "SLFPrimaryDataAssets.h"
#include "Widgets/W_HUD.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Blueprint/UserWidget.h"

// Helper function to strip Blueprint property name suffixes
// Blueprint struct property names have format: PropertyName_NumericSuffix_GUID
// e.g., "Entry_3_E3D06962403A4DF403DFBF892EECBE72" -> "Entry"
static FString StripBlueprintPropertySuffix(const FString& PropName)
{
	FString BaseName = PropName;

	// Strip GUID suffix if present (32 hex chars after last underscore)
	int32 UnderscoreIndex;
	if (BaseName.FindLastChar(TEXT('_'), UnderscoreIndex) && UnderscoreIndex > 0)
	{
		FString Suffix = BaseName.RightChop(UnderscoreIndex + 1);
		if (Suffix.Len() == 32) // GUID suffix is 32 hex chars
		{
			BaseName = BaseName.Left(UnderscoreIndex);
		}
	}

	// Strip numeric suffix if present (e.g., "_3" from "Entry_3")
	if (BaseName.FindLastChar(TEXT('_'), UnderscoreIndex) && UnderscoreIndex > 0)
	{
		FString NumericSuffix = BaseName.RightChop(UnderscoreIndex + 1);
		// Check if suffix is purely numeric
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
	CachedHUD = nullptr;
	bIsFinishingDialog = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DIALOG ACCESS
// ═══════════════════════════════════════════════════════════════════════════════

FSLFDialogEntry UAIInteractionManagerComponent::GetCurrentDialogEntry(UDataTable* DataTable, const TArray<FName>& Rows)
{
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] GetCurrentDialogEntry - Index: %d, RowsNum: %d"),
		CurrentIndex, Rows.Num());

	FSLFDialogEntry Result;

	if (!IsValid(DataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] GetCurrentDialogEntry - Invalid DataTable"));
		return Result;
	}

	// Update max index
	MaxIndex = Rows.Num();

	// Validate current index
	if (CurrentIndex < 0 || CurrentIndex >= Rows.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] GetCurrentDialogEntry - Invalid CurrentIndex: %d (Max: %d)"), CurrentIndex, Rows.Num());
		return Result;
	}

	// Get row name at current index
	FName RowName = Rows[CurrentIndex];

	// ═══════════════════════════════════════════════════════════════════════════════
	// REFLECTION FALLBACK FOR BLUEPRINT STRUCT DATATABLES
	// The DataTables use Blueprint-defined FDialogEntry struct, not C++ FSLFDialogEntry.
	// FindRow<T>() fails because struct paths don't match:
	//   Blueprint: /Game/SoulslikeFramework/Structures/Dialog/FDialogEntry
	//   C++:       /Script/SLFConversion.FSLFDialogEntry
	// Solution: Detect struct type and use FindRowUnchecked + reflection for Blueprint structs.
	// ═══════════════════════════════════════════════════════════════════════════════

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
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Reading Blueprint struct via reflection for row: %s"), *RowName.ToString());

			// Extract "Entry" FText property
			for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
			{
				FProperty* Prop = *PropIt;
				FString BaseName = StripBlueprintPropertySuffix(Prop->GetName());

				// Match "Entry" property (the dialog text)
				if (BaseName.Equals(TEXT("Entry"), ESearchCase::IgnoreCase))
				{
					if (FTextProperty* TextProp = CastField<FTextProperty>(Prop))
					{
						Result.Entry = TextProp->GetPropertyValue_InContainer(RowData);
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Extracted Entry: \"%s\""),
							*Result.Entry.ToString().Left(80));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] 'Entry' property is not FText, type: %s"),
							*Prop->GetClass()->GetName());
					}
				}
				// Extract GameplayEvents array
				else if (BaseName.Equals(TEXT("GameplayEvents"), ESearchCase::IgnoreCase))
				{
					if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
					{
						FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(RowData));
						int32 NumEvents = ArrayHelper.Num();
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Found %d GameplayEvents"), NumEvents);

						// Get the inner struct type
						FStructProperty* InnerStructProp = CastField<FStructProperty>(ArrayProp->Inner);
						if (InnerStructProp && InnerStructProp->Struct)
						{
							UScriptStruct* EventStruct = InnerStructProp->Struct;
							UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Event struct: %s"), *EventStruct->GetName());

							for (int32 i = 0; i < NumEvents; ++i)
							{
								const uint8* EventData = ArrayHelper.GetRawPtr(i);
								if (!EventData) continue;

								FSLFDialogGameplayEvent GameplayEvent;

								// Extract EventTag and AdditionalTag from the event struct
								for (TFieldIterator<FProperty> EventPropIt(EventStruct); EventPropIt; ++EventPropIt)
								{
									FProperty* EventProp = *EventPropIt;
									FString EventBaseName = StripBlueprintPropertySuffix(EventProp->GetName());

									if (EventBaseName.Equals(TEXT("EventTag"), ESearchCase::IgnoreCase))
									{
										if (FStructProperty* TagProp = CastField<FStructProperty>(EventProp))
										{
											const FGameplayTag* TagPtr = TagProp->ContainerPtrToValuePtr<FGameplayTag>(EventData);
											if (TagPtr)
											{
												GameplayEvent.EventTag = *TagPtr;
												UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Event[%d] EventTag: %s"),
													i, *GameplayEvent.EventTag.ToString());
											}
										}
									}
									else if (EventBaseName.Equals(TEXT("AdditionalTag"), ESearchCase::IgnoreCase))
									{
										if (FStructProperty* TagProp = CastField<FStructProperty>(EventProp))
										{
											const FGameplayTag* TagPtr = TagProp->ContainerPtrToValuePtr<FGameplayTag>(EventData);
											if (TagPtr)
											{
												GameplayEvent.AdditionalTag = *TagPtr;
												UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Event[%d] AdditionalTag: %s"),
													i, *GameplayEvent.AdditionalTag.ToString());
											}
										}
									}
									// CustomData is FInstancedStruct - skip for now as it's complex
								}

								Result.GameplayEvents.Add(GameplayEvent);
							}
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] FindRowUnchecked failed for row: %s"), *RowName.ToString());
		}
	}

	return Result;
}

void UAIInteractionManagerComponent::GetCurrentDialogEntryOutParams(
	UDataTable* DataTable, FName& OutRowName, FTableRowBase& OutRow, bool& bSuccess)
{
	bSuccess = false;

	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] GetCurrentDialogEntryOutParams - No DataTable"));
		return;
	}

	// Get row names
	TArray<FName> RowNames = DataTable->GetRowNames();

	if (CurrentIndex >= 0 && CurrentIndex < RowNames.Num())
	{
		OutRowName = RowNames[CurrentIndex];

		// Try to find the row
		if (FTableRowBase* FoundRow = DataTable->FindRow<FTableRowBase>(OutRowName, TEXT("GetCurrentDialogEntryOutParams")))
		{
			OutRow = *FoundRow;
			bSuccess = true;
			UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] GetCurrentDialogEntryOutParams - Found row %s at index %d"),
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
	CurrentIndex = 0;  // Reset to first entry
	bIsFinishingDialog = false;  // Reset guard flag for new dialog session

	// Cache HUD reference for later dialog advancement
	CachedHUD = Cast<UW_HUD>(HUD);
	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] BeginDialog - CachedHUD: %s"),
		CachedHUD ? TEXT("valid") : TEXT("null"));

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

		// Store active table and load synchronously for immediate display
		if (!DialogTable.IsNull())
		{
			ActiveTable = DialogTable;
			UDataTable* LoadedTable = DialogTable.LoadSynchronous();
			if (LoadedTable)
			{
				TArray<FName> RowNames;
				UDataTableFunctionLibrary::GetDataTableRowNames(LoadedTable, RowNames);
				MaxIndex = RowNames.Num();
				UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] BeginDialog - Loaded %d dialog entries"),
					MaxIndex);

				// Show first dialog entry
				if (MaxIndex > 0 && CachedHUD)
				{
					FSLFDialogEntry Entry = GetCurrentDialogEntry(LoadedTable, RowNames);
					CachedHUD->EventSetupDialog(Entry.Entry);
				}
			}
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
	// Guard against re-entry during dialog finish sequence
	// This can happen if input fires twice before OnDialogFinished unbinds the delegate
	if (bIsFinishingDialog)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] AdjustIndexForExit - Already finishing dialog, ignoring re-entry"));
		return;
	}

	// Increment index to move to next dialog entry
	CurrentIndex++;

	UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] AdjustIndexForExit - After increment: CurrentIndex: %d, MaxIndex: %d"),
		CurrentIndex, MaxIndex);

	// Check if we've reached the end of dialog
	if (CurrentIndex >= MaxIndex)
	{
		// Set guard flag to prevent re-entry
		bIsFinishingDialog = true;

		UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] AdjustIndexForExit - End of dialog, finishing..."));

		// Set progress to Completed if the dialog has completion requirements
		if (UPDA_Dialog* Dialog = Cast<UPDA_Dialog>(DialogAsset))
		{
			FGameplayTag CompletionTag;
			if (Dialog->GetCompletionProgressTag(CompletionTag) && CompletionTag.IsValid())
			{
				// Set progress via ProgressManager
				if (ProgressManager)
				{
					if (UProgressManagerComponent* PMC = Cast<UProgressManagerComponent>(ProgressManager))
					{
						PMC->SetProgress(CompletionTag, ESLFProgress::Completed);
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Set progress %s = Completed via UProgressManagerComponent"),
							*CompletionTag.ToString());
					}
					else if (UAC_ProgressManager* ACPM = Cast<UAC_ProgressManager>(ProgressManager))
					{
						ACPM->SetProgress(CompletionTag, ESLFProgress::Completed);
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Set progress %s = Completed via UAC_ProgressManager"),
							*CompletionTag.ToString());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] Could not set progress - no ProgressManager!"));
				}
			}
		}

		// Reset for next dialog
		CurrentIndex = 0;

		// Finish dialog on HUD
		if (CachedHUD)
		{
			CachedHUD->EventFinishDialog();

			// ═══════════════════════════════════════════════════════════════════════
			// CRITICAL: If this NPC has a VendorAsset, show the NPC window NOW
			// In bp_only, EventSetupNpcWindow is called AFTER dialog finishes, not before
			// ═══════════════════════════════════════════════════════════════════════
			if (VendorAsset)
			{
				UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Dialog finished - showing NPC window for vendor: %s"), *Name.ToString());
				CachedHUD->EventSetupNpcWindow(Name, VendorAsset, this);
			}
		}

		// Broadcast dialog finished
		OnDialogFinished.Broadcast();

		// Reset guard flag now that dialog is fully finished
		bIsFinishingDialog = false;
	}
	else
	{
		// Show next dialog entry
		if (!ActiveTable.IsNull())
		{
			UDataTable* LoadedTable = ActiveTable.LoadSynchronous();
			if (LoadedTable)
			{
				TArray<FName> RowNames;
				UDataTableFunctionLibrary::GetDataTableRowNames(LoadedTable, RowNames);

				UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] AdjustIndexForExit - Showing entry %d of %d"),
					CurrentIndex, RowNames.Num());

				if (CachedHUD)
				{
					FSLFDialogEntry Entry = GetCurrentDialogEntry(LoadedTable, RowNames);

					// Execute gameplay events for this dialog entry (item rewards, progress updates, etc.)
					if (Entry.GameplayEvents.Num() > 0)
					{
						UE_LOG(LogTemp, Log, TEXT("[AIInteractionManager] Processing %d gameplay events for entry %d"),
							Entry.GameplayEvents.Num(), CurrentIndex);

						// Execute events via progress manager (try both class types)
						if (ProgressManager)
						{
							// Try UProgressManagerComponent first
							if (UProgressManagerComponent* PMC = Cast<UProgressManagerComponent>(ProgressManager))
							{
								PMC->ExecuteDialogGameplayEvents(Entry.GameplayEvents);
							}
							// Fall back to UAC_ProgressManager (uses ExecuteGameplayEvents instead)
							else if (UAC_ProgressManager* ACPM = Cast<UAC_ProgressManager>(ProgressManager))
							{
								ACPM->ExecuteGameplayEvents(Entry.GameplayEvents);
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] ProgressManager is unknown type: %s"),
									*ProgressManager->GetClass()->GetName());
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[AIInteractionManager] ProgressManager is null, cannot execute gameplay events"));
						}
					}

					CachedHUD->EventSetupDialog(Entry.Entry);
				}
			}
		}
	}
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
