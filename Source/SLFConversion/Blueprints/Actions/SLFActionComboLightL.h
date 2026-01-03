// SLFActionComboLightL.h
// C++ base for B_Action_ComboLight_L - Left-hand light attack combo
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action_ComboLight_L
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from ActionBase)
// Functions:         1/1 (ExecuteAction override)
// Event Dispatchers: 0/0 (inherits from ActionBase)
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionComboLightL.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionComboLightL : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionComboLightL();
	virtual void ExecuteAction_Implementation() override;
};
