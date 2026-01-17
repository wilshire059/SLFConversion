// W_ItemInfoEntry_StatScaling.cpp
// C++ Widget implementation for W_ItemInfoEntry_StatScaling
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to update TextBlock widgets

#include "Widgets/W_ItemInfoEntry_StatScaling.h"
#include "Components/TextBlock.h"

UW_ItemInfoEntry_StatScaling::UW_ItemInfoEntry_StatScaling(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EntryText(nullptr)
	, ValueText(nullptr)
{
}

void UW_ItemInfoEntry_StatScaling::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Update the TextBlock widgets with the current data
	if (EntryText)
	{
		EntryText->SetText(EntryNameText);
	}

	if (ValueText)
	{
		// Display the scaling grade (S, A, B, C, D, E)
		ValueText->SetText(Value);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry_StatScaling::NativeConstruct - Name: %s, Value: %s"),
		*EntryNameText.ToString(), *Value.ToString());
}

void UW_ItemInfoEntry_StatScaling::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_ItemInfoEntry_StatScaling::NativeDestruct"));
}

void UW_ItemInfoEntry_StatScaling::CacheWidgetReferences()
{
	if (!EntryText)
	{
		EntryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("EntryText")));
	}
	if (!ValueText)
	{
		ValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ValueText")));
	}
}

