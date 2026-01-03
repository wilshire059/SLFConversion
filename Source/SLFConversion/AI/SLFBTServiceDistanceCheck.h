// SLFBTServiceDistanceCheck.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SLFBTServiceDistanceCheck.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTServiceDistanceCheck : public UBTService
{
	GENERATED_BODY()

public:
	USLFBTServiceDistanceCheck();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceOutputKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
