// SLFActionTwoHandedStanceL.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionTwoHandedStanceL.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionTwoHandedStanceL : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionTwoHandedStanceL();
	virtual void ExecuteAction_Implementation() override;
};
