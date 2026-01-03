// SLFEnemyGuard.h
// C++ base class for B_Soulslike_Enemy_Guard
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Enemy_Guard
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 migrated
// Functions:         0/0 migrated (OnDeath event handling in EventGraph)
// Event Dispatchers: 0/0
// Components:        4/4 (RimLight, AreaLight, Lantern, Weapon - values in Blueprint)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard
// Parent: B_Soulslike_Enemy_C -> ASLFSoulslikeEnemy
//
// PURPOSE: Guard enemy - enemy type with lighting and weapon components

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "SLFEnemyGuard.generated.h"

/**
 * Enemy Guard - Guard enemy with lighting and weapon
 * Components (RimLight, AreaLight, Lantern, Weapon) stay in Blueprint to preserve configured values
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemyGuard : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFEnemyGuard();

protected:
	virtual void BeginPlay() override;
};
