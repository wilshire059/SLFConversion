// SLFActionWeaponAbility.h
// Weapon ability/skill action - consumes FP, plays montage, spawns effect
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionWeaponAbility.generated.h"

class UPDA_WeaponAbility;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionWeaponAbility : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionWeaponAbility();
	virtual void ExecuteAction_Implementation() override;

protected:
	/**
	 * BeginSpecialAttack - Execute the weapon ability
	 * Matches bp_only "Event BeginSpecialAttack" custom event
	 *
	 * - Plays the ability montage using PlaySoftMontageReplicated
	 * - Spawns AdditionalEffectClass if valid
	 * - Consumes the AffectedStat (usually FP) by the Cost amount
	 */
	void BeginSpecialAttack(UPDA_WeaponAbility* Ability);
};
