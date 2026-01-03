// SLFStatWeight.h
// C++ base for B_Weight - Equipment weight/encumbrance stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Weight
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Weight tag, current = equipped weight, max = carry capacity
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatWeight.generated.h"

/**
 * USLFStatWeight - Equipment weight stat
 * Tracks current equipment weight vs carry capacity
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatWeight : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatWeight();
};
