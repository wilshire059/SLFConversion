// SLFActionThrowProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionThrowProjectile.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionThrowProjectile : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionThrowProjectile();
	virtual void ExecuteAction_Implementation() override;
};
