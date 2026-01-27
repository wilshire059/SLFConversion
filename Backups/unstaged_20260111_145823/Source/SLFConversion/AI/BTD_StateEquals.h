// BTD_StateEquals.h
// Custom BTDecorator that checks ESLFAIStates and logs every evaluation
// This gives us runtime visibility into why the behavior tree isn't selecting Combat

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SLFConversion/SLFGameTypes.h"
#include "BTD_StateEquals.generated.h"

UCLASS()
class SLFCONVERSION_API UBTD_StateEquals : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_StateEquals();

	// The state value to check against
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	ESLFAIStates RequiredState;

	// The blackboard key for the State value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FName StateKeyName;

	// Enable verbose logging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bEnableLogging;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
