// B_Soulslike_Enemy.h
// C++ class for Blueprint B_Soulslike_Enemy
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/Blueprint/B_Soulslike_Enemy.json
// Parent: B_BaseCharacter_C -> ASLFBaseCharacter
// Variables: 2 | Functions: 11 interface + 2 local | Dispatchers: 1 | Components: 7

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Interfaces/BPI_Enemy.h"
#include "Interfaces/BPI_Executable.h"
#include "B_Soulslike_Enemy.generated.h"

// Forward declarations
class UAnimMontage;
class UAIPerceptionComponent;
class UWidgetComponent;
class UAICombatManagerComponent;
class UAIBehaviorManagerComponent;
class UNiagaraComponent;
class ULootDropManagerComponent;
class UArrowComponent;
class UBillboardComponent;
class AB_PatrolPath;
class UTimelineComponent;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_Soulslike_Enemy_OnAttackEnd);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_Enemy : public ASLFBaseCharacter, public IBPI_Enemy, public IBPI_Executable
{
	GENERATED_BODY()

public:
	AB_Soulslike_Enemy();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	/** Enemy unique ID for saving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	FGuid EnemyId;

	/** Patrol path reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AB_PatrolPath* PatrolPath;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (7)
	// ═══════════════════════════════════════════════════════════════════════

	/** Healthbar widget component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* Healthbar;

	/** AI Combat Manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAICombatManagerComponent* AC_AI_CombatManager;

	/** AI Behavior Manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIBehaviorManagerComponent* AC_AI_BehaviorManager;

	/** Souls Niagara effect component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NS_Souls;

	/** Loot Drop Manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	ULootDropManagerComponent* AC_LootDropManager;

	/** Execution move-to arrow component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ExecutionMoveTo;

	/** Visualizer billboard component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* VisualizerBillboard;

	/** Timeline component for rotate towards target */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimelineComponent* TL_RotateTowardsTarget;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Soulslike_Enemy_OnAttackEnd OnAttackEnd;

	// ═══════════════════════════════════════════════════════════════════════
	// LOCAL FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Enemy")
	void CheckSense(UAIPerceptionComponent* AiPerceptionComponent, AActor* Actor, ESLFAISenses Sense, bool& OutReturnValue, FAIStimulus& OutStimulus);
	virtual void CheckSense_Implementation(UAIPerceptionComponent* AiPerceptionComponent, AActor* Actor, ESLFAISenses Sense, bool& OutReturnValue, FAIStimulus& OutStimulus);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "B_Soulslike_Enemy|AI")
	void PerformAbility();

	// ═══════════════════════════════════════════════════════════════════════
	// BPI_Enemy INTERFACE (8 functions)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void GetExecutionMoveToTransform_Implementation(FVector& MoveToLocation, FRotator& Rotation) override;
	virtual void StopRotateTowardsTarget_Implementation() override;
	virtual void RotateTowardsTarget_Implementation(double Duration) override;
	virtual void PickAndSpawnLoot_Implementation() override;
	virtual void GetPatrolPath_Implementation(AActor*& OutPatrolPath) override;
	virtual void DisplayDeathVfx_Implementation(FVector AttractorPosition) override;
	virtual void ToggleHealthbarVisual_Implementation(bool bVisible) override;
	virtual void UpdateEnemyHealth_Implementation(double CurrentValue, double MaxValue, double Change) override;

	// ═══════════════════════════════════════════════════════════════════════
	// BPI_Executable INTERFACE (3 functions)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void OnExecutionStarted_Implementation() override;
	virtual void OnExecuted_Implementation(FGameplayTag ExecutionTag) override;
	virtual void OnBackstabbed_Implementation(FGameplayTag ExecutionTag) override;

protected:
	// Timer handle for rotation
	FTimerHandle RotateToTargetTimerHandle;
};
