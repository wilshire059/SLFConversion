// SLFActionJumpAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionJumpAttack.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionJumpAttack : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionJumpAttack();
	virtual void ExecuteAction_Implementation() override;
};
