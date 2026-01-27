// W_RestMenu_Button.cpp
// C++ Widget implementation for W_RestMenu_Button
//
// 20-PASS VALIDATION: 2026-01-07

#include "Widgets/W_RestMenu_Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UW_RestMenu_Button::UW_RestMenu_Button(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedBtnBorder = nullptr;
	CachedTxt = nullptr;
	CachedBtn = nullptr;
	Selected = false;
	SelectedColor = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f); // Gold
}

void UW_RestMenu_Button::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Set initial text from ButtonText property
	if (CachedTxt && !ButtonText.IsEmpty())
	{
		CachedTxt->SetText(ButtonText);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::NativeConstruct - Text: %s"), *ButtonText.ToString());
}

void UW_RestMenu_Button::NativeDestruct()
{
	// Unbind button click
	if (CachedBtn)
	{
		CachedBtn->OnClicked.RemoveDynamic(this, &UW_RestMenu_Button::OnBtnClicked);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::NativeDestruct"));
}

void UW_RestMenu_Button::CacheWidgetReferences()
{
	// Find child widgets by name
	CachedBtnBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BtnBorder")));
	CachedTxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt")));
	CachedBtn = Cast<UButton>(GetWidgetFromName(TEXT("Btn")));

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::CacheWidgetReferences - BtnBorder: %s, Txt: %s, Btn: %s"),
		CachedBtnBorder ? TEXT("Found") : TEXT("NULL"),
		CachedTxt ? TEXT("Found") : TEXT("NULL"),
		CachedBtn ? TEXT("Found") : TEXT("NULL"));

	// Bind button click event
	if (CachedBtn)
	{
		CachedBtn->OnClicked.AddDynamic(this, &UW_RestMenu_Button::OnBtnClicked);
	}
}

void UW_RestMenu_Button::SetRestMenuButtonSelected_Implementation(bool InSelected)
{
	Selected = InSelected;

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::SetRestMenuButtonSelected - %s -> %s"),
		*ButtonText.ToString(), InSelected ? TEXT("SELECTED") : TEXT("unselected"));

	// Always update text to show ButtonText (in case it was set after NativeConstruct)
	if (CachedTxt && !ButtonText.IsEmpty())
	{
		CachedTxt->SetText(ButtonText);
	}

	// Update border color to show selection
	if (CachedBtnBorder)
	{
		if (InSelected)
		{
			CachedBtnBorder->SetBrushColor(SelectedColor);
		}
		else
		{
			// Transparent when not selected
			CachedBtnBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}

	// Broadcast selection event if selected
	if (InSelected)
	{
		OnRestMenuButtonSelected.Broadcast(this);
	}
}

void UW_RestMenu_Button::EventRestMenuButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::EventRestMenuButtonPressed - %s"), *ButtonText.ToString());

	// Broadcast press event
	OnRestMenuButtonPressed.Broadcast();
}

void UW_RestMenu_Button::OnBtnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_Button::OnBtnClicked - %s"), *ButtonText.ToString());

	// Call the event which broadcasts the dispatcher
	EventRestMenuButtonPressed();
}
