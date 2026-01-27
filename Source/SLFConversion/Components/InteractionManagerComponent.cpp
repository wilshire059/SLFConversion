// InteractionManagerComponent.cpp
// C++ implementation for AC_InteractionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_InteractionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         19/19 (initialized in constructor)
// Functions:         13/13 implemented
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "InteractionManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/BPI_Interactable.h"
#include "Interfaces/SLFInteractableInterface.h"

UInteractionManagerComponent::UInteractionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// Initialize interaction config
	InteractionRadius = 200.0f;
	DebugDraw = EDrawDebugTrace::None;

	// Initialize target lock config
	MaxTargetLockDistance = 2000.0;
	TargetLockStrength = 5.0;
	TargetLockTraceDistance = 2000.0;
	TargetLockTraceRadius = 500.0f;
	TargetLockDebugDrawType = EDrawDebugTrace::None;

	// Initialize runtime
	NearestInteractable = nullptr;
	bTargetLocked = false;
	LockedOnTarget = nullptr;
	TargetIndex = 0;
	PlayerCamera = nullptr;
	LastTracedNPC = nullptr;
}

void UInteractionManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[InteractionManager] BeginPlay on %s"), *GetOwner()->GetName());
	Initialize();
}

void UInteractionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTargetLocked && LockedOnTarget)
	{
		UpdateTargetLock(DeltaTime);
	}
}

void UInteractionManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInteractionManagerComponent, bTargetLocked);
	DOREPLIFETIME(UInteractionManagerComponent, LockedOnTarget);
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION [3-5/13]
// ═══════════════════════════════════════════════════════════════════════════════

void UInteractionManagerComponent::TraceForInteractables_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	TArray<FHitResult> HitResults;
	FVector Origin = Owner->GetActorLocation();

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, Origin, Origin, InteractionRadius,
		TraceChannels, false, ActorsToIgnore, DebugDraw, HitResults, true);

	NearbyInteractables.Empty();
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				// Only add actors that implement the interactable interface
				if (HitActor->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()))
				{
					NearbyInteractables.AddUnique(HitActor);
				}
			}
		}
	}

	NearestInteractable = GetNearestFromArray(NearbyInteractables);
}

void UInteractionManagerComponent::TryInteract_Implementation()
{
	if (NearestInteractable)
	{
		UE_LOG(LogTemp, Log, TEXT("[InteractionManager] TryInteract with %s"), *NearestInteractable->GetName());

		// Call OnInteract via interface - check both BPI_Interactable and SLFInteractableInterface
		// B_DeathCurrency and other interactables use ISLFInteractableInterface
		if (NearestInteractable->GetClass()->ImplementsInterface(USLFInteractableInterface::StaticClass()))
		{
			ISLFInteractableInterface::Execute_OnInteract(NearestInteractable, GetOwner());
			UE_LOG(LogTemp, Log, TEXT("[InteractionManager] Called ISLFInteractableInterface::OnInteract on %s"), *NearestInteractable->GetName());
		}
		else if (NearestInteractable->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()))
		{
			IBPI_Interactable::Execute_OnInteract(NearestInteractable, GetOwner());
			UE_LOG(LogTemp, Log, TEXT("[InteractionManager] Called IBPI_Interactable::OnInteract on %s"), *NearestInteractable->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[InteractionManager] %s does not implement any interactable interface"), *NearestInteractable->GetName());
		}
	}
}

AActor* UInteractionManagerComponent::GetNearestFromArray_Implementation(const TArray<AActor*>& Actors)
{
	AActor* Owner = GetOwner();
	if (!Owner || Actors.Num() == 0) return nullptr;

	AActor* Nearest = nullptr;
	float MinDist = MAX_FLT;

	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			float Dist = FVector::Dist(Owner->GetActorLocation(), Actor->GetActorLocation());
			if (Dist < MinDist)
			{
				MinDist = Dist;
				Nearest = Actor;
			}
		}
	}
	return Nearest;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TARGET LOCK [6-10/13]
// ═══════════════════════════════════════════════════════════════════════════════

void UInteractionManagerComponent::ToggleTargetLock_Implementation()
{
	if (bTargetLocked)
	{
		UnlockTarget();
	}
	else
	{
		TraceForTargets();
		if (Targets.Num() > 0)
		{
			LockOnTarget(Targets[0]);
		}
	}
}

void UInteractionManagerComponent::TraceForTargets_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner || !PlayerCamera) return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	FVector Start = PlayerCamera->GetComponentLocation();
	FVector End = Start + PlayerCamera->GetForwardVector() * TargetLockTraceDistance;

	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, Start, End, TargetLockTraceRadius,
		TargetLockTraceObjectTypes, false, ActorsToIgnore,
		TargetLockDebugDrawType, HitResults, true);

	Targets.Empty();
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				for (const FName& Tag : TargetLockAcceptedActorTags)
				{
					if (HitActor->ActorHasTag(Tag))
					{
						Targets.AddUnique(HitActor);
						break;
					}
				}
			}
		}
	}
}

void UInteractionManagerComponent::LockOnTarget_Implementation(AActor* Target)
{
	if (!Target) return;

	bTargetLocked = true;
	LockedOnTarget = Target;
	TargetIndex = Targets.IndexOfByKey(Target);

	UE_LOG(LogTemp, Log, TEXT("[InteractionManager] LockOnTarget: %s"), *Target->GetName());

	SRV_SetTargetLock(Target, true);
}

void UInteractionManagerComponent::UnlockTarget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[InteractionManager] UnlockTarget"));

	bTargetLocked = false;
	LockedOnTarget = nullptr;
	Targets.Empty();

	SRV_SetTargetLock(nullptr, false);
}

void UInteractionManagerComponent::SwitchTarget_Implementation(bool bNext)
{
	if (Targets.Num() <= 1) return;

	if (bNext)
		TargetIndex = (TargetIndex + 1) % Targets.Num();
	else
		TargetIndex = (TargetIndex - 1 + Targets.Num()) % Targets.Num();

	LockedOnTarget = Targets[TargetIndex];
	UE_LOG(LogTemp, Log, TEXT("[InteractionManager] SwitchTarget: %s"),
		LockedOnTarget ? *LockedOnTarget->GetName() : TEXT("null"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [11-13/13]
// ═══════════════════════════════════════════════════════════════════════════════

void UInteractionManagerComponent::UpdateTargetLock_Implementation(float DeltaTime)
{
	if (!LockedOnTarget) return;

	// Check distance
	AActor* Owner = GetOwner();
	if (!Owner) return;

	float Distance = FVector::Dist(Owner->GetActorLocation(), LockedOnTarget->GetActorLocation());
	if (Distance > MaxTargetLockDistance)
	{
		UnlockTarget();
		return;
	}

	// Rotate camera towards target
	if (APlayerController* PC = Cast<APlayerController>(Cast<APawn>(Owner)->GetController()))
	{
		FVector TargetLoc = LockedOnTarget->GetActorLocation();
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			PlayerCamera->GetComponentLocation(), TargetLoc);

		FRotator CurrentRot = PC->GetControlRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, TargetLockStrength);
		PC->SetControlRotation(NewRot);
	}
}

void UInteractionManagerComponent::SRV_SetTargetLock_Implementation(AActor* Target, bool bLocked)
{
	bTargetLocked = bLocked;
	LockedOnTarget = Target;
}

void UInteractionManagerComponent::Initialize_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[InteractionManager] Initialize"));

	// Cache camera component
	if (AActor* Owner = GetOwner())
	{
		PlayerCamera = Owner->FindComponentByClass<UCameraComponent>();
	}

	// Start interaction trace timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(InteractionTimer, this,
			&UInteractionManagerComponent::TraceForInteractables_Implementation,
			0.1f, true);
	}
}
