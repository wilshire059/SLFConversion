// W_CharacterSelectionCard.cpp
// C++ Widget implementation for W_CharacterSelectionCard
//
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelectionCard.json

#include "Widgets/W_CharacterSelectionCard.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

UW_CharacterSelectionCard::UW_CharacterSelectionCard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CharacterClassAsset(nullptr)
	, Selected(false)
	, SelectionBorder(nullptr)
	, ClassNameText(nullptr)
	, CharacterImage(nullptr)
	, CardBtn(nullptr)
	, StatBox(nullptr)
{
}

void UW_CharacterSelectionCard::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelectionCard::NativeConstruct - Asset: %s"),
		CharacterClassAsset ? *CharacterClassAsset->GetName() : TEXT("null"));

	// Bind button click
	if (CardBtn)
	{
		CardBtn->OnClicked.AddDynamic(this, &UW_CharacterSelectionCard::OnCardBtnClicked);
	}

	// Initially hide selection border
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Populate display from CharacterClassAsset
	PopulateCardDisplay();
}

void UW_CharacterSelectionCard::NativeDestruct()
{
	if (CardBtn)
	{
		CardBtn->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelectionCard::NativeDestruct"));
}

void UW_CharacterSelectionCard::PopulateCardDisplay()
{
	if (!CharacterClassAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelectionCard] No CharacterClassAsset set"));
		return;
	}

	UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(CharacterClassAsset);
	if (!CharInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_CharacterSelectionCard] CharacterClassAsset is not UPDA_BaseCharacterInfo"));
		return;
	}

	// Set character class name text
	if (ClassNameText)
	{
		// Use CharacterClassName first, fall back to DisplayName, then asset name
		FText NameText = CharInfo->CharacterClassName;
		if (NameText.IsEmpty())
		{
			NameText = CharInfo->DisplayName;
		}
		if (NameText.IsEmpty())
		{
			NameText = FText::FromString(CharInfo->GetName());
		}
		ClassNameText->SetText(NameText);
		UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelectionCard] Set name: %s"), *NameText.ToString());
	}

	// Set character image from BaseClassTexture
	if (CharacterImage && !CharInfo->BaseClassTexture.IsNull())
	{
		UTexture2D* Texture = CharInfo->BaseClassTexture.LoadSynchronous();
		if (Texture)
		{
			CharacterImage->SetBrushFromTexture(Texture);
			UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelectionCard] Set image: %s"), *Texture->GetName());
		}
	}

	// Base stats display would go in StatBox - the stat entries are embedded in UMG
	// so the Blueprint designer handles their layout. We just need to set the data.
	if (StatBox && CharInfo->BaseStats.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelectionCard] %d base stats available"), CharInfo->BaseStats.Num());
	}
}

void UW_CharacterSelectionCard::SetCardSelected_Implementation(bool InSelected)
{
	Selected = InSelected;

	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelectionCard::SetCardSelected - %s, Selected: %s"),
		CharacterClassAsset ? *CharacterClassAsset->GetName() : TEXT("null"),
		InSelected ? TEXT("true") : TEXT("false"));

	// Toggle SelectionBorder visibility (bp_only pattern: visibility, not brush color)
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(InSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// NOTE: Do NOT broadcast OnCardSelected here - it causes infinite recursion:
	// SetCardSelected -> OnCardSelected -> EventOnCardSelected -> UpdateCardSelection -> SetCardSelected
	// OnCardSelected is only broadcast from user interaction (button click / hover)
}

void UW_CharacterSelectionCard::InitializeStatEntries_Implementation(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& InputPin)
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelectionCard::InitializeStatEntries - %d stats"), InputPin.Num());
	// Stat entries are embedded in UMG designer, their display is handled by their own NativeConstruct
}

void UW_CharacterSelectionCard::EventOnCardPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_CharacterSelectionCard::EventOnCardPressed - %s"),
		CharacterClassAsset ? *CharacterClassAsset->GetName() : TEXT("null"));

	// Broadcast OnCardClicked with the character class asset
	if (CharacterClassAsset)
	{
		OnCardClicked.Broadcast(CharacterClassAsset);
	}
}

void UW_CharacterSelectionCard::OnCardBtnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_CharacterSelectionCard] CardBtn clicked"));
	EventOnCardPressed();
}
