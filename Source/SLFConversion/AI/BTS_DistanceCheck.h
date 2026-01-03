// BTS_DistanceCheck.h
// C++ AI class for BTS_DistanceCheck
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTS_DistanceCheck.json
// Parent: UBTService_BlueprintBase
// Variables: 4 | Functions: 0

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTS_DistanceCheck.generated.h"

UCLASS()
class SLFCONVERSION_API UBTS_DistanceCheck : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTS_DistanceCheck();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector DistanceKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector KeyToSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ESLFCompareOperation CompareType;

	// Service tick
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

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
