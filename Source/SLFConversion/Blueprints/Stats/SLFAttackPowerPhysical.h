// SLFAttackPowerPhysical.h
// C++ base for B_AP_Physical - Physical attack power
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_AP_Physical
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Physical attack power scaling
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFAttackPowerPhysical.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAttackPowerPhysical : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFAttackPowerPhysical();
};
