// BTT_SwitchState.h
// C++ AI class for BTT_SwitchState
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTT_SwitchState.json
// Parent: UBTTask_BlueprintBase
// Variables: 2 | Functions: 0

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTT_SwitchState.generated.h"

UCLASS()
class SLFCONVERSION_API UBTT_SwitchState : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_SwitchState();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ESLFAIStates NewState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FInstancedStruct Data;

	// Task execution
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════


protected:
	// Helper to get blackboard value
	template<typename T>
	T GetBlackboardValue(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& Key) const;
	
	// Helper to set blackboard value
	template<typename T>
	void SetBlackboardValue(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& Key, T Value);
};
