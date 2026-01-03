// SLFStatStamina.h
// C++ base for B_Stamina - Stamina stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Stamina
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Stamina tag, 100 current/max, regenerates
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatStamina.generated.h"

/**
 * USLFStatStamina - Stamina stat
 * Resource for actions like attacks, dodges, and sprinting
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatStamina : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatStamina();
};
