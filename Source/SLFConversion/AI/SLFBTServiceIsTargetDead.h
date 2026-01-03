// SLFBTServiceIsTargetDead.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SLFBTServiceIsTargetDead.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTServiceIsTargetDead : public UBTService
{
	GENERATED_BODY()

public:
	USLFBTServiceIsTargetDead();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsDeadOutputKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
