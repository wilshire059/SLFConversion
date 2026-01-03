// SLFBTTaskSwitchState.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "SLFBTTaskSwitchState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBTTaskSwitchState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLFBTTaskSwitchState();

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag NewState;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
