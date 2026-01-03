// SLFNPCShowcaseVendor.h
// C++ base class for B_Soulslike_NPC_ShowcaseVendor
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_NPC_ShowcaseVendor
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 migrated
// Functions:         0/0 migrated (BeginPlay uses Super)
// Event Dispatchers: 0/0
// Components:        4/4 (Head, Body, Arms, Legs SkeletalMeshComponents)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor
// Parent: B_Soulslike_NPC_C -> ASLFSoulslikeNPC
//
// PURPOSE: Showcase vendor NPC - NPC with modular mesh parts for shops

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SkeletalMergingLibrary.h"
#include "SLFNPCShowcaseVendor.generated.h"

/**
 * Showcase Vendor NPC - NPC with modular skeletal mesh parts
 * Components (Head, Body, Arms, Legs) stay in Blueprint to preserve configured values
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFNPCShowcaseVendor : public ASLFSoulslikeNPC
{
	GENERATED_BODY()

public:
	ASLFNPCShowcaseVendor();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** Mesh merge data for combining modular mesh parts */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FSkeletalMeshMergeParams MeshMergeData;
};
