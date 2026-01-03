// SLFStatFaith.h
// C++ base for B_Faith - Faith primary attribute
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Faith
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from StatBase)
// Functions:         0/0 (inherits from StatBase)
// Event Dispatchers: 0/0 (inherits from StatBase)
// CDO Defaults:      Faith tag, affects holy/incantation damage scaling
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatFaith.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFStatFaith : public USLFStatBase
{
	GENERATED_BODY()

public:
	USLFStatFaith();
};
