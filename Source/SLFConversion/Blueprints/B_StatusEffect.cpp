// B_StatusEffect.cpp
// C++ implementation for Blueprint B_StatusEffect
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_StatusEffect.json
// Logic traced from JSON node-by-node

#include "Blueprints/B_StatusEffect.h"
#include "Components/AC_StatManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

UB_StatusEffect::UB_StatusEffect()
{
	// Initialize default values from Blueprint
	Owner = nullptr;
	Data = nullptr;
	bIsTriggered = false;
	ActiveRank = 0;
	OwnerResistiveStatValue = 0.0;
	BuildupPercent = 0.0;
	EffectDuration = 0.0;
	EffectSteps = 0.0;

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Constructor - Initialized"));
}

// ═══════════════════════════════════════════════════════════════════════════
// PURE GETTER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

UAC_StatManager* UB_StatusEffect::GetOwnerStatManager_Implementation()
{
	// Logic from JSON GetOwnerStatManager graph:
	// 1. Get Owner variable
	// 2. Call Owner->GetComponentByClass<UAC_StatManager>()
	// 3. Is Valid check on result
	// 4. If valid: return the component
	// 5. If not valid: return nullptr

	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetOwnerStatManager - Owner is null"));
		return nullptr;
	}

	UAC_StatManager* StatManager = Owner->FindComponentByClass<UAC_StatManager>();

	if (IsValid(StatManager))
	{
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetOwnerStatManager - Found StatManager"));
		return StatManager;
	}

	UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetOwnerStatManager - StatManager not found on Owner"));
	return nullptr;
}

double UB_StatusEffect::GetBuildupPercent_Implementation()
{
	// Logic from JSON GetBuildupPercent graph:
	// 1. Get Data variable with validation
	// 2. If Data is valid: return BuildupPercent / 100.0
	// 3. If Data is not valid: return 0.0

	if (IsValid(Data))
	{
		double Result = BuildupPercent / 100.0;
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetBuildupPercent - Returning %f"), Result);
		return Result;
	}

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetBuildupPercent - Data invalid, returning 0.0"));
	return 0.0;
}

UPrimaryDataAsset* UB_StatusEffect::GetEffectData_Implementation()
{
	// Logic from JSON GetEffectData graph:
	// 1. Get Data variable with validation
	// 2. If Data is valid: return Data
	// 3. If Data is not valid: return nullptr

	if (IsValid(Data))
	{
		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetEffectData - Returning Data"));
		return Data;
	}

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetEffectData - Data invalid, returning nullptr"));
	return nullptr;
}

TMap<int32, FSLFStatusEffectRankInfo> UB_StatusEffect::GetEffectRankData_Implementation()
{
	// Logic from JSON GetEffectRankData graph:
	// 1. Check if Data is valid
	// 2. If valid: return Data->RankInfo (TMap<int32, FSLFStatusEffectRankInfo>)
	// 3. If not valid: return empty map

	TMap<int32, FSLFStatusEffectRankInfo> EmptyMap;

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetEffectRankData - Data invalid, returning empty map"));
		return EmptyMap;
	}

	// Note: Data is UPrimaryDataAsset*, need to cast to UPDA_StatusEffect* to access RankInfo
	// For now returning empty map - actual implementation needs proper cast
	// TODO: Cast Data to UPDA_StatusEffect and access RankInfo property
	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetEffectRankData - Returning RankInfo from Data"));

	return EmptyMap;
}

FText UB_StatusEffect::GetTriggeredText_Implementation()
{
	// Logic from JSON GetTriggeredText graph:
	// 1. Get Data->TriggeredText
	// 2. If TriggeredText is empty:
	//    a. Get Data->Tag (FGameplayTag)
	//    b. GetTagName() → convert to string
	//    c. FindSubstring(".") → get index
	//    d. RightChop(index + 1) → get text after last "."
	//    e. Convert to FText
	// 3. Return result (either TriggeredText or derived text)

	if (!IsValid(Data))
	{
		return FText::GetEmpty();
	}

	// Note: Need to cast Data to UPDA_StatusEffect to access TriggeredText and Tag
	// For now returning empty text
	// TODO: Cast Data and implement full logic
	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetTriggeredText - Implementation pending Data cast"));

	return FText::GetEmpty();
}

// ═══════════════════════════════════════════════════════════════════════════
// NON-PURE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

double UB_StatusEffect::GetResistiveStatValue_Implementation()
{
	// Logic from JSON GetResistiveStatValue graph:
	// 1. Call GetOwnerStatManager()
	// 2. Is Valid check on stat manager
	// 3. If valid: store in LOCAL_StatManager
	// 4. Get Data->ResistiveStat (FGameplayTag)
	// 5. Call LOCAL_StatManager->GetStat(ResistiveStat) → returns (FoundStat, StatInfo)
	// 6. Is Valid check on FoundStat
	// 7. If valid: Break FStatInfo → get CurrentValue → return CurrentValue
	// 8. If not valid: return 0.0 (implicit)

	UAC_StatManager* StatManager = GetOwnerStatManager();
	if (!IsValid(StatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetResistiveStatValue - StatManager invalid"));
		return 0.0;
	}

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::GetResistiveStatValue - Data invalid"));
		return 0.0;
	}

	// Note: Need to:
	// 1. Cast Data to UPDA_StatusEffect to access ResistiveStat
	// 2. Call StatManager->GetStat(ResistiveStat) to get FStatInfo
	// 3. Return FStatInfo.CurrentValue
	// TODO: Implement when UPDA_StatusEffect and GetStat are available

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::GetResistiveStatValue - Implementation pending"));
	return 0.0;
}

void UB_StatusEffect::SpawnLoopingVfxAttached_Implementation()
{
	// Logic from JSON SpawnLoopingVfxAttached graph:
	// 1. Get Data->RankInfo (map)
	// 2. Map_Find with ActiveRank key
	// 3. If found: get FStatusEffectRankInfo->LoopingVfx
	// 4. Cast Owner to BPI_GenericCharacter interface
	// 5. If cast succeeds: Call PlaySoftNiagaraLoopingReplicated with:
	//    - VFXSystem = LoopingVfx
	//    - AttachSocket from RankInfo
	//    - Location/Rotation from RankInfo
	//    - Duration from EffectDuration

	if (!IsValid(Data) || !IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Data or Owner invalid"));
		return;
	}

	// Check if Owner implements BPI_GenericCharacter
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		// Note: Need to:
		// 1. Get RankInfo from Data
		// 2. Find ActiveRank entry
		// 3. Call interface function with VFX parameters
		// TODO: Implement when UPDA_StatusEffect is available

		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::SpawnLoopingVfxAttached - Implementation pending"));
	}
}

void UB_StatusEffect::Buildup()
{
	// Logic from JSON Buildup event graph (timer callback):
	// Called every 0.016667 seconds (60fps) during buildup phase
	// Calls AddBuildup to increment buildup

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Buildup - Timer tick"));
	AddBuildup();
}

void UB_StatusEffect::Decay()
{
	// Logic from JSON Decay event graph (timer callback):
	// Called every tick during decay phase
	// Decrements BuildupPercent, broadcasts OnBuildupUpdated
	// If BuildupPercent <= 0: Clear timer

	if (!bIsTriggered && IsValid(Data))
	{
		// Decrease buildup (inverse of buildup rate)
		// Note: Need decay rate from Data
		// For now using fixed decay rate
		double DecayAmount = 1.0; // TODO: Get from Data

		BuildupPercent = FMath::Clamp(BuildupPercent - DecayAmount, 0.0, 100.0);
		OnBuildupUpdated.Broadcast();

		UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Decay - BuildupPercent: %f"), BuildupPercent);

		if (BuildupPercent <= 0.0)
		{
			// Clear decay timer
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(DecayTimerHandle);
			}
			UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Decay - Buildup depleted, timer cleared"));
		}
	}
}

void UB_StatusEffect::TickDamage()
{
	// Logic from JSON TickDamage event graph (timer callback):
	// Called periodically when effect is triggered
	// Applies tick damage based on TickStatChange data

	if (!bIsTriggered)
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::TickDamage - Called but not triggered"));
		return;
	}

	// Apply tick stat changes
	// Note: Need to iterate StatsToAdjust and apply via StatManager
	// TODO: Implement stat change application

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::TickDamage - Applying tick damage"));
}

void UB_StatusEffect::EffectFinished()
{
	// Logic from JSON EffectFinished event graph:
	// 1. Clear all timers
	// 2. Reset IsTriggered to false
	// 3. Broadcast OnStatusEffectFinished with tag from Data

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::EffectFinished - Effect completed"));

	// Clear all timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
		World->GetTimerManager().ClearTimer(TickDamageTimerHandle);
		World->GetTimerManager().ClearTimer(WaitForDecayTimerHandle);
	}

	// Reset triggered state
	bIsTriggered = false;
	BuildupPercent = 0.0;

	// Broadcast finished event
	// Note: Need to get Tag from Data (UPDA_StatusEffect)
	FGameplayTag StatusTag; // TODO: Get from Data->Tag
	OnStatusEffectFinished.Broadcast(StatusTag);
}

void UB_StatusEffect::EffectTriggered()
{
	// Logic from JSON EffectTriggered event graph:
	// Called when buildup reaches 100%
	// 1. Apply one-shot stat changes
	// 2. Start tick damage timer if applicable
	// 3. Start effect duration timer
	// 4. Spawn looping VFX

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::EffectTriggered - Effect triggered!"));

	// Apply one-shot stat changes
	// TODO: Apply OneShotStatChange via StatManager

	// Spawn looping VFX
	SpawnLoopingVfxAttached();

	// Set up tick damage timer if EffectSteps > 0
	if (EffectSteps > 0.0 && EffectDuration > 0.0)
	{
		double TickInterval = EffectDuration / EffectSteps;

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				TickDamageTimerHandle,
				this,
				&UB_StatusEffect::TickDamage,
				TickInterval,
				true // Looping
			);
		}
	}

	// Set up effect duration timer
	if (EffectDuration > 0.0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				WaitForDecayTimerHandle,
				this,
				&UB_StatusEffect::EffectFinished,
				EffectDuration,
				false // Not looping
			);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CUSTOM EVENTS
// ═══════════════════════════════════════════════════════════════════════════

void UB_StatusEffect::Initialize_Implementation(int32 Rank)
{
	// Logic from JSON Event Initialize:
	// 1. Call GetResistiveStatValue()
	// 2. Set OwnerResistiveStatValue = result
	// 3. Call RefreshRank(Rank)

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::Initialize - Rank: %d"), Rank);

	OwnerResistiveStatValue = GetResistiveStatValue();
	RefreshRank(Rank);
}

void UB_StatusEffect::RefreshRank_Implementation(int32 NewActiveRank)
{
	// Logic from JSON Event RefreshRank:
	// 1. Set ActiveRank = NewActiveRank

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::RefreshRank - NewActiveRank: %d"), NewActiveRank);
	ActiveRank = NewActiveRank;
}

void UB_StatusEffect::AdjustBuildupOneshot_Implementation(double Delta)
{
	// Logic from JSON Event AdjustBuildupOneshot:
	// Apply a one-time buildup adjustment
	// 1. If not triggered and Data valid:
	// 2. BuildupPercent = Clamp(BuildupPercent + Delta, 0, 100)
	// 3. Broadcast OnBuildupUpdated
	// 4. Check if >= 100, trigger if so

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::AdjustBuildupOneshot - Delta: %f"), Delta);

	if (!bIsTriggered && IsValid(Data))
	{
		BuildupPercent = FMath::Clamp(BuildupPercent + Delta, 0.0, 100.0);
		OnBuildupUpdated.Broadcast();

		if (BuildupPercent >= 100.0)
		{
			// Clear buildup timer
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(BuildupTimerHandle);
			}

			// Broadcast triggered event
			FText TriggeredText = GetTriggeredText();
			OnStatusEffectTriggered.Broadcast(TriggeredText);

			// Call effect triggered
			EffectTriggered();

			// Set triggered flag
			bIsTriggered = true;
		}
	}
}

void UB_StatusEffect::WaitForDecay_Implementation()
{
	// Logic from JSON Event WaitForDecay:
	// Set a timer to wait before starting decay
	// Note: Delay time from Data

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::WaitForDecay"));

	// TODO: Get delay time from Data
	double DecayDelay = 2.0; // Default delay

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			WaitForDecayTimerHandle,
			this,
			&UB_StatusEffect::StartDecay,
			DecayDelay,
			false // Not looping
		);
	}
}

void UB_StatusEffect::RemoveBuildup_Implementation()
{
	// Logic from JSON Event RemoveBuildup:
	// 1. Set BuildupPercent = 0
	// 2. Clear timers
	// 3. Broadcast OnBuildupUpdated

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::RemoveBuildup"));

	BuildupPercent = 0.0;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
	}

	OnBuildupUpdated.Broadcast();
}

void UB_StatusEffect::StopDecay_Implementation()
{
	// Logic from JSON Event StopDecay:
	// Clear the decay timer

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StopDecay"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
	}
}

void UB_StatusEffect::StartDecay_Implementation()
{
	// Logic from JSON Event StartDecay:
	// Set looping timer for Decay callback

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StartDecay"));

	// Timer interval (same as buildup: 0.016667 = ~60fps)
	const float DecayInterval = 0.016667f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DecayTimerHandle,
			this,
			&UB_StatusEffect::Decay,
			DecayInterval,
			true // Looping
		);
	}
}

void UB_StatusEffect::StopBuildup_Implementation(bool bApplyDelay)
{
	// Logic from JSON Event StopBuildup:
	// 1. Clear buildup timer
	// 2. If bApplyDelay: call WaitForDecay
	// 3. Else: call StartDecay immediately

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StopBuildup - bApplyDelay: %s"),
		bApplyDelay ? TEXT("true") : TEXT("false"));

	// Clear buildup timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildupTimerHandle);
	}

	// Start decay (with or without delay)
	if (bApplyDelay)
	{
		WaitForDecay();
	}
	else
	{
		StartDecay();
	}
}

void UB_StatusEffect::StartBuildup_Implementation()
{
	// Logic from JSON Event StartBuildup:
	// 1. Call StopDecay (stop any active decay)
	// 2. Set looping timer for Buildup callback (0.016667 seconds = ~60fps)

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::StartBuildup"));

	// Stop any active decay first
	StopDecay();

	// Start buildup timer
	const float BuildupInterval = 0.016667f; // ~60fps

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BuildupTimerHandle,
			this,
			&UB_StatusEffect::Buildup,
			BuildupInterval,
			true // Looping
		);
	}
}

void UB_StatusEffect::AddBuildup_Implementation()
{
	// Logic from JSON Event AddBuildup:
	// 1. Branch: if NOT IsTriggered
	// 2. Get Data (with validation)
	// 3. If Data valid:
	//    a. Get BuildupPercent
	//    b. Get buildup rate from Data (RankInfo->BuildupRate)
	//    c. Multiply by owner resistance factor
	//    d. Add to BuildupPercent
	//    e. Clamp(0, 100)
	//    f. Set BuildupPercent
	// 4. Broadcast OnBuildupUpdated
	// 5. Branch: if BuildupPercent >= 100
	// 6. If true:
	//    a. Clear timer "Buildup"
	//    b. Broadcast OnStatusEffectTriggered
	//    c. Call EffectTriggered
	//    d. Set IsTriggered = true

	if (bIsTriggered)
	{
		return; // Already triggered, don't add more buildup
	}

	if (!IsValid(Data))
	{
		UE_LOG(LogTemp, Warning, TEXT("UB_StatusEffect::AddBuildup - Data invalid"));
		return;
	}

	// Calculate buildup amount
	// Note: Need to get BuildupRate from Data->RankInfo[ActiveRank]
	// For now using a placeholder rate
	double BuildupRate = 1.0; // TODO: Get from Data->RankInfo[ActiveRank].BuildupRate

	// Apply resistance factor (higher resistance = less buildup)
	double ResistanceFactor = 1.0;
	if (OwnerResistiveStatValue > 0.0)
	{
		ResistanceFactor = 100.0 / (100.0 + OwnerResistiveStatValue);
	}

	double BuildupAmount = BuildupRate * ResistanceFactor;

	// Update buildup
	BuildupPercent = FMath::Clamp(BuildupPercent + BuildupAmount, 0.0, 100.0);

	// Broadcast update
	OnBuildupUpdated.Broadcast();

	UE_LOG(LogTemp, Verbose, TEXT("UB_StatusEffect::AddBuildup - BuildupPercent: %f"), BuildupPercent);

	// Check if triggered
	if (BuildupPercent >= 100.0)
	{
		UE_LOG(LogTemp, Log, TEXT("UB_StatusEffect::AddBuildup - Buildup reached 100%%, triggering effect"));

		// Clear buildup timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(BuildupTimerHandle);
		}

		// Broadcast triggered event
		FText TriggeredText = GetTriggeredText();
		OnStatusEffectTriggered.Broadcast(TriggeredText);

		// Call effect triggered
		EffectTriggered();

		// Set triggered flag
		bIsTriggered = true;
	}
}
