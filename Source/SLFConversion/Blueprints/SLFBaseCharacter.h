// SLFBaseCharacter.h
// C++ base class for B_BaseCharacter
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_BaseCharacter
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         6+ migrated (IK, cache, components)
// Functions:         33 interface functions
// Event Dispatchers: 3 migrated
// Interfaces:        BPI_GenericCharacter (33 functions)
// Components:        7 (StatManager, StatusEffectManager, BuffManager, etc.)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter
//
// PURPOSE: Base character for all character types (player, enemy, NPC)
// PARENT: ACharacter (Engine)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SceneComponent.h"
#include "SLFBaseCharacter.generated.h"

// Forward declarations
class UStatManagerComponent;
class UStatusEffectManagerComponent;
class UAC_StatusEffectManager;
class UBuffManagerComponent;
class ULadderManagerComponent;
class UAC_CombatManager;
class UNiagaraSystem;
class UCameraShakeBase;
class UAnimMontage;
class AB_PickupItem;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 3/3 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/3] Called when location lerp animation ends */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationLerpEnd);

/** [2/3] Called when rotation lerp animation ends */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotationLerpEnd);

/** [3/3] Called when both location and rotation lerp animations end */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationRotationLerpEnd);

/**
 * Base character class for all Soulslike characters
 * Provides IK, caching, lerp, and all BPI_GenericCharacter interface functions
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBaseCharacter : public ACharacter, public IBPI_GenericCharacter
{
	GENERATED_BODY()

public:
	ASLFBaseCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS: 7
	// ═══════════════════════════════════════════════════════════════════

	/** Stat manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStatManagerComponent> CachedStatManager;

	/** Status effect manager component - using UAC_StatusEffectManager (NOT UStatusEffectManagerComponent) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAC_StatusEffectManager> CachedStatusEffectManager;

	/** Buff manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBuffManagerComponent> CachedBuffManager;

	/** Target lock-on component (billboard) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBillboardComponent> CachedTargetLockonComponent;

	/** Projectile homing position */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBillboardComponent> CachedProjectileHomingPosition;

	/** Target lock-on widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> CachedTargetLockonWidget;

	/** Execution widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> CachedExecutionWidget;

	/** Default scene root for Blueprint components to attach to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// ═══════════════════════════════════════════════════════════════════
	// TIMELINES
	// ═══════════════════════════════════════════════════════════════════

	/** Timeline for generic rotation lerp */
	UPROPERTY(BlueprintReadWrite, Category = "Timelines")
	TObjectPtr<UTimelineComponent> CachedTL_GenericRotation;

	/** Timeline for generic location lerp */
	UPROPERTY(BlueprintReadWrite, Category = "Timelines")
	TObjectPtr<UTimelineComponent> CachedTL_GenericLocation;

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 6
	// ═══════════════════════════════════════════════════════════════════

	/** IK weight for procedural animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|IK")
	double IK_Weight;

	/** Cached rotation for lerp calculations */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	FRotator Cache_Rotation;

	/** Cached location for lerp calculations */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Cache")
	FVector Cache_Location;

	/** Whether mesh is initialized */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Mesh")
	bool bMeshInitialized;

	/** Target location for lerp */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Lerp")
	FVector TargetLerpLocation;

	/** Target rotation for lerp */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Lerp")
	FRotator TargetLerpRotation;

	/** Lerp start time */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Lerp")
	double LerpStartTime = 0.0;

	/** Lerp duration */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Lerp")
	double LerpDuration = 1.0;

	/** Whether lerping both location and rotation together */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Lerp")
	bool bLerpingBoth = false;

	/** Timer handle for location lerp */
	FTimerHandle LocationLerpTimerHandle;

	/** Timer handle for rotation lerp */
	FTimerHandle RotationLerpTimerHandle;

	/** Timer handle for IK flinch reaction animation */
	FTimerHandle IKReactionTimerHandle;

	/** IK flinch animation start time */
	double IKReactionStartTime = 0.0;

	/** IK flinch animation duration (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|IK")
	double IKReactionDuration = 0.3;

	/** Peak IK weight during flinch */
	double IKReactionPeakWeight = 1.0;

	/** Update IK reaction animation (called by timer) */
	void UpdateIKReaction();

	// ═══════════════════════════════════════════════════════════════════
	// DOOR INTERACTION (for anim notify callback)
	// ═══════════════════════════════════════════════════════════════════

	/** Cached door actor for notify callback */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Door")
	TWeakObjectPtr<AActor> PendingDoorActor;

	/** Cached target location for door walk-through */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Door")
	FVector PendingDoorTargetLocation;

	/** Cached target rotation for door walk-through */
	UPROPERTY(BlueprintReadWrite, Category = "Character|Door")
	FRotator PendingDoorTargetRotation;

	/** Handle montage notify begin - triggers door movement */
	UFUNCTION()
	void OnDoorMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Handle montage ended - cleanup */
	UFUNCTION()
	void OnDoorMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Whether door movement has been triggered (prevents double-triggering from multiple notifies) */
	bool bDoorMovementTriggered = false;

	/** Cached montage to play after movement completes */
	UPROPERTY()
	TObjectPtr<UAnimMontage> PendingDoorMontage;

	/** Called when door movement lerp completes - plays the montage */
	UFUNCTION()
	void OnDoorMovementComplete();

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLocationLerpEnd OnLocationLerpEnd;

	/** [2/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRotationLerpEnd OnRotationLerpEnd;

	/** [3/3] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLocationRotationLerpEnd OnLocationRotationLerpEnd;

	// ═══════════════════════════════════════════════════════════════════
	// INTERFACE IMPLEMENTATIONS: BPI_GenericCharacter (33 functions)
	// ═══════════════════════════════════════════════════════════════════

	// Sound
	virtual void PlaySoftSoundAtLocation_Implementation(
		const TSoftObjectPtr<USoundBase>& SoundBase,
		FVector Location,
		FRotator Rotation,
		double Volume,
		double Pitch,
		double StartTime,
		USoundAttenuation* Attenuation,
		USoundConcurrency* Concurrency,
		AActor* Owner,
		UInitialActiveSoundParams* InitialParams) override;

	// Getters
	virtual void GetHomingPositionComponent_Implementation(USceneComponent*& Component) override;
	virtual void GetMeshInitialized_Implementation(bool& bIsInitialized) override;
	virtual void GetLockonComponent_Implementation(USceneComponent*& Component) override;
	virtual void GetSoulslikeAnimInstance_Implementation(UAnimInstance*& AnimInstance) override;
	virtual void GetStatManager_Implementation(UActorComponent*& StatManager) override;

	// UI/Widget
	virtual void ToggleLockonWidget_Implementation(bool bVisible) override;

	// Spawning
	virtual void SpawnProjectile_Implementation(
		AActor* TargetActor,
		TSubclassOf<AActor> Projectile,
		FTransform InitialTransform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator) override;

	virtual void SpawnActorReplicated_Implementation(
		TSubclassOf<AActor> Actor,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator) override;

	virtual void SpawnSoftActorReplicated_Implementation(
		const TSoftClassPtr<AActor>& Actor,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator) override;

	virtual void SpawnPickupItemReplicated_Implementation(
		UPrimaryDataAsset* ItemAsset,
		int32 ItemAmount,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		bool bUsePhysics) override;

	// Interaction
	virtual void OpenContainer_Implementation(UAnimMontage* Montage, AActor* Container) override;
	virtual void OpenDoor_Implementation(UAnimMontage* Montage, AActor* Door) override;
	virtual void TryClimbLadder_Implementation(AActor* Ladder, bool bIsTopdown) override;

	// Camera
	virtual void StartWorldCameraShake_Implementation(
		TSubclassOf<UCameraShakeBase> Shake,
		FVector Epicenter,
		double InnerRadius,
		double OuterRadius,
		double Falloff,
		bool bOrientTowardsEpicenter) override;

	virtual void StartCameraShake_Implementation(TSubclassOf<UCameraShakeBase> Shake, double Scale) override;

	// VFX (Niagara)
	virtual void PlaySoftNiagaraOneshotAtLocationReplicated_Implementation(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck) override;

	virtual void PlaySoftNiagaraLoopingReplicated_Implementation(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck,
		double DurationValue) override;

	virtual void PlaySoftNiagaraOneshotReplicated_Implementation(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck) override;

	// Movement/Physics
	virtual void EnableRagdoll_Implementation() override;
	virtual void SetMovementMode_Implementation(ESLFMovementType Type) override;
	virtual void GenericLocationLerp_Implementation(double Scale, FVector TargetLocation) override;
	virtual void GenericRotationLerp_Implementation(double Scale, FRotator TargetRotation) override;
	virtual void GenericLocationAndRotationLerp_Implementation(double Scale, FVector TargetLocation, FRotator TargetRotation) override;
	virtual void JumpReplicated_Implementation() override;
	virtual void SetSpeedReplicated_Implementation(double NewSpeed) override;

	// IK/Animation
	virtual void PlayIkReaction_Implementation(double IKScale) override;

	// Montage
	virtual void StopActiveMontage_Implementation(double BlendOutDuration) override;
	virtual void PlayMontageReplicated_Implementation(
		UAnimMontage* Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection) override;

	virtual void PlayPrioMontageReplicated_Implementation(
		UAnimMontage* Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection) override;

	virtual void PlaySoftMontageReplicated_Implementation(
		const TSoftObjectPtr<UObject>& Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection,
		bool bPrio) override;

	// Combat
	virtual void ToggleGuardReplicated_Implementation(bool bToggled, bool bIgnoreGracePeriod) override;
	virtual void StartHitstop_Implementation(double Duration) override;

	// ═══════════════════════════════════════════════════════════════════
	// SERVER RPCs: Network replicated spawning and actions
	// ═══════════════════════════════════════════════════════════════════

	/** Server RPC: Spawn actor on server
	 *  Called from SpawnActorReplicated when client needs server to spawn
	 */
	UFUNCTION(Server, Reliable)
	void SRV_SpawnActor(
		UClass* ActorClass,
		FTransform SpawnTransform,
		ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,
		AActor* InOwner,
		APawn* InInstigator);

	/** Server RPC: Spawn pickup item on server */
	UFUNCTION(Server, Reliable)
	void SRV_SpawnPickupItem(
		UPrimaryDataAsset* ItemAsset,
		int32 ItemAmount,
		FTransform SpawnTransform,
		ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,
		bool bUsePhysics);

	/** Server RPC: Play montage on server (for replication) */
	UFUNCTION(Server, Reliable)
	void SRV_PlayMontage(
		UAnimMontage* Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection);

	/** Server RPC: Spawn Niagara oneshot at location on server */
	UFUNCTION(Server, Reliable)
	void SRV_SpawnNiagaraOneshotLocation(
		UNiagaraSystem* VFXSystem,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck);

	/** Server RPC: Spawn Niagara looping attached on server */
	UFUNCTION(Server, Reliable)
	void SRV_SpawnNiagaraLoopingAttached(
		UNiagaraSystem* VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck,
		double DurationValue);

	/** Server RPC: Spawn Niagara oneshot attached on server */
	UFUNCTION(Server, Reliable)
	void SRV_SpawnNiagaraOneshotAttached(
		UNiagaraSystem* VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck);

	/** Server RPC: Destroy actor on server */
	UFUNCTION(Server, Reliable)
	void SRV_DestroyActor(AActor* ActorToDestroy);

	/** Server RPC: Jump on server */
	UFUNCTION(Server, Reliable)
	void SRV_Jump();

	/** Server RPC: Set movement speed on server */
	UFUNCTION(Server, Reliable)
	void SRV_SetSpeed(double NewSpeed);

protected:
	// Timeline callbacks
	UFUNCTION()
	void OnLocationLerpUpdate();

	UFUNCTION()
	void OnLocationLerpFinished();

	UFUNCTION()
	void OnRotationLerpUpdate();

	UFUNCTION()
	void OnRotationLerpFinished();
};
