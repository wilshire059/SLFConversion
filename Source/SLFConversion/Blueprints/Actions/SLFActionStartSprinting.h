// SLFActionStartSprinting.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionStartSprinting.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionStartSprinting : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionStartSprinting();
	virtual void ExecuteAction_Implementation() override;
};
