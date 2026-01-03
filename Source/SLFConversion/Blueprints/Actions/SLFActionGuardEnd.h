// SLFActionGuardEnd.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionGuardEnd.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionGuardEnd : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionGuardEnd();
	virtual void ExecuteAction_Implementation() override;
};
