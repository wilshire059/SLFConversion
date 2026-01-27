// W_Browser_Action.cpp
// C++ Widget implementation for W_Browser_Action
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Browser_Action.h"
#include "Widgets/W_Browser_Action_Tooltip.h"

UW_Browser_Action::UW_Browser_Action(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Browser_Action::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Browser_Action::NativeConstruct"));
}

void UW_Browser_Action::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Browser_Action::NativeDestruct"));
}

void UW_Browser_Action::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

UWidget* UW_Browser_Action::GetToolTipWidget_Implementation()
{
	// Return the Tooltip widget for this action browser entry
	return Cast<UWidget>(Tooltip);
}

FEventReply UW_Browser_Action::OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Broadcast selection with double click flag
	OnSelected.Broadcast(this, true, true);
	return FEventReply(true);
}
void UW_Browser_Action::EventClear_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Browser_Action::EventClear_Implementation"));
}
