// SLFActionScrollWheelLeftHand.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionScrollWheelLeftHand.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionScrollWheelLeftHand : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionScrollWheelLeftHand();
	virtual void ExecuteAction_Implementation() override;
};
