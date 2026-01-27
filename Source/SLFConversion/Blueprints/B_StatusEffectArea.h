// B_StatusEffectArea.h
// C++ class for Blueprint B_StatusEffectArea
//
// 20-PASS VALIDATION: 2026-01-19
// Source: BlueprintDNA_v2/Blueprint/B_StatusEffectArea.json
// Parent: Actor -> AActor
// Variables: 2 | Functions: 0 | Dispatchers: 0
//
// Blueprint Logic:
// - OnComponentBeginOverlap(Box): Check Player tag -> StartBuildup
// - OnComponentEndOverlap(Box): Check Player tag -> StopBuildup

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Components/BoxComponent.h"
#include "B_StatusEffectArea.generated.h"

// Forward declarations
class UPrimaryDataAsset;
class UAC_StatusEffectManager;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_StatusEffectArea : public AActor
{
	GENERATED_BODY()

public:
	AB_StatusEffectArea();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2) - from Blueprint
	// ═══════════════════════════════════════════════════════════════════════

	// The status effect data asset to apply (e.g., DA_StatusEffect_Poison)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	UPrimaryDataAsset* StatusEffectToApply;

	// The rank/intensity of the effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
	int32 EffectRank;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENT REFERENCES - cached from Blueprint SCS
	// ═══════════════════════════════════════════════════════════════════════

	// Cached reference to the Box collision component (owned by Blueprint SCS)
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CachedBoxComponent;

protected:
	// ═══════════════════════════════════════════════════════════════════════
	// OVERLAP HANDLERS - implements Blueprint EventGraph logic
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
