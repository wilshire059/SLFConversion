// SLFBTTaskGetRandomPoint.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SLFBTTaskGetRandomPoint.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTTaskGetRandomPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLFBTTaskGetRandomPoint();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector OutputKey;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float Radius = 500.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
