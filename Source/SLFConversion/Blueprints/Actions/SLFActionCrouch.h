// SLFActionCrouch.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionCrouch.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionCrouch : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionCrouch();
	virtual void ExecuteAction_Implementation() override;
};
