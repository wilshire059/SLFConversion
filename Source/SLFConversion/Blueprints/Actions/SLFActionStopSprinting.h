// SLFActionStopSprinting.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionStopSprinting.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionStopSprinting : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionStopSprinting();
	virtual void ExecuteAction_Implementation() override;
};
