// SLFStatIncantationPower.h
// C++ base for B_IncantationPower - Incantation scaling stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_IncantationPower
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Incantation power scaling
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatIncantationPower.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatIncantationPower : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatIncantationPower();
};
