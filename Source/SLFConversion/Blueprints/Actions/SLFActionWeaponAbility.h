// SLFActionWeaponAbility.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionWeaponAbility.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionWeaponAbility : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionWeaponAbility();
	virtual void ExecuteAction_Implementation() override;
};
