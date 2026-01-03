// B_Door.h
// C++ class for Blueprint B_Door
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Door.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 9 | Functions: 3 | Dispatchers: 1

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
#include "B_Door.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_Door_OnDoorOpened);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Door : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_Door();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (9)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Config")
	TSoftObjectPtr<UAnimMontage> InteractMontage_RH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Config")
	TSoftObjectPtr<UAnimMontage> InteractMontage_LH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Config")
	FSLFDoorLockInfo LockInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FRotator Cache_Rotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door Config")
	double MoveToDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Config")
	double YawRotationAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Config")
	double PlayRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool OpenForwards;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAnimMontage* SelectedMontage;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Door_OnDoorOpened OnDoorOpened;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Door")
	void ValidateUnlockRequirements(AActor* Actor, bool& OutSuccess, bool& OutSuccess1, bool& OutSuccess2, bool& OutSuccess3);
	virtual void ValidateUnlockRequirements_Implementation(AActor* Actor, bool& OutSuccess, bool& OutSuccess1, bool& OutSuccess2, bool& OutSuccess3);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Door")
	FText GetRequiredItemsParsed();
	virtual FText GetRequiredItemsParsed_Implementation();
};
