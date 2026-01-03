// SLFActionScrollWheelTools.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionScrollWheelTools.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionScrollWheelTools : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionScrollWheelTools();
	virtual void ExecuteAction_Implementation() override;
};
