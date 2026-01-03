// SLFBTTaskToggleFocus.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SLFBTTaskToggleFocus.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTTaskToggleFocus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLFBTTaskToggleFocus();

	UPROPERTY(EditAnywhere, Category = "Focus")
	bool bEnableFocus = true;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FocusActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
