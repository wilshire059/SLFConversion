// SLFStatDexterity.h
// C++ base for B_Dexterity - Dexterity primary attribute
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Dexterity
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Dexterity tag, affects dex weapon scaling
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatDexterity.generated.h"

/**
 * USLFStatDexterity - Dexterity attribute
 * Increases dexterity weapon damage scaling
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatDexterity : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatDexterity();
};
