// SLFActionGuardCancel.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionGuardCancel.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionGuardCancel : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionGuardCancel();
	virtual void ExecuteAction_Implementation() override;
};
