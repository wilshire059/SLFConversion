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
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/StatManagerComponent.h"

UW_Boss_Healthbar::UW_Boss_Healthbar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HealthPercentCache = 1.0;
	TotalDamage = 0.0;
	CachedBossActor = nullptr;
	CachedBossHPStat = nullptr;
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
	// Unbind from HP stat
	if (IsValid(CachedBossHPStat))
	{
		CachedBossHPStat->OnStatUpdated.RemoveDynamic(this, &UW_Boss_Healthbar::EventOnBossHealthUpdated);
		CachedBossHPStat = nullptr;
	}

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
	// Unbind from HP stat
	if (IsValid(CachedBossHPStat))
	{
		CachedBossHPStat->OnStatUpdated.RemoveDynamic(this, &UW_Boss_Healthbar::EventOnBossHealthUpdated);
		CachedBossHPStat = nullptr;
		UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] Unbound from HP stat"));
	}

	// Hide the boss health bar widget
	SetVisibility(ESlateVisibility::Collapsed);

	// Clear timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TickTimer);
		GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
	}

	// Clear cached references
	CachedBossActor = nullptr;
}

void UW_Boss_Healthbar::EventHideDamage_Implementation()
{
	// Hide damage text
	if (IsValid(DamageText))
	{
		DamageText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Reset damage counter
	TotalDamage = 0.0;
}

void UW_Boss_Healthbar::EventInitializeBossBar_Implementation(const FText& InName, AActor* BossActor)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] EventInitializeBossBar - Name: %s, BossActor: %s"),
		*InName.ToString(), BossActor ? *BossActor->GetName() : TEXT("null"));

	// Cache boss actor
	CachedBossActor = BossActor;

	// Unbind from previous HP stat if any
	if (IsValid(CachedBossHPStat))
	{
		CachedBossHPStat->OnStatUpdated.RemoveDynamic(this, &UW_Boss_Healthbar::EventOnBossHealthUpdated);
		CachedBossHPStat = nullptr;
	}

	// Bind to boss's HP stat for health updates
	if (IsValid(BossActor) && BossActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatManagerComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(BossActor, StatManagerComp);

		if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatManagerComp))
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] Found StatManager on boss"));

			// Get HP stat - tag is "SoulslikeFramework.Stat.Secondary.HP"
			FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);
			if (HPTag.IsValid())
			{
				UObject* HPStatObj = nullptr;
				FStatInfo HPStatInfo;
				if (StatManager->GetStat(HPTag, HPStatObj, HPStatInfo))
				{
					CachedBossHPStat = Cast<USLFStatBase>(HPStatObj);
					if (IsValid(CachedBossHPStat))
					{
						// Bind to OnStatUpdated delegate
						CachedBossHPStat->OnStatUpdated.AddDynamic(this, &UW_Boss_Healthbar::EventOnBossHealthUpdated);
						UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] Bound to HP stat OnStatUpdated. HP: %.0f/%.0f"),
							HPStatInfo.CurrentValue, HPStatInfo.MaxValue);

						// Set initial health percent
						HealthPercentCache = (HPStatInfo.MaxValue > 0.0) ? (HPStatInfo.CurrentValue / HPStatInfo.MaxValue) : 1.0;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] HP stat object cast failed!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] Could not get HP stat from StatManager!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] HP GameplayTag not valid!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] StatManager not found or wrong type on boss!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] BossActor does not implement BPI_GenericCharacter!"));
	}

	// Set boss name text
	if (IsValid(NameText))
	{
		NameText->SetText(InName);
		UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] Set NameText to: %s"), *InName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] NameText widget is NULL - boss name will not display!"));
	}

	// Reset health displays (use actual percent if we have it)
	if (IsValid(HealthBar_Front))
	{
		HealthBar_Front->SetPercent(static_cast<float>(HealthPercentCache));
	}
	if (IsValid(HealthBar_Back))
	{
		HealthBar_Back->SetPercent(static_cast<float>(HealthPercentCache));
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
		UE_LOG(LogTemp, Warning, TEXT("[W_Boss_Healthbar] EventOnBossHealthUpdated - UpdatedStat is invalid!"));
		return;
	}

	// Get current health percent from stat
	const FStatInfo& StatInfo = UpdatedStat->GetStatInfo();
	double NewPercent = (StatInfo.MaxValue > 0.0) ? (StatInfo.CurrentValue / StatInfo.MaxValue) : 0.0;
	NewPercent = FMath::Clamp(NewPercent, 0.0, 1.0);

	UE_LOG(LogTemp, Log, TEXT("[W_Boss_Healthbar] EventOnBossHealthUpdated - HP: %.0f/%.0f (%.1f%%), Change: %.0f"),
		StatInfo.CurrentValue, StatInfo.MaxValue, NewPercent * 100.0, Change);

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
		if (IsValid(DamageText))
		{
			DamageText->SetText(FText::AsNumber(static_cast<int32>(TotalDamage)));
			DamageText->SetVisibility(ESlateVisibility::Visible);
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
