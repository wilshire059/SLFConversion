// BTS_DebugLog.h
// Diagnostic BTService that logs every tick to trace BT execution

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTS_DebugLog.generated.h"

UCLASS()
class SLFCONVERSION_API UBTS_DebugLog : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTS_DebugLog();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FString ServiceName;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;
};
