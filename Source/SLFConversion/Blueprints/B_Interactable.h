// B_Interactable.h
// C++ class for Blueprint B_Interactable
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Interactable.json
// Parent: Actor -> AActor
// Variables: 5 | Functions: 3 | Dispatchers: 0
//
// COMPONENT OWNERSHIP: Blueprint SCS owns all components.
// C++ only caches references at runtime. See CLAUDE.md for pattern.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "B_Interactable.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Interactable : public AActor, public ISLFInteractableInterface
{
	GENERATED_BODY()

public:
	AB_Interactable();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENT REFERENCES (found at runtime from Blueprint SCS)
	// NOTE: Blueprint SCS owns the actual components (has mesh assignments)
	// C++ caches references in BeginPlay for runtime access
	// ═══════════════════════════════════════════════════════════════════════

	/** Cached reference to static mesh component (from Blueprint SCS "Interactable SM") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InteractableSM;

	/** Cached reference to skeletal mesh component (from Blueprint SCS "Interactable SK") */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* InteractableSK;

	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config|Saving (Generate GUID for Saving)")
	FGuid ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	bool CanBeTraced;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	bool IsActivated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	FText InteractableDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	FText InteractionText;


	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Interactable")
	void AddInteractableStateToSaveData();
	virtual void AddInteractableStateToSaveData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Interactable")
	void AddSpawnedInteractableToSaveData(const FInstancedStruct& AdditionalDataToSave);
	virtual void AddSpawnedInteractableToSaveData_Implementation(const FInstancedStruct& AdditionalDataToSave);

	// ═══════════════════════════════════════════════════════════════════════
	// ISLFInteractableInterface IMPLEMENTATION
	// ═══════════════════════════════════════════════════════════════════════

	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnTraced_Implementation(AActor* TracedBy) override;
	virtual void OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData) override;
	virtual FSLFItemInfo TryGetItemInfo_Implementation() override;
};
