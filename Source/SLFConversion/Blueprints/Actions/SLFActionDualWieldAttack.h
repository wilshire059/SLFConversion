// SLFActionDualWieldAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionDualWieldAttack.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionDualWieldAttack : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionDualWieldAttack();
	virtual void ExecuteAction_Implementation() override;
};
