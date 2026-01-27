// AIBehaviorManagerComponent.h
// C++ base class for AC_AI_BehaviorManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_AI_BehaviorManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         11/11 migrated
// Functions:         7/7 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 0/0
// Graphs:            8 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager
//
// PURPOSE: AI behavior tree management - state machine, patrol, combat targeting

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLFEnums.h"
#include "AIBehaviorManagerComponent.generated.h"

// Forward declarations
class UBehaviorTree;
class UBlackboardComponent;

// Types used from SLFEnums.h:
// - ESLFAIStates

// Local type (not in shared headers)
UENUM(BlueprintType)
enum class ESLFStrafeMethod : uint8
{
	Orbit       UMETA(DisplayName = "Orbit"),
	BackAndForth UMETA(DisplayName = "Back And Forth"),
	Random      UMETA(DisplayName = "Random")
};

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UAIBehaviorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIBehaviorManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Debug timing
	float DebugLogTimer = 0.0f;
	static constexpr float DebugLogInterval = 2.0f;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 11/11 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Behavior Tree (1) ---

	/** [1/11] Behavior tree asset to run */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	UBehaviorTree* Behavior;

	// --- State (3) ---

	/** [2/11] Current AI state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	ESLFAIStates CurrentState;

	/** [3/11] Previous AI state */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	ESLFAIStates PreviousState;

	/** [4/11] Current combat target */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* CurrentTarget;

	// --- Patrol (1) ---

	/** [5/11] Patrol path actor reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	AActor* PatrolPath;

	// --- Distance Thresholds (4) ---

	/** [6/11] Max distance to chase target before giving up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	double MaxChaseDistanceThreshold;

	/** [7/11] Distance at which to start attacking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	double AttackDistanceThreshold;

	/** [8/11] Distance for strafing behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	double StrafeDistanceThreshold;

	/** [9/11] Distance for speed adjustment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	double SpeedAdjustDistanceThreshold;

	// --- Strafe Config (2) ---

	/** [10/11] Minimum distance between strafe points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	double MinimumStrafePointDistanceThreshold;

	/** [11/11] Available strafe methods */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Related")
	TArray<ESLFStrafeMethod> StrafeMethods;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 7/7 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- State Management (2) ---

	/** [1/7] Set AI state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Behavior|State")
	void SetState(ESLFAIStates NewState);
	virtual void SetState_Implementation(ESLFAIStates NewState);

	/** [2/7] Get current AI state */
	UFUNCTION(BlueprintPure, Category = "AI Behavior|State")
	ESLFAIStates GetState() const { return CurrentState; }

	// --- Target Management (2) ---

	/** [3/7] Set current target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Behavior|Target")
	void SetTarget(AActor* NewTarget);
	virtual void SetTarget_Implementation(AActor* NewTarget);

	/** [4/7] Get current target */
	UFUNCTION(BlueprintPure, Category = "AI Behavior|Target")
	AActor* GetTarget() const { return CurrentTarget; }

	// --- Patrol (1) ---

	/** [5/7] Set patrol path */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Behavior|Patrol")
	void SetPatrolPath(AActor* NewPath);
	virtual void SetPatrolPath_Implementation(AActor* NewPath);

	// --- Blackboard (2) ---

	/** [6/7] Get blackboard component */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Behavior|Blackboard")
	UBlackboardComponent* GetBlackboard();
	virtual UBlackboardComponent* GetBlackboard_Implementation();

	/** [7/7] Set blackboard key value */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Behavior|Blackboard")
	void SetKeyValue(FName KeyName, UObject* Value);
	virtual void SetKeyValue_Implementation(FName KeyName, UObject* Value);
};
