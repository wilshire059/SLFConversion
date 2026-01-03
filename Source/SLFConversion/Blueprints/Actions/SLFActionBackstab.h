// SLFActionBackstab.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionBackstab.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionBackstab : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionBackstab();
	virtual void ExecuteAction_Implementation() override;
};
