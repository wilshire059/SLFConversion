// SLFBTTaskSimpleMoveTo.h
// C++ base for BTT_SimpleMoveTo - Move AI to blackboard location
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - BTT_SimpleMoveTo
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (TargetKey, RadiusKey)
// Functions:         1/1 (ExecuteTask)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SLFBTTaskSimpleMoveTo.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTTaskSimpleMoveTo : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLFBTTaskSimpleMoveTo();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RadiusKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
