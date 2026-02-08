// W_LoadGame_Entry.cpp
// C++ Widget implementation for W_LoadGame_Entry

#include "Widgets/W_LoadGame_Entry.h"
#include "Blueprints/SG_SoulslikeFramework.h"
#include "Kismet/GameplayStatics.h"

UW_LoadGame_Entry::UW_LoadGame_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_LoadGame_Entry::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	// If SetSaveSlotName was called before NativeConstruct (common: parent calls it before AddChild),
	// the text widgets were null at that time. Re-apply saved data now that widgets are available.
	if (SGO)
	{
		ApplySaveDataToWidgets();
	}

	// Start with selection border hidden
	if (CachedHighlightBorder)
	{
		CachedHighlightBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_LoadGame_Entry::NativeDestruct()
{
	Super::NativeDestruct();
}

void UW_LoadGame_Entry::CacheWidgetReferences()
{
	CachedCharacterClassText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CharacterClassText")));
	CachedLevelText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LevelText")));
	CachedPlayTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayTimeText")));
	CachedHighlightBorder = GetWidgetFromName(TEXT("SelectionBorder"));

	UE_LOG(LogTemp, Log, TEXT("[W_LoadGame_Entry] CacheWidgetReferences - ClassText:%s LevelText:%s PlayTimeText:%s SelectionBorder:%s"),
		CachedCharacterClassText ? TEXT("OK") : TEXT("NULL"),
		CachedLevelText ? TEXT("OK") : TEXT("NULL"),
		CachedPlayTimeText ? TEXT("OK") : TEXT("NULL"),
		CachedHighlightBorder ? TEXT("OK") : TEXT("NULL"));
}

void UW_LoadGame_Entry::ApplySaveDataToWidgets()
{
	if (!SGO) return;

	FSLFSaveGameInfo SaveData = SGO->GetSavedData();

	// Display level
	if (CachedLevelText)
	{
		CachedLevelText->SetText(FText::AsNumber(SaveData.Level));
	}

	// Display playtime as HH:MM:SS
	if (CachedPlayTimeText)
	{
		int32 TotalSeconds = (int32)SaveData.PlayTime.GetTotalSeconds();
		int32 Hours = TotalSeconds / 3600;
		int32 Minutes = (TotalSeconds % 3600) / 60;
		int32 Seconds = TotalSeconds % 60;
		FString TimeStr = FString::Printf(TEXT("%02dh:%02dm:%02ds"), Hours, Minutes, Seconds);
		CachedPlayTimeText->SetText(FText::FromString(TimeStr));
	}

	// Display character class name - extract from slot name (SLF_Quinn_0 -> Quinn)
	if (CachedCharacterClassText)
	{
		FString CharName = SaveSlotName;
		if (CharName.StartsWith(TEXT("SLF_")))
		{
			CharName = CharName.RightChop(4); // Remove "SLF_"
			int32 LastUnderscore;
			if (CharName.FindLastChar('_', LastUnderscore))
			{
				CharName = CharName.Left(LastUnderscore); // Remove "_0"
			}
		}
		CachedCharacterClassText->SetText(FText::FromString(CharName));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_LoadGame_Entry] ApplySaveDataToWidgets: %s - Level=%d, PlayTime=%s"),
		*SaveSlotName, SaveData.Level, *SaveData.PlayTime.ToString());
}

void UW_LoadGame_Entry::SetSaveSlotSelected_Implementation(bool InSelected)
{
	Selected = InSelected;

	// Toggle selection border visibility for visual selection feedback
	if (CachedHighlightBorder)
	{
		CachedHighlightBorder->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UW_LoadGame_Entry::EventOnLoadEntryPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::EventOnLoadEntryPressed - Slot: %s"), *SaveSlotName);
	OnSaveSlotSelected.Broadcast(this);
}

void UW_LoadGame_Entry::SetSaveSlotName(const FString& InSlotName)
{
	SaveSlotName = InSlotName;

	// Load the save game to extract display data
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		USaveGame* LoadedSave = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0);
		SGO = Cast<USG_SoulslikeFramework>(LoadedSave);

		if (SGO)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_LoadGame_Entry] Loaded save: %s"), *SaveSlotName);

			// Try to apply now (works if NativeConstruct already ran)
			// If widgets are null, ApplySaveDataToWidgets will be called again from NativeConstruct
			ApplySaveDataToWidgets();
		}
	}
}

void UW_LoadGame_Entry::SetSelected(bool bInSelected)
{
	Selected = bInSelected;
	SetSaveSlotSelected(bInSelected);
}
