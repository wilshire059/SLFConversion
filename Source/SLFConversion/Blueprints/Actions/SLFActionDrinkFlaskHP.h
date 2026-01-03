// SLFActionDrinkFlaskHP.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionDrinkFlaskHP.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionDrinkFlaskHP : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionDrinkFlaskHP();
	virtual void ExecuteAction_Implementation() override;
};
