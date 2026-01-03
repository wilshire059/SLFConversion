// SLFResistanceImmunity.h
// C++ base for B_Resistance_Immunity - Poison/rot resistance
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Resistance_Immunity
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Resistance to poison/plague effects
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFResistanceImmunity.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFResistanceImmunity : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFResistanceImmunity();
};
