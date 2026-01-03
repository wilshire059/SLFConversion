// SLFStatPoise.h
// C++ base for B_Poise - Poise/stagger resistance stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Poise
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Poise tag, determines stagger threshold
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatPoise.generated.h"

/**
 * USLFStatPoise - Poise stat
 * Determines resistance to stagger/interruption
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatPoise : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatPoise();
};
