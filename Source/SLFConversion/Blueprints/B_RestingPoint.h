// B_RestingPoint.h
// C++ class for Blueprint B_RestingPoint
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Blueprint/B_RestingPoint.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 10 | Functions: 2 | Dispatchers: 2 | Interface: ISLFRestingPointInterface

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Interfaces/SLFRestingPointInterface.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"
#include "B_RestingPoint.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_RestingPoint_OnReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_RestingPoint_OnExited);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_RestingPoint : public AB_Interactable, public ISLFRestingPointInterface
{
	GENERATED_BODY()

public:
	AB_RestingPoint();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (from SCS)
	// ═══════════════════════════════════════════════════════════════════════

	/** Billboard component that defines the sitting zone position */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SittingZone;

	/** Scene component for spawn position */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	/** Camera arm for rest view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraArm;

	/** Lookat camera for rest view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* LookatCamera;

	/** Unlock effect (Niagara) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* UnlockEffect;

	/** Active effect (Niagara) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* Effect;

	/** Point light for effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* EffectLight;

	/** Environment point light */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* EnvironmentLight;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Point Settings")
	FText LocationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UAnimMontage> DiscoverInteractionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	double SittingAngle;

	/** The actor currently sitting at this resting point (usually the player) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* SittingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	bool ForceFaceSittingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	double CameraDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	FRotator CameraRotation;

	/** Set of items to replenish when resting (e.g., health flasks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Replenish Settings")
	TSet<UPrimaryDataAsset*> ItemsToReplenish;

	/** Tags of stats to replenish when resting (e.g., HP, FP) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Replenish Settings")
	FGameplayTagContainer StatsToReplenish;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_RestingPoint_OnReady OnReady;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_RestingPoint_OnExited OnExited;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get the items and stats to replenish when resting */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_RestingPoint")
	void GetReplenishData(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish);
	virtual void GetReplenishData_Implementation(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish);

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE: ISLFRestingPointInterface
	// ═══════════════════════════════════════════════════════════════════════

	/** Get spawn position for respawning at this resting point */
	virtual void GetRestingPointSpawnPosition_Implementation(bool& OutSuccess, FVector& OutLocation, FRotator& OutRotation) override;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE: ISLFInteractableInterface (EventGraph Logic)
	// ═══════════════════════════════════════════════════════════════════════

	/** Handle interaction - either discover (first time) or rest (subsequent) */
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

	/** Update interaction text based on IsActivated state */
	virtual void OnTraced_Implementation(AActor* TracedBy) override;

	// ═══════════════════════════════════════════════════════════════════════
	// CUSTOM FUNCTIONS (EventGraph)
	// ═══════════════════════════════════════════════════════════════════════

	/** Called when player discovers this resting point for the first time */
	UFUNCTION(BlueprintCallable, Category = "B_RestingPoint")
	void DiscoverPoint(AActor* DiscoveringActor);

protected:
	/** Called after delay when sitting down - positions actor at SittingZone */
	void PositionSittingActor();

	/** Timer handle for delayed positioning */
	FTimerHandle PositionTimerHandle;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTION (UserConstructionScript)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void OnConstruction(const FTransform& Transform) override;
};
