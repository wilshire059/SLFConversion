// W_TargetExecutionIndicator.h
// C++ Widget class for W_TargetExecutionIndicator
//
// 20-PASS VALIDATION: 2026-01-15 - Full implementation matching bp_only
// Source: BlueprintDNA_v2/WidgetBlueprint/W_TargetExecutionIndicator.json
// Parent: UUserWidget
// Interface: ISLFExecutionIndicatorInterface
// Variables: 1 (ExecuteLockIcon) | Functions: 1 (ToggleExecutionIcon) | Animations: 1 (FadeExecuteIcon)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Interfaces/SLFExecutionIndicatorInterface.h"
#include "W_TargetExecutionIndicator.generated.h"

/**
 * W_TargetExecutionIndicator
 * Shows a visual indicator (dot/icon) above poise-broken enemies to signal
 * that the player can perform an execution attack.
 *
 * bp_only flow (W_TargetExecutionIndicator.json):
 * - ToggleExecutionIcon(Visible?)
 *   - Branch on Visible?
 *     - TRUE:  SetVisibility(ExecuteLockIcon, Visible) → PlayAnimationForward(FadeExecuteIcon)
 *     - FALSE: PlayAnimation(FadeExecuteIcon, Reverse) → On Finished: SetVisibility(ExecuteLockIcon, Collapsed)
 */
UCLASS()
class SLFCONVERSION_API UW_TargetExecutionIndicator : public UUserWidget, public ISLFExecutionIndicatorInterface
{
	GENERATED_BODY()

public:
	UW_TargetExecutionIndicator(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (from WidgetTree)
	// ═══════════════════════════════════════════════════════════════════════

	/** The lock icon image that shows when execution is available (bp_only: ExecuteLockIcon) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UImage> ExecuteLockIcon;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATIONS (from JSON Animations array)
	// ═══════════════════════════════════════════════════════════════════════

	/** Fade animation for the execute icon (0.5s duration in bp_only) */
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> FadeExecuteIcon;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE IMPLEMENTATION: ISLFExecutionIndicatorInterface
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Toggle execution icon visibility with animation
	 * bp_only flow:
	 * - Visible=true:  Show icon immediately, play fade-in animation
	 * - Visible=false: Play fade-out animation (reverse), hide when finished
	 */
	virtual void ToggleExecutionIcon_Implementation(bool bVisible) override;

protected:
	// Cache references
	void CacheWidgetReferences();

	/** Called when fade-out animation finishes - hides the icon */
	void OnFadeOutAnimationFinished(UUMGSequencePlayer& Player);
};
