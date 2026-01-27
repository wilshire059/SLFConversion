// B_DeathCurrency.h
// C++ class for Blueprint B_DeathCurrency
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DeathCurrency.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 4 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "NiagaraComponent.h"
#include "B_DeathCurrency.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_DeathCurrency : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_DeathCurrency();

	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENT REFERENCES
	// ═══════════════════════════════════════════════════════════════════════

	/** Cached reference to Niagara component (from Blueprint SCS)
	 *  Named differently than Blueprint variable to avoid property collision during reparent */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* CachedDeathCurrencyNiagara;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 CurrencyAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector PlayerLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTransform CacheTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FInstancedStruct> CacheTest;

	// ═══════════════════════════════════════════════════════════════════════
	// ISLFInteractableInterface OVERRIDE
	// ═══════════════════════════════════════════════════════════════════════

	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData) override;
};
