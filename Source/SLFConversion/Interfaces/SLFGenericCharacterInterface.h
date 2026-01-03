// SLFGenericCharacterInterface.h
// C++ interface for BPI_GenericCharacter
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_GenericCharacter.json
// Functions: 33

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
#include "SLFGenericCharacterInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFGenericCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_GenericCharacter
 * Generated from JSON with 33 functions
 */
class SLFCONVERSION_API ISLFGenericCharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SpawnActorReplicated(TSubclassOf<AActor> InActor, const FTransform& Transform, ESpawnActorCollisionHandlingMethod Collision, AActor* Owner, APawn* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SpawnPickupItemReplicated(UPrimaryDataAsset* ItemAsset, int32 ItemAmount, const FTransform& Transform, ESpawnActorCollisionHandlingMethod Collision, bool UsePhysics);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	UAC_StatManager* GetStatManager();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlayMontageReplicated(UAnimMontage* Montage, double PlayRate, double StartPosition, const FName& StartSection);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SetSpeedReplicated(double NewSpeed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void JumpReplicated();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlaySoftMontageReplicated(const TSoftObjectPtr<UObject>& Montage, double PlayRate, double StartPosition, const FName& StartSection, bool Prio);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SpawnSoftActorReplicated(const TSoftClassPtr<AActor>& InActor, const FTransform& Transform, ESpawnActorCollisionHandlingMethod Collision, AActor* Owner, APawn* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlayPrioMontageReplicated(UAnimMontage* Montage, double PlayRate, double StartPosition, const FName& StartSection);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void StartHitstop(double Duration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void StartCameraShake(TSubclassOf<UCameraShakeBase> Shake, double Scale);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void ToggleGuardReplicated(bool Toggled, bool IgnoreGracePeriod);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SetMovementMode(ESLFMovementType InType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlaySoftNiagaraOneshotReplicated(const TSoftObjectPtr<UNiagaraSystem>& VFXSystem, const FName& AttachSocket, const FVector& Location, const FRotator& Rotation, bool AutoDestroy, bool AutoActivate, bool PreCullCheck);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlaySoftNiagaraLoopingReplicated(const TSoftObjectPtr<UNiagaraSystem>& VFXSystem, const FName& AttachSocket, const FVector& Location, const FRotator& Rotation, bool AutoDestroy, bool AutoActivate, bool PreCullCheck, double DurationValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void OpenDoor(UAnimMontage* Montage, AB_Door* Door);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	UAnimInstance* GetSoulslikeAnimInstance();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void StopActiveMontage(double BlendOutDuration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void TryClimbLadder(AB_Ladder* Ladder, bool IsTopdown);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void GenericLocationLerp(double Scale, const FVector& TargetLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void GenericRotationLerp(double Scale, const FRotator& TargetRotation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void GenericLocationAndRotationLerp(double Scale, const FVector& TargetLocation, const FRotator& TargetRotation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlayIkReaction(double IKScale);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void EnableRagdoll();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlaySoftNiagaraOneshotAtLocationReplicated(const TSoftObjectPtr<UNiagaraSystem>& VFXSystem, const FVector& Location, const FRotator& Rotation, bool AutoDestroy, bool AutoActivate, bool PreCullCheck);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void StartWorldCameraShake(TSubclassOf<UCameraShakeBase> Shake, const FVector& Epicenter, double InnerRadius, double OuterRadius, double Falloff, bool OrientTowardsEpicenter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void OpenContainer(UAnimMontage* Montage, AB_Container* Container);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void SpawnProjectile(AActor* TargetActor, TSubclassOf<AActor> Projectile, const FTransform& InitialTransform, ESpawnActorCollisionHandlingMethod Collision, AActor* Owner, APawn* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void ToggleLockonWidget(bool Visible);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	USceneComponent* GetLockonComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	bool GetMeshInitialized();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	USceneComponent* GetHomingPositionComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericCharacter")
	void PlaySoftSoundAtLocation(const TSoftObjectPtr<USoundBase>& SoundBase, const FVector& Location, const FRotator& Rotation, double Volume, double Pitch, double StartTime, USoundAttenuation* Attenuation, USoundConcurrency* Concurrency, AActor* Owner, UInitialActiveSoundParams* InitialParams);
};
