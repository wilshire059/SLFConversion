// SLFBossEncounter.h
// Actor that manages a boss fight: fog gate, boss spawn, health bar, phase transitions,
// victory handling. Place in the level at boss arena entrance.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "SLFBossEncounter.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossEncounterPhaseChanged, ESLFBossPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossEncounterStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossEncounterVictory);

/**
 * Boss Encounter Manager - controls the lifecycle of a boss fight.
 *
 * Setup:
 * 1. Place at boss arena entrance
 * 2. Configure BossConfig (class, name, drops, phases)
 * 3. Set ArenaCenter + ArenaRadius for boundary
 * 4. FogGateTrigger activates on player overlap
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBossEncounter : public AActor
{
	GENERATED_BODY()

public:
	ASLFBossEncounter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ── Configuration ──

	/** Boss configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	FSLFBossConfig BossConfig;

	/** Zone this boss belongs to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	FGameplayTag ZoneTag;

	/** Where the boss spawns in the arena */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	FVector BossSpawnOffset = FVector(0, 500, 0);

	/** Arena center (for boundary checks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	FVector ArenaCenter = FVector::ZeroVector;

	/** Arena radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	float ArenaRadius = 2000.0f;

	/** Whether to show a boss health bar on the HUD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	bool bShowBossHealthBar = true;

	/** Whether the fog gate blocks exit during fight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Config")
	bool bBlockExitDuringFight = true;

	// ── Runtime State ──

	/** Current boss phase */
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	ESLFBossPhase CurrentPhase = ESLFBossPhase::NotBoss;

	/** Spawned boss actor */
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	TObjectPtr<ACharacter> BossActor;

	/** Whether the encounter is active */
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bEncounterActive = false;

	/** Whether this boss has already been defeated (persisted via ProgressManager) */
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bAlreadyDefeated = false;

	// ── Events ──

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossEncounterPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossEncounterStarted OnEncounterStarted;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossEncounterVictory OnEncounterVictory;

	// ── Functions ──

	/** Start the boss encounter (called when player enters fog gate) */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void StartEncounter();

	/** Called when boss dies */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void OnBossDefeated();

	/** Transition to the next boss phase */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void TransitionToPhase(ESLFBossPhase NewPhase);

	/** Check boss health and trigger phase transitions */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void CheckPhaseTransition(float CurrentHealthPercent);

	/** Reset encounter (player died, returned to resting point) */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void ResetEncounter();

	/** Enable/disable the fog gate blocking */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SetFogGateActive(bool bActive);

private:
	/** Fog gate trigger volume */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> FogGateTrigger;

	/** Fog gate visual mesh */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FogGateMesh;

	/** Exit blocker volume */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> ExitBlocker;

	UFUNCTION()
	void OnFogGateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** Spawn the boss actor */
	ACharacter* SpawnBoss();

	/** Handle boss death event */
	UFUNCTION()
	void HandleBossDeath(AActor* DeadActor);

	/** Check if this boss was already defeated via ProgressManager */
	void CheckAlreadyDefeated();
};
