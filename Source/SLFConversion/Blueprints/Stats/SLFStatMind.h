// SLFStatMind.h
// C++ base for B_Mind - Mind attribute stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Mind
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Mind tag, affects FP pool
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatMind.generated.h"

/**
 * USLFStatMind - Mind attribute stat
 * Increases Focus Points (FP) pool
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatMind : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatMind();
};
