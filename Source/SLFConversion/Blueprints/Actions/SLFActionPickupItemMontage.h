// SLFActionPickupItemMontage.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionPickupItemMontage.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionPickupItemMontage : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionPickupItemMontage();
	virtual void ExecuteAction_Implementation() override;
};
