// BTT_TryExecuteAbility.h
// C++ AI class for BTT_TryExecuteAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTT_TryExecuteAbility.json
// Parent: UBTTask_BlueprintBase
// Variables: 0 | Functions: 0
//
// ASYNC TASK: Binds to OnAttackEnd, calls PerformAbility, waits for completion

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTT_TryExecuteAbility.generated.h"

// Forward declaration
class AB_Soulslike_Enemy;
class ASLFSoulslikeEnemy;

UCLASS()
class SLFCONVERSION_API UBTT_TryExecuteAbility : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_TryExecuteAbility();

	// Task execution - returns InProgress for async operation
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// Callback when attack ends
	UFUNCTION()
	void OnAttackEndCallback();

	// Cleanup delegate binding
	void CleanupDelegateBinding();

private:
	// Cached reference to old enemy class for delegate unbinding
	UPROPERTY()
	TWeakObjectPtr<AB_Soulslike_Enemy> CachedEnemy;

	// Cached reference to new SLF enemy class for delegate unbinding
	UPROPERTY()
	TWeakObjectPtr<ASLFSoulslikeEnemy> CachedSLFEnemy;

	// Cached behavior tree component for FinishLatentTask
	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	// Delegate handle for cleanup
	FDelegateHandle OnAttackEndDelegateHandle;
};
