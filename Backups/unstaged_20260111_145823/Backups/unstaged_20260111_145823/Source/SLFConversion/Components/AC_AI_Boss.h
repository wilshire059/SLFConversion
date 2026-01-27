// AC_AI_Boss.h
// C++ component for AC_AI_Boss
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_Boss.json
// Variables: 18 | Functions: 5 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFStatTypes.h"
#include "Components/AudioComponent.h"
#include "LevelSequence.h"
#include "AC_AI_Boss.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UAIBehaviorManagerComponent;
class UW_HUD;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_AI_Boss : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_AI_Boss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (18)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Trigger Settings")
	double DoorCheckDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Trigger Settings")
	ESLFAIBossEncounterType EncounterTriggerType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool FightActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Trigger Settings")
	double TriggerCollisionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Trigger Settings")
	bool ShowBossBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Phase Settings")
	TArray<FSLFAiBossPhase> Phases;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FSLFAiBossPhase ActivePhase;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	int32 ActivePhaseIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Debug Settings")
	bool DebugActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Debug Settings")
	int32 DebugPhase;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Phase Settings|Death")
	TSoftObjectPtr<ULevelSequence> DeathSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Phase Settings|Death|Text")
	FText DeathText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Phase Settings|Death|Text")
	double DeathTextDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> RelatedBossDoors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Phase Settings|Death")
	bool HiddenInDeathSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAudioComponent* ActiveMusicComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Phase Settings|Death")
	TSubclassOf<AActor> BossDoorClass;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_Boss")
	void SetFightActive(bool InFightActive);
	virtual void SetFightActive_Implementation(bool InFightActive);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_Boss")
	void SetPhase(int32 PhaseIndex);
	virtual void SetPhase_Implementation(int32 PhaseIndex);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_Boss")
	void EvaluatePhaseRequirement(const FSLFAiRuleStat& StatRequirement, bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	virtual void EvaluatePhaseRequirement_Implementation(const FSLFAiRuleStat& StatRequirement, bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_Boss")
	void TryGetBossDoors();
	virtual void TryGetBossDoors_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_Boss")
	void TryUnlockBossDoors();
	virtual void TryUnlockBossDoors_Implementation();

protected:
	/**
	 * Helper: Get HUD for the player that is targeting this boss
	 * Replicates BFL_Helper::TryGetHudForActor logic inline
	 */
	bool TryGetHudForTarget(UW_HUD*& OutHUD);

	/**
	 * Helper: Get the current target actor from the AI Behavior Manager
	 * This is the player actor that triggered the boss fight
	 */
	AActor* GetBehaviorManagerTarget();
};
