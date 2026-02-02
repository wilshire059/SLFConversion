// SLFNPCShowcaseVendor.h
// C++ base class for B_Soulslike_NPC_ShowcaseVendor
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_NPC_ShowcaseVendor
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 migrated (MeshMergeData, DefaultVendorMesh, DefaultDialogAsset, DefaultVendorAsset, NPCDisplayName)
// Functions:         1/1 migrated (BeginPlay - mesh + dialog setup)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor
// Parent: B_Soulslike_NPC_C -> ASLFSoulslikeNPC
//
// PURPOSE: Showcase vendor NPC - sets up character mesh, animations, and vendor dialog
// DIALOG: DA_ExampleDialog_Vendor - vendor-specific options (Buy, Sell, Trade, Leave)
// VENDOR: DA_ExampleVendor - vendor item inventory
// NOTE: Original had modular mesh parts (Head/Body/Arms/Legs) with runtime merge.
//       Simplified to use pre-merged SKM_Quinn_DemoGuide mesh directly.

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SkeletalMergingLibrary.h"
#include "SLFNPCShowcaseVendor.generated.h"

/**
 * Showcase Vendor NPC - Demo vendor character
 * Uses SKM_Quinn_DemoGuide mesh and vendor dialog system
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
	// MESH VARIABLES
	// ═══════════════════════════════════════════════════════════════════

	/** Mesh merge data for runtime mesh merging */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FSkeletalMeshMergeParams MeshMergeData;

	// ═══════════════════════════════════════════════════════════════════
	// MODULAR MESH PARTS - Manny character (bp_only uses mesh merging at runtime)
	// ═══════════════════════════════════════════════════════════════════

	/** Head mesh part */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Mesh")
	TSoftObjectPtr<USkeletalMesh> HeadMesh;

	/** Upper body mesh part */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Mesh")
	TSoftObjectPtr<USkeletalMesh> UpperBodyMesh;

	/** Arms mesh part */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Mesh")
	TSoftObjectPtr<USkeletalMesh> ArmsMesh;

	/** Lower body mesh part */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Mesh")
	TSoftObjectPtr<USkeletalMesh> LowerBodyMesh;

	/** Cached modular component references from Blueprint SCS */
	UPROPERTY(Transient)
	USkeletalMeshComponent* HeadComponent;

	UPROPERTY(Transient)
	USkeletalMeshComponent* BodyComponent;

	UPROPERTY(Transient)
	USkeletalMeshComponent* ArmsComponent;

	UPROPERTY(Transient)
	USkeletalMeshComponent* LegsComponent;

	/** Fallback default skeletal mesh (only used if mesh merge fails) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Mesh")
	TSoftObjectPtr<USkeletalMesh> DefaultVendorMesh;

	// ═══════════════════════════════════════════════════════════════════
	// NPC CONFIGURATION - Dialog and Vendor data
	// ═══════════════════════════════════════════════════════════════════

	/** Dialog data asset for vendor NPC
	 * Default: DA_ExampleDialog_Vendor - Buy/Sell/Trade/Leave options */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> DefaultDialogAsset;

	/** Vendor data asset with items for sale
	 * Default: DA_ExampleVendor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> DefaultVendorAsset;

	/** Display name for this NPC (shown in dialog UI) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vendor|Dialog")
	FText NPCDisplayName;
};
