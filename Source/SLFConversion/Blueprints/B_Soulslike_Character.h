// B_Soulslike_Character.h
// C++ class for Blueprint B_Soulslike_Character
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_Character.json
// Parent: B_BaseCharacter_C -> AB_BaseCharacter
// Variables: 13 | Functions: 5 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_BaseCharacter.h"
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
#include "Interfaces/SLFPlayerInterface.h"
#include "B_Soulslike_Character.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_Soulslike_Character_OnInputDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FB_Soulslike_Character_OnInteractPressed, bool, Pressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_Soulslike_Character_OnCameraResetFinished);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_Character : public AB_BaseCharacter, public ISLFPlayerInterface
{
	GENERATED_BODY()

public:
	AB_Soulslike_Character();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (13)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Keybindings")
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Camera")
	double CameraPitchLock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "References")
	UPrimaryDataAsset* CustomGameSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh Merging")
	UPrimaryDataAsset* DefaultMeshInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh Merging")
	FSkeletalMeshMergeParams MeshMergeData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	bool Cache_isHoldingInteract;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	bool Cache_isHoldingSprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double Cache_InteractElapsed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double Cache_SpringArmLength;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	bool Cache_IsAsyncEquipmentBusy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	ULevelSequencePlayer* Cache_ActiveCameraSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	FRotator Cache_ControlRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh Merging")
	bool IsMeshInitialized;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Soulslike_Character_OnInputDetected OnInputDetected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Soulslike_Character_OnInteractPressed OnInteractPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Soulslike_Character_OnCameraResetFinished OnCameraResetFinished;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Character")
	void HandleTargetLock();
	virtual void HandleTargetLock_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Character")
	double GetAxisValue(double AxisValue, bool X_Axis);
	virtual double GetAxisValue_Implementation(double AxisValue, bool X_Axis);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Character")
	void MakeModularMeshData();
	virtual void MakeModularMeshData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Character")
	bool GetMeshInitialized();
	virtual bool GetMeshInitialized_Implementation();
};
