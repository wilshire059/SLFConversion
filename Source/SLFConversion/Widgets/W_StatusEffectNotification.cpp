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
#include "Components/AC_StatusEffectManager.h"
#include "Components/StatusEffectManagerComponent.h"
#include "TimerManager.h"

UW_StatusEffectNotification::UW_StatusEffectNotification(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NotificationDuration = 2.0; // Default 2 seconds
	TxtStatusEffect = nullptr;
	CachedStatusEffectManager = nullptr;
	CachedStatusEffectManagerComponent = nullptr;
}

void UW_StatusEffectNotification::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Start hidden - only show when status effect triggers
	// bp_only: Widget shows on OnStatusEffectTriggered, hides on EventFinishNotification
	SetVisibility(ESlateVisibility::Collapsed);

	// Bind to status effect manager (matches bp_only Construct graph)
	BindToStatusEffectManager();

	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::NativeConstruct - Widget starts collapsed, waiting for status effect"));
}

void UW_StatusEffectNotification::NativeDestruct()
{
	// Unbind from status effect manager to prevent dangling delegates
	if (IsValid(CachedStatusEffectManager))
	{
		CachedStatusEffectManager->OnStatusEffectTriggered.RemoveDynamic(this, &UW_StatusEffectNotification::OnStatusEffectTriggeredHandler);
	}
	if (IsValid(CachedStatusEffectManagerComponent))
	{
		CachedStatusEffectManagerComponent->OnStatusEffectTextTriggered.RemoveDynamic(this, &UW_StatusEffectNotification::OnStatusEffectTriggeredHandler);
	}

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
 * Blueprint Logic (from bp_only JSON graphs):
 * 1. Called when notification display is complete
 * 2. Plays FadeOut animation
 * 3. When FadeOut finishes → SetVisibility(Collapsed)
 *
 * CRITICAL: bp_only does NOT call RemoveFromParent()!
 * The widget stays in the hierarchy but becomes collapsed/hidden.
 * This allows it to be reused for subsequent status effect triggers.
 */
void UW_StatusEffectNotification::EventFinishNotification_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::EventFinishNotification - Hiding widget (not removing)"));

	// bp_only plays FadeOut animation here, then sets visibility to Collapsed
	// Since we can't easily play Blueprint animations from C++, just hide immediately
	// The widget stays in the hierarchy and can be reused
	SetVisibility(ESlateVisibility::Collapsed);
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

	// Make widget visible (bp_only sets visibility to Visible here)
	SetVisibility(ESlateVisibility::Visible);

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

/**
 * BindToStatusEffectManager
 *
 * Blueprint Logic (from bp_only JSON Construct graph):
 * 1. GetOwningPlayerPawn()
 * 2. GetComponentByClass(AC_StatusEffectManager_C)
 * 3. Bind Event to OnStatusEffectTriggered
 *
 * This allows the widget to receive notifications when status effects trigger.
 */
void UW_StatusEffectNotification::BindToStatusEffectManager()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_StatusEffectNotification::BindToStatusEffectManager - No owning pawn"));
		return;
	}

	// Try to find AC_StatusEffectManager first (Blueprint component)
	CachedStatusEffectManager = OwningPawn->FindComponentByClass<UAC_StatusEffectManager>();
	if (CachedStatusEffectManager)
	{
		// Bind to OnStatusEffectTriggered
		CachedStatusEffectManager->OnStatusEffectTriggered.AddDynamic(this, &UW_StatusEffectNotification::OnStatusEffectTriggeredHandler);
		UE_LOG(LogTemp, Warning, TEXT("UW_StatusEffectNotification::BindToStatusEffectManager - Bound to AC_StatusEffectManager (0x%p) on %s"),
			CachedStatusEffectManager, *OwningPawn->GetName());
		return;
	}

	// Try StatusEffectManagerComponent (C++ component)
	CachedStatusEffectManagerComponent = OwningPawn->FindComponentByClass<UStatusEffectManagerComponent>();
	if (CachedStatusEffectManagerComponent)
	{
		// Bind to OnStatusEffectTextTriggered
		CachedStatusEffectManagerComponent->OnStatusEffectTextTriggered.AddDynamic(this, &UW_StatusEffectNotification::OnStatusEffectTriggeredHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectNotification::BindToStatusEffectManager - Bound to StatusEffectManagerComponent on %s"),
			*OwningPawn->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UW_StatusEffectNotification::BindToStatusEffectManager - No StatusEffectManager found on pawn %s"),
		*OwningPawn->GetName());
}

/**
 * OnStatusEffectTriggeredHandler
 *
 * Handler called when status effect manager broadcasts OnStatusEffectTriggered.
 * Calls EventOnStatusEffectTriggered to display the notification.
 */
void UW_StatusEffectNotification::OnStatusEffectTriggeredHandler(FText TriggeredText)
{
	UE_LOG(LogTemp, Warning, TEXT("UW_StatusEffectNotification::OnStatusEffectTriggeredHandler - Received: %s (Widget 0x%p, Manager 0x%p)"),
		*TriggeredText.ToString(), this, CachedStatusEffectManager);

	// Call the event function to display the notification
	EventOnStatusEffectTriggered(TriggeredText);
}
