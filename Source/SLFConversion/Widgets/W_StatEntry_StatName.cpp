// W_StatEntry_StatName.cpp
// C++ Widget implementation for W_StatEntry_StatName
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_StatEntry_StatName.h"
#include "Components/TextBlock.h"

UW_StatEntry_StatName::UW_StatEntry_StatName(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CachedStatNameText(nullptr)
{
}

void UW_StatEntry_StatName::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Apply Text property to internal StatName TextBlock (matches Blueprint PreConstruct logic)
	// Blueprint uses variable named "StatName" which is a TextBlock
	if (!Text.IsEmpty())
	{
		// Get the StatName TextBlock by name (it's a variable in the Blueprint's WidgetTree)
		if (UTextBlock* StatNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName"))))
		{
			StatNameText->SetText(Text);
			UE_LOG(LogTemp, Log, TEXT("W_StatEntry_StatName::NativePreConstruct - Set StatName to: %s"), *Text.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("W_StatEntry_StatName::NativePreConstruct - StatName widget not found!"));
		}
	}
}

void UW_StatEntry_StatName::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Apply Text if already set before construction
	if (!Text.IsEmpty() && CachedStatNameText)
	{
		CachedStatNameText->SetText(Text);
		UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::NativeConstruct - Applied Text: %s"), *Text.ToString());
	}

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::NativeConstruct"));
}

void UW_StatEntry_StatName::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::NativeDestruct"));
}

void UW_StatEntry_StatName::CacheWidgetReferences()
{
	// Cache the StatName TextBlock reference
	CachedStatNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName")));

	if (CachedStatNameText)
	{
		UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::CacheWidgetReferences - Found StatName TextBlock"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_StatEntry_StatName::CacheWidgetReferences - StatName TextBlock NOT found!"));
	}
}

void UW_StatEntry_StatName::SetStatText(const FText& InText)
{
	// Store the text
	Text = InText;

	// Update the TextBlock if we have it cached
	if (CachedStatNameText)
	{
		CachedStatNameText->SetText(InText);
		UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::SetStatText - Set to: %s"), *InText.ToString());
	}
	else
	{
		// Try to find and update it directly
		if (UTextBlock* StatNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName"))))
		{
			StatNameText->SetText(InText);
			CachedStatNameText = StatNameText;
			UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::SetStatText - Found and set to: %s"), *InText.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_StatEntry_StatName::SetStatText - Cannot find StatName TextBlock!"));
		}
	}
}

