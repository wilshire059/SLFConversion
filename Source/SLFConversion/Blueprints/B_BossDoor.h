// B_BossDoor.h
// C++ class for Blueprint B_BossDoor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_BossDoor.json
// Parent: B_Door_C -> AB_Door
// Variables: 2 | Functions: 4 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Door.h"
#include "Interfaces/SLFBossDoorInterface.h"
#include "NiagaraComponent.h"
#include "Components/BillboardComponent.h"
#include "B_BossDoor.generated.h"

/**
 * Boss Door - Locks the arena during boss fights
 * Implements BPI_BossDoor interface for boss component interaction
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_BossDoor : public AB_Door, public ISLFBossDoorInterface
{
	GENERATED_BODY()

public:
	AB_BossDoor();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════════

	// Front portal fog effect
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* Portal_Front;

	// Back portal fog effect
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* Portal_Back;

	// Location where death currency spawns when boss is defeated
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* DeathCurrencyDropLocation;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// Whether this door can currently be traced/interacted with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bCanBeTraced;

	// Whether this door has been activated (boss fight started)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bIsActivated;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Interface implementations (ISLFBossDoorInterface)
	virtual USceneComponent* GetDeathCurrencySpawnPoint_Implementation() override;
	virtual void UnlockBossDoor_Implementation() override;

	// Override parent door's OnInteract to handle fog gate collision
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

	// Initialize door state from save data
	UFUNCTION(BlueprintCallable, Category = "Boss Door")
	void InitializeLoadedStates(bool bInCanBeTraced, bool bInIsActivated);

protected:
	virtual void BeginPlay() override;

	// Called when parent door opens - re-enables collision
	UFUNCTION()
	void HandleDoorOpened();

private:
	// Helper to find and set collision on the Interactable SM component
	void SetInteractableMeshCollision(bool bEnableCollision);
};
