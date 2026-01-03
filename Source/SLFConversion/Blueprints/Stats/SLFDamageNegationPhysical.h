// SLFDamageNegationPhysical.h
// C++ base for B_DN_Physical - Physical damage negation
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_DN_Physical
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Physical damage reduction percentage
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFDamageNegationPhysical.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFDamageNegationPhysical : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFDamageNegationPhysical();
};
