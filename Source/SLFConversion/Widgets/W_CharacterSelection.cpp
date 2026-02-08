// W_CharacterSelection.cpp
// C++ Widget implementation for W_CharacterSelection
//
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelection.json

#include "Widgets/W_CharacterSelection.h"
#include "Widgets/W_CharacterSelectionCard.h"
#include "Components/ScrollBox.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"

UW_CharacterSelection::UW_CharacterSelection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NavigationIndex(0)
	, ClassesScrollBox(nullptr)
{
}

void UW_CharacterSelection::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelection] Hidden MainBlur widget"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::NativeConstruct"));

	// bp_only: Event Construct → Delay(0.2) → InitializeAndStoreClassAssets → CreateCardWidgets
	// We skip the delay since C++ construct is synchronous
	InitializeAndStoreClassAssets();
	CreateCardWidgets();
}

void UW_CharacterSelection::NativeDestruct()
{
	// Unbind card events
	for (UW_CharacterSelectionCard* Card : CharacterCardEntries)
	{
		if (IsValid(Card))
		{
			Card->OnCardClicked.RemoveAll(this);
			Card->OnCardSelected.RemoveAll(this);
		}
	}

	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::NativeDestruct"));
}

void UW_CharacterSelection::InitializeAndStoreClassAssets_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::InitializeAndStoreClassAssets"));

	BaseCharacterAssets.Empty();

	// bp_only uses AssetRegistry to find all PDA_BaseCharacterInfo instances.
	// GetAssetsByClass with native C++ class doesn't find Blueprint-derived instances,
	// so we scan the known data path and filter by Cast instead.
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	// Scan the BaseCharacters data directory for all assets
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssetsByPath(FName(TEXT("/Game/SoulslikeFramework/Data/BaseCharacters")), AssetDataList, false);

	UE_LOG(LogTemp, Log, TEXT("  AssetRegistry found %d assets in BaseCharacters path"), AssetDataList.Num());

	for (const FAssetData& AssetData : AssetDataList)
	{
		UObject* LoadedAsset = AssetData.GetAsset();
		if (UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(LoadedAsset))
		{
			BaseCharacterAssets.Add(CharInfo);
			UE_LOG(LogTemp, Log, TEXT("  Found character: %s (ClassName: '%s')"),
				*CharInfo->GetName(), *CharInfo->CharacterClassName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("  Skipped non-character asset: %s (class: %s)"),
				*AssetData.AssetName.ToString(),
				*AssetData.AssetClassPath.ToString());
		}
	}

	// Fallback: if no assets found via path scan, try loading known assets directly
	if (BaseCharacterAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Path scan found 0 characters, trying direct load"));

		const TCHAR* KnownPaths[] = {
			TEXT("/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn.DA_Quinn"),
			TEXT("/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny.DA_Manny"),
		};

		for (const TCHAR* Path : KnownPaths)
		{
			UObject* LoadedAsset = StaticLoadObject(UObject::StaticClass(), nullptr, Path);
			if (UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(LoadedAsset))
			{
				BaseCharacterAssets.Add(CharInfo);
				UE_LOG(LogTemp, Log, TEXT("  Direct loaded: %s (ClassName: '%s')"),
					*CharInfo->GetName(), *CharInfo->CharacterClassName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Failed to load or cast: %s (got: %s)"),
					Path, LoadedAsset ? *LoadedAsset->GetClass()->GetName() : TEXT("null"));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Total character assets: %d"), BaseCharacterAssets.Num());
}

void UW_CharacterSelection::CreateCardWidgets()
{
	UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelection] CreateCardWidgets - %d assets"), BaseCharacterAssets.Num());

	// Clear existing cards
	CharacterCardEntries.Empty();
	if (IsValid(ClassesScrollBox))
	{
		ClassesScrollBox->ClearChildren();
	}

	if (BaseCharacterAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelection] No character assets found!"));
		return;
	}

	// Try to load the Blueprint card widget class
	UClass* CardClass = LoadClass<UW_CharacterSelectionCard>(
		nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/MainMenu/W_CharacterSelectionCard.W_CharacterSelectionCard_C")
	);

	if (!CardClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelection] Could not load W_CharacterSelectionCard Blueprint class"));
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelection] No owning player controller"));
		return;
	}

	// Create a card widget for each character asset
	for (UPrimaryDataAsset* Asset : BaseCharacterAssets)
	{
		UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(Asset);
		if (!CharInfo)
		{
			continue;
		}

		UW_CharacterSelectionCard* NewCard = CreateWidget<UW_CharacterSelectionCard>(PC, CardClass);
		if (!IsValid(NewCard))
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelection] Failed to create card for %s"), *CharInfo->GetName());
			continue;
		}

		// Set the character class asset on the card BEFORE adding to viewport
		NewCard->CharacterClassAsset = CharInfo;

		// Add to scroll box
		if (IsValid(ClassesScrollBox))
		{
			ClassesScrollBox->AddChild(NewCard);
		}

		// Bind card events
		NewCard->OnCardClicked.AddDynamic(this, &UW_CharacterSelection::OnCardClickedHandler);
		NewCard->OnCardSelected.AddDynamic(this, &UW_CharacterSelection::OnCardSelectedHandler);

		CharacterCardEntries.Add(NewCard);

		UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelection] Created card for: %s"), *CharInfo->DisplayName.ToString());
	}

	// Select first card
	NavigationIndex = 0;
	UpdateCardSelection();

	UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelection] Created %d cards"), CharacterCardEntries.Num());
}

void UW_CharacterSelection::UpdateCardSelection()
{
	for (int32 i = 0; i < CharacterCardEntries.Num(); ++i)
	{
		if (IsValid(CharacterCardEntries[i]))
		{
			CharacterCardEntries[i]->SetCardSelected(i == NavigationIndex);
		}
	}
}

void UW_CharacterSelection::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventNavigateCancel_Implementation"));

	// Broadcast close event - parent (W_MainMenu) handles return to menu
	OnCharacterSelectionClosed.Broadcast();
}

void UW_CharacterSelection::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventNavigateLeft_Implementation"));

	if (CharacterCardEntries.Num() == 0)
	{
		return;
	}

	// Move to previous card (wrap around)
	NavigationIndex = (NavigationIndex - 1 + CharacterCardEntries.Num()) % CharacterCardEntries.Num();
	UpdateCardSelection();

	// Scroll to selected card
	if (IsValid(ClassesScrollBox) && CharacterCardEntries.IsValidIndex(NavigationIndex))
	{
		ClassesScrollBox->ScrollWidgetIntoView(CharacterCardEntries[NavigationIndex]);
	}
}

void UW_CharacterSelection::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventNavigateRight_Implementation"));

	if (CharacterCardEntries.Num() == 0)
	{
		return;
	}

	// Move to next card (wrap around)
	NavigationIndex = (NavigationIndex + 1) % CharacterCardEntries.Num();
	UpdateCardSelection();

	// Scroll to selected card
	if (IsValid(ClassesScrollBox) && CharacterCardEntries.IsValidIndex(NavigationIndex))
	{
		ClassesScrollBox->ScrollWidgetIntoView(CharacterCardEntries[NavigationIndex]);
	}
}

void UW_CharacterSelection::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventNavigateOk_Implementation - Index: %d"), NavigationIndex);

	if (!CharacterCardEntries.IsValidIndex(NavigationIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelection] NavigateOk - invalid index %d (cards: %d)"),
			NavigationIndex, CharacterCardEntries.Num());
		return;
	}

	UW_CharacterSelectionCard* SelectedCard = CharacterCardEntries[NavigationIndex];
	if (IsValid(SelectedCard))
	{
		// Trigger the card's pressed event
		SelectedCard->EventOnCardPressed();
	}
}

void UW_CharacterSelection::EventOnCardPressed_Implementation(UPDA_BaseCharacterInfo* ClassAsset)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventOnCardPressed - %s"), *GetNameSafe(ClassAsset));

	// Broadcast to parent (W_MainMenu) which handles the new game flow
	if (IsValid(ClassAsset))
	{
		OnClassCardClicked.Broadcast(ClassAsset);
	}
}

void UW_CharacterSelection::EventOnCardSelected_Implementation(UW_CharacterSelectionCard* Card)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventOnCardSelected - %s"), *GetNameSafe(Card));

	// Update NavigationIndex to match the selected card
	int32 FoundIndex = CharacterCardEntries.Find(Card);
	if (FoundIndex != INDEX_NONE)
	{
		NavigationIndex = FoundIndex;
		UpdateCardSelection();
	}
}

void UW_CharacterSelection::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelection::EventOnVisibilityChanged - %d"), InVisibility);

	// Refresh cards when becoming visible
	if (InVisibility == static_cast<uint8>(ESlateVisibility::Visible) ||
		InVisibility == static_cast<uint8>(ESlateVisibility::SelfHitTestInvisible))
	{
		if (CharacterCardEntries.Num() == 0)
		{
			InitializeAndStoreClassAssets();
			CreateCardWidgets();
		}
	}
}

// Card delegate handlers - bridge from card events to parent events
void UW_CharacterSelection::OnCardClickedHandler(UPrimaryDataAsset* ClassAsset)
{
	UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(ClassAsset);
	if (CharInfo)
	{
		EventOnCardPressed(CharInfo);
	}
}

void UW_CharacterSelection::OnCardSelectedHandler(UW_CharacterSelectionCard* Card)
{
	EventOnCardSelected(Card);
}
