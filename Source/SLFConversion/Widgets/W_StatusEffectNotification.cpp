// W_StatusEffectNotification.cpp
// C++ Widget implementation for W_StatusEffectNotification
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA_v2/WidgetBlueprint/W_StatusEffectNotification.json
//
// Blueprint Logic:
// - EventOnStatusEffectTriggered: Sets TxtStatusEffect text, plays fade-in animation
// - EventFinishNotification: Plays fade-out animation, removes from parent

#include "Widgets/W_StatusEffectNotification.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

UW_StatusEffectNotification::UW_StatusEffectNotification(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NotificationDuration = 2.0; // Default 2 seconds
	TxtStatusEffect = nullptr;
}

void UW_StatusEffectNotification::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::NativeConstruct"));
}

void UW_StatusEffectNotification::NativeDestruct()
{
	// Clear any pending timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NotificationTimerHandle);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::NativeDestruct"));
}

void UW_StatusEffectNotification::CacheWidgetReferences()
{
	// TxtStatusEffect is bound via BindWidget meta
	// If not bound, try to find it manually
	if (!TxtStatusEffect)
	{
		TxtStatusEffect = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtStatusEffect")));
	}
}

/**
 * EventFinishNotification
 *
 * Blueprint Logic (from JSON graphs):
 * 1. Called when notification display is complete
 * 2. Plays fade-out animation
 * 3. Removes widget from parent
 *
 * Note: Animation playback is handled by UMG in Blueprint.
 * C++ just removes the widget since animation is optional.
 */
void UW_StatusEffectNotification::EventFinishNotification_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::EventFinishNotification"));

	// Remove from parent
	RemoveFromParent();
}

/**
 * EventOnStatusEffectTriggered
 *
 * Blueprint Logic (from JSON graphs):
 * 1. Set TxtStatusEffect text to TriggeredText
 * 2. Play fade-in animation
 * 3. After NotificationDuration, call EventFinishNotification
 */
void UW_StatusEffectNotification::EventOnStatusEffectTriggered_Implementation(const FText& TriggeredText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::EventOnStatusEffectTriggered - Text: %s"),
		*TriggeredText.ToString());

	// Set the text
	if (IsValid(TxtStatusEffect))
	{
		TxtStatusEffect->SetText(TriggeredText);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  TxtStatusEffect widget is null"));
	}

	// Note: Blueprint plays UMG animations here. In C++, we just handle the timer for duration.
	// Animations should be set up in the Blueprint UMG designer and played via PlayAnimation()
	// if needed, but typically the Blueprint handles visual effects.

	// Set timer to finish notification after duration
	if (NotificationDuration > 0.0 && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			NotificationTimerHandle,
			FTimerDelegate::CreateLambda([this]()
			{
				EventFinishNotification();
			}),
			static_cast<float>(NotificationDuration),
			false
		);
	}
}
