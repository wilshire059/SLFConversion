// W_StatEntry_StatName.cpp
// C++ Widget implementation for W_StatEntry_StatName
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_StatEntry_StatName.h"
#include "Components/TextBlock.h"

UW_StatEntry_StatName::UW_StatEntry_StatName(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_StatEntry_StatName::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Apply Text property to internal StatName TextBlock (matches Blueprint PreConstruct logic)
	if (!Text.IsEmpty())
	{
		// The root widget is the StatName TextBlock - access via GetRootWidget()
		if (UTextBlock* StatNameText = Cast<UTextBlock>(GetRootWidget()))
		{
			StatNameText->SetText(Text);
		}
	}
}

void UW_StatEntry_StatName::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::NativeConstruct"));
}

void UW_StatEntry_StatName::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_StatName::NativeDestruct"));
}

void UW_StatEntry_StatName::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

