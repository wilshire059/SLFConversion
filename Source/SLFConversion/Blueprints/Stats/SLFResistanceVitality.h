// SLFResistanceVitality.h
// C++ base for B_Resistance_Vitality - Death/instant kill resistance
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Resistance_Vitality
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Resistance to death blight effects
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFResistanceVitality.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFResistanceVitality : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFResistanceVitality();
};
