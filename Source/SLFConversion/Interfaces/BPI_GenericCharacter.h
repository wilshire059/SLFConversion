// BPI_GenericCharacter.h
// C++ Interface for BPI_GenericCharacter
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_GenericCharacter.json
// Functions: 33 (ALL functions included - no shortcuts)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "NiagaraSystem.h"
#include "SLFEnums.h"
#include "BPI_GenericCharacter.generated.h"

// Forward declarations
class USceneComponent;
class UAnimInstance;
class UAnimMontage;
class UActorComponent;
class UCameraShakeBase;
class UInitialActiveSoundParams;

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_GenericCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * Generic Character Interface
 * Provides common functionality for all characters (player, enemy, NPC)
 * Contains 33 functions for sound, VFX, movement, combat, and more
 */
class SLFCONVERSION_API IBPI_GenericCharacter
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// SOUND FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Play soft sound at location with full parameters
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Sound")
	void PlaySoftSoundAtLocation(
		const TSoftObjectPtr<USoundBase>& SoundBase,
		FVector Location,
		FRotator Rotation,
		double Volume,
		double Pitch,
		double StartTime,
		USoundAttenuation* Attenuation,
		USoundConcurrency* Concurrency,
		AActor* Owner,
		UInitialActiveSoundParams* InitialParams);

	// ═══════════════════════════════════════════════════════════════════════
	// GETTER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Get homing position component for projectile tracking
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Getters")
	void GetHomingPositionComponent(USceneComponent*& Component);

	// Check if mesh is initialized
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Mesh Initialization")
	void GetMeshInitialized(bool& bIsInitialized);

	// Get lock-on component
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void GetLockonComponent(USceneComponent*& Component);

	// Get soulslike anim instance
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void GetSoulslikeAnimInstance(UAnimInstance*& AnimInstance);

	// Get stat manager component
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Getters")
	void GetStatManager(UActorComponent*& StatManager);

	// ═══════════════════════════════════════════════════════════════════════
	// UI/WIDGET FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Toggle lock-on widget visibility
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void ToggleLockonWidget(bool bVisible);

	// ═══════════════════════════════════════════════════════════════════════
	// SPAWNING FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Spawn projectile with target
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void SpawnProjectile(
		AActor* TargetActor,
		TSubclassOf<AActor> Projectile,
		FTransform InitialTransform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator);

	// Spawn actor replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void SpawnActorReplicated(
		TSubclassOf<AActor> Actor,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator);

	// Spawn soft actor replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void SpawnSoftActorReplicated(
		const TSoftClassPtr<AActor>& Actor,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		AActor* Owner,
		APawn* Instigator);

	// Spawn pickup item replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void SpawnPickupItemReplicated(
		UPrimaryDataAsset* ItemAsset,
		int32 ItemAmount,
		FTransform Transform,
		ESpawnActorCollisionHandlingMethod Collision,
		bool bUsePhysics);

	// ═══════════════════════════════════════════════════════════════════════
	// INTERACTION FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Open container with montage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void OpenContainer(UAnimMontage* Montage, AActor* Container);

	// Open door with montage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void OpenDoor(UAnimMontage* Montage, AActor* Door);

	// Try to climb ladder
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void TryClimbLadder(AActor* Ladder, bool bIsTopdown);

	// ═══════════════════════════════════════════════════════════════════════
	// CAMERA FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Start world camera shake
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void StartWorldCameraShake(
		TSubclassOf<UCameraShakeBase> Shake,
		FVector Epicenter,
		double InnerRadius,
		double OuterRadius,
		double Falloff,
		bool bOrientTowardsEpicenter);

	// Start camera shake
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void StartCameraShake(TSubclassOf<UCameraShakeBase> Shake, double Scale);

	// ═══════════════════════════════════════════════════════════════════════
	// VFX FUNCTIONS (Niagara)
	// ═══════════════════════════════════════════════════════════════════════

	// Play soft Niagara oneshot at location replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|VFX")
	void PlaySoftNiagaraOneshotAtLocationReplicated(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck);

	// Play soft Niagara looping replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|VFX")
	void PlaySoftNiagaraLoopingReplicated(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck,
		double DurationValue);

	// Play soft Niagara oneshot replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|VFX")
	void PlaySoftNiagaraOneshotReplicated(
		const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
		FName AttachSocket,
		FVector Location,
		FRotator Rotation,
		bool bAutoDestroy,
		bool bAutoActivate,
		bool bPreCullCheck);

	// ═══════════════════════════════════════════════════════════════════════
	// MOVEMENT/PHYSICS FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Enable ragdoll
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void EnableRagdoll();

	// Set movement mode
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void SetMovementMode(ESLFMovementType Type);

	// Generic location lerp
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void GenericLocationLerp(double Scale, FVector TargetLocation);

	// Generic rotation lerp
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void GenericRotationLerp(double Scale, FRotator TargetRotation);

	// Generic location and rotation lerp
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void GenericLocationAndRotationLerp(double Scale, FVector TargetLocation, FRotator TargetRotation);

	// Jump replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void JumpReplicated();

	// Set speed replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void SetSpeedReplicated(double NewSpeed);

	// ═══════════════════════════════════════════════════════════════════════
	// IK/ANIMATION FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Play IK reaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void PlayIkReaction(double IKScale);

	// ═══════════════════════════════════════════════════════════════════════
	// MONTAGE FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Stop active montage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void StopActiveMontage(double BlendOutDuration);

	// Play montage replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|Montages")
	void PlayMontageReplicated(
		UAnimMontage* Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection);

	// Play priority montage replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|Montages")
	void PlayPrioMontageReplicated(
		UAnimMontage* Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection);

	// Play soft montage replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated|Montages")
	void PlaySoftMontageReplicated(
		const TSoftObjectPtr<UObject>& Montage,
		double PlayRate,
		double StartPosition,
		FName StartSection,
		bool bPrio);

	// ═══════════════════════════════════════════════════════════════════════
	// COMBAT FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Toggle guard replicated
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common|Replicated")
	void ToggleGuardReplicated(bool bToggled, bool bIgnoreGracePeriod);

	// Start hitstop
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Interface|Common")
	void StartHitstop(double Duration);
};
