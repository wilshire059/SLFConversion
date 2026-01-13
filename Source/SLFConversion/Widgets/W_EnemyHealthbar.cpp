// W_EnemyHealthbar.cpp
// C++ Widget implementation for W_EnemyHealthbar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_EnemyHealthbar.h"
#include "Components/ProgressBar.h"

UW_EnemyHealthbar::UW_EnemyHealthbar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CachedHealthBar_Front(nullptr)
	, CachedHealthBar_Back(nullptr)
{
}

void UW_EnemyHealthbar::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_EnemyHealthbar::NativeConstruct"));
}

void UW_EnemyHealthbar::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_EnemyHealthbar::NativeDestruct"));
}

void UW_EnemyHealthbar::CacheWidgetReferences()
{
	// Cache progress bar references from the Blueprint widget tree
	if (!CachedHealthBar_Front)
	{
		CachedHealthBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Front")));
	}
	if (!CachedHealthBar_Back)
	{
		CachedHealthBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Back")));
	}

	// Try alternative names if not found
	if (!CachedHealthBar_Front)
	{
		CachedHealthBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));
	}
	if (!CachedHealthBar_Front)
	{
		CachedHealthBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_Health")));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_EnemyHealthbar] CacheWidgetReferences - Front: %s, Back: %s"),
		CachedHealthBar_Front ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedHealthBar_Back ? TEXT("Found") : TEXT("NOT FOUND"));
}

void UW_EnemyHealthbar::UpdateEnemyHealthbar_Implementation(double CurrentHealth, double MaxHealth, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("[W_EnemyHealthbar] UpdateEnemyHealthbar - Current: %.0f, Max: %.0f, Change: %.0f"),
		CurrentHealth, MaxHealth, Change);

	// Calculate percent
	float Percent = 0.0f;
	if (MaxHealth > 0.0)
	{
		Percent = static_cast<float>(CurrentHealth / MaxHealth);
		Percent = FMath::Clamp(Percent, 0.0f, 1.0f);
	}

	// Update front bar immediately
	if (CachedHealthBar_Front)
	{
		CachedHealthBar_Front->SetPercent(Percent);
	}

	// Update back bar with lerp (shows damage taken)
	if (CachedHealthBar_Back)
	{
		float CurrentBack = CachedHealthBar_Back->GetPercent();
		if (CurrentBack > Percent)
		{
			// Damage taken - lerp back bar to catch up
			float NewBack = FMath::FInterpTo(CurrentBack, Percent, 0.016f, 50.0f);
			CachedHealthBar_Back->SetPercent(NewBack);
		}
		else
		{
			// Healing - back bar matches front immediately
			CachedHealthBar_Back->SetPercent(Percent);
		}
	}

	// Update cached percent
	HealthPercentCache = static_cast<double>(Percent);

	// Track total damage for display
	if (Change < 0)
	{
		TotalDamage += FMath::Abs(Change);
	}

	// Make sure widget is visible when health changes
	SetVisibility(ESlateVisibility::Visible);
}

void UW_EnemyHealthbar::EventHideDamage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_EnemyHealthbar::EventHideDamage_Implementation"));
}


void UW_EnemyHealthbar::EventLerpHealthPositive_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_EnemyHealthbar::EventLerpHealthPositive_Implementation"));
}


void UW_EnemyHealthbar::EventTimerTick_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_EnemyHealthbar::EventTimerTick_Implementation"));
}
