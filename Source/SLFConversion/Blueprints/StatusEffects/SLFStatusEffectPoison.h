// SLFStatusEffectPoison.h
// C++ base for B_StatusEffect_Poison - Poison DOT effect
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_StatusEffect_Poison
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatusEffectBase)
// Functions:         0/0 (uses base class tick damage)
// Event Dispatchers: 0/0 (inherits from StatusEffectBase)
// CDO Defaults:      Poison DOT configuration
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatusEffectBase.h"
#include "SLFStatusEffectPoison.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatusEffectPoison : public USLFStatusEffectBase
{
	GENERATED_BODY()

public:
	USLFStatusEffectPoison();
};
