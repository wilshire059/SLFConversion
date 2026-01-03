// SLFStatHP.h
// C++ base for B_HP - Health Points stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_HP
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      HP tag, 500 current/max, can regenerate
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatHP.generated.h"

/**
 * USLFStatHP - Health Points stat
 * Primary defensive stat that determines character health
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatHP : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatHP();
};
