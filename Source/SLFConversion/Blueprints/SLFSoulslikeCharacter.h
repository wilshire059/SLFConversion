// SLFSoulslikeCharacter.h
// C++ base class for B_Soulslike_Character
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Character
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 migrated (excluding 3 dispatchers counted separately)
// Functions:         4/4 migrated (excluding EventGraph)
// Event Dispatchers: 3/3 migrated
// Graphs:            8 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Soulslike_Character
//
// PURPOSE: Player character base - input handling, camera, equipment
// PARENT: B_BaseCharacter

#pragma once

#include "CoreMinimal.h"
#include "SLFBaseCharacter.h"
#include "Interfaces/BPI_Player.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "SLFSoulslikeCharacter.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class UDataAsset;
class UPDA_DefaultMeshData;
class ULevelSequence;
class ULevelSequencePlayer;
class UPrimitiveComponent;
class UInputBufferComponent;
class UAC_ActionManager;
class UAC_CombatManager;
class UAC_InteractionManager;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

/**
 * Modular mesh data for character customization
 */
USTRUCT(BlueprintType)
struct FSLFModularMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TArray<USkeletalMesh*> MeshParts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeleton* Skeleton;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 3/3 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/3] Called when any input is detected */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputDetected);

/** [2/3] Called when interact is pressed - param is elapsed hold time */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractPressed, float, ElapsedTime);

/** [3/3] Called when camera reset sequence finishes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraResetFinished);

/**
 * Soulslike player character - input, camera, mesh customization
 * Implements IBPI_Player interface for camera, execution, equipment, and events
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSoulslikeCharacter : public ASLFBaseCharacter, public IBPI_Player
{
	GENERATED_BODY()

public:
	ASLFSoulslikeCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Crouch state change handlers - update AnimInstance
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// Helper to update AnimInstance crouch state
	void UpdateAnimInstanceCrouchState(bool bCrouching);

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Input Config (1) ---

	/** [1/13] Player input mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Input")
	UInputMappingContext* PlayerMappingContext;

	// --- Camera Config (1) ---

	/** [2/13] Camera pitch lock value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Camera")
	double CameraPitchLock;

	// --- Settings (2) ---

	/** [3/13] Custom game settings data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings")
	UDataAsset* CustomGameSettings;

	/** [4/13] Default mesh information data asset (PDA_DefaultMeshData or derived) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings")
	UDataAsset* DefaultMeshInfo;

	// --- Mesh Merge (1) ---

	/** [5/13] Skeletal mesh merge parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Mesh")
	FSkeletalMeshMergeParams MeshMergeData;

	// --- Cache (8) ---

	/** [6/13] Cached interact hold state */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	bool bCache_IsHoldingInteract;

	/** [7/13] Cached sprint hold state */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	bool bCache_IsHoldingSprint;

	/** [8/13] Cached interact elapsed time */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	double Cache_InteractElapsed;

	/** [9/13] Cached spring arm length */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	double Cache_SpringArmLength;

	/** [10/13] Whether async equipment is busy */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	bool bCache_IsAsyncEquipmentBusy;

	/** [11/13] Active camera sequence player */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	ULevelSequencePlayer* Cache_ActiveCameraSequence;

	/** [12/13] Cached control rotation */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	FRotator Cache_ControlRotation;

	/** [13/13] Whether mesh has been initialized */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	bool bIsMeshInitialized;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInputDetected OnInputDetected;

	/** [2/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractPressed OnInteractPressed;

	/** [3/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCameraResetFinished OnCameraResetFinished;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/4] Handle target lock input
	 * Manages target locking behavior for combat
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character|Combat")
	void HandleTargetLock();
	virtual void HandleTargetLock_Implementation();

	/** [2/4] Get axis input value
	 * @param AxisName - Name of the input axis
	 * @return Current axis value
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character|Input")
	float GetAxisValue(FName AxisName);
	virtual float GetAxisValue_Implementation(FName AxisName);

	/** [3/4] Create modular mesh data from parts
	 * @return Constructed modular mesh data struct
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character|Mesh")
	FSLFModularMeshData MakeModularMeshData();
	virtual FSLFModularMeshData MakeModularMeshData_Implementation();

	/** [4/4] Get mesh initialization state
	 * @return Whether mesh has been initialized
	 */
	UFUNCTION(BlueprintPure, Category = "Character|Mesh")
	bool GetMeshInitialized() const { return bIsMeshInitialized; }

	/** Refresh merged mesh after equipment changes
	 * Collects meshes from modular components, merges them, and applies to main skeleton
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|Mesh")
	void RefreshModularPieces();

	// ═══════════════════════════════════════════════════════════════════
	// INPUT ACTIONS (Enhanced Input)
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Dodge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Guard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Interact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_TargetLock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
	UInputAction* IA_Crouch;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENT CACHE (populated in BeginPlay)
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadOnly, Category = "Character|Components")
	UInputBufferComponent* CachedInputBuffer;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Components")
	UAC_ActionManager* CachedActionManager;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Components")
	UAC_CombatManager* CachedCombatManager;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Components")
	UAC_InteractionManager* CachedInteractionManager;

	// NOTE: CachedStatManager is inherited from ASLFBaseCharacter (UStatManagerComponent*)
	// Use the inherited variable for stat checks like IsStatMoreThan()

	// Sprint/Dodge timing (for tap vs hold detection)
	// Original Blueprint: If ElapsedSeconds <= 0.2, it's a dodge tap
	double SprintDodgeStartTime;

	// ═══════════════════════════════════════════════════════════════════
	// CAMERA COMPONENTS (from JSON Components section)
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USpringArmComponent> CachedCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> CachedFollowCamera;

	// ═══════════════════════════════════════════════════════════════════
	// MODULAR MESH COMPONENTS (defined in Blueprint SCS, cached here)
	// NOTE: NOT UPROPERTY - Blueprint already has variables with these names
	// These are cache pointers populated in CacheComponentReferences()
	// Used by equipment change functions (ChangeGreaves, ChangeArmor, etc.)
	// ═══════════════════════════════════════════════════════════════════

	USkeletalMeshComponent* Head;
	USkeletalMeshComponent* UpperBody;
	USkeletalMeshComponent* LowerBody;
	USkeletalMeshComponent* Arms;

	// ═══════════════════════════════════════════════════════════════════
	// BPI_PLAYER INTERFACE IMPLEMENTATIONS (23 functions)
	// ═══════════════════════════════════════════════════════════════════

	// Camera Functions (3)
	virtual void ResetCameraView_Implementation(double TimeScale) override;
	virtual void StopActiveCameraSequence_Implementation() override;
	virtual void PlayCameraSequence_Implementation(ULevelSequence* Sequence, FMovieSceneSequencePlaybackSettings Settings) override;

	// Execution/Combat Functions (2)
	virtual void PlayBackstabMontage_Implementation(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag) override;
	virtual void PlayExecuteMontage_Implementation(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag) override;

	// Chaos/Physics Functions (1)
	virtual void TriggerChaosField_Implementation(bool Enable) override;

	// Resting Point Functions (2)
	virtual void DiscoverRestingPoint_Implementation(UAnimMontage* InteractionMontage, AActor* Point) override;
	virtual void OnRest_Implementation(AActor* TargetCampfire) override;

	// Dialog/NPC Functions (2)
	virtual void OnDialogStarted_Implementation(UActorComponent* DialogComponent) override;
	virtual void OnNpcTraced_Implementation(AActor* NPC) override;

	// Getter Functions (1)
	virtual void GetSoulslikeCharacter_Implementation(ACharacter*& B_SoulslikeCharacter) override;

	// Movement Functions (1)
	virtual void ToggleCrouchReplicated_Implementation() override;

	// Equipment Reset Functions (4)
	virtual void ResetGreaves_Implementation() override;
	virtual void ResetGloves_Implementation() override;
	virtual void ResetArmor_Implementation() override;
	virtual void ResetHeadpiece_Implementation() override;

	// Equipment Change Functions (4)
	virtual void ChangeGreaves_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh) override;
	virtual void ChangeGloves_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh) override;
	virtual void ChangeArmor_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh) override;
	virtual void ChangeHeadpiece_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh) override;

	// Target/Lock-On Functions (1)
	virtual void OnTargetLocked_Implementation(bool bTargetLocked, bool bRotateTowards) override;

	// Item/Interaction Functions (2)
	virtual void OnLootItem_Implementation(AActor* Item) override;
	virtual void OnInteractableTraced_Implementation(AActor* Interactable) override;

protected:
	// ═══════════════════════════════════════════════════════════════════
	// INPUT HANDLING (from EventGraph INPUT & BUFFERING section)
	// ═══════════════════════════════════════════════════════════════════

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Called when input buffer consumes an action - dispatches to ActionManager */
	UFUNCTION()
	void OnInputBufferConsumed(FGameplayTag Action);

	/** Cache component references for faster access */
	void CacheComponentReferences();

	/** Initialize modular mesh from DefaultMeshInfo and merge to main skeleton */
	void InitializeModularMesh();

	// --- Input Action Handlers ---

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleSprintStarted();
	void HandleSprintCompleted();
	void HandleDodge();
	void HandleAttack();
	void HandleGuardStarted();
	void HandleGuardCompleted();
	void HandleInteractStarted();
	void HandleInteractCompleted();
	void HandleTargetLockInput();
	void HandleCrouch();

	/** Queue action to input buffer (checks buffer state) */
	void QueueActionToBuffer(const FGameplayTag& ActionTag);

	/** Execute action immediately (bypasses buffer) */
	void ExecuteActionImmediately(const FGameplayTag& ActionTag);

	/** Update AnimInstance overlay states from EquipmentManager
	 * Called each tick to sync overlay states for weapon animations
	 * Uses reflection to set Blueprint enum variables since AnimBP isn't reparented
	 */
	void UpdateAnimInstanceOverlayStates();

	/** Tick-based target lock rotation
	 * Continuously rotates the character to face the locked target
	 * Uses RInterpTo with TargetLockStrength for smooth rotation
	 * @param DeltaTime - Frame time for interpolation
	 */
	void TickTargetLockRotation(float DeltaTime);
};
