// B_RestingPoint.h
// C++ class for Blueprint B_RestingPoint
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_RestingPoint.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 10 | Functions: 2 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
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
#include "Interfaces/SLFRestingPointInterface.h"
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

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Point Settings")
	FText LocationName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UAnimMontage> DiscoverInteractionMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|View Settings")
	double SittingAngle;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Replenish Settings")
	UPrimaryDataAsset* ItemsToReplenish;
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
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_RestingPoint")
	void GetReplenishData(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish);
	virtual void GetReplenishData_Implementation(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish);
};
