// SLFStatStrength.h
// C++ base for B_Strength - Strength primary attribute
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Strength
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Strength tag, affects physical damage and equip load
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatStrength.generated.h"

/**
 * USLFStatStrength - Strength attribute
 * Increases physical damage scaling and equip load
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatStrength : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatStrength();
};
