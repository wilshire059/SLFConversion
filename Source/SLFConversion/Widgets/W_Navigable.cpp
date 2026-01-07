// W_Navigable.cpp
// C++ Widget implementation for W_Navigable
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Navigable.h"

UW_Navigable::UW_Navigable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Navigable::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Navigable::NativeConstruct"));
}

void UW_Navigable::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Navigable::NativeDestruct"));
}

void UW_Navigable::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

bool UW_Navigable::CanNavigate_Implementation()
{
	// By default, can navigate only if widget is visible
	// Can be overridden in children widgets which inherit from W_Navigable
	return IsVisible();
}
