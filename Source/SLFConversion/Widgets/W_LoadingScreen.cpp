// W_LoadingScreen.cpp
// C++ Widget implementation for W_LoadingScreen
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-20 - Implemented actual level loading logic per bp_only JSON
//
// Level Loading Flow (from BlueprintDNA_v2):
// 1. EventOpenLevelByReferenceAndFadeOut(Level, bAbsolute, Options)
//    - Stores parameters for later use
//    - Checks AdditionalWaitDuration > 0
//    - If true: Sets timer, then plays FadeOut animation, then opens level
//    - If false: Plays FadeOut animation, then opens level
// 2. Uses UGameplayStatics::OpenLevelBySoftObjectPtr for level loading

#include "Widgets/W_LoadingScreen.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

UW_LoadingScreen::UW_LoadingScreen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LoadingScreenAsset(nullptr)
	, AdditionalWaitDuration(0.0)
	, AutoRemove(false)
	, FadeIn(nullptr)
	, FadeOut(nullptr)
	, LoadingScreenImage(nullptr)
	, TipTitleText(nullptr)
	, TipDescription(nullptr)
	, bPendingAbsolute(true)
{
}

void UW_LoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Play fade in animation on construct
	if (FadeIn)
	{
		PlayAnimationForward(FadeIn);
		UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::NativeConstruct - Playing FadeIn animation"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::NativeConstruct"));
}

void UW_LoadingScreen::NativeDestruct()
{
	// Clear any pending timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AdditionalWaitTimerHandle);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::NativeDestruct"));
}

void UW_LoadingScreen::CacheWidgetReferences()
{
	// Widget bindings should be automatically resolved via meta = (BindWidgetOptional)
	// Log status for debugging
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::CacheWidgetReferences - FadeIn: %s, FadeOut: %s, Image: %s"),
		FadeIn ? TEXT("Bound") : TEXT("Missing"),
		FadeOut ? TEXT("Bound") : TEXT("Missing"),
		LoadingScreenImage ? TEXT("Bound") : TEXT("Missing"));
}

void UW_LoadingScreen::EventFadeOutAndNotifyListeners_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventFadeOutAndNotifyListeners_Implementation"));

	// Play fade out animation with callback
	if (FadeOut)
	{
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UW_LoadingScreen::OnFadeOutFinished_NotifyListeners);
		BindToAnimationFinished(FadeOut, AnimFinishedDelegate);
		PlayAnimationForward(FadeOut);
	}
	else
	{
		// No animation, notify immediately
		OnFadeOutFinished_NotifyListeners();
	}
}

void UW_LoadingScreen::OnFadeOutFinished_NotifyListeners()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::OnFadeOutFinished_NotifyListeners - Broadcasting OnLoadingScreenFinished"));
	OnLoadingScreenFinished.Broadcast();

	if (AutoRemove)
	{
		RemoveFromParent();
	}
}

void UW_LoadingScreen::EventFadeOutAndOpenLevelByName_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventFadeOutAndOpenLevelByName_Implementation"));

	// Play fade out animation with callback
	if (FadeOut)
	{
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UW_LoadingScreen::OnFadeOutFinished_OpenLevelByName);
		BindToAnimationFinished(FadeOut, AnimFinishedDelegate);
		PlayAnimationForward(FadeOut);
	}
	else
	{
		// No animation, open level immediately
		OnFadeOutFinished_OpenLevelByName();
	}
}

void UW_LoadingScreen::OnFadeOutFinished_OpenLevelByName()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::OnFadeOutFinished_OpenLevelByName - Opening level: %s"), *PendingLevelName.ToString());

	// Use GameplayStatics to open level by name
	UGameplayStatics::OpenLevel(this, PendingLevelName, bPendingAbsolute, PendingOptions);
}

void UW_LoadingScreen::EventFadeOutAndOpenLevelByRef_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventFadeOutAndOpenLevelByRef_Implementation"));

	// Play fade out animation with callback
	if (FadeOut)
	{
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UW_LoadingScreen::OnFadeOutFinished_OpenLevelByRef);
		BindToAnimationFinished(FadeOut, AnimFinishedDelegate);
		PlayAnimationForward(FadeOut);
	}
	else
	{
		// No animation, open level immediately
		OnFadeOutFinished_OpenLevelByRef();
	}
}

void UW_LoadingScreen::OnFadeOutFinished_OpenLevelByRef()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::OnFadeOutFinished_OpenLevelByRef - Opening level by reference"));

	// Use GameplayStatics to open level by soft object pointer
	if (!PendingLevelRef.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, PendingLevelRef, bPendingAbsolute, PendingOptions);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_LoadingScreen::OnFadeOutFinished_OpenLevelByRef - PendingLevelRef is null!"));
	}
}

void UW_LoadingScreen::EventOpenLevelByNameAndFadeOut_Implementation(FName LevelName, bool bAbsolute, const FString& Options)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventOpenLevelByNameAndFadeOut_Implementation - Level: %s, bAbsolute: %d"),
		*LevelName.ToString(), bAbsolute);

	// Store parameters for later use in callback
	PendingLevelName = LevelName;
	bPendingAbsolute = bAbsolute;
	PendingOptions = Options;

	// Check if we need to wait before fading
	if (AdditionalWaitDuration > 0.0)
	{
		// Set timer to start fade after additional wait
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				AdditionalWaitTimerHandle,
				FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					EventFadeOutAndOpenLevelByName();
				}),
				static_cast<float>(AdditionalWaitDuration),
				false
			);
			UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventOpenLevelByNameAndFadeOut - Waiting %.2f seconds before fade"), AdditionalWaitDuration);
		}
	}
	else
	{
		// Fade out immediately
		EventFadeOutAndOpenLevelByName();
	}
}

void UW_LoadingScreen::EventOpenLevelByReferenceAndFadeOut_Implementation(const TSoftObjectPtr<UWorld>& Level, bool bAbsolute, const FString& Options)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventOpenLevelByReferenceAndFadeOut_Implementation - bAbsolute: %d"), bAbsolute);

	// Store parameters for later use in callback
	PendingLevelRef = Level;
	bPendingAbsolute = bAbsolute;
	PendingOptions = Options;

	// Check if we need to wait before fading
	if (AdditionalWaitDuration > 0.0)
	{
		// Set timer to start fade after additional wait
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				AdditionalWaitTimerHandle,
				FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					EventFadeOutAndOpenLevelByRef();
				}),
				static_cast<float>(AdditionalWaitDuration),
				false
			);
			UE_LOG(LogTemp, Log, TEXT("UW_LoadingScreen::EventOpenLevelByReferenceAndFadeOut - Waiting %.2f seconds before fade"), AdditionalWaitDuration);
		}
	}
	else
	{
		// Fade out immediately
		EventFadeOutAndOpenLevelByRef();
	}
}
