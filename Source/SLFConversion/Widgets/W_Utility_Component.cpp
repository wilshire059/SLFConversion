// W_Utility_Component.cpp
// C++ Widget implementation for W_Utility_Component
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Utility_Component.h"
#include "Widgets/W_Utility_AssetTooltip.h"

UW_Utility_Component::UW_Utility_Component(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Utility_Component::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Utility_Component::NativeConstruct"));
}

void UW_Utility_Component::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Utility_Component::NativeDestruct"));
}

void UW_Utility_Component::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

UWidget* UW_Utility_Component::GetToolTipWidget_Implementation()
{
	// Return the tooltip widget (cast to base UWidget)
	return Cast<UWidget>(Tooltip);
}

FEventReply UW_Utility_Component::OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Broadcast OnDoubleClick event
	OnDoubleClick.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[W_Utility_Component] OnMouseDoubleClick"));
	return FEventReply(true);
}
