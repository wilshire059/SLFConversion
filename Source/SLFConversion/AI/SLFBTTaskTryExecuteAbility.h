// SLFBTTaskTryExecuteAbility.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "SLFBTTaskTryExecuteAbility.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTTaskTryExecuteAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLFBTTaskTryExecuteAbility();

	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
