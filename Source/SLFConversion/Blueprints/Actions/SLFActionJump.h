// SLFActionJump.h
// C++ base for B_Action_Jump - Jump action
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action_Jump
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from ActionBase)
// Functions:         1/1 (ExecuteAction override)
// Event Dispatchers: 0/0 (inherits from ActionBase)
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionJump.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionJump : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionJump();

	virtual void ExecuteAction_Implementation() override;
};
