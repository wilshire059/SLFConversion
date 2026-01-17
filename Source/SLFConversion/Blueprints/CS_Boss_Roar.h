// CS_Boss_Roar.h
// C++ class for Blueprint CS_Boss_Roar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/CS_Boss_Roar.json
// Parent: LegacyCameraShake (UMatineeCameraShake in UE5)
// Variables: 0 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Shakes/LegacyCameraShake.h"
#include "CS_Boss_Roar.generated.h"

/**
 * Boss Roar Camera Shake
 * Applied when boss roars/enters combat
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UCS_Boss_Roar : public ULegacyCameraShake
{
	GENERATED_BODY()

public:
	UCS_Boss_Roar();
};
