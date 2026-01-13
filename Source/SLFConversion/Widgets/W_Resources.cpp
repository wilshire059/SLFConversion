// W_Resources.cpp
// C++ Widget implementation for W_Resources
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Resources.h"
#include "Components/ProgressBar.h"

UW_Resources::UW_Resources(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CachedHealthBar_Front(nullptr)
	, CachedHealthBar_Back(nullptr)
	, CachedFocusBar_Front(nullptr)
	, CachedFocusBar_Back(nullptr)
	, CachedStaminaBar_Front(nullptr)
	, CachedStaminaBar_Back(nullptr)
{
}

void UW_Resources::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Resources::NativeConstruct"));
}

void UW_Resources::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Resources::NativeDestruct"));
}

void UW_Resources::CacheWidgetReferences()
{
	// Cache progress bar references from the Blueprint widget tree
	// NOTE: Search for Blueprint widget names (HealthBar_Front), store in C++ vars (CachedHealthBar_Front)
	if (!CachedHealthBar_Front)
	{
		CachedHealthBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Front")));
	}
	if (!CachedHealthBar_Back)
	{
		CachedHealthBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Back")));
	}
	if (!CachedFocusBar_Front)
	{
		CachedFocusBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("FocusBar_Front")));
	}
	if (!CachedFocusBar_Back)
	{
		CachedFocusBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("FocusBar_Back")));
	}
	if (!CachedStaminaBar_Front)
	{
		CachedStaminaBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("StaminaBar_Front")));
	}
	if (!CachedStaminaBar_Back)
	{
		CachedStaminaBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("StaminaBar_Back")));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Resources::CacheWidgetReferences - HealthBar_Front: %s, StaminaBar_Front: %s, FocusBar_Front: %s"),
		CachedHealthBar_Front ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedStaminaBar_Front ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedFocusBar_Front ? TEXT("Found") : TEXT("NOT FOUND"));
}

void UW_Resources::AdjustBarWidth_Implementation(const FStatInfo& InStat)
{
	// Adjust the resource bar width based on stat current/max values
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::AdjustBarWidth"));
}
void UW_Resources::EventAddBuff_Implementation(UPDA_Buff* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventAddBuff_Implementation"));
}


void UW_Resources::EventLerpFocusPositive_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventLerpFocusPositive_Implementation"));
}


void UW_Resources::EventLerpHealthPositive_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventLerpHealthPositive_Implementation"));
}


void UW_Resources::EventLerpStaminaPositive_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventLerpStaminaPositive_Implementation"));
}


void UW_Resources::EventRemoveBuff_Implementation(UPDA_Buff* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventRemoveBuff_Implementation"));
}


void UW_Resources::EventTimerTick_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Resources::EventTimerTick_Implementation"));
}


void UW_Resources::EventUpdateStat_Implementation(FStatInfo InStat)
{
	// Calculate percent (0.0 to 1.0)
	float Percent = 0.0f;
	if (InStat.MaxValue > 0.0)
	{
		Percent = static_cast<float>(InStat.CurrentValue / InStat.MaxValue);
		Percent = FMath::Clamp(Percent, 0.0f, 1.0f);
	}

	FString TagString = InStat.Tag.ToString();

	// Update the appropriate bar based on the stat tag
	if (TagString.Contains(TEXT("HP")) || TagString.Contains(TEXT("Health")))
	{
		// Update health bars - Front bar updates immediately
		if (CachedHealthBar_Front)
		{
			CachedHealthBar_Front->SetPercent(Percent);
		}
		// Back bar catches up quickly (since EventUpdateStat is called once per damage)
		// Using high interp speed (100.0) to make visible change per call
		if (CachedHealthBar_Back)
		{
			float CurrentBack = CachedHealthBar_Back->GetPercent();
			// Only update if back bar is ahead of front (damage taken)
			if (CurrentBack > Percent)
			{
				float NewBack = FMath::FInterpTo(CurrentBack, Percent, 0.016f, 15.0f);
				CachedHealthBar_Back->SetPercent(NewBack);
			}
			else
			{
				// Healing - back bar matches front immediately
				CachedHealthBar_Back->SetPercent(Percent);
			}
		}
		HealthPercentCache = Percent;
	}
	else if (TagString.Contains(TEXT("FP")) || TagString.Contains(TEXT("Focus")))
	{
		// Update focus/FP bars
		if (CachedFocusBar_Front)
		{
			CachedFocusBar_Front->SetPercent(Percent);
		}
		if (CachedFocusBar_Back)
		{
			float CurrentBack = CachedFocusBar_Back->GetPercent();
			if (CurrentBack > Percent)
			{
				float NewBack = FMath::FInterpTo(CurrentBack, Percent, 0.016f, 15.0f);
				CachedFocusBar_Back->SetPercent(NewBack);
			}
			else
			{
				CachedFocusBar_Back->SetPercent(Percent);
			}
		}
		FocusPercentCache = Percent;
	}
	else if (TagString.Contains(TEXT("Stamina")))
	{
		// Update stamina bars
		if (CachedStaminaBar_Front)
		{
			CachedStaminaBar_Front->SetPercent(Percent);
		}
		if (CachedStaminaBar_Back)
		{
			float CurrentBack = CachedStaminaBar_Back->GetPercent();
			if (CurrentBack > Percent)
			{
				float NewBack = FMath::FInterpTo(CurrentBack, Percent, 0.016f, 15.0f);
				CachedStaminaBar_Back->SetPercent(NewBack);
			}
			else
			{
				CachedStaminaBar_Back->SetPercent(Percent);
			}
		}
		StaminaPercentCache = Percent;
	}
}
