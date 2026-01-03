// SLFResistanceRobustness.h
// C++ base for B_Resistance_Robustness - Bleed/frost resistance
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Resistance_Robustness
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Resistance to bleed/frostbite effects
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFResistanceRobustness.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFResistanceRobustness : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFResistanceRobustness();
};
