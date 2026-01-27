// SLFPlayerInterface.h
// C++ interface for BPI_Player
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Player.json
// Functions: 23

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UMGSequencePlayMode.h"
#include "SLFPlayerInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Player
 * Generated from JSON with 23 functions
 */
class SLFCONVERSION_API ISLFPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnInteractableTraced(AB_Interactable* Interactable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnLootItem(AB_PickupItem* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnTargetLocked(bool TargetLocked, bool RotateTowards);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnRest(AActor* TargetCampfire);  // AActor* for compatibility with both AB_RestingPoint and ASLFRestingPointBase
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ChangeHeadpiece(const TSoftObjectPtr<USkeletalMesh>& NewMesh);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ChangeArmor(const TSoftObjectPtr<USkeletalMesh>& NewMesh);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ChangeGloves(const TSoftObjectPtr<USkeletalMesh>& NewMesh);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ChangeGreaves(const TSoftObjectPtr<USkeletalMesh>& NewMesh);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ResetHeadpiece();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ResetArmor();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ResetGloves();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ResetGreaves();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ToggleCrouchReplicated();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	AB_Soulslike_Character* GetSoulslikeCharacter();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnNpcTraced(AB_Soulslike_NPC* NPC);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnDialogStarted(UAC_AI_InteractionManager* DialogComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void DiscoverRestingPoint(UAnimMontage* InteractionMontage, AActor* Point);  // AActor* for compatibility with both AB_RestingPoint and ASLFRestingPointBase
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void TriggerChaosField(bool Enable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void PlayExecuteMontage(const TSoftObjectPtr<UAnimMontage>& Montage, const FGameplayTag& ExecutionTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void PlayBackstabMontage(const TSoftObjectPtr<UAnimMontage>& Montage, const FGameplayTag& ExecutionTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void PlayCameraSequence(ULevelSequence* Sequence, const FMovieSceneSequencePlaybackSettings& Settings);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void StopActiveCameraSequence();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void ResetCameraView(double TimeScale);
};
