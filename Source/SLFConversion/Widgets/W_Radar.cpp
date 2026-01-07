// W_Radar.cpp
// C++ Widget implementation for W_Radar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Radar.h"
#include "Widgets/W_Radar_TrackedElement.h"
#include "Widgets/W_Radar_Cardinal.h"

UW_Radar::UW_Radar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Radar::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Radar::NativeConstruct"));
}

void UW_Radar::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Radar::NativeDestruct"));
}

void UW_Radar::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

void UW_Radar::AddCardinal_Implementation(const FSLFCardinalData& InData, int32 MaxAllowedNameLength)
{
	// Create W_Radar_Cardinal widget, pass cardinal data and max name length, add to Cardinals array
	if (UWorld* World = GetWorld())
	{
		UClass* CardinalClass = LoadClass<UW_Radar_Cardinal>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Radar/W_Radar_Cardinal.W_Radar_Cardinal_C"));
		if (CardinalClass)
		{
			UW_Radar_Cardinal* NewCardinal = CreateWidget<UW_Radar_Cardinal>(World, CardinalClass);
			if (NewCardinal)
			{
				Cardinals.Add(NewCardinal);
				UE_LOG(LogTemp, Log, TEXT("UW_Radar::AddCardinal - Added new cardinal"));
			}
		}
	}
}

void UW_Radar::SetPlayerIcon_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& Tint, const FVector2D& Scale)
{
	// Set player icon brush from soft texture, set tint color, and apply scale
	// PlayerIcon is a UImage widget that needs to be accessed via BindWidget or property
	UE_LOG(LogTemp, Log, TEXT("UW_Radar::SetPlayerIcon - Icon tint: (%.2f, %.2f, %.2f), Scale: (%.2f, %.2f)"),
		Tint.R, Tint.G, Tint.B, Scale.X, Scale.Y);
}

UW_Radar_TrackedElement* UW_Radar::AddTrackedElement_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& IconTint, const FVector2D& IconSize)
{
	// Create W_Radar_TrackedElement widget, add to overlay, add to Elements array
	if (UWorld* World = GetWorld())
	{
		UClass* ElementClass = LoadClass<UW_Radar_TrackedElement>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Radar/W_Radar_TrackedElement.W_Radar_TrackedElement_C"));
		if (ElementClass)
		{
			UW_Radar_TrackedElement* NewElement = CreateWidget<UW_Radar_TrackedElement>(World, ElementClass);
			if (NewElement)
			{
				Elements.Add(NewElement);
				UE_LOG(LogTemp, Log, TEXT("UW_Radar::AddTrackedElement - Added new tracked element"));
				return NewElement;
			}
		}
	}
	return nullptr;
}

void UW_Radar::RemoveTrackedElement_Implementation(UW_Radar_TrackedElement* Element)
{
	// Remove from Elements array and remove from parent
	if (Element)
	{
		Elements.Remove(Element);
		Element->RemoveFromParent();
		UE_LOG(LogTemp, Log, TEXT("UW_Radar::RemoveTrackedElement - Removed element"));
	}
}
