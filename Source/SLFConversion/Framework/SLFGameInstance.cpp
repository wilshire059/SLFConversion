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
	bFirstTimeOnDemoLevel = true;
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
			UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Loaded save slots (%d slots)"), SGO_Slots->Slots.Num());
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
	// JSON Logic: IsValid(SGO_Slots) → GetAllSlots() → Return SaveSlots
	if (IsValid(SGO_Slots))
	{
		SaveSlots = SGO_Slots->GetAllSlots();
		UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] GetAllSaveSlots: %d slots"), SaveSlots.Num());
	}
	else
	{
		SaveSlots.Empty();
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] GetAllSaveSlots: SGO_Slots is invalid"));
	}
}

void USLFGameInstance::SetLastSlotNameToActive_Implementation()
{
	// JSON Logic: IsValid(SGO_Slots) → GetLastSaveSlot() → Set ActiveSlot
	if (IsValid(SGO_Slots))
	{
		ActiveSlot = SGO_Slots->GetLastSaveSlot();
		UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] SetLastSlotNameToActive: %s"), *ActiveSlot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameInstance] SetLastSlotNameToActive: SGO_Slots is invalid"));
	}
}

void USLFGameInstance::DoesAnySaveExist_Implementation(bool& ReturnValue)
{
	// JSON Logic: DoesSaveGameExist(SlotsSaveName, 0) → Return
	ReturnValue = UGameplayStatics::DoesSaveGameExist(SlotsSaveName, 0);
	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] DoesAnySaveExist: %s"), ReturnValue ? TEXT("true") : TEXT("false"));
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
