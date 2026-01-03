// SLFBossMalgareth.h
// C++ base class for B_Soulslike_Boss_Malgareth
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Boss_Malgareth
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 migrated
// Functions:         0/0 migrated (OnDeath event handling in EventGraph)
// Event Dispatchers: 0/0
// Components:        2/2 (Weapon_L, Weapon_R - values in Blueprint)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth
// Parent: B_Soulslike_Boss_C -> ASLFSoulslikeBoss
//
// PURPOSE: Boss Malgareth - main boss with dual weapons

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeBoss.h"
#include "SLFBossMalgareth.generated.h"

/**
 * Boss Malgareth - Main boss character with dual weapons
 * Components (Weapon_L, Weapon_R) stay in Blueprint to preserve configured values
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBossMalgareth : public ASLFSoulslikeBoss
{
	GENERATED_BODY()

public:
	ASLFBossMalgareth();

protected:
	virtual void BeginPlay() override;
};
