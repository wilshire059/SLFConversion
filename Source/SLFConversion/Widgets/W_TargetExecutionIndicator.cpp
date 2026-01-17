// W_TargetExecutionIndicator.cpp
// C++ Widget implementation for W_TargetExecutionIndicator
//
// 20-PASS VALIDATION: 2026-01-15 - Full implementation matching bp_only
// Source: BlueprintDNA_v2/WidgetBlueprint/W_TargetExecutionIndicator.json

#include "Widgets/W_TargetExecutionIndicator.h"
#include "Components/Image.h"
#include "Animation/UMGSequencePlayer.h"

UW_TargetExecutionIndicator::UW_TargetExecutionIndicator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// ExecuteLockIcon and FadeExecuteIcon are bound via BindWidget/BindWidgetAnim
}

void UW_TargetExecutionIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] NativeConstruct - ExecuteLockIcon=%s, FadeExecuteIcon=%s"),
		ExecuteLockIcon ? TEXT("valid") : TEXT("null"),
		FadeExecuteIcon ? TEXT("valid") : TEXT("null"));

	// NOTE: Do NOT set ExecuteLockIcon to Collapsed here!
	// ToggleExecutionIcon may be called BEFORE NativeConstruct completes (race condition).
	// The Blueprint widget tree should have ExecuteLockIcon starting as Collapsed,
	// and we control visibility via ToggleExecutionIcon.
}

void UW_TargetExecutionIndicator::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] NativeDestruct"));
}

void UW_TargetExecutionIndicator::CacheWidgetReferences()
{
	// BindWidget meta should auto-populate ExecuteLockIcon and FadeExecuteIcon
	// This is just for logging/verification
	if (!ExecuteLockIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] ExecuteLockIcon NOT bound - check Blueprint widget tree"));
	}

	if (!FadeExecuteIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] FadeExecuteIcon animation NOT bound - check Blueprint animations"));
	}
}

void UW_TargetExecutionIndicator::ToggleExecutionIcon_Implementation(bool bVisible)
{
	// ═══════════════════════════════════════════════════════════════════════════════
	// bp_only flow (W_TargetExecutionIndicator.json lines 636-741, 798-954):
	// Event ToggleExecutionIcon(Visible?)
	//   → Branch on Visible?
	//     → TRUE:  SetVisibility(ExecuteLockIcon, Visible) → PlayAnimationForward(FadeExecuteIcon)
	//     → FALSE: PlayAnimation(FadeExecuteIcon, Reverse) → On Finished: SetVisibility(ExecuteLockIcon, Collapsed)
	// ═══════════════════════════════════════════════════════════════════════════════

	UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] ToggleExecutionIcon - bVisible=%s"),
		bVisible ? TEXT("true") : TEXT("false"));

	if (bVisible)
	{
		// ═══════════════════════════════════════════════════════════════════════════════
		// SHOW: SetVisibility(Visible) → PlayAnimationForward(FadeExecuteIcon)
		// bp_only lines 251-355 (SetVisibility Visible) and 798-954 (PlayAnimationForward)
		// ═══════════════════════════════════════════════════════════════════════════════
		if (ExecuteLockIcon)
		{
			ExecuteLockIcon->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] ExecuteLockIcon set to Visible"));
		}

		if (FadeExecuteIcon)
		{
			// PlayAnimationForward with default speed (1.0)
			PlayAnimationForward(FadeExecuteIcon, 1.0f, false);
			UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] Playing FadeExecuteIcon animation FORWARD"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] FadeExecuteIcon is null - cannot play animation"));
		}
	}
	else
	{
		// ═══════════════════════════════════════════════════════════════════════════════
		// HIDE: PlayAnimation(FadeExecuteIcon, Reverse) → On Finished: SetVisibility(Collapsed)
		// bp_only lines 414-635 (PlayAnimation Reverse) and 1046-1145 (SetVisibility Collapsed)
		// ═══════════════════════════════════════════════════════════════════════════════
		if (FadeExecuteIcon)
		{
			// Play animation in reverse mode
			// bp_only uses "Play Animation with Finished event (legacy)" with PlayMode = Reverse
			// We need to bind to animation finished to then collapse the widget

			// Play in reverse - we'll hide the icon after animation finishes
			UUMGSequencePlayer* Player = PlayAnimation(FadeExecuteIcon, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f, false);

			if (Player)
			{
				// Bind to OnSequenceFinishedPlaying to hide icon when animation completes
				Player->OnSequenceFinishedPlaying().AddUObject(this, &UW_TargetExecutionIndicator::OnFadeOutAnimationFinished);
				UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] Playing FadeExecuteIcon animation REVERSE (will hide on finish)"));
			}
			else
			{
				// If animation fails to play, just hide immediately
				if (ExecuteLockIcon)
				{
					ExecuteLockIcon->SetVisibility(ESlateVisibility::Collapsed);
				}
				UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] PlayAnimation returned null - hiding icon immediately"));
			}
		}
		else
		{
			// No animation - just hide immediately
			if (ExecuteLockIcon)
			{
				ExecuteLockIcon->SetVisibility(ESlateVisibility::Collapsed);
			}
			UE_LOG(LogTemp, Warning, TEXT("[W_TargetExecutionIndicator] FadeExecuteIcon is null - hiding icon immediately"));
		}
	}
}

void UW_TargetExecutionIndicator::OnFadeOutAnimationFinished(UUMGSequencePlayer& Player)
{
	// bp_only flow (lines 1046-1145):
	// On "Finished" output of PlayAnimation → SetVisibility(ExecuteLockIcon, Collapsed)
	if (ExecuteLockIcon)
	{
		ExecuteLockIcon->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_TargetExecutionIndicator] Fade-out finished - ExecuteLockIcon set to Collapsed"));
	}
}
