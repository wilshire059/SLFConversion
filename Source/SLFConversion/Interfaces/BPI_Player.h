// BPI_Player.h
// C++ Interface for BPI_Player
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Player.json
// Functions: 23 (ALL functions from JSON - no made-up functions)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "BPI_Player.generated.h"

// Forward declarations
class ULevelSequence;
class UAnimMontage;
class UActorComponent;
class USkeletalMesh;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Player : public UInterface
{
	GENERATED_BODY()
};

/**
 * Player Interface
 * Provides player-specific functionality for camera, execution, equipment, and events
 * Contains exactly 23 functions as defined in Blueprint JSON
 */
class SLFCONVERSION_API IBPI_Player
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// CAMERA FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	// Reset camera view with time scale
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ResetCameraView(double TimeScale);

	// Stop active camera sequence
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void StopActiveCameraSequence();

	// Play camera sequence with settings
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void PlayCameraSequence(ULevelSequence* Sequence, FMovieSceneSequencePlaybackSettings Settings);

	// ═══════════════════════════════════════════════════════════════════════
	// EXECUTION/COMBAT FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Play backstab montage with execution tag
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void PlayBackstabMontage(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag);

	// Play execute montage with execution tag
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void PlayExecuteMontage(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag);

	// ═══════════════════════════════════════════════════════════════════════
	// CHAOS/PHYSICS FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	// Trigger chaos field enable/disable
	// NOTE: Parameter name matches Blueprint interface exactly (Enable?)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void TriggerChaosField(bool Enable);

	// ═══════════════════════════════════════════════════════════════════════
	// RESTING POINT FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Discover a resting point with interaction montage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void DiscoverRestingPoint(UAnimMontage* InteractionMontage, AActor* Point);

	// Called when resting at campfire
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnRest(AActor* TargetCampfire);

	// ═══════════════════════════════════════════════════════════════════════
	// DIALOG/NPC FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Called when dialog starts with NPC
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnDialogStarted(UActorComponent* DialogComponent);

	// Called when NPC is traced by interaction system
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnNpcTraced(AActor* NPC);

	// ═══════════════════════════════════════════════════════════════════════
	// GETTER FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	// Get soulslike character reference
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void GetSoulslikeCharacter(ACharacter*& B_SoulslikeCharacter);

	// ═══════════════════════════════════════════════════════════════════════
	// MOVEMENT FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	// Toggle crouch replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ToggleCrouchReplicated();

	// ═══════════════════════════════════════════════════════════════════════
	// EQUIPMENT RESET FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	// Reset greaves to default
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ResetGreaves();

	// Reset gloves to default
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ResetGloves();

	// Reset armor to default
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ResetArmor();

	// Reset headpiece to default
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ResetHeadpiece();

	// ═══════════════════════════════════════════════════════════════════════
	// EQUIPMENT CHANGE FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	// Change greaves mesh
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ChangeGreaves(const TSoftObjectPtr<USkeletalMesh>& NewMesh);

	// Change gloves mesh
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ChangeGloves(const TSoftObjectPtr<USkeletalMesh>& NewMesh);

	// Change armor mesh
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ChangeArmor(const TSoftObjectPtr<USkeletalMesh>& NewMesh);

	// Change headpiece mesh
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void ChangeHeadpiece(const TSoftObjectPtr<USkeletalMesh>& NewMesh);

	// ═══════════════════════════════════════════════════════════════════════
	// TARGET/LOCK-ON FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	// Called when target is locked/unlocked
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnTargetLocked(bool bTargetLocked, bool bRotateTowards);

	// ═══════════════════════════════════════════════════════════════════════
	// ITEM/INTERACTION FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// Called when looting an item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnLootItem(AActor* Item);

	// Called when interactable is traced
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interface")
	void OnInteractableTraced(AActor* Interactable);
};
