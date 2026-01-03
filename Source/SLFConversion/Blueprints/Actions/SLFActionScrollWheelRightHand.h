// SLFActionScrollWheelRightHand.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionScrollWheelRightHand.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionScrollWheelRightHand : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionScrollWheelRightHand();
	virtual void ExecuteAction_Implementation() override;
};
