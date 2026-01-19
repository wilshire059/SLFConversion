// W_Resources.cpp
// C++ Widget implementation for W_Resources
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Resources.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"

UW_Resources::UW_Resources(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BaseWidthHp(175.0)       // Exact bp_only value
	, BaseWidthFp(50.0)        // Exact bp_only value
	, BaseWidthStamina(200.0)  // Exact bp_only value
	, BaseHeightHp(0.0)       // 0 = don't override height
	, BaseHeightFp(0.0)
	, BaseHeightStamina(0.0)
	, CachedHealthBar_Front(nullptr)
	, CachedHealthBar_Back(nullptr)
	, CachedFocusBar_Front(nullptr)
	, CachedFocusBar_Back(nullptr)
	, CachedStaminaBar_Front(nullptr)
	, CachedStaminaBar_Back(nullptr)
	, CachedHealthbarSizer(nullptr)
	, CachedFocusbarSizer(nullptr)
	, CachedStaminabarSizer(nullptr)
{
}

void UW_Resources::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Cache widget references early
	CacheWidgetReferences();

	// NOTE: Do NOT set width overrides here!
	// The Blueprint's UMG Designer already defines the correct initial SizeBox dimensions.
	// We only modify widths when AdjustBarWidth is called from EventUpdateStat at runtime.
	// Setting overrides here would clobber the Blueprint's visual design.

	UE_LOG(LogTemp, Log, TEXT("UW_Resources::NativePreConstruct"));
}

void UW_Resources::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references (may already be cached from PreConstruct)
	CacheWidgetReferences();

	// CRITICAL: Blueprint CDO overrides C++ constructor defaults!
	// Force exact bp_only values (extracted from Blueprint JSON)
	// Formula: BaseWidth * (MaxValue / ScalingFactor)
	// EACH BAR HAS ITS OWN SCALING FACTOR!
	constexpr double CorrectBaseWidthHp = 175.0;
	constexpr double CorrectBaseWidthFp = 50.0;
	constexpr double CorrectBaseWidthStamina = 200.0;
	constexpr double ScalingFactorHp = 800.0;
	constexpr double ScalingFactorFp = 50.0;
	constexpr double ScalingFactorStamina = 70.0;

	// Override the member variables with correct values
	BaseWidthHp = CorrectBaseWidthHp;
	BaseWidthFp = CorrectBaseWidthFp;
	BaseWidthStamina = CorrectBaseWidthStamina;

	// Set initial widths using the formula with default MaxValues
	constexpr double DefaultMaxHp = 760.0;
	constexpr double DefaultMaxFp = 100.0;
	constexpr double DefaultMaxStamina = 100.0;

	double InitialWidthHp = BaseWidthHp * (DefaultMaxHp / ScalingFactorHp);
	double InitialWidthFp = BaseWidthFp * (DefaultMaxFp / ScalingFactorFp);
	double InitialWidthStamina = BaseWidthStamina * (DefaultMaxStamina / ScalingFactorStamina);

	if (CachedHealthbarSizer)
	{
		CachedHealthbarSizer->SetWidthOverride(static_cast<float>(InitialWidthHp));
	}
	if (CachedFocusbarSizer)
	{
		CachedFocusbarSizer->SetWidthOverride(static_cast<float>(InitialWidthFp));
	}
	if (CachedStaminabarSizer)
	{
		CachedStaminabarSizer->SetWidthOverride(static_cast<float>(InitialWidthStamina));
	}

	// Hide the slider widgets that create white lines
	HideBarSeparators();

	UE_LOG(LogTemp, Log, TEXT("[W_Resources] NativeConstruct - Forced BaseWidths: HP=%.1f, FP=%.1f, Stamina=%.1f -> Widths: HP=%.0f, FP=%.0f, Stamina=%.0f"),
		BaseWidthHp, BaseWidthFp, BaseWidthStamina, InitialWidthHp, InitialWidthFp, InitialWidthStamina);
}

void UW_Resources::HideBarSeparators()
{
	// Hide the Slider widgets - their thumb/handle creates white lines at the fill position
	TArray<FString> SliderNames = {
		TEXT("HealthSlider"), TEXT("FocusSlider"), TEXT("StaminaSlider")
	};

	for (const FString& Name : SliderNames)
	{
		if (UWidget* Widget = GetWidgetFromName(*Name))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_Resources] Hid slider widget: %s"), *Name);
		}
	}
}

void UW_Resources::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Resources::NativeDestruct"));
}

void UW_Resources::CacheWidgetReferences()
{
	// Cache progress bar references from the Blueprint widget tree
	// CRITICAL: Blueprint uses lowercase 'b' for Focusbar and Staminabar (not FocusBar, StaminaBar)
	if (!CachedHealthBar_Front)
	{
		CachedHealthBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Front")));
	}
	if (!CachedHealthBar_Back)
	{
		CachedHealthBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar_Back")));
	}
	// NOTE: Blueprint names are Focusbar_Front, Focusbar_Back (lowercase 'b')
	if (!CachedFocusBar_Front)
	{
		CachedFocusBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("Focusbar_Front")));
	}
	if (!CachedFocusBar_Back)
	{
		CachedFocusBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("Focusbar_Back")));
	}
	// NOTE: Blueprint names are Staminabar_Front, Staminabar_Back (lowercase 'b')
	if (!CachedStaminaBar_Front)
	{
		CachedStaminaBar_Front = Cast<UProgressBar>(GetWidgetFromName(TEXT("Staminabar_Front")));
	}
	if (!CachedStaminaBar_Back)
	{
		CachedStaminaBar_Back = Cast<UProgressBar>(GetWidgetFromName(TEXT("Staminabar_Back")));
	}

	// Cache SizeBox widgets that control bar widths
	if (!CachedHealthbarSizer)
	{
		CachedHealthbarSizer = Cast<USizeBox>(GetWidgetFromName(TEXT("HealthbarSizer")));
	}
	if (!CachedFocusbarSizer)
	{
		CachedFocusbarSizer = Cast<USizeBox>(GetWidgetFromName(TEXT("FocusbarSizer")));
	}
	if (!CachedStaminabarSizer)
	{
		CachedStaminabarSizer = Cast<USizeBox>(GetWidgetFromName(TEXT("StaminabarSizer")));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Resources] CacheWidgetReferences - HealthBar_Front: %s, Focusbar_Front: %s, Staminabar_Front: %s"),
		CachedHealthBar_Front ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedFocusBar_Front ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedStaminaBar_Front ? TEXT("Found") : TEXT("NOT FOUND"));
	UE_LOG(LogTemp, Log, TEXT("[W_Resources] CacheWidgetReferences - HealthbarSizer: %s, FocusbarSizer: %s, StaminabarSizer: %s"),
		CachedHealthbarSizer ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedFocusbarSizer ? TEXT("Found") : TEXT("NOT FOUND"),
		CachedStaminabarSizer ? TEXT("Found") : TEXT("NOT FOUND"));
}

void UW_Resources::AdjustBarWidth_Implementation(FStatInfo Stat)
{
	// bp_only formula: BaseWidth * (MaxValue / ScalingFactor)
	// EACH BAR HAS ITS OWN SCALING FACTOR (extracted from bp_only JSON)
	constexpr double ScalingFactorHp = 800.0;
	constexpr double ScalingFactorFp = 50.0;
	constexpr double ScalingFactorStamina = 70.0;

	FString TagString = Stat.Tag.ToString();
	double MaxValue = Stat.MaxValue;

	if (TagString.Contains(TEXT("HP")) || TagString.Contains(TEXT("Health")))
	{
		if (CachedHealthbarSizer)
		{
			double Width = BaseWidthHp * (MaxValue / ScalingFactorHp);
			CachedHealthbarSizer->SetWidthOverride(static_cast<float>(Width));
		}
	}
	else if (TagString.Contains(TEXT("FP")) || TagString.Contains(TEXT("Focus")))
	{
		if (CachedFocusbarSizer)
		{
			double Width = BaseWidthFp * (MaxValue / ScalingFactorFp);
			CachedFocusbarSizer->SetWidthOverride(static_cast<float>(Width));
		}
	}
	else if (TagString.Contains(TEXT("Stamina")))
	{
		if (CachedStaminabarSizer)
		{
			double Width = BaseWidthStamina * (MaxValue / ScalingFactorStamina);
			CachedStaminabarSizer->SetWidthOverride(static_cast<float>(Width));
		}
	}
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
	// FIRST: Adjust bar WIDTH based on MaxValue (bars scale with max stat value)
	// This must happen before setting percent so the bar size is correct
	AdjustBarWidth(InStat);

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
