// SLFBuffAttackPower.h
// C++ base for B_Buff_AttackPower - Attack power buff
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Buff_AttackPower
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from BuffBase)
// Functions:         2/2 (OnGranted, OnRemoved overrides)
// Event Dispatchers: 0/0 (inherits from BuffBase)
// CDO Defaults:      Modifies AttackPower stat
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFBuffBase.h"
#include "SLFBuffAttackPower.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBuffAttackPower : public USLFBuffBase
{
	GENERATED_BODY()

public:
	USLFBuffAttackPower();

	virtual void OnGranted_Implementation() override;
	virtual void OnRemoved_Implementation() override;
};
