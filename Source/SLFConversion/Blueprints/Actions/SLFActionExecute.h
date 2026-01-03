// SLFActionExecute.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionExecute.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionExecute : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionExecute();
	virtual void ExecuteAction_Implementation() override;
};
