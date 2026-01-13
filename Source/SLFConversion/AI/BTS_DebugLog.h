// BTS_DebugLog.h
// Debug service that logs blackboard State key every tick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_DebugLog.generated.h"

UCLASS()
class SLFCONVERSION_API UBTS_DebugLog : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_DebugLog();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float LogTimer = 0.0f;
};
