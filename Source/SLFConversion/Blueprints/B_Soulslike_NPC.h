// B_Soulslike_NPC.h
// C++ class for Blueprint B_Soulslike_NPC
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/Blueprint/B_Soulslike_NPC.json
// Parent: B_BaseCharacter_C -> AB_BaseCharacter
// Variables: 3 | Functions: 5 interface + 1 local | Dispatchers: 0 | Components: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Interfaces/BPI_NPC.h"
#include "Interfaces/BPI_Interactable.h"
#include "B_Soulslike_NPC.generated.h"

// Forward declarations
class UAIInteractionManagerComponent;
class USphereComponent;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_NPC : public AB_BaseCharacter, public IBPI_NPC, public IBPI_Interactable
{
	GENERATED_BODY()

public:
	AB_Soulslike_NPC();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	/** Whether this NPC can be traced by interaction system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanBeTraced;

	/** Component to look at when interacting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimitiveComponent* LookAtComponent;

	/** NPC unique ID for saving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	FGuid NpcId;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (2)
	// ═══════════════════════════════════════════════════════════════════════

	/** AI Interaction Manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIInteractionManagerComponent* AC_AI_InteractionManager;

	/** Look At Radius sphere component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* LookAtRadius;

	// ═══════════════════════════════════════════════════════════════════════
	// LOCAL FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	/** Get the location to look at */
	UFUNCTION(BlueprintPure, Category = "B_Soulslike_NPC")
	void GetLookAtLocation(FVector& Location);

	// ═══════════════════════════════════════════════════════════════════════
	// BPI_Interactable INTERFACE (4 functions)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void TryGetItemInfo_Implementation(FSLFItemInfo& ItemInfo) override;
	virtual void OnSpawnedFromSave_Implementation(FGuid Id, FInstancedStruct CustomData) override;
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnTraced_Implementation(AActor* TracedBy) override;
};
