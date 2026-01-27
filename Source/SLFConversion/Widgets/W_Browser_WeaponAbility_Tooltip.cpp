// W_Browser_WeaponAbility_Tooltip.cpp
// C++ Widget implementation for W_Browser_WeaponAbility_Tooltip
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Browser_WeaponAbility_Tooltip.h"

UW_Browser_WeaponAbility_Tooltip::UW_Browser_WeaponAbility_Tooltip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Browser_WeaponAbility_Tooltip::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Browser_WeaponAbility_Tooltip::NativeConstruct"));
}

void UW_Browser_WeaponAbility_Tooltip::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Browser_WeaponAbility_Tooltip::NativeDestruct"));
}

void UW_Browser_WeaponAbility_Tooltip::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

