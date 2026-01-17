// W_Boss_Healthbar.cpp
// C++ Widget implementation for W_Boss_Healthbar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Boss_Healthbar.h"
#include "Blueprints/SLFStatBase.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

UW_Boss_Healthbar::UW_Boss_Healthbar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HealthPercentCache = 1.0;
	TotalDamage = 0.0;
	CachedBossActor = nullptr;
}

void UW_Boss_Healthbar::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Initialize progress bars
	if (IsValid(HealthBar_Front))
	{
		HealthBar_Front->SetPercent(1.0f);
	}
	if (IsValid(HealthBar_Back))
	{
		HealthBar_Back->SetPercent(1.0f);
	}
}

void UW_Boss_Healthbar::NativeDestruct()
{
	// Clear timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TickTimer);
		GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
	}

	Super::NativeDestruct();
}

void UW_Boss_Healthbar::CacheWidgetReferences()
{
	// Widget bindings are handled automatically via meta = (BindWidget)
	// Additional lookups can be done here if needed
}

void UW_Boss_Healthbar::EventHideBossBar_Implementation()
{
	// Hide the boss health bar widget
	SetVisibility(ESlateVisibility::Collapsed);

	// Clear timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TickTimer);
		GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
	}
}

void UW_Boss_Healthbar::EventHideDamage_Implementation()
{
	// Hide damage text
	if (IsValid(Txt_DamageAmount))
	{
		Txt_DamageAmount->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Reset damage counter
	TotalDamage = 0.0;
}

void UW_Boss_Healthbar::EventInitializeBossBar_Implementation(const FText& InName, AActor* BossActor)
{
	// Cache boss actor
	CachedBossActor = BossActor;

	// Set boss name text
	if (IsValid(Txt_BossName))
	{
		Txt_BossName->SetText(InName);
	}

	// Reset health displays
	HealthPercentCache = 1.0;
	if (IsValid(HealthBar_Front))
	{
		HealthBar_Front->SetPercent(1.0f);
	}
	if (IsValid(HealthBar_Back))
	{
		HealthBar_Back->SetPercent(1.0f);
	}

	// Show widget
	SetVisibility(ESlateVisibility::Visible);

	// Start timer tick for smooth updates
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TickTimer,
			FTimerDelegate::CreateUObject(this, &UW_Boss_Healthbar::EventTimerTick),
			0.016f, // ~60fps
			true
		);
	}
}

void UW_Boss_Healthbar::EventLerpHealthPositive_Implementation()
{
	// Lerp the back health bar to match front (when healing)
	if (!IsValid(HealthBar_Front) || !IsValid(HealthBar_Back))
	{
		return;
	}

	float CurrentFront = HealthBar_Front->GetPercent();
	float CurrentBack = HealthBar_Back->GetPercent();

	if (CurrentBack < CurrentFront)
	{
		// Lerp back bar up to match front
		float NewBack = UKismetMathLibrary::FInterpTo(CurrentBack, CurrentFront, 0.016f, 5.0f);
		HealthBar_Back->SetPercent(NewBack);
	}
}

void UW_Boss_Healthbar::EventOnBossHealthUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType)
{
	if (!IsValid(UpdatedStat))
	{
		return;
	}

	// Get current health percent from stat
	const FStatInfo& StatInfo = UpdatedStat->GetStatInfo();
	double NewPercent = (StatInfo.MaxValue > 0.0) ? (StatInfo.CurrentValue / StatInfo.MaxValue) : 0.0;
	NewPercent = FMath::Clamp(NewPercent, 0.0, 1.0);

	// Update front health bar immediately
	if (IsValid(HealthBar_Front))
	{
		HealthBar_Front->SetPercent(static_cast<float>(NewPercent));
	}

	// If taking damage (negative change)
	if (Change < 0.0)
	{
		// Accumulate damage for display
		TotalDamage += FMath::Abs(Change);

		// Update damage text
		if (IsValid(Txt_DamageAmount))
		{
			Txt_DamageAmount->SetText(FText::AsNumber(static_cast<int32>(TotalDamage)));
			Txt_DamageAmount->SetVisibility(ESlateVisibility::Visible);
		}

		// Reset damage timer (hide damage text after delay)
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				DamageTimer,
				FTimerDelegate::CreateUObject(this, &UW_Boss_Healthbar::EventHideDamage),
				2.0f, // 2 second delay
				false
			);
		}
	}

	// Cache new health percent
	HealthPercentCache = NewPercent;
}

void UW_Boss_Healthbar::EventTimerTick_Implementation()
{
	// Lerp back health bar to match front (for damage decay effect)
	if (!IsValid(HealthBar_Front) || !IsValid(HealthBar_Back))
	{
		return;
	}

	float CurrentFront = HealthBar_Front->GetPercent();
	float CurrentBack = HealthBar_Back->GetPercent();

	// Only lerp if back is higher than front (took damage)
	if (CurrentBack > CurrentFront)
	{
		float NewBack = UKismetMathLibrary::FInterpTo(CurrentBack, CurrentFront, 0.016f, 2.0f);
		HealthBar_Back->SetPercent(NewBack);
	}
}
