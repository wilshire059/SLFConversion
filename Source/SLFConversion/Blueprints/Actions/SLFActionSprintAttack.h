// SLFActionSprintAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionSprintAttack.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionSprintAttack : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionSprintAttack();
	virtual void ExecuteAction_Implementation() override;
};
