// SLFStatStance.h
// C++ base for B_Stance - Stance/guard break stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Stance
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Stance tag, determines guard break threshold
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatStance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatStance : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatStance();
};
