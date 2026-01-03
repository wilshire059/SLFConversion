// SLFStatArcane.h
// C++ base for B_Arcane - Arcane primary attribute
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Arcane
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Arcane tag, affects item discovery and arcane scaling
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatArcane.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatArcane : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatArcane();
};
