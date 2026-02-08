// SLFGameInstance.cpp
// C++ implementation for GI_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation

#include "SLFGameInstance.h"
#include "Blueprints/SG_SaveSlots.h"
#include "Kismet/GameplayStatics.h"

USLFGameInstance::USLFGameInstance()
{
	CustomGameSettings = nullptr;
	SelectedBaseClass = nullptr;
	SGO_Slots = nullptr;
	FirstTimeOnDemoLevel = true;
}

void USLFGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Init"));

	// Load or create save slots
	// JSON Logic: DoesSaveGameExist(SlotsSaveName) → Branch
	//   TRUE:  LoadGameFromSlot → Cast to SG_SaveSlots → Set SGO_Slots
	//   FALSE: CreateSaveGameObject(SG_SaveSlots) → Set SGO_Slots
	if (UGameplayStatics::DoesSaveGameExist(SlotsSaveName, 0))
	{
		// TRUE branch: Load existing save slots
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotsSaveName, 0);
		SGO_Slots = Cast<USG_SaveSlots>(LoadedGame);

		if (SGO_Slots)
		{
			// Clean up invalid/empty slot names (legacy bp_only saves may have empty entries)
			int32 OrigCount = SGO_Slots->Slots.Num();
			SGO_Slots->Slots.RemoveAll([](const FString& S) { return S.IsEmpty(); });

			for (int32 i = 0; i < SGO_Slots->Slots.Num(); ++i)
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Slot[%d] = '%s'"), i, *SGO_Slots->Slots[i]);
			}

			if (OrigCount != SGO_Slots->Slots.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] Removed %d empty slot entries (legacy save cleanup)"),
					OrigCount - SGO_Slots->Slots.Num());

				// Re-save cleaned slots
				UGameplayStatics::SaveGameToSlot(SGO_Slots, SlotsSaveName, 0);
			}

			UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Loaded save slots (%d valid slots, LastSavedSlot='%s')"),
				SGO_Slots->Slots.Num(), *SGO_Slots->LastSavedSlot);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] Failed to cast loaded save to USG_SaveSlots"));
		}
	}
	else
	{
		// FALSE branch: Create new save slots object
		SGO_Slots = Cast<USG_SaveSlots>(UGameplayStatics::CreateSaveGameObject(USG_SaveSlots::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Created new save slots object"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_GameInstance INTERFACE IMPLEMENTATIONS [10/10]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFGameInstance::GetAllSaveSlots_Implementation(TArray<FString>& SaveSlots)
{
	if (IsValid(SGO_Slots))
	{
		SaveSlots = SGO_Slots->GetAllSlots();

		// Filter out empty slot names (legacy cleanup)
		SaveSlots.RemoveAll([](const FString& S) { return S.IsEmpty(); });

		for (int32 i = 0; i < SaveSlots.Num(); ++i)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetAllSaveSlots[%d]: '%s'"), i, *SaveSlots[i]);
		}
		UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetAllSaveSlots: %d valid slots"), SaveSlots.Num());
	}
	else
	{
		SaveSlots.Empty();
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] GetAllSaveSlots: SGO_Slots is invalid"));
	}
}

void USLFGameInstance::SetLastSlotNameToActive_Implementation()
{
	if (IsValid(SGO_Slots))
	{
		ActiveSlot = SGO_Slots->GetLastSaveSlot();

		// Fallback: if LastSavedSlot is empty, use the last entry in Slots array
		if (ActiveSlot.IsEmpty() && SGO_Slots->Slots.Num() > 0)
		{
			ActiveSlot = SGO_Slots->Slots.Last();
			UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] SetLastSlotNameToActive: LastSavedSlot was empty, falling back to last slot: '%s'"), *ActiveSlot);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] SetLastSlotNameToActive: %s"), *ActiveSlot);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] SetLastSlotNameToActive: SGO_Slots is invalid"));
	}
}

void USLFGameInstance::DoesAnySaveExist_Implementation(bool& ReturnValue)
{
	// Check both that the file exists AND there are valid (non-empty) slots
	if (UGameplayStatics::DoesSaveGameExist(SlotsSaveName, 0) && IsValid(SGO_Slots) && SGO_Slots->Slots.Num() > 0)
	{
		// Verify at least one slot actually has a corresponding save file
		bool bHasValidSlot = false;
		for (const FString& Slot : SGO_Slots->Slots)
		{
			if (!Slot.IsEmpty() && UGameplayStatics::DoesSaveGameExist(Slot, 0))
			{
				bHasValidSlot = true;
				break;
			}
		}
		ReturnValue = bHasValidSlot;
	}
	else
	{
		ReturnValue = false;
	}
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] DoesAnySaveExist: %s (Slots: %d)"),
		ReturnValue ? TEXT("true") : TEXT("false"),
		IsValid(SGO_Slots) ? SGO_Slots->Slots.Num() : 0);
}

void USLFGameInstance::AddAndSaveSlots_Implementation(const FString& NewSlotName)
{
	// JSON Logic:
	//   1. Print "SaveSlots: Adding Save slot {NewSlotName}"
	//   2. IsValid(SGO_Slots) → AddSlot(NewSlotName)
	//   3. AsyncSaveGameToSlot(SGO_Slots, SlotsSaveName, 0) with completion callback

	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] SaveSlots: Adding Save slot %s"), *NewSlotName);

	if (IsValid(SGO_Slots))
	{
		SGO_Slots->AddSlot(NewSlotName);

		// Async save with callback
		FAsyncSaveGameToSlotDelegate SaveDelegate;
		SaveDelegate.BindLambda([](const FString& SlotName, int32 UserIndex, bool bSuccess)
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Save slots saved successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] Failed to save slots"));
			}
		});

		UGameplayStatics::AsyncSaveGameToSlot(SGO_Slots, SlotsSaveName, 0, SaveDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] AddAndSaveSlots: SGO_Slots is invalid"));
	}
}

void USLFGameInstance::GetActiveSlotName_Implementation(FString& ActiveSlotName)
{
	// JSON Logic: Return ActiveSlot
	ActiveSlotName = ActiveSlot;
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetActiveSlotName: %s"), *ActiveSlotName);
}

void USLFGameInstance::SetActiveSlot_Implementation(const FString& ActiveSlotName)
{
	// JSON Logic: Set ActiveSlot = ActiveSlotName
	ActiveSlot = ActiveSlotName;
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] SetActiveSlot: %s"), *ActiveSlot);
}

void USLFGameInstance::GetSelectedClass_Implementation(UPrimaryDataAsset*& SelectedBaseCharacterClass)
{
	// JSON Logic: Return SelectedBaseClass
	SelectedBaseCharacterClass = SelectedBaseClass;
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetSelectedClass: %s"),
		SelectedBaseCharacterClass ? *SelectedBaseCharacterClass->GetName() : TEXT("null"));
}

void USLFGameInstance::SetSelectedClass_Implementation(UPrimaryDataAsset* BaseCharacterClass)
{
	// JSON Logic:
	//   1. Set SelectedBaseClass = BaseCharacterClass
	//   2. Call OnSelectedClassChanged (broadcast delegate)
	SelectedBaseClass = BaseCharacterClass;
	OnSelectedClassChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] SetSelectedClass: %s (broadcast OnSelectedClassChanged)"),
		BaseCharacterClass ? *BaseCharacterClass->GetName() : TEXT("null"));
}

void USLFGameInstance::GetSoulslikeGameInstance_Implementation(UGameInstance*& ReturnValue)
{
	// JSON Logic: Return self
	ReturnValue = this;
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetSoulslikeGameInstance: returning self"));
}

void USLFGameInstance::GetCustomGameSettings_Implementation(UPrimaryDataAsset*& CustomSettingsAsset)
{
	// JSON Logic: Return CustomGameSettings
	CustomSettingsAsset = CustomGameSettings;
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetCustomGameSettings: %s"),
		CustomSettingsAsset ? *CustomSettingsAsset->GetName() : TEXT("null"));
}
