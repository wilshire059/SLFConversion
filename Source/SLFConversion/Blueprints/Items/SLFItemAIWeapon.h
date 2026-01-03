// SLFItemAIWeapon.h
// AI variant of weapon - used by enemies
#pragma once

#include "CoreMinimal.h"
#include "SLFItemWeapon.h"
#include "SLFItemAIWeapon.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFItemAIWeapon : public ASLFItemWeapon
{
	GENERATED_BODY()

public:
	ASLFItemAIWeapon();
};
