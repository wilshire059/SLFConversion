// W_Radar.cpp
// C++ Widget implementation for W_Radar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-20 - Fixed compass functionality

#include "Widgets/W_Radar.h"
#include "Widgets/W_Radar_TrackedElement.h"
#include "Widgets/W_Radar_Cardinal.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Engine/Texture2D.h"

UW_Radar::UW_Radar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RadarOL(nullptr)
	, PlayerIcon(nullptr)
	, IconSizer(nullptr)
	, CardinalContainer(nullptr)
{
}

void UW_Radar::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Radar::NativeConstruct - RadarOL: %s, PlayerIcon: %s"),
		RadarOL ? TEXT("Found") : TEXT("Missing"),
		PlayerIcon ? TEXT("Found") : TEXT("Missing"));
}

void UW_Radar::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Radar::NativeDestruct"));
}

void UW_Radar::CacheWidgetReferences()
{
	// Widgets are bound via meta = (BindWidgetOptional), so they should already be set
	// If not bound, try to find them by name
	if (!RadarOL)
	{
		RadarOL = Cast<UOverlay>(GetWidgetFromName(TEXT("RadarOL")));
	}
	if (!PlayerIcon)
	{
		PlayerIcon = Cast<UImage>(GetWidgetFromName(TEXT("PlayerIcon")));
	}
	if (!IconSizer)
	{
		IconSizer = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSizer")));
	}
	if (!CardinalContainer)
	{
		CardinalContainer = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("CardinalContainer")));
	}
}

void UW_Radar::AddCardinal_Implementation(const FSLFCardinalData& InData, int32 MaxAllowedNameLength)
{
	if (UWorld* World = GetWorld())
	{
		UClass* CardinalClass = LoadClass<UW_Radar_Cardinal>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Radar/W_Radar_Cardinal.W_Radar_Cardinal_C"));
		if (CardinalClass)
		{
			UW_Radar_Cardinal* NewCardinal = CreateWidget<UW_Radar_Cardinal>(World, CardinalClass);
			if (NewCardinal)
			{
				// Set cardinal data
				NewCardinal->CardinalData = InData;
				NewCardinal->MaxAllowedNameLength = MaxAllowedNameLength;

				// Add to RadarOL overlay (cardinals are positioned at top of compass)
				if (RadarOL)
				{
					UOverlaySlot* OverlaySlot = RadarOL->AddChildToOverlay(NewCardinal);
					if (OverlaySlot)
					{
						OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
						OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
					}
				}

				// Update the text display after setting CardinalData
				NewCardinal->UpdateCardinalDisplay();

				Cardinals.Add(NewCardinal);
				UE_LOG(LogTemp, Log, TEXT("UW_Radar::AddCardinal - Added cardinal: %s (Angle: %.0f)"),
					*InData.UIDisplayText, InData.Value);
			}
		}
	}
}

void UW_Radar::SetPlayerIcon_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& Tint, const FVector2D& Scale)
{
	if (PlayerIcon)
	{
		// Set the texture from soft pointer (bMatchSize = false per bp_only)
		PlayerIcon->SetBrushFromSoftTexture(Icon, false);

		// Set the brush tint color (not SetColorAndOpacity - per bp_only)
		PlayerIcon->SetBrushTintColor(FSlateColor(Tint));
	}

	// Set the icon size via the size box
	if (IconSizer)
	{
		IconSizer->SetWidthOverride(Scale.X);
		IconSizer->SetHeightOverride(Scale.Y);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Radar::SetPlayerIcon - Tint: (%.2f, %.2f, %.2f), Scale: (%.2f, %.2f)"),
		Tint.R, Tint.G, Tint.B, Scale.X, Scale.Y);
}

UW_Radar_TrackedElement* UW_Radar::AddTrackedElement_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& IconTint, const FVector2D& IconSize)
{
	if (UWorld* World = GetWorld())
	{
		UClass* ElementClass = LoadClass<UW_Radar_TrackedElement>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Radar/W_Radar_TrackedElement.W_Radar_TrackedElement_C"));
		if (ElementClass)
		{
			UW_Radar_TrackedElement* NewElement = CreateWidget<UW_Radar_TrackedElement>(World, ElementClass);
			if (NewElement)
			{
				// Set the element's icon properties
				NewElement->Icon = Icon;
				NewElement->IconTint = IconTint;
				NewElement->IconSize = IconSize;

				// Add to RadarOL overlay
				if (RadarOL)
				{
					UOverlaySlot* OverlaySlot = RadarOL->AddChildToOverlay(NewElement);
					if (OverlaySlot)
					{
						// Top alignment - positioning done via render transform
						OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
						OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
					}
				}

				Elements.Add(NewElement);
				UE_LOG(LogTemp, Log, TEXT("UW_Radar::AddTrackedElement - Added element, total: %d"), Elements.Num());
				return NewElement;
			}
		}
	}
	return nullptr;
}

void UW_Radar::RemoveTrackedElement_Implementation(UW_Radar_TrackedElement* Element)
{
	if (Element)
	{
		Elements.Remove(Element);
		Element->RemoveFromParent();
		UE_LOG(LogTemp, Log, TEXT("UW_Radar::RemoveTrackedElement - Removed element, remaining: %d"), Elements.Num());
	}
}
