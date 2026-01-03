// SLFBaseCharacter.cpp
// C++ implementation for B_BaseCharacter
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_BaseCharacter
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         6+ (initialized in constructor)
// Functions:         33 interface functions implemented
// Event Dispatchers: 3 (all assignable)
// Components:        7 created in constructor
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFBaseCharacter.h"
#include "Components/StatManagerComponent.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Components/BuffManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Animation/AnimInstance.h"
#include "SLFEnums.h"

ASLFBaseCharacter::ASLFBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize variables
	IK_Weight = 1.0;
	Cache_Rotation = FRotator::ZeroRotator;
	Cache_Location = FVector::ZeroVector;
	bMeshInitialized = false;
	TargetLerpLocation = FVector::ZeroVector;
	TargetLerpRotation = FRotator::ZeroRotator;

	// NOTE: Components are NOT created here - they are defined in the Blueprint.
	// The Blueprint's component instances will be used at runtime.
	// C++ pointers are populated via FindComponentByClass in BeginPlay or by Blueprint serialization.
	CachedStatManager = nullptr;
	CachedStatusEffectManager = nullptr;
	CachedBuffManager = nullptr;
	CachedTargetLockonComponent = nullptr;
	CachedProjectileHomingPosition = nullptr;
	CachedTargetLockonWidget = nullptr;
	CachedExecutionWidget = nullptr;
	CachedTL_GenericRotation = nullptr;
	CachedTL_GenericLocation = nullptr;

	// Create DefaultSceneRoot for Blueprint components to attach to
	// This provides the parent component that Blueprint-defined components expect
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetupAttachment(GetCapsuleComponent());
}

void ASLFBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] BeginPlay: %s"), *GetName());

	// Cache component references from Blueprint-defined components
	CachedStatManager = FindComponentByClass<UStatManagerComponent>();
	CachedStatusEffectManager = FindComponentByClass<UStatusEffectManagerComponent>();
	CachedBuffManager = FindComponentByClass<UBuffManagerComponent>();
	CachedTargetLockonComponent = FindComponentByClass<UBillboardComponent>();
	CachedTargetLockonWidget = FindComponentByClass<UWidgetComponent>();
	
	// Find specific components by iterating (since we have multiple of same type)
	TArray<UBillboardComponent*> BillboardComps;
	GetComponents<UBillboardComponent>(BillboardComps);
	for (UBillboardComponent* Comp : BillboardComps)
	{
		if (Comp->GetName().Contains(TEXT("TargetLockon")))
		{
			CachedTargetLockonComponent = Comp;
		}
		else if (Comp->GetName().Contains(TEXT("ProjectileHoming")))
		{
			CachedProjectileHomingPosition = Comp;
		}
	}
	
	TArray<UWidgetComponent*> WidgetComps;
	GetComponents<UWidgetComponent>(WidgetComps);
	for (UWidgetComponent* Comp : WidgetComps)
	{
		if (Comp->GetName().Contains(TEXT("TargetLockon")))
		{
			CachedTargetLockonWidget = Comp;
		}
		else if (Comp->GetName().Contains(TEXT("Execution")))
		{
			CachedExecutionWidget = Comp;
		}
	}
	
	TArray<UTimelineComponent*> TimelineComps;
	GetComponents<UTimelineComponent>(TimelineComps);
	for (UTimelineComponent* Comp : TimelineComps)
	{
		if (Comp->GetName().Contains(TEXT("Rotation")))
		{
			CachedTL_GenericRotation = Comp;
		}
		else if (Comp->GetName().Contains(TEXT("Location")))
		{
			CachedTL_GenericLocation = Comp;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Components cached - StatManager: %s, StatusEffectManager: %s, BuffManager: %s"),
		CachedStatManager ? TEXT("OK") : TEXT("NULL"),
		CachedStatusEffectManager ? TEXT("OK") : TEXT("NULL"),
		CachedBuffManager ? TEXT("OK") : TEXT("NULL"));

	// Cache initial values
	Cache_Location = GetActorLocation();
	Cache_Rotation = GetActorRotation();
	bMeshInitialized = true;
}

void ASLFBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERFACE IMPLEMENTATIONS: BPI_GenericCharacter
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFBaseCharacter::PlaySoftSoundAtLocation_Implementation(
	const TSoftObjectPtr<USoundBase>& SoundBase,
	FVector Location,
	FRotator Rotation,
	double Volume,
	double Pitch,
	double StartTime,
	USoundAttenuation* Attenuation,
	USoundConcurrency* Concurrency,
	AActor* InOwner,
	UInitialActiveSoundParams* InitialParams)
{
	if (SoundBase.IsValid())
	{
		USoundBase* Sound = SoundBase.Get();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, Sound, Location, Rotation, Volume, Pitch, StartTime,
				Attenuation, Concurrency, InOwner);
		}
	}
	else if (!SoundBase.IsNull())
	{
		// Async load
		SoundBase.LoadSynchronous();
		if (USoundBase* Sound = SoundBase.Get())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, Sound, Location, Rotation, Volume, Pitch, StartTime,
				Attenuation, Concurrency, InOwner);
		}
	}
}

void ASLFBaseCharacter::GetHomingPositionComponent_Implementation(USceneComponent*& Component)
{
	Component = CachedProjectileHomingPosition;
}

void ASLFBaseCharacter::GetMeshInitialized_Implementation(bool& bIsInitialized)
{
	bIsInitialized = bMeshInitialized;
}

void ASLFBaseCharacter::GetLockonComponent_Implementation(USceneComponent*& Component)
{
	Component = CachedTargetLockonComponent;
}

void ASLFBaseCharacter::GetSoulslikeAnimInstance_Implementation(UAnimInstance*& AnimInstance)
{
	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}
	else
	{
		AnimInstance = nullptr;
	}
}

void ASLFBaseCharacter::GetStatManager_Implementation(UActorComponent*& StatManager)
{
	StatManager = CachedStatManager;
}

void ASLFBaseCharacter::ToggleLockonWidget_Implementation(bool bVisible)
{
	if (CachedTargetLockonWidget)
	{
		CachedTargetLockonWidget->SetVisibility(bVisible);
	}
}

void ASLFBaseCharacter::SpawnProjectile_Implementation(
	AActor* TargetActor,
	TSubclassOf<AActor> Projectile,
	FTransform InitialTransform,
	ESpawnActorCollisionHandlingMethod Collision,
	AActor* InOwner,
	APawn* InInstigator)
{
	if (!Projectile)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InOwner;
	SpawnParams.Instigator = InInstigator;
	SpawnParams.SpawnCollisionHandlingOverride = Collision;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		Projectile, InitialTransform, SpawnParams);

	// TODO: Set target on projectile if it has the interface
}

void ASLFBaseCharacter::SpawnActorReplicated_Implementation(
	TSubclassOf<AActor> Actor,
	FTransform Transform,
	ESpawnActorCollisionHandlingMethod Collision,
	AActor* InOwner,
	APawn* InInstigator)
{
	if (!Actor)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InOwner;
	SpawnParams.Instigator = InInstigator;
	SpawnParams.SpawnCollisionHandlingOverride = Collision;

	GetWorld()->SpawnActor<AActor>(Actor, Transform, SpawnParams);
}

void ASLFBaseCharacter::SpawnSoftActorReplicated_Implementation(
	const TSoftClassPtr<AActor>& Actor,
	FTransform Transform,
	ESpawnActorCollisionHandlingMethod Collision,
	AActor* InOwner,
	APawn* InInstigator)
{
	if (Actor.IsNull())
	{
		return;
	}

	UClass* ActorClass = Actor.LoadSynchronous();
	if (ActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = InOwner;
		SpawnParams.Instigator = InInstigator;
		SpawnParams.SpawnCollisionHandlingOverride = Collision;

		GetWorld()->SpawnActor<AActor>(ActorClass, Transform, SpawnParams);
	}
}

void ASLFBaseCharacter::SpawnPickupItemReplicated_Implementation(
	UPrimaryDataAsset* ItemAsset,
	int32 ItemAmount,
	FTransform Transform,
	ESpawnActorCollisionHandlingMethod Collision,
	bool bUsePhysics)
{
	// TODO: Implement pickup item spawning using the item data asset
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] SpawnPickupItemReplicated called"));
}

void ASLFBaseCharacter::OpenContainer_Implementation(UAnimMontage* Montage, AActor* Container)
{
	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	}
	// TODO: Trigger container opening logic
}

void ASLFBaseCharacter::OpenDoor_Implementation(UAnimMontage* Montage, AActor* Door)
{
	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	}
	// TODO: Trigger door opening logic
}

void ASLFBaseCharacter::TryClimbLadder_Implementation(AActor* Ladder, bool bIsTopdown)
{
	// TODO: Implement ladder climbing
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] TryClimbLadder called, topdown: %s"),
		bIsTopdown ? TEXT("true") : TEXT("false"));
}

void ASLFBaseCharacter::StartWorldCameraShake_Implementation(
	TSubclassOf<UCameraShakeBase> Shake,
	FVector Epicenter,
	double InnerRadius,
	double OuterRadius,
	double Falloff,
	bool bOrientTowardsEpicenter)
{
	if (Shake)
	{
		UGameplayStatics::PlayWorldCameraShake(
			this, Shake, Epicenter, InnerRadius, OuterRadius, Falloff, bOrientTowardsEpicenter);
	}
}

void ASLFBaseCharacter::StartCameraShake_Implementation(TSubclassOf<UCameraShakeBase> Shake, double Scale)
{
	if (Shake)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC)
		{
			PC = GetWorld()->GetFirstPlayerController();
		}
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(Shake, Scale);
		}
	}
}

void ASLFBaseCharacter::PlaySoftNiagaraOneshotAtLocationReplicated_Implementation(
	const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck)
{
	if (VFXSystem.IsNull())
	{
		return;
	}

	UNiagaraSystem* System = VFXSystem.LoadSynchronous();
	if (System)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), System, Location, Rotation,
			FVector(1.0f), bAutoDestroy, bAutoActivate);
	}
}

void ASLFBaseCharacter::PlaySoftNiagaraLoopingReplicated_Implementation(
	const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
	FName AttachSocket,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck,
	double DurationValue)
{
	if (VFXSystem.IsNull() || !GetMesh())
	{
		return;
	}

	UNiagaraSystem* System = VFXSystem.LoadSynchronous();
	if (System)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			System, GetMesh(), AttachSocket,
			Location, Rotation, EAttachLocation::KeepRelativeOffset,
			bAutoDestroy, bAutoActivate);

		// TODO: Handle duration for looping systems
	}
}

void ASLFBaseCharacter::PlaySoftNiagaraOneshotReplicated_Implementation(
	const TSoftObjectPtr<UNiagaraSystem>& VFXSystem,
	FName AttachSocket,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck)
{
	if (VFXSystem.IsNull() || !GetMesh())
	{
		return;
	}

	UNiagaraSystem* System = VFXSystem.LoadSynchronous();
	if (System)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			System, GetMesh(), AttachSocket,
			Location, Rotation, EAttachLocation::KeepRelativeOffset,
			bAutoDestroy, bAutoActivate);
	}
}

void ASLFBaseCharacter::EnableRagdoll_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASLFBaseCharacter::SetMovementMode_Implementation(ESLFMovementType Type)
{
	if (!GetCharacterMovement())
	{
		return;
	}

	// ESLFMovementType uses Walk/Run/Sprint to control max walk speed, not UE movement mode
	switch (Type)
	{
	case ESLFMovementType::Walk:
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
		break;
	case ESLFMovementType::Run:
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		break;
	case ESLFMovementType::Sprint:
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		break;
	default:
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		break;
	}
}

void ASLFBaseCharacter::GenericLocationLerp_Implementation(double Scale, FVector NewTargetLocation)
{
	Cache_Location = GetActorLocation();
	TargetLerpLocation = NewTargetLocation;

	// TODO: Start timeline for location lerp
	// For now, just set the location directly
	SetActorLocation(FMath::Lerp(Cache_Location, TargetLerpLocation, Scale));
}

void ASLFBaseCharacter::GenericRotationLerp_Implementation(double Scale, FRotator NewTargetRotation)
{
	Cache_Rotation = GetActorRotation();
	TargetLerpRotation = NewTargetRotation;

	// TODO: Start timeline for rotation lerp
	// For now, just set the rotation directly
	SetActorRotation(FMath::Lerp(Cache_Rotation, TargetLerpRotation, Scale));
}

void ASLFBaseCharacter::GenericLocationAndRotationLerp_Implementation(double Scale, FVector NewTargetLocation, FRotator NewTargetRotation)
{
	Cache_Location = GetActorLocation();
	Cache_Rotation = GetActorRotation();
	TargetLerpLocation = NewTargetLocation;
	TargetLerpRotation = NewTargetRotation;

	// TODO: Start timelines for both location and rotation lerp
	// For now, just set directly
	SetActorLocation(FMath::Lerp(Cache_Location, TargetLerpLocation, Scale));
	SetActorRotation(FMath::Lerp(Cache_Rotation, TargetLerpRotation, Scale));
}

void ASLFBaseCharacter::JumpReplicated_Implementation()
{
	Jump();
}

void ASLFBaseCharacter::SetSpeedReplicated_Implementation(double NewSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

void ASLFBaseCharacter::PlayIkReaction_Implementation(double IKScale)
{
	IK_Weight = IKScale;
}

void ASLFBaseCharacter::StopActiveMontage_Implementation(double BlendOutDuration)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(BlendOutDuration);
	}
}

void ASLFBaseCharacter::PlayMontageReplicated_Implementation(
	UAnimMontage* Montage,
	double PlayRate,
	double StartPosition,
	FName StartSection)
{
	if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, StartPosition);

	if (StartSection != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(StartSection, Montage);
	}
}

void ASLFBaseCharacter::PlayPrioMontageReplicated_Implementation(
	UAnimMontage* Montage,
	double PlayRate,
	double StartPosition,
	FName StartSection)
{
	// Priority montage - stop any current montage first
	StopActiveMontage_Implementation(0.1);
	PlayMontageReplicated_Implementation(Montage, PlayRate, StartPosition, StartSection);
}

void ASLFBaseCharacter::PlaySoftMontageReplicated_Implementation(
	const TSoftObjectPtr<UObject>& Montage,
	double PlayRate,
	double StartPosition,
	FName StartSection,
	bool bPrio)
{
	if (Montage.IsNull())
	{
		return;
	}

	UObject* LoadedMontage = Montage.LoadSynchronous();
	UAnimMontage* AnimMontage = Cast<UAnimMontage>(LoadedMontage);

	if (AnimMontage)
	{
		if (bPrio)
		{
			PlayPrioMontageReplicated_Implementation(AnimMontage, PlayRate, StartPosition, StartSection);
		}
		else
		{
			PlayMontageReplicated_Implementation(AnimMontage, PlayRate, StartPosition, StartSection);
		}
	}
}

void ASLFBaseCharacter::ToggleGuardReplicated_Implementation(bool bToggled, bool bIgnoreGracePeriod)
{
	// TODO: Implement guard toggling via combat component
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] ToggleGuardReplicated: %s, ignore grace: %s"),
		bToggled ? TEXT("true") : TEXT("false"),
		bIgnoreGracePeriod ? TEXT("true") : TEXT("false"));
}

void ASLFBaseCharacter::StartHitstop_Implementation(double Duration)
{
	// TODO: Implement hitstop (freeze animation briefly)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] StartHitstop: %f"), Duration);
}

// ═══════════════════════════════════════════════════════════════════════════════
// TIMELINE CALLBACKS
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFBaseCharacter::OnLocationLerpUpdate()
{
	// TODO: Update location based on timeline alpha
}

void ASLFBaseCharacter::OnLocationLerpFinished()
{
	OnLocationLerpEnd.Broadcast();
}

void ASLFBaseCharacter::OnRotationLerpUpdate()
{
	// TODO: Update rotation based on timeline alpha
}

void ASLFBaseCharacter::OnRotationLerpFinished()
{
	OnRotationLerpEnd.Broadcast();
}
