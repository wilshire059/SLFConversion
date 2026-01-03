// BTS_ChaseBounds.h
// C++ AI class for BTS_ChaseBounds
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AI/BTS_ChaseBounds.json
// Parent: UBTService_BlueprintBase
// Variables: 5 | Functions: 0

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "InstancedStruct.h"
#include "BTS_ChaseBounds.generated.h"

UCLASS()
class SLFCONVERSION_API UBTS_ChaseBounds : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTS_ChaseBounds();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector StartPositionKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector ChaseDistanceKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FBlackboardKeySelector StateKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ESLFAIStates State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool InverseCondition;

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
