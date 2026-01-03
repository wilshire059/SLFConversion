// BTT_SimpleMoveTo.h
// C++ AI class for BTT_SimpleMoveTo
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTT_SimpleMoveTo.json
// Parent: UBTTask_BlueprintBase
// Variables: 2 | Functions: 0
//
// ASYNC TASK: Uses AI MoveTo, always succeeds per Blueprint comment
// "Moveto, but simpler - Will finish with success upon failure as well"

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTT_SimpleMoveTo.generated.h"

UCLASS()
class SLFCONVERSION_API UBTT_SimpleMoveTo : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_SimpleMoveTo();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector RadiusKey;

	// Task execution - returns InProgress for async move
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// Callback when move is completed
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	// Cleanup delegate binding
	void CleanupMoveDelegate();

private:
	// Cached behavior tree component for FinishLatentTask
	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	// Delegate handle for cleanup
	FDelegateHandle MoveCompletedDelegateHandle;
};
