// SLFStatDeathCurrency.h
// C++ base for B_DeathCurrency - Souls/currency stat
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_DeathCurrency
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Currency dropped on death, no max
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatDeathCurrency.generated.h"

/**
 * USLFStatDeathCurrency - Death currency (souls) stat
 * Currency used for leveling up and purchases, dropped on death
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatDeathCurrency : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatDeathCurrency();
};
