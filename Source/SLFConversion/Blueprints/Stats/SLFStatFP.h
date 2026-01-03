// SLFStatFP.h
// C++ base for B_FP - Focus Points stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_FP
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      FP tag, 100 current/max, no regen
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatFP.generated.h"

/**
 * USLFStatFP - Focus Points stat
 * Resource for magic and special abilities
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatFP : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatFP();
};
