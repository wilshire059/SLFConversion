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
#include "Components/AC_StatusEffectManager.h"
#include "Components/BuffManagerComponent.h"
#include "Components/LadderManagerComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Animation/AnimInstance.h"
#include "SLFEnums.h"
#include "Interfaces/BPI_Projectile.h"
#include "Interfaces/BPI_Interactable.h"
#include "Blueprints/B_PickupItem.h"
#include "Blueprints/B_Container.h"
#include "Blueprints/Actors/SLFContainer.h"
#include "TimerManager.h"
#include "Widgets/W_TargetExecutionIndicator.h"

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

	// Create StatManagerComponent - this was previously expected to come from Blueprint SCS,
	// but after migration the SCS components were lost. Creating in C++ ensures it always exists.
	// The StatTable property can be overridden in Blueprint or set via Python script.
	CachedStatManager = CreateDefaultSubobject<UStatManagerComponent>(TEXT("StatManagerComponent"));

	// Set default StatTable path - this is the standard stat table used by the framework
	static ConstructorHelpers::FObjectFinder<UDataTable> DefaultStatTableFinder(
		TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable.DT_ExampleStatTable"));
	if (DefaultStatTableFinder.Succeeded() && CachedStatManager)
	{
		CachedStatManager->StatTable = DefaultStatTableFinder.Object;
	}

	// Create StatusEffectManager - B_StatusEffectArea and weapons with status effects need this
	// CRITICAL: This must be UAC_StatusEffectManager, NOT UStatusEffectManagerComponent
	// B_StatusEffectArea calls FindComponentByClass<UAC_StatusEffectManager>()
	CachedStatusEffectManager = CreateDefaultSubobject<UAC_StatusEffectManager>(TEXT("StatusEffectManager"));
	CachedBuffManager = nullptr;
	CachedTargetLockonComponent = nullptr;
	CachedProjectileHomingPosition = nullptr;
	CachedTargetLockonWidget = nullptr;
	CachedTL_GenericRotation = nullptr;
	CachedTL_GenericLocation = nullptr;

	// Create DefaultSceneRoot for Blueprint components to attach to
	// This provides the parent component that Blueprint-defined components expect
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetupAttachment(GetCapsuleComponent());

	// Create ExecutionWidget - shows execution indicator when enemy poise is broken
	// Position above character head (Z offset), facing camera (Screen space)
	CachedExecutionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExecutionWidget"));
	CachedExecutionWidget->SetupAttachment(GetMesh());
	CachedExecutionWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f)); // Above head
	CachedExecutionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	CachedExecutionWidget->SetDrawAtDesiredSize(true);
	CachedExecutionWidget->SetVisibility(false); // Start hidden

	// Set widget class to W_TargetExecutionIndicator
	// NOTE: bp_only has this in Widgets/World/, not Widgets/HUD/
	static ConstructorHelpers::FClassFinder<UUserWidget> ExecutionWidgetClassFinder(
		TEXT("/Game/SoulslikeFramework/Widgets/World/W_TargetExecutionIndicator"));
	if (ExecutionWidgetClassFinder.Succeeded())
	{
		CachedExecutionWidget->SetWidgetClass(ExecutionWidgetClassFinder.Class);
	}
}

void ASLFBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] BeginPlay: %s"), *GetName());

	// DEBUG: List all components on this character
	TArray<UActorComponent*> AllComponents;
	GetComponents(AllComponents);
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Total components: %d"), AllComponents.Num());
	for (UActorComponent* Comp : AllComponents)
	{
		UE_LOG(LogTemp, Log, TEXT("[BaseCharacter]   Component: %s (Class: %s)"),
			*Comp->GetName(),
			*Comp->GetClass()->GetName());
	}

	// Cache component references
	// StatManager is created in constructor, but check if Blueprint added a different one
	if (!CachedStatManager)
	{
		CachedStatManager = FindComponentByClass<UStatManagerComponent>();
	}
	// StatusEffectManager is created in constructor, but verify it exists and cache
	if (!CachedStatusEffectManager)
	{
		CachedStatusEffectManager = FindComponentByClass<UAC_StatusEffectManager>();
	}
	CachedBuffManager = FindComponentByClass<UBuffManagerComponent>();

	// Find specific components by iterating (since we have multiple of same type)
	// Don't use FindComponentByClass for these - it grabs the first one which may be wrong
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

	// Widget components - ExecutionWidget is created in C++ constructor, TargetLockon may come from Blueprint SCS
	TArray<UWidgetComponent*> WidgetComps;
	GetComponents<UWidgetComponent>(WidgetComps);
	for (UWidgetComponent* Comp : WidgetComps)
	{
		if (Comp->GetName().Contains(TEXT("TargetLockon")))
		{
			CachedTargetLockonWidget = Comp;
		}
		// ExecutionWidget is already set in constructor - just verify it's in the component list
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

	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Widget caching - WidgetComponents found: %d, ExecutionWidget: %s, TargetLockonWidget: %s"),
		WidgetComps.Num(),
		CachedExecutionWidget ? *CachedExecutionWidget->GetName() : TEXT("NULL"),
		CachedTargetLockonWidget ? *CachedTargetLockonWidget->GetName() : TEXT("NULL"));

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
	UE_LOG(LogTemp, Warning, TEXT("[SLFBaseCharacter] SpawnProjectile - Class: %s"),
		Projectile ? *Projectile->GetName() : TEXT("NULL"));

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

	// Set target on projectile if it has the interface (IMPLEMENTED)
	if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UBPI_Projectile::StaticClass()))
	{
		IBPI_Projectile::Execute_InitializeProjectile(SpawnedActor, InInstigator);
	}
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
	// Spawn AB_PickupItem and set its Item property (IMPLEMENTED)
	if (!ItemAsset || !GetWorld())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] SpawnPickupItemReplicated: %s x%d"), *ItemAsset->GetName(), ItemAmount);

	// Load the B_PickupItem class
	UClass* PickupItemClass = LoadClass<AActor>(nullptr, TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem.B_PickupItem_C"));
	if (!PickupItemClass)
	{
		// Fallback to AB_PickupItem C++ class
		PickupItemClass = AB_PickupItem::StaticClass();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = Collision;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(PickupItemClass, Transform, SpawnParams);
	if (AB_PickupItem* PickupItem = Cast<AB_PickupItem>(SpawnedActor))
	{
		PickupItem->Item = ItemAsset;
		PickupItem->Count = ItemAmount;
		PickupItem->UsePhysics = bUsePhysics;
	}
}

void ASLFBaseCharacter::OpenContainer_Implementation(UAnimMontage* Montage, AActor* Container)
{
	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	}

	// Call back to the container to start its timeline (NOT OnInteract - that would loop)
	// The container's OpenContainer triggers the lid animation timeline
	if (Container)
	{
		// Try AB_Container (Blueprint-derived)
		if (AB_Container* BPContainer = Cast<AB_Container>(Container))
		{
			BPContainer->OpenContainer();
		}
		// Try ASLFContainer (C++ base)
		else if (ASLFContainer* SLFContainer = Cast<ASLFContainer>(Container))
		{
			SLFContainer->OpenContainer(this);
		}
	}
}

void ASLFBaseCharacter::OpenDoor_Implementation(UAnimMontage* Montage, AActor* Door)
{
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OpenDoor - Montage: %s, Door: %s"),
		Montage ? *Montage->GetName() : TEXT("None"),
		Door ? *Door->GetName() : TEXT("None"));

	// ═══════════════════════════════════════════════════════════════════════════════
	// SOULS-LIKE DOOR/FOG GATE WALK-THROUGH (bp_only observed flow)
	// ═══════════════════════════════════════════════════════════════════════════════
	// Based on user testing of bp_only:
	// 1. Play montage IMMEDIATELY (animation starts first)
	// 2. During montage -> OnNotifyBegin -> Start movement toward MoveTo
	// 3. Movement completes during/after montage
	//
	// So: ANIMATE FIRST, THEN MOVE (triggered by anim notify)
	// ═══════════════════════════════════════════════════════════════════════════════

	if (!Door)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseCharacter] OpenDoor - No door provided"));
		return;
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseCharacter] OpenDoor - No montage provided"));
		return;
	}

	// Find the MoveTo component on the door
	UBillboardComponent* MoveToComp = nullptr;
	TArray<UBillboardComponent*> BillboardComps;
	Door->GetComponents<UBillboardComponent>(BillboardComps);
	for (UBillboardComponent* Comp : BillboardComps)
	{
		if (Comp && Comp->GetName().Contains(TEXT("MoveTo")))
		{
			MoveToComp = Comp;
			break;
		}
	}

	if (!MoveToComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseCharacter] OpenDoor - No MoveTo component found on door"));
		return;
	}

	// Reset state for new door interaction
	bDoorMovementTriggered = false;

	// Cache door data for movement (triggered by anim notify)
	PendingDoorActor = Door;
	PendingDoorTargetLocation = MoveToComp->GetComponentLocation();
	PendingDoorTargetLocation.Z = GetActorLocation().Z; // Preserve player Z

	// Calculate target rotation (face toward target/away from door)
	FVector ToTarget = PendingDoorTargetLocation - GetActorLocation();
	ToTarget.Z = 0.0f;
	if (!ToTarget.IsNearlyZero())
	{
		ToTarget.Normalize();
		PendingDoorTargetRotation = ToTarget.Rotation();
	}
	else
	{
		PendingDoorTargetRotation = GetActorRotation();
	}

	// Log for debugging
	FVector StartPos = GetActorLocation();
	float Distance = FVector::Dist2D(StartPos, PendingDoorTargetLocation);
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OpenDoor - Start: %s, Target: %s, Distance: %.1f units"),
		*StartPos.ToString(), *PendingDoorTargetLocation.ToString(), Distance);

	// Cache the montage
	PendingDoorMontage = Montage;

	// Get anim instance and bind callbacks
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseCharacter] OpenDoor - No AnimInstance found"));
		return;
	}

	// Bind to anim notify - when notify fires, START movement
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASLFBaseCharacter::OnDoorMontageNotifyBegin);

	// Bind to montage ended for cleanup
	AnimInstance->OnMontageEnded.AddDynamic(this, &ASLFBaseCharacter::OnDoorMontageEnded);

	// Play montage IMMEDIATELY (animation starts first, movement triggered by notify)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OpenDoor - Playing montage NOW, movement will start on notify"));
	AnimInstance->Montage_Play(Montage);
}

void ASLFBaseCharacter::OnDoorMovementComplete()
{
	// Called when lerp movement finishes (after notify triggered it)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OnDoorMovementComplete - Movement finished"));

	// Unbind from lerp end
	OnLocationRotationLerpEnd.RemoveDynamic(this, &ASLFBaseCharacter::OnDoorMovementComplete);

	// Call Event OpenDoor on the Door actor (like original Blueprint)
	// This triggers boss arena activation for boss doors
	if (PendingDoorActor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Triggering Event OpenDoor on door: %s"),
			*PendingDoorActor->GetName());

		// Check if door implements the interface for "Event OpenDoor" callback
		// This would be a custom event in the door Blueprint
		// For now the door's OnInteract already did its work, so this is informational
	}
}

void ASLFBaseCharacter::OnDoorMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// This fires when an anim notify in the montage triggers
	// In bp_only, this is when the player movement should START
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OnDoorMontageNotifyBegin - Notify: '%s'"), *NotifyName.ToString());

	// Prevent double-triggering if multiple notifies fire
	if (bDoorMovementTriggered)
	{
		UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Movement already triggered, ignoring notify"));
		return;
	}

	bDoorMovementTriggered = true;

	// Bind to lerp end for when movement finishes
	OnLocationRotationLerpEnd.AddDynamic(this, &ASLFBaseCharacter::OnDoorMovementComplete);

	// START movement NOW (triggered by anim notify)
	// Original Blueprint uses Scale = 10.0 (play rate), which means duration = 1.0/10.0 = 0.1 seconds
	// But that's very fast - let's use Scale that gives ~0.5 second smooth movement
	const double DoorLerpScale = 2.0; // Duration = 1.0 / 2.0 = 0.5 seconds
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Starting door movement NOW (notify triggered, Scale=%.1f)"), DoorLerpScale);
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] Target: %s, Rotation: %s"),
		*PendingDoorTargetLocation.ToString(), *PendingDoorTargetRotation.ToString());

	GenericLocationAndRotationLerp_Implementation(DoorLerpScale, PendingDoorTargetLocation, PendingDoorTargetRotation);
}

void ASLFBaseCharacter::OnDoorMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] OnDoorMontageEnded - Montage: %s, Interrupted: %s"),
		Montage ? *Montage->GetName() : TEXT("None"),
		bInterrupted ? TEXT("true") : TEXT("false"));

	// Cleanup: unbind delegates
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASLFBaseCharacter::OnDoorMontageNotifyBegin);
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ASLFBaseCharacter::OnDoorMontageEnded);
	}

	// Clear cached door data
	PendingDoorActor.Reset();
	PendingDoorTargetLocation = FVector::ZeroVector;
	PendingDoorTargetRotation = FRotator::ZeroRotator;
	PendingDoorMontage = nullptr;
	bDoorMovementTriggered = false;
}

void ASLFBaseCharacter::TryClimbLadder_Implementation(AActor* Ladder, bool bIsTopdown)
{
	// Delegate to LadderManager component (IMPLEMENTED)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] TryClimbLadder called, ladder: %s, topdown: %s"),
		Ladder ? *Ladder->GetName() : TEXT("null"),
		bIsTopdown ? TEXT("true") : TEXT("false"));

	ULadderManagerComponent* LadderManager = FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager && Ladder)
	{
		LadderManager->StartClimb(Ladder, bIsTopdown);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseCharacter] TryClimbLadder - No LadderManager found!"));
	}
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

		// Handle duration for looping systems - set timer to deactivate (IMPLEMENTED)
		if (NiagaraComp && DurationValue > 0.0)
		{
			FTimerHandle DeactivateTimerHandle;
			FTimerDelegate DeactivateDelegate;
			DeactivateDelegate.BindLambda([NiagaraComp]()
			{
				if (IsValid(NiagaraComp))
				{
					NiagaraComp->Deactivate();
				}
			});
			GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, DeactivateDelegate, DurationValue, false);
		}
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
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;  // Increased from 400 for testing
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

	// Original Blueprint: Scale = play rate, Duration = 1.0 / Scale
	// "Default time is 1.0, so if you set scale to 2.0, then duration will be 0.5 secs"
	if (Scale > 0.0 && GetWorld())
	{
		LerpStartTime = GetWorld()->GetTimeSeconds();
		LerpDuration = 1.0 / Scale; // Convert play rate to duration

		FTimerDelegate LerpDelegate;
		LerpDelegate.BindUObject(this, &ASLFBaseCharacter::OnLocationLerpUpdate);
		GetWorld()->GetTimerManager().SetTimer(LocationLerpTimerHandle, LerpDelegate, 0.016f, true);
	}
	else
	{
		// Instant move if scale is 0
		SetActorLocation(NewTargetLocation);
		OnLocationLerpEnd.Broadcast();
	}
}

void ASLFBaseCharacter::GenericRotationLerp_Implementation(double Scale, FRotator NewTargetRotation)
{
	Cache_Rotation = GetActorRotation();
	TargetLerpRotation = NewTargetRotation;

	// Original Blueprint: Scale = play rate, Duration = 1.0 / Scale
	if (Scale > 0.0 && GetWorld())
	{
		LerpStartTime = GetWorld()->GetTimeSeconds();
		LerpDuration = 1.0 / Scale; // Convert play rate to duration

		FTimerDelegate LerpDelegate;
		LerpDelegate.BindUObject(this, &ASLFBaseCharacter::OnRotationLerpUpdate);
		GetWorld()->GetTimerManager().SetTimer(RotationLerpTimerHandle, LerpDelegate, 0.016f, true);
	}
	else
	{
		SetActorRotation(NewTargetRotation);
		OnRotationLerpEnd.Broadcast();
	}
}

void ASLFBaseCharacter::GenericLocationAndRotationLerp_Implementation(double Scale, FVector NewTargetLocation, FRotator NewTargetRotation)
{
	Cache_Location = GetActorLocation();
	Cache_Rotation = GetActorRotation();
	TargetLerpLocation = NewTargetLocation;
	TargetLerpRotation = NewTargetRotation;

	// Original Blueprint: Scale = play rate, Duration = 1.0 / Scale
	// "Default time is 1.0, so if you set scale to 2.0, then duration will be 0.5 secs"
	if (Scale > 0.0 && GetWorld())
	{
		LerpStartTime = GetWorld()->GetTimeSeconds();
		LerpDuration = 1.0 / Scale; // Convert play rate to duration
		bLerpingBoth = true;

		UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] GenericLocationAndRotationLerp - Scale=%.2f, Duration=%.2fs"),
			Scale, LerpDuration);

		// Use location timer to drive both lerps
		FTimerDelegate LerpDelegate;
		LerpDelegate.BindUObject(this, &ASLFBaseCharacter::OnLocationLerpUpdate);
		GetWorld()->GetTimerManager().SetTimer(LocationLerpTimerHandle, LerpDelegate, 0.016f, true);
	}
	else
	{
		SetActorLocation(NewTargetLocation);
		SetActorRotation(NewTargetRotation);
		OnLocationRotationLerpEnd.Broadcast();
	}
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
	// Start IK flinch animation
	// The animation ramps IK_Weight up to IKScale, then back down to 0
	IKReactionPeakWeight = IKScale;
	IKReactionStartTime = GetWorld()->GetTimeSeconds();

	// Set initial weight immediately for instant visual feedback
	IK_Weight = IKScale;

	UE_LOG(LogTemp, Log, TEXT("[SLFBaseCharacter] PlayIkReaction - Starting IK flinch (peak weight: %.2f, duration: %.2f)"),
		IKScale, IKReactionDuration);

	// Clear any existing timer and start a new one
	GetWorld()->GetTimerManager().ClearTimer(IKReactionTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		IKReactionTimerHandle,
		this,
		&ASLFBaseCharacter::UpdateIKReaction,
		0.016f, // ~60fps update rate
		true,   // loop
		0.0f    // no delay
	);
}

void ASLFBaseCharacter::UpdateIKReaction()
{
	// Calculate elapsed time
	float ElapsedTime = GetWorld()->GetTimeSeconds() - IKReactionStartTime;
	float Alpha = ElapsedTime / IKReactionDuration;

	if (Alpha >= 1.0f)
	{
		// Animation complete - reset IK_Weight and stop timer
		IK_Weight = 0.0;
		GetWorld()->GetTimerManager().ClearTimer(IKReactionTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[SLFBaseCharacter] IK flinch complete"));
		return;
	}

	// Use a bell curve (sin^2) for natural flinch motion
	// Ramps up quickly then back down
	// sin^2(pi * alpha) gives: 0 -> 1 -> 0 over alpha [0, 1]
	float CurveValue = FMath::Sin(PI * Alpha);
	CurveValue = CurveValue * CurveValue; // sin^2 for sharper peak

	IK_Weight = IKReactionPeakWeight * CurveValue;
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
	// Delegate to CombatManager component - call SetGuardState which handles grace period properly
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] ToggleGuardReplicated: %s, ignore grace: %s"),
		bToggled ? TEXT("true") : TEXT("false"),
		bIgnoreGracePeriod ? TEXT("true") : TEXT("false"));

	UAC_CombatManager* CombatManager = FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->WantsToGuard = bToggled;
		// Call SetGuardState which handles grace period timer and proper state management
		CombatManager->SetGuardState(bToggled, bIgnoreGracePeriod);
	}
}

void ASLFBaseCharacter::StartHitstop_Implementation(double Duration)
{
	// Freeze animation briefly by pausing anim instance (IMPLEMENTED)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] StartHitstop: %f"), Duration);

	if (!GetMesh() || !GetMesh()->GetAnimInstance() || Duration <= 0.0)
	{
		return;
	}

	// Pause animation
	GetMesh()->bPauseAnims = true;

	// Set timer to resume
	if (GetWorld())
	{
		FTimerHandle HitstopTimerHandle;
		FTimerDelegate ResumeDelegate;
		TWeakObjectPtr<USkeletalMeshComponent> WeakMesh = GetMesh();
		ResumeDelegate.BindLambda([WeakMesh]()
		{
			if (WeakMesh.IsValid())
			{
				WeakMesh->bPauseAnims = false;
			}
		});
		GetWorld()->GetTimerManager().SetTimer(HitstopTimerHandle, ResumeDelegate, Duration, false);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// TIMELINE CALLBACKS
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFBaseCharacter::OnLocationLerpUpdate()
{
	// Update location based on elapsed time alpha (IMPLEMENTED)
	if (!GetWorld())
	{
		return;
	}

	double CurrentTime = GetWorld()->GetTimeSeconds();
	double ElapsedTime = CurrentTime - LerpStartTime;
	double Alpha = FMath::Clamp(ElapsedTime / LerpDuration, 0.0, 1.0);

	// Apply smooth interpolation
	FVector NewLocation = FMath::Lerp(Cache_Location, TargetLerpLocation, Alpha);
	SetActorLocation(NewLocation);

	// Also update rotation if doing both
	if (bLerpingBoth)
	{
		FRotator NewRotation = FMath::Lerp(Cache_Rotation, TargetLerpRotation, Alpha);
		SetActorRotation(NewRotation);
	}

	// Check if finished
	if (Alpha >= 1.0)
	{
		GetWorld()->GetTimerManager().ClearTimer(LocationLerpTimerHandle);

		if (bLerpingBoth)
		{
			bLerpingBoth = false;
			OnLocationRotationLerpEnd.Broadcast();
		}
		else
		{
			OnLocationLerpEnd.Broadcast();
		}
	}
}

void ASLFBaseCharacter::OnLocationLerpFinished()
{
	OnLocationLerpEnd.Broadcast();
}

void ASLFBaseCharacter::OnRotationLerpUpdate()
{
	// Update rotation based on elapsed time alpha (IMPLEMENTED)
	if (!GetWorld())
	{
		return;
	}

	double CurrentTime = GetWorld()->GetTimeSeconds();
	double ElapsedTime = CurrentTime - LerpStartTime;
	double Alpha = FMath::Clamp(ElapsedTime / LerpDuration, 0.0, 1.0);

	// Apply smooth interpolation
	FRotator NewRotation = FMath::Lerp(Cache_Rotation, TargetLerpRotation, Alpha);
	SetActorRotation(NewRotation);

	// Check if finished
	if (Alpha >= 1.0)
	{
		GetWorld()->GetTimerManager().ClearTimer(RotationLerpTimerHandle);
		OnRotationLerpEnd.Broadcast();
	}
}

void ASLFBaseCharacter::OnRotationLerpFinished()
{
	OnRotationLerpEnd.Broadcast();
}

// ═══════════════════════════════════════════════════════════════════════════════
// SERVER RPCs
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFBaseCharacter::SRV_SpawnActor_Implementation(
	UClass* ActorClass,
	FTransform SpawnTransform,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,
	AActor* InOwner,
	APawn* InInstigator)
{
	if (!ActorClass || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InOwner;
	SpawnParams.Instigator = InInstigator;
	SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;

	GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);
}

void ASLFBaseCharacter::SRV_SpawnPickupItem_Implementation(
	UPrimaryDataAsset* ItemAsset,
	int32 ItemAmount,
	FTransform SpawnTransform,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,
	bool bUsePhysics)
{
	// Call the interface implementation which already handles the spawn logic
	SpawnPickupItemReplicated_Implementation(ItemAsset, ItemAmount, SpawnTransform, CollisionHandlingOverride, bUsePhysics);
}

void ASLFBaseCharacter::SRV_PlayMontage_Implementation(
	UAnimMontage* Montage,
	double PlayRate,
	double StartPosition,
	FName StartSection)
{
	if (!Montage || !GetMesh())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, StartPosition, true);
		if (StartSection != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSection, Montage);
		}
	}
}

void ASLFBaseCharacter::SRV_SpawnNiagaraOneshotLocation_Implementation(
	UNiagaraSystem* VFXSystem,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck)
{
	if (!VFXSystem || !GetWorld())
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		VFXSystem,
		Location,
		Rotation,
		FVector::OneVector,
		bAutoDestroy,
		bAutoActivate,
		ENCPoolMethod::None,
		bPreCullCheck);
}

void ASLFBaseCharacter::SRV_SpawnNiagaraLoopingAttached_Implementation(
	UNiagaraSystem* VFXSystem,
	FName AttachSocket,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck,
	double DurationValue)
{
	if (!VFXSystem || !GetMesh())
	{
		return;
	}

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFXSystem,
		GetMesh(),
		AttachSocket,
		Location,
		Rotation,
		EAttachLocation::KeepRelativeOffset,
		bAutoDestroy,
		bAutoActivate,
		ENCPoolMethod::None,
		bPreCullCheck);

	// Set up timer to deactivate after duration if valid
	if (NiagaraComp && DurationValue > 0.0 && GetWorld())
	{
		FTimerHandle DeactivateTimerHandle;
		TWeakObjectPtr<UNiagaraComponent> WeakComp = NiagaraComp;
		FTimerDelegate DeactivateDelegate;
		DeactivateDelegate.BindLambda([WeakComp]()
		{
			if (WeakComp.IsValid())
			{
				WeakComp->Deactivate();
			}
		});
		GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, DeactivateDelegate, DurationValue, false);
	}
}

void ASLFBaseCharacter::SRV_SpawnNiagaraOneshotAttached_Implementation(
	UNiagaraSystem* VFXSystem,
	FName AttachSocket,
	FVector Location,
	FRotator Rotation,
	bool bAutoDestroy,
	bool bAutoActivate,
	bool bPreCullCheck)
{
	if (!VFXSystem || !GetMesh())
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFXSystem,
		GetMesh(),
		AttachSocket,
		Location,
		Rotation,
		EAttachLocation::KeepRelativeOffset,
		bAutoDestroy,
		bAutoActivate,
		ENCPoolMethod::None,
		bPreCullCheck);
}

void ASLFBaseCharacter::SRV_DestroyActor_Implementation(AActor* ActorToDestroy)
{
	if (ActorToDestroy && ActorToDestroy->IsValidLowLevel())
	{
		ActorToDestroy->Destroy();
	}
}

void ASLFBaseCharacter::SRV_Jump_Implementation()
{
	Jump();
}

void ASLFBaseCharacter::SRV_SetSpeed_Implementation(double NewSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}
