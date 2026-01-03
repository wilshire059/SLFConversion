// B_Buff.h
// C++ class for Blueprint B_Buff
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Buff.json
// Parent: Object -> UObject
// Variables: 3 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
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
#include "B_Buff.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_Buff : public UObject
{
	GENERATED_BODY()

public:
	UB_Buff();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 Rank;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* BuffData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	AActor* OwnerActor;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Buff")
	void GetOwnerStatManager(UAC_StatManager*& OutReturnValue, UAC_StatManager*& OutReturnValue1);
	virtual void GetOwnerStatManager_Implementation(UAC_StatManager*& OutReturnValue, UAC_StatManager*& OutReturnValue1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Buff")
	void GetMultiplierForCurrentRank(double& OutReturnValue, double& OutReturnValue1);
	virtual void GetMultiplierForCurrentRank_Implementation(double& OutReturnValue, double& OutReturnValue1);

	// Event OnGranted - Called when buff is granted to the owner
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Buff")
	void EventOnGranted();
	virtual void EventOnGranted_Implementation();

	// Event OnRemoved - Called when buff is removed from the owner
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Buff")
	void EventOnRemoved();
	virtual void EventOnRemoved_Implementation();

	// Helper to get the tag from BuffData (for tag-based queries)
	UFUNCTION(BlueprintCallable, Category = "B_Buff")
	FGameplayTag GetBuffTag() const;
};
