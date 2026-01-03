// SLFNPCShowcaseGuide.h
// C++ base class for B_Soulslike_NPC_ShowcaseGuide
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_NPC_ShowcaseGuide
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 migrated
// Functions:         0/0 migrated
// Event Dispatchers: 0/0
// Components:        0/0
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide
// Parent: B_Soulslike_NPC_C -> ASLFSoulslikeNPC
//
// PURPOSE: Showcase guide NPC - minimal NPC for tutorial/showcase areas

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SLFNPCShowcaseGuide.generated.h"

/**
 * Showcase Guide NPC - Simple NPC used in showcase/tutorial areas
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFNPCShowcaseGuide : public ASLFSoulslikeNPC
{
	GENERATED_BODY()

public:
	ASLFNPCShowcaseGuide();

protected:
	virtual void BeginPlay() override;
};
