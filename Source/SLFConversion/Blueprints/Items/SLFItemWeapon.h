// SLFItemWeapon.h
// C++ base for B_Item_Weapon - Equipable weapon actor
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Item_Weapon
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 (trail, impact effects, sounds, debug)
// Functions:         0/0 (inherits from WeaponBase)
// Event Dispatchers: 0/0
// CDO Defaults:      Generic weapon configuration
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFWeaponBase.h"
#include "SLFItemWeapon.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFItemWeapon : public ASLFWeaponBase
{
	GENERATED_BODY()

public:
	ASLFItemWeapon();
};
