// AIBossComponent.h
// C++ base class for AC_AI_Boss
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_AI_Boss
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         18/18 migrated
// Functions:         17/17 migrated (excluding ExecuteUbergraph and montage callbacks)
// Event Dispatchers: 0/0
// Graphs:            6 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss
//
// PURPOSE: Boss encounter system - phases, triggers, death sequence, boss doors

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "AIBossComponent.generated.h"

// Forward declarations
class UAnimMontage;
class UAudioComponent;
class ULevelSequencePlayer;
class ALevelSequenceActor;

// Types used from SLFEnums.h/SLFGameTypes.h:
// - ESLFAIBossEncounterType
// - FSLFAiBossPhase

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UAIBossComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIBossComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 18/18 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Display (2) ---

	/** [1/18] Boss display name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	FText Name;

	/** [2/18] Whether to show boss health bar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	bool bShowBossBar;

	// --- Trigger Config (3) ---

	/** [3/18] How encounter is triggered */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Trigger")
	ESLFAIBossEncounterType EncounterTriggerType;

	/** [4/18] Trigger collision radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Trigger")
	double TriggerCollisionRadius;

	/** [5/18] Door check distance for door-based triggers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Trigger")
	double DoorCheckDistance;

	// --- Phase Config (4) ---

	/** [6/18] Array of boss phases */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phases")
	TArray<FSLFAiBossPhase> Phases;

	/** [7/18] Currently active phase data */
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Phases")
	FSLFAiBossPhase ActivePhase;

	/** [8/18] Current phase index */
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Phases")
	int32 ActivePhaseIndex;

	// --- Death Config (4) ---

	/** [9/18] Death sequence montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Death")
	UAnimMontage* DeathSequence;

	/** [10/18] Death text to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Death")
	FText DeathText;

	/** [11/18] Delay before showing death text */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Death")
	float DeathTextDelay;

	/** [12/18] Whether boss is hidden during death sequence */
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Death")
	bool bHiddenInDeathSequence;

	// --- Boss Doors (2) ---

	/** [13/18] Related boss door actors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Doors")
	TArray<AActor*> RelatedBossDoors;

	/** [14/18] Boss door class for spawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Doors")
	TSubclassOf<AActor> BossDoorClass;

	// --- Runtime State (2) ---

	/** [15/18] Whether fight is currently active */
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Runtime")
	bool bFightActive;

	/** [16/18] Active music audio component */
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Runtime")
	UAudioComponent* ActiveMusicComponent;

	// --- Debug (2) ---

	/** [17/18] Whether debug mode is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Debug")
	bool bDebugActive;

	/** [18/18] Debug phase override */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Debug")
	int32 DebugPhase;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 17/17 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Fight Control (2) ---

	/** [1/17] Set fight active state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Fight")
	void SetFightActive(bool bActive);
	virtual void SetFightActive_Implementation(bool bActive);

	/** [2/17] End fight and cleanup */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Fight")
	void EndFight();
	virtual void EndFight_Implementation();

	// --- Phase Management (4) ---

	/** [3/17] Set current phase by index */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Phases")
	void SetPhase(int32 PhaseIndex);
	virtual void SetPhase_Implementation(int32 PhaseIndex);

	/** [4/17] Evaluate if phase requirement is met */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Phases")
	bool EvaluatePhaseRequirement(int32 PhaseIndex, float CurrentHealthPercent);
	virtual bool EvaluatePhaseRequirement_Implementation(int32 PhaseIndex, float CurrentHealthPercent);

	/** [5/17] Handle phase change transition */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Phases")
	void HandlePhaseChange(int32 NewPhaseIndex);
	virtual void HandlePhaseChange_Implementation(int32 NewPhaseIndex);

	/** [6/17] Called when phase transition sequence finishes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Phases")
	void OnPhaseSequenceFinished();
	virtual void OnPhaseSequenceFinished_Implementation();

	// --- Boss Doors (2) ---

	/** [7/17] Try to get boss door actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Doors")
	bool TryGetBossDoors(TArray<AActor*>& OutDoors);
	virtual bool TryGetBossDoors_Implementation(TArray<AActor*>& OutDoors);

	/** [8/17] Try to unlock boss doors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Doors")
	void TryUnlockBossDoors();
	virtual void TryUnlockBossDoors_Implementation();

	// --- Death Handling (4) ---

	/** [9/17] Handle boss death */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Death")
	void OnBossDeath(AActor* Killer);
	virtual void OnBossDeath_Implementation(AActor* Killer);

	/** [10/17] Called when death sequence finishes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Death")
	void OnDeathSequenceFinished();
	virtual void OnDeathSequenceFinished_Implementation();

	/** [11/17] Show death text on screen */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Death")
	void ShowDeathText();
	virtual void ShowDeathText_Implementation();

	// --- Event Handlers (3) ---

	/** [12/17] Handler for stat update events (health check for phase) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss|Events")
	void OnStatUpdated(FGameplayTag StatTag, float CurrentValue, float MaxValue);
	virtual void OnStatUpdated_Implementation(FGameplayTag StatTag, float CurrentValue, float MaxValue);

	// --- Getters (5) ---

	/** [13/17] Get boss name */
	UFUNCTION(BlueprintPure, Category = "Boss|Getters")
	FText GetBossName() const { return Name; }

	/** [14/17] Get current phase index */
	UFUNCTION(BlueprintPure, Category = "Boss|Getters")
	int32 GetActivePhaseIndex() const { return ActivePhaseIndex; }

	/** [15/17] Get whether fight is active */
	UFUNCTION(BlueprintPure, Category = "Boss|Getters")
	bool GetFightActive() const { return bFightActive; }

	/** [16/17] Get active phase data */
	UFUNCTION(BlueprintPure, Category = "Boss|Getters")
	FSLFAiBossPhase GetActivePhase() const { return ActivePhase; }

	/** [17/17] Get phase count */
	UFUNCTION(BlueprintPure, Category = "Boss|Getters")
	int32 GetPhaseCount() const { return Phases.Num(); }

private:
	/** Active level sequence player for cinematic cleanup */
	UPROPERTY(Transient)
	ULevelSequencePlayer* ActiveSequencePlayer;

	/** Active level sequence actor for cinematic cleanup */
	UPROPERTY(Transient)
	ALevelSequenceActor* ActiveSequenceActor;

	/** Handler for level sequence OnFinished delegate */
	UFUNCTION()
	void OnLevelSequenceFinished();

	/** Stop and clean up any active cinematic sequence */
	void CleanupActiveSequence();
};
