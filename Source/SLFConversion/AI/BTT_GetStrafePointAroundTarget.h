// BTT_GetStrafePointAroundTarget.h
// C++ AI class for BTT_GetStrafePointAroundTarget
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTT_GetStrafePointAroundTarget.json
// Parent: UBTTask_BlueprintBase
// Variables: 7 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTT_GetStrafePointAroundTarget.generated.h"

UCLASS()
class SLFCONVERSION_API UBTT_GetStrafePointAroundTarget : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_GetStrafePointAroundTarget();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector StrafeLocationKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector RadiusKey;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFAIStrafeLocations LocationList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFAIStrafeLocations PossibleMethods;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFAIStrafeLocations PickedStrafeMethod;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector PickedStrafeLoc;

	// Task execution
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category = "BTT_GetStrafePointAroundTarget")
	void SetStrafeLocations();
	UFUNCTION(BlueprintCallable, Category = "BTT_GetStrafePointAroundTarget")
	ESLFAIStrafeLocations PickStrafeMethodBasedOnScore(const FSLFAiStrafeInfo& StrafeMethods);

protected:
	// Helper to get blackboard value
	template<typename T>
	T GetBlackboardValue(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& Key) const;
	
	// Helper to set blackboard value
	template<typename T>
	void SetBlackboardValue(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& Key, T Value);
};
