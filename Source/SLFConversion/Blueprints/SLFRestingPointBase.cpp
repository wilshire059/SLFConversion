// SLFRestingPointBase.cpp
// C++ implementation for B_RestingPoint
//
// ═══════════════════════════════════════════════════════════════════════════════
// FULL EVENTGRAPH MIGRATION 2026-01-16
// ═══════════════════════════════════════════════════════════════════════════════
// Source: BlueprintDNA_v2/Blueprint/B_RestingPoint.json
//
// EventGraph Logic Migrated:
// - OnInteract: Branch on IsActivated -> DiscoverPoint (first time) or Rest (subsequent)
// - OnTraced: Set InteractionText based on IsActivated
// - DiscoverPoint: Activate effect, load montage, call BPI_Player::DiscoverRestingPoint, save
// - PositionSittingActor: Move player to SittingZone after 1s delay, broadcast OnReady
// - OnConstruction: Camera setup (SittingAngle + 180, CameraDistance, CameraOffset, CameraRotation)

#include "SLFRestingPointBase.h"
#include "Interfaces/SLFPlayerInterface.h"
#include "Interfaces/SLFControllerInterface.h"
#include "Interfaces/SLFRestingPointInterface.h"
#include "Interfaces/BPI_Player.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SLFEnums.h"

ASLFRestingPointBase::ASLFRestingPointBase()
{
	// ═══════════════════════════════════════════════════════════════════════
	// INTERACTION COLLISION COMPONENT (Created in C++)
	// ═══════════════════════════════════════════════════════════════════════
	// B_RestingPoint doesn't have a mesh component for collision detection.
	// The visual bonfire is typically level geometry. We need a collision
	// sphere so AC_InteractionManager can detect this actor via sphere trace.
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	if (InteractionCollision)
	{
		// Attach to inherited DefaultSceneRoot (from SLFInteractableBase)
		// DO NOT set as root - Blueprint SCS components expect DefaultSceneRoot to be root
		InteractionCollision->SetupAttachment(GetRootComponent());

		// Set sphere radius for interaction detection
		InteractionCollision->InitSphereRadius(100.0f);

		// CRITICAL: Set collision to GameTraceChannel1 ("Interactable" custom channel)
		// This is what AC_InteractionManager traces for
		InteractionCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// Don't block anything, just respond to traces
		InteractionCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		InteractionCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

		// Make it invisible but keep collision
		InteractionCollision->SetHiddenInGame(true);
	}

	// ═══════════════════════════════════════════════════════════════════════
	// CACHED COMPONENTS: From Blueprint SCS (NOT created here)
	// ═══════════════════════════════════════════════════════════════════════
	// DO NOT use CreateDefaultSubobject - Blueprint SCS already has these:
	// - SittingZone (Billboard)
	// - Scene/SpawnScene
	// - CameraArm (SpringArm)
	// - LookatCamera (Camera)
	// - UnlockEffect (Niagara)
	// - Effect (Niagara)
	// - EffectLight (PointLight)
	// - EnvironmentLight (PointLight)

	// Initialize cached component pointers to nullptr
	CachedSittingZone = nullptr;
	CachedSpawnScene = nullptr;
	CachedCameraArm = nullptr;
	CachedLookatCamera = nullptr;
	CachedUnlockEffect = nullptr;
	CachedEffect = nullptr;
	CachedEffectLight = nullptr;
	CachedEnvironmentLight = nullptr;

	// ═══════════════════════════════════════════════════════════════════════
	// DEFAULT VALUES
	// ═══════════════════════════════════════════════════════════════════════

	LocationName = FText::FromString(TEXT("Resting Point"));
	SittingAngle = 0.0;
	bForceFaceSittingActor = false;
	CameraDistance = 300.0f;
	CameraOffset = FVector(0.0f, 0.0f, 100.0f);
	CameraRotation = FRotator(-20.0f, 0.0f, 0.0f);
	SittingActor = nullptr;

	// CRITICAL: Set default InteractionText for undiscovered state
	InteractionText = FText::FromString(TEXT("Discover Resting Point"));

	// CRITICAL: Default CanBeTraced to TRUE so it can be detected
	CanBeTraced = true;

	// CRITICAL: Default IsActivated to FALSE (not discovered yet)
	IsActivated = false;
}

void ASLFRestingPointBase::BeginPlay()
{
	Super::BeginPlay();

	// ═══════════════════════════════════════════════════════════════════════
	// FIND COMPONENTS FROM BLUEPRINT SCS
	// ═══════════════════════════════════════════════════════════════════════
	// These components are created by Blueprint's SimpleConstructionScript
	// We need to find them by name to get references for C++ logic

	TArray<UActorComponent*> AllComponents;
	GetComponents(AllComponents);

	for (UActorComponent* Comp : AllComponents)
	{
		FString CompName = Comp->GetName();

		if (CompName.Contains(TEXT("SittingZone")))
		{
			CachedSittingZone = Cast<UBillboardComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("Scene")) && !CachedSpawnScene)
		{
			CachedSpawnScene = Cast<USceneComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("CameraArm")))
		{
			CachedCameraArm = Cast<USpringArmComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("LookatCamera")))
		{
			CachedLookatCamera = Cast<UCameraComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("UnlockEffect")))
		{
			CachedUnlockEffect = Cast<UNiagaraComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("Effect")) && !CachedEffect && !CompName.Contains(TEXT("Unlock")) && !CompName.Contains(TEXT("Light")))
		{
			CachedEffect = Cast<UNiagaraComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("EffectLight")))
		{
			CachedEffectLight = Cast<UPointLightComponent>(Comp);
		}
		else if (CompName.Contains(TEXT("EnvironmentLight")))
		{
			CachedEnvironmentLight = Cast<UPointLightComponent>(Comp);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] BeginPlay: %s, IsActivated: %s, CanBeTraced: %s"),
		*LocationName.ToString(),
		IsActivated ? TEXT("true") : TEXT("false"),
		CanBeTraced ? TEXT("true") : TEXT("false"));

	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Components - SittingZone: %s, CameraArm: %s, LookatCamera: %s, UnlockEffect: %s"),
		CachedSittingZone ? TEXT("found") : TEXT("null"),
		CachedCameraArm ? TEXT("found") : TEXT("null"),
		CachedLookatCamera ? TEXT("found") : TEXT("null"),
		CachedUnlockEffect ? TEXT("found") : TEXT("null"));

	// Log collision component status (created in C++ constructor)
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] InteractionCollision component: %s"),
		InteractionCollision ? TEXT("created") : TEXT("null"));

	// Set interaction text based on activation state
	if (IsActivated)
	{
		InteractionText = FText::FromString(TEXT("Rest"));
	}
	else
	{
		InteractionText = FText::FromString(TEXT("Discover Resting Point"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// OnConstruction (UserConstructionScript Logic)
// Blueprint: CameraArm->SetRelativeRotation(0, SittingAngle + 180, 0)
//            CameraArm->TargetArmLength = CameraDistance
//            LookatCamera->SetRelativeRotation(CameraRotation)
//            LookatCamera->SetRelativeLocation(CameraOffset)
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFRestingPointBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Find components if not yet cached (OnConstruction runs before BeginPlay)
	if (!CachedCameraArm || !CachedLookatCamera)
	{
		TArray<UActorComponent*> AllComponents;
		GetComponents(AllComponents);
		for (UActorComponent* Comp : AllComponents)
		{
			FString CompName = Comp->GetName();
			if (CompName.Contains(TEXT("CameraArm")))
			{
				CachedCameraArm = Cast<USpringArmComponent>(Comp);
			}
			else if (CompName.Contains(TEXT("LookatCamera")))
			{
				CachedLookatCamera = Cast<UCameraComponent>(Comp);
			}
		}
	}

	if (IsValid(CachedCameraArm))
	{
		FRotator ArmRotation = FRotator(0.0, SittingAngle + 180.0, 0.0);
		CachedCameraArm->SetRelativeRotation(ArmRotation);
		CachedCameraArm->TargetArmLength = CameraDistance;
	}

	if (IsValid(CachedLookatCamera))
	{
		CachedLookatCamera->SetRelativeRotation(CameraRotation);
		CachedLookatCamera->SetRelativeLocation(CameraOffset);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// GetReplenishData
// ═══════════════════════════════════════════════════════════════════════════════
FSLFReplenishData ASLFRestingPointBase::GetReplenishData_Implementation()
{
	FSLFReplenishData Data;
	Data.ItemsToReplenish = ItemsToReplenish;
	Data.StatsToReplenish = StatsToReplenish;
	return Data;
}

// ═══════════════════════════════════════════════════════════════════════════════
// OnInteract (EventGraph Logic)
// Blueprint:
//   Branch: IsActivated?
//     FALSE -> DiscoverPoint(InteractingActor)
//     TRUE  -> SittingActor = InteractingActor
//              Cast to BPI_Player -> OnRest(this)
//              Delay 1.0s -> PositionSittingActor()
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFRestingPointBase::OnInteract_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] OnInteract - %s, IsActivated: %s"),
		*GetName(), IsActivated ? TEXT("true") : TEXT("false"));

	if (!IsActivated)
	{
		// First discovery
		DiscoverPoint(Interactor);
	}
	else
	{
		// Guard against re-interaction while REST is in progress
		// This prevents double-click or input buffer re-trigger from resetting the timer
		if (IsValid(SittingActor))
		{
			UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Already resting, ignoring re-interact"));
			return;
		}

		// Already activated - player sits down to rest
		SittingActor = Interactor;

		// Call OnRest on player via BPI_Player interface
		if (IsValid(SittingActor))
		{
			// Try SLFPlayerInterface first (C++ interface)
			if (SittingActor->GetClass()->ImplementsInterface(USLFPlayerInterface::StaticClass()))
			{
				ISLFPlayerInterface::Execute_OnRest(SittingActor, this);
				UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Called ISLFPlayerInterface::OnRest"));
			}
			// Fallback to BPI_Player (Blueprint interface)
			else if (SittingActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
			{
				IBPI_Player::Execute_OnRest(SittingActor, this);
				UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Called IBPI_Player::OnRest"));
			}
		}

		// Delay 1.0 second then position the sitting actor
		GetWorld()->GetTimerManager().SetTimer(
			PositionTimerHandle,
			this,
			&ASLFRestingPointBase::PositionSittingActor,
			1.0f,  // 1 second delay from Blueprint
			false  // Don't loop
		);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// OnTraced (EventGraph Logic)
// Blueprint:
//   Select Text (IsActivated):
//     TRUE  -> "Rest"
//     FALSE -> "Discover Resting Point"
//   Set InteractionText
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFRestingPointBase::OnTraced_Implementation(AActor* TracedBy)
{
	// Set interaction text based on activation state
	if (IsActivated)
	{
		InteractionText = FText::FromString(TEXT("Rest"));
	}
	else
	{
		InteractionText = FText::FromString(TEXT("Discover Resting Point"));
	}

	UE_LOG(LogTemp, Verbose, TEXT("[RestingPoint] OnTraced - InteractionText: %s"),
		*InteractionText.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// DiscoverPoint (EventGraph Custom Event)
// Blueprint:
//   1. Activate UnlockEffect (Niagara)
//   2. Async Load DiscoverInteractionMontage
//   3. Cast InteractingActor to BPI_Player -> DiscoverRestingPoint(Montage, this)
//   4. Set IsActivated = true
//   5. Get Player Controller -> Cast to BPI_Controller
//      -> SerializeAllDataForSaving(SaveBehavior: SaveInstantly)
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFRestingPointBase::DiscoverPoint(AActor* DiscoveringActor)
{
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] DiscoverPoint - %s"), *GetName());

	// 1. Activate unlock effect
	if (IsValid(CachedUnlockEffect))
	{
		CachedUnlockEffect->Activate(false);
		UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Activated UnlockEffect"));
	}

	// 2 & 3. Load discovery montage and call DiscoverRestingPoint on player
	if (!DiscoverInteractionMontage.IsNull())
	{
		UAnimMontage* Montage = DiscoverInteractionMontage.LoadSynchronous();
		if (Montage && IsValid(DiscoveringActor))
		{
			// Try SLFPlayerInterface first
			if (DiscoveringActor->GetClass()->ImplementsInterface(USLFPlayerInterface::StaticClass()))
			{
				ISLFPlayerInterface::Execute_DiscoverRestingPoint(DiscoveringActor, Montage, this);
				UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Called ISLFPlayerInterface::DiscoverRestingPoint"));
			}
			// Fallback to BPI_Player
			else if (DiscoveringActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
			{
				IBPI_Player::Execute_DiscoverRestingPoint(DiscoveringActor, Montage, this);
				UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Called IBPI_Player::DiscoverRestingPoint"));
			}
		}
	}

	// 4. Mark as activated
	IsActivated = true;
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] IsActivated = true"));

	// Update interaction text now that it's activated
	InteractionText = FText::FromString(TEXT("Rest"));

	// 4b. Register for fast travel
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (IsValid(PC))
	{
		if (USaveLoadManagerComponent* SaveMgr = PC->FindComponentByClass<USaveLoadManagerComponent>())
		{
			FSLFRestPointSaveInfo RestInfo;
			RestInfo.RestPointId = ID; // FGuid from ASLFInteractableBase
			RestInfo.LocationName = LocationName;
			RestInfo.WorldLocation = GetActorLocation();
			// Get spawn position from interface
			if (GetClass()->ImplementsInterface(USLFRestingPointInterface::StaticClass()))
			{
				bool bSuccess = false;
				ISLFRestingPointInterface::Execute_GetRestingPointSpawnPosition(this, bSuccess, RestInfo.SpawnLocation, RestInfo.SpawnRotation);
				if (!bSuccess)
				{
					RestInfo.SpawnLocation = GetActorLocation();
					RestInfo.SpawnRotation = GetActorRotation();
				}
			}
			else
			{
				RestInfo.SpawnLocation = GetActorLocation();
				RestInfo.SpawnRotation = GetActorRotation();
			}
			SaveMgr->RegisterDiscoveredRestPoint(RestInfo);
		}

		// 5. Trigger save via BPI_Controller::SerializeAllDataForSaving
		if (PC->GetClass()->ImplementsInterface(USLFControllerInterface::StaticClass()))
		{
			ISLFControllerInterface::Execute_SerializeAllDataForSaving(PC, ESLFSaveBehavior::SaveInstantly);
			UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Called SerializeAllDataForSaving (SaveInstantly)"));
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PositionSittingActor (Helper - called after 1s delay)
// Blueprint:
//   1. Get SittingZone world location
//   2. SetActorLocation(SittingActor, SittingZone.WorldLocation)
//   3. If bForceFaceSittingActor:
//        Calculate rotation from actor to camera
//        SetActorRotation(SittingActor, LookAtRotation)
//   4. Broadcast OnReady
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFRestingPointBase::PositionSittingActor()
{
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] PositionSittingActor"));

	if (!IsValid(SittingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RestingPoint] SittingActor is not valid"));
		// Still broadcast OnReady so rest menu can open
		OnReady.Broadcast();
		return;
	}

	if (IsValid(CachedSittingZone))
	{
		// Move actor to sitting position
		FVector SittingLocation = CachedSittingZone->GetComponentLocation();
		SittingActor->SetActorLocation(SittingLocation);
		UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Positioned SittingActor at: %s"), *SittingLocation.ToString());
	}

	// Rotate to face camera if configured
	if (bForceFaceSittingActor && IsValid(CachedLookatCamera))
	{
		FVector CameraLocation = CachedLookatCamera->GetComponentLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			SittingActor->GetActorLocation(),
			CameraLocation
		);
		SittingActor->SetActorRotation(LookAtRotation);
		UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Rotated SittingActor to face camera: %s"), *LookAtRotation.ToString());
	}

	// Broadcast OnReady - rest menu can now open
	OnReady.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[RestingPoint] Broadcast OnReady"));
}
