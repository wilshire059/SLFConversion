// SLFNPCShowcaseGuide.h
// C++ base class for B_Soulslike_NPC_ShowcaseGuide
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_NPC_ShowcaseGuide
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 migrated (DefaultDialogAsset, NPCDisplayName)
// Functions:         0/0 migrated
// Event Dispatchers: 0/0
// Components:        0/0 (uses parent's AIInteractionManager)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide
// Parent: B_Soulslike_NPC_C -> ASLFSoulslikeNPC
//
// PURPOSE: Showcase guide NPC - tutorial/showcase NPC with dialog (no vendor)
// DIALOG: DA_ExampleDialog - simple talk options, no buy/sell

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SLFNPCShowcaseGuide.generated.h"

/**
 * Showcase Guide NPC - Tutorial/showcase NPC with dialog options
 * Uses DA_ExampleDialog for Talk-based interactions (NOT vendor)
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFNPCShowcaseGuide : public ASLFSoulslikeNPC
{
	GENERATED_BODY()

public:
	ASLFNPCShowcaseGuide();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// NPC CONFIGURATION - Dialog data for this NPC type
	// ═══════════════════════════════════════════════════════════════════

	/** Dialog data asset for this NPC
	 * Default: DA_ExampleDialog - located in ShowcaseGuideNpc folder */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "NPC|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> DefaultDialogAsset;

	/** Display name for this NPC (shown in dialog UI) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "NPC|Dialog")
	FText NPCDisplayName;
};
