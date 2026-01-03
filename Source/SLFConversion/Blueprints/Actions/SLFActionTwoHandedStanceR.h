// SLFActionTwoHandedStanceR.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionTwoHandedStanceR.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionTwoHandedStanceR : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionTwoHandedStanceR();
	virtual void ExecuteAction_Implementation() override;
};
