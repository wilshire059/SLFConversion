// SLFResistanceFocus.h
// C++ base for B_Resistance_Focus - Focus resistance stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Resistance_Focus
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Resistance to madness/focus effects
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFResistanceFocus.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFResistanceFocus : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFResistanceFocus();
};
