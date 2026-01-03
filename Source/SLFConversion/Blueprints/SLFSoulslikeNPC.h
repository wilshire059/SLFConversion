// SLFSoulslikeNPC.h
// C++ base class for B_Soulslike_NPC
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_NPC
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         3/3 migrated
// Functions:         2/2 migrated (GetLookAtLocation, TeleportToLocation)
// Event Dispatchers: 0/0
// Interfaces:        2/2 (ISLFNPCInterface, ISLFInteractableInterface)
// Interface Funcs:   4/4 (OnTraced, OnInteract, OnSpawnedFromSave, TryGetItemInfo)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC
//
// PURPOSE: Base NPC character - look-at targeting, dialogue, interaction
// PARENT: ASLFBaseCharacter
// INTERFACES: BPI_NPC (ISLFNPCInterface), BPI_Interactable (ISLFInteractableInterface)

#pragma once

#include "CoreMinimal.h"
#include "SLFBaseCharacter.h"
#include "Interfaces/SLFNPCInterface.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "SLFSoulslikeNPC.generated.h"

// Forward declarations
class UPrimitiveComponent;

/**
 * Soulslike NPC character - look-at targeting, dialogue system, interaction
 * Implements BPI_NPC (ISLFNPCInterface) and BPI_Interactable (ISLFInteractableInterface)
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSoulslikeNPC : public ASLFBaseCharacter, public ISLFNPCInterface, public ISLFInteractableInterface
{
	GENERATED_BODY()

public:
	ASLFSoulslikeNPC();

	/** Look-at component for targeting (AnimBP property access) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UPrimitiveComponent* LookAtComponent;

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 3/3 (renamed to avoid Blueprint collision)
	// Original names: CanBeTraced?, LookAtComponent, NpcId
	// ═══════════════════════════════════════════════════════════════════

	/** Whether NPC can be traced/targeted (Original: "CanBeTraced?") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsTraceable;

	/** Unique identifier for this NPC (Original: "NpcId") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	FGuid SavedNpcId;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** Get the world location for look-at targeting
	 * @param Location Output location in world space for look-at target
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "NPC")
	void GetLookAtLocation(FVector& Location);
	virtual void GetLookAtLocation_Implementation(FVector& Location);

	/** Teleport NPC to a new location and rotation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC")
	void TeleportToLocation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport);
	virtual void TeleportToLocation_Implementation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport);

	// ═══════════════════════════════════════════════════════════════════
	// INTERFACE FUNCTIONS: ISLFInteractableInterface (4/4)
	// ═══════════════════════════════════════════════════════════════════

	/** Called when this actor is traced by another actor */
	virtual void OnTraced_Implementation(AActor* TracedBy) override;

	/** Called when this actor is interacted with */
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

	/** Called when this actor is spawned from a save game */
	virtual void OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData) override;

	/** Try to get item info for this interactable */
	virtual FSLFItemInfo TryGetItemInfo_Implementation() override;

protected:
	/** Cached look-at component (found from Blueprint's LookAtRadius sphere) */
	UPROPERTY(Transient)
	UPrimitiveComponent* CachedLookAtComponent;
};
