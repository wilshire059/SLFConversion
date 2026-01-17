// B_Destructible.h
// C++ class for Blueprint B_Destructible
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA/Blueprint/B_Destructible.json
// Parent: Actor -> AActor
// Variables: 2 | Components: 2 | Event Handlers: 1

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Components/BillboardComponent.h"
#include "Sound/SoundBase.h"
#include "B_Destructible.generated.h"

/**
 * Destructible actor using Chaos Destruction (GeometryCollection)
 *
 * Blueprint Logic:
 * - OnConstruction: Sets GeometryCollection on the component
 * - OnChaosBreakEvent: Async loads destruction sound and plays at location (once)
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Destructible : public AActor
{
	GENERATED_BODY()

public:
	AB_Destructible();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (2) - From Blueprint SCS
	// ═══════════════════════════════════════════════════════════════════════

	/** GeometryCollection mesh for chaos destruction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGeometryCollectionComponent* GC_DestructibleMesh;

	/** Billboard for editor visualization */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* Billboard;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2) - From Blueprint Variables
	// ═══════════════════════════════════════════════════════════════════════

	/** The GeometryCollection asset to use for this destructible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UGeometryCollection* GeometryCollection;

	/** Sound to play when destroyed (soft reference for async loading) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<USoundBase> DestructionSound;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Handle chaos break event - plays destruction sound once */
	UFUNCTION()
	void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

private:
	/** Ensures destruction sound only plays once */
	bool bHasPlayedDestructionSound;
};
