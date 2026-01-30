// AC_AI_Boss.cpp
// C++ component implementation for AC_AI_Boss
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_Boss.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_AI_Boss.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AC_StatManager.h"
#include "Components/AC_AI_BehaviorManager.h"
#include "Widgets/W_HUD.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Interfaces/SLFControllerInterface.h"
#include "Blueprints/B_Stat.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

UAC_AI_Boss::UAC_AI_Boss()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	DoorCheckDistance = 1000.0;
	EncounterTriggerType = ESLFAIBossEncounterType::Perception;
	FightActive = false;
	TriggerCollisionRadius = 500.0;
	ShowBossBar = true;
	ActivePhaseIndex = 0;
	DebugActive = false;
	DebugPhase = 0;
	DeathTextDelay = 2.0;
	HiddenInDeathSequence = false;
	ActiveMusicComponent = nullptr;
}

void UAC_AI_Boss::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::BeginPlay"));

	// Initialize first phase if available
	if (Phases.Num() > 0)
	{
		ActivePhase = Phases[0];
	}

	// Find related boss doors
	TryGetBossDoors();
}

void UAC_AI_Boss::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * SetFightActive - Activate or deactivate the boss fight
 *
 * Blueprint Logic (from JSON):
 * 1. Set FightActive variable
 * 2. Branch on FightActive:
 *    - TRUE path:
 *      a. Branch on ShowBossBar
 *         - TRUE: TryGetHudForActor -> Branch on Success -> EventShowBossBar(Name, Owner)
 *         - FALSE: Continue to SetPhase or skip
 *      b. Branch on DebugActive
 *         - TRUE: SetPhase(DebugPhase)
 *         - FALSE: SetPhase(0)
 *    - FALSE path:
 *      a. Sequence:
 *         - then_0: TryGetHudForActor -> Branch on Success -> EventHideBossBar
 *         - then_1: TryUnlockBossDoors
 */
void UAC_AI_Boss::SetFightActive_Implementation(bool InFightActive)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::SetFightActive - %s"), InFightActive ? TEXT("true") : TEXT("false"));

	// Step 1: Set the variable
	FightActive = InFightActive;

	if (FightActive)
	{
		// TRUE path: Fight is starting

		// Step 2a: Branch on ShowBossBar
		if (ShowBossBar)
		{
			UE_LOG(LogTemp, Log, TEXT("  ShowBossBar is true, attempting to show boss bar"));

			// TryGetHudForActor: Get Target from BehaviorManager, then get HUD
			UW_HUD* HUD = nullptr;
			if (TryGetHudForTarget(HUD))
			{
				UE_LOG(LogTemp, Log, TEXT("  HUD found, calling EventShowBossBar"));

				// Call EventShowBossBar with BossName and BossActor
				AActor* Owner = GetOwner();
				if (HUD && Owner)
				{
					HUD->EventShowBossBar(Name, Owner);
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("  Failed to get HUD for target"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("  ShowBossBar is false, skipping boss bar display"));
		}

		// Step 2b: Branch on DebugActive, then SetPhase
		if (DebugActive)
		{
			UE_LOG(LogTemp, Log, TEXT("  DebugActive, setting phase to DebugPhase: %d"), DebugPhase);
			SetPhase(DebugPhase);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("  Setting initial phase 0"));
			SetPhase(0);
		}
	}
	else
	{
		// FALSE path: Fight is ending

		// Sequence then_0: Hide boss bar
		UE_LOG(LogTemp, Log, TEXT("  Fight ending, hiding boss bar"));

		UW_HUD* HUD = nullptr;
		if (TryGetHudForTarget(HUD))
		{
			if (HUD)
			{
				UE_LOG(LogTemp, Log, TEXT("  HUD found, calling EventHideBossBar"));
				HUD->EventHideBossBar();
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("  Failed to get HUD for target (fight end)"));
		}

		// Sequence then_1: Unlock boss doors
		UE_LOG(LogTemp, Log, TEXT("  Unlocking boss doors"));
		TryUnlockBossDoors();
	}
}

/**
 * SetPhase - Change the boss to a specific phase
 *
 * Blueprint Logic:
 * 1. Get Phases[PhaseIndex]
 * 2. Set ActivePhaseIndex and ActivePhase
 * 3. If PhaseStartSequence is valid:
 *    - Create and play level sequence player directly
 */
void UAC_AI_Boss::SetPhase_Implementation(int32 PhaseIndex)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::SetPhase - Index: %d"), PhaseIndex);

	if (PhaseIndex >= 0 && PhaseIndex < Phases.Num())
	{
		ActivePhaseIndex = PhaseIndex;
		ActivePhase = Phases[PhaseIndex];

		// Play the phase start sequence if valid
		if (!ActivePhase.PhaseStartSequence.IsNull())
		{
			ULevelSequence* Sequence = ActivePhase.PhaseStartSequence.LoadSynchronous();
			if (IsValid(Sequence))
			{
				UE_LOG(LogTemp, Log, TEXT("  Playing PhaseStartSequence: %s"), *Sequence->GetName());

				// Create level sequence player and play directly
				FMovieSceneSequencePlaybackSettings PlaybackSettings;
				PlaybackSettings.bAutoPlay = true;

				ALevelSequenceActor* OutActor = nullptr;
				ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(
					GetWorld(),
					Sequence,
					PlaybackSettings,
					OutActor
				);

				if (IsValid(Player))
				{
					Player->Play();
					UE_LOG(LogTemp, Log, TEXT("  Started playing phase sequence"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("  Failed to create level sequence player"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid phase index: %d"), PhaseIndex);
	}
}

/**
 * EvaluatePhaseRequirement - Check if a stat requirement for phase transition is met
 *
 * Blueprint Logic (from JSON EvaluatePhaseRequirement graph):
 * 1. Branch on StatRequirement (entry check)
 * 2. Get Owner -> GetComponentByClass(AC_StatManager)
 * 3. GetStat(StatRequirement.StatTag) -> Found Stat + Stat Info
 * 4. GetStatPercentValue(StatInfo) -> Max Value (this is PERCENT, despite name)
 *    NOTE: The Blueprint uses "Max Value" output but it's actually the percent value!
 * 5. Compare percent value vs StatRequirement.Percent using ComparisonType:
 *    - float < float (Less)
 *    - float > float (Greater)
 *    - float <= float (LessOrEqual)
 *    - float >= float (GreaterOrEqual)
 *    - Equal, NotEqual
 * 6. Select result based on comparison match
 * 7. IsValid(Found Stat) branch -> Return selected result or false
 */
void UAC_AI_Boss::EvaluatePhaseRequirement_Implementation(const FSLFAiRuleStat& StatRequirement, bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::EvaluatePhaseRequirement - Tag: %s, Percent: %.2f"),
		*StatRequirement.StatTag.ToString(), StatRequirement.Percent);

	bool bMet = false;

	// Get stat manager from owner
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Owner is invalid"));
		OutReturnValue = false;
		OutReturnValue_1 = false;
		OutReturnValue_2 = false;
		return;
	}

	UAC_StatManager* StatManager = Owner->FindComponentByClass<UAC_StatManager>();
	if (!IsValid(StatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatManager not found on owner"));
		OutReturnValue = false;
		OutReturnValue_1 = false;
		OutReturnValue_2 = false;
		return;
	}

	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	StatManager->GetStat(StatRequirement.StatTag, FoundStat, StatInfo);

	// IsValid check on Found Stat - if not valid, return false
	if (!IsValid(FoundStat))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Stat not found for tag: %s"), *StatRequirement.StatTag.ToString());
		OutReturnValue = false;
		OutReturnValue_1 = false;
		OutReturnValue_2 = false;
		return;
	}

	// GetStatPercentValue - returns Current/Max as a ratio (0.0 to 1.0)
	// Blueprint multiplies by 100 or uses raw value, but comparison is against StatRequirement.Percent
	// The Blueprint "Max Value" output is actually the PERCENT value (CurrentValue / MaxValue)
	double CurrentPercent = 0.0;
	if (StatInfo.MaxValue > 0.0)
	{
		CurrentPercent = StatInfo.CurrentValue / StatInfo.MaxValue;
	}

	double RequiredPercent = StatRequirement.Percent;

	UE_LOG(LogTemp, Log, TEXT("  CurrentValue: %.2f, MaxValue: %.2f, CurrentPercent: %.2f, RequiredPercent: %.2f"),
		StatInfo.CurrentValue, StatInfo.MaxValue, CurrentPercent, RequiredPercent);

	// Compare based on comparison type
	// Blueprint uses Select node with all 6 comparison results feeding into it
	switch (StatRequirement.ComparisonType)
	{
	case ESLFComparisonMethod::Less:
		bMet = (CurrentPercent < RequiredPercent);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f < %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	case ESLFComparisonMethod::LessOrEqual:
		bMet = (CurrentPercent <= RequiredPercent);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f <= %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	case ESLFComparisonMethod::Greater:
		bMet = (CurrentPercent > RequiredPercent);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f > %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	case ESLFComparisonMethod::GreaterOrEqual:
		bMet = (CurrentPercent >= RequiredPercent);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f >= %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	case ESLFComparisonMethod::Equal:
		bMet = FMath::IsNearlyEqual(CurrentPercent, RequiredPercent, 0.001);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f == %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	case ESLFComparisonMethod::NotEqual:
		bMet = !FMath::IsNearlyEqual(CurrentPercent, RequiredPercent, 0.001);
		UE_LOG(LogTemp, Log, TEXT("  Comparison: %.2f != %.2f = %s"), CurrentPercent, RequiredPercent, bMet ? TEXT("true") : TEXT("false"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("  Unknown comparison type"));
		bMet = false;
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("  Result: %s"), bMet ? TEXT("MET") : TEXT("NOT MET"));

	OutReturnValue = bMet;
	OutReturnValue_1 = bMet;
	OutReturnValue_2 = bMet;
}

/**
 * TryGetBossDoors - Find all boss doors within range
 */
void UAC_AI_Boss::TryGetBossDoors_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetBossDoors"));

	RelatedBossDoors.Empty();

	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !BossDoorClass)
	{
		return;
	}

	// Find all boss doors within DoorCheckDistance
	TArray<AActor*> FoundDoors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BossDoorClass, FoundDoors);

	FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* Door : FoundDoors)
	{
		if (IsValid(Door))
		{
			float Distance = FVector::Dist(OwnerLocation, Door->GetActorLocation());
			if (Distance <= DoorCheckDistance)
			{
				RelatedBossDoors.Add(Door);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Total doors found: %d"), RelatedBossDoors.Num());
}

/**
 * TryUnlockBossDoors - Unlock all related boss doors
 */
void UAC_AI_Boss::TryUnlockBossDoors_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryUnlockBossDoors - Unlocking %d doors"), RelatedBossDoors.Num());

	for (AActor* Door : RelatedBossDoors)
	{
		if (IsValid(Door))
		{
			// Door unlocking would be done via interface call
			UE_LOG(LogTemp, Log, TEXT("  Unlocking: %s"), *Door->GetName());
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════
// HELPER METHODS
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetBehaviorManagerTarget - Get the target actor from AI Behavior Manager
 *
 * Blueprint Logic (from SetFightActive graph):
 * 1. GetOwner() -> GetComponentByClass(AC_AI_BehaviorManager)
 * 2. AC_AI_BehaviorManager->GetTarget() -> returns AActor* (the player)
 */
AActor* UAC_AI_Boss::GetBehaviorManagerTarget()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::GetBehaviorManagerTarget - Owner is invalid"));
		return nullptr;
	}

	UAC_AI_BehaviorManager* BehaviorManager = Owner->FindComponentByClass<UAC_AI_BehaviorManager>();
	if (!IsValid(BehaviorManager))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::GetBehaviorManagerTarget - BehaviorManager not found"));
		return nullptr;
	}

	AActor* Target = BehaviorManager->GetTarget();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::GetBehaviorManagerTarget - Target: %s"),
		Target ? *Target->GetName() : TEXT("None"));

	return Target;
}

/**
 * TryGetHudForTarget - Get the HUD widget for the player targeting this boss
 *
 * This is a C++ implementation of the BFL_Helper::TryGetHudForActor logic:
 * 1. Get target actor from BehaviorManager
 * 2. Get Instigator (pawn) from target actor
 * 3. Get Controller from the pawn
 * 4. Check if controller implements ISLFControllerInterface
 * 5. Call GetPlayerHUD through the interface
 * 6. Return success if HUD is valid
 *
 * This replaces the Blueprint call chain:
 *   GetOwner -> GetComponentByClass(AC_AI_BehaviorManager) ->
 *   GetTarget -> BFL_Helper::TryGetHudForActor(Target) -> Success, HUD
 */
bool UAC_AI_Boss::TryGetHudForTarget(UW_HUD*& OutHUD)
{
	OutHUD = nullptr;

	// Step 1: Get the target from BehaviorManager (this is the player actor)
	AActor* TargetActor = GetBehaviorManagerTarget();
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - No target actor"));
		return false;
	}

	// Step 2: Get the Instigator (the pawn that owns this actor)
	APawn* InstigatorPawn = TargetActor->GetInstigator();
	if (!IsValid(InstigatorPawn))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - No instigator pawn"));
		return false;
	}

	// Step 3: Get the controller from the pawn
	AController* Controller = InstigatorPawn->GetController();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - No controller"));
		return false;
	}

	// Step 4: Check if controller implements ISLFControllerInterface
	if (!Controller->GetClass()->ImplementsInterface(USLFControllerInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - Controller does not implement ISLFControllerInterface"));
		return false;
	}

	// Step 5: Call GetPlayerHUD through the interface
	OutHUD = ISLFControllerInterface::Execute_GetPlayerHUD(Controller);

	// Step 6: Return success if HUD is valid
	if (IsValid(OutHUD))
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - HUD found: %s"), *OutHUD->GetName());
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("UAC_AI_Boss::TryGetHudForTarget - HUD is null"));
	return false;
}
