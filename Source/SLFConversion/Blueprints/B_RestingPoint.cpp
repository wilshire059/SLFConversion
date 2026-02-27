// B_RestingPoint.cpp
// C++ implementation for Blueprint B_RestingPoint
//
// 20-PASS VALIDATION: 2026-01-16
// Source: BlueprintDNA/Blueprint/B_RestingPoint.json
// EventGraph Logic: OnInteract, OnTraced, DiscoverPoint, OnConstruction

#include "Blueprints/B_RestingPoint.h"
#include "Interfaces/SLFPlayerInterface.h"
#include "Interfaces/SLFControllerInterface.h"
#include "Interfaces/SLFRestingPointInterface.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimMontage.h"

AB_RestingPoint::AB_RestingPoint()
{
	// IMPORTANT: Do NOT create components here with CreateDefaultSubobject
	// The Blueprint SCS defines these components with their proper assets (Niagara systems, etc.)
	// If C++ creates them, it shadows the Blueprint components and loses the asset references

	// Initialize pointers to nullptr - they'll be found by name in BeginPlay
	SittingZone = nullptr;
	Scene = nullptr;
	CameraArm = nullptr;
	LookatCamera = nullptr;
	UnlockEffect = nullptr;
	Effect = nullptr;
	EffectLight = nullptr;
	EnvironmentLight = nullptr;

	// Default values
	SittingAngle = 0.0;
	ForceFaceSittingActor = false;
	CameraDistance = 300.0;
	CameraOffset = FVector::ZeroVector;
	CameraRotation = FRotator::ZeroRotator;
	SittingActor = nullptr;
}

void AB_RestingPoint::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::BeginPlay - %s"), *GetName());

	// Find components by name from Blueprint SCS
	// These were defined in the Blueprint with proper Niagara system assets, attachments, etc.
	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		FString CompName = Comp->GetName();

		if (CompName == TEXT("SittingZone"))
		{
			SittingZone = Cast<UBillboardComponent>(Comp);
		}
		else if (CompName == TEXT("Scene"))
		{
			Scene = Cast<USceneComponent>(Comp);
		}
		else if (CompName == TEXT("CameraArm"))
		{
			CameraArm = Cast<USpringArmComponent>(Comp);
		}
		else if (CompName == TEXT("LookatCamera"))
		{
			LookatCamera = Cast<UCameraComponent>(Comp);
		}
		else if (CompName == TEXT("UnlockEffect"))
		{
			UnlockEffect = Cast<UNiagaraComponent>(Comp);
		}
		else if (CompName == TEXT("Effect"))
		{
			Effect = Cast<UNiagaraComponent>(Comp);
		}
		else if (CompName == TEXT("EffectLight"))
		{
			EffectLight = Cast<UPointLightComponent>(Comp);
		}
		else if (CompName == TEXT("EnvironmentLight"))
		{
			EnvironmentLight = Cast<UPointLightComponent>(Comp);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Found components - SittingZone: %s, Scene: %s, CameraArm: %s, LookatCamera: %s"),
		SittingZone ? TEXT("YES") : TEXT("NO"),
		Scene ? TEXT("YES") : TEXT("NO"),
		CameraArm ? TEXT("YES") : TEXT("NO"),
		LookatCamera ? TEXT("YES") : TEXT("NO"));
	UE_LOG(LogTemp, Log, TEXT("  UnlockEffect: %s, Effect: %s, EffectLight: %s, EnvironmentLight: %s"),
		UnlockEffect ? TEXT("YES") : TEXT("NO"),
		Effect ? TEXT("YES") : TEXT("NO"),
		EffectLight ? TEXT("YES") : TEXT("NO"),
		EnvironmentLight ? TEXT("YES") : TEXT("NO"));

	// Log Niagara system info if found
	if (UnlockEffect)
	{
		UNiagaraSystem* NiagaraSys = UnlockEffect->GetAsset();
		UE_LOG(LogTemp, Log, TEXT("  UnlockEffect NiagaraSystem: %s"),
			NiagaraSys ? *NiagaraSys->GetName() : TEXT("NONE"));
	}
}

void AB_RestingPoint::GetReplenishData_Implementation(TArray<UPrimaryDataAsset*>& OutItemsToReplenish, TArray<FGameplayTag>& OutStatsToReplenish)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::GetReplenishData"));

	// Convert ItemsToReplenish TSet to TArray
	// Blueprint: ItemsToReplenish (TSet) -> To Array -> OutItemsToReplenish
	OutItemsToReplenish = ItemsToReplenish.Array();

	UE_LOG(LogTemp, Log, TEXT("  Items to replenish: %d"), OutItemsToReplenish.Num());

	// Convert StatsToReplenish GameplayTagContainer to TArray<FGameplayTag>
	// Blueprint: StatsToReplenish -> Break Gameplay Tag Container -> OutStatsToReplenish
	OutStatsToReplenish.Empty();
	StatsToReplenish.GetGameplayTagArray(OutStatsToReplenish);

	UE_LOG(LogTemp, Log, TEXT("  Stats to replenish: %d"), OutStatsToReplenish.Num());
}

void AB_RestingPoint::GetRestingPointSpawnPosition_Implementation(bool& OutSuccess, FVector& OutLocation, FRotator& OutRotation)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::GetRestingPointSpawnPosition"));

	// Blueprint Logic:
	// 1. Check if Scene component is valid
	// 2. Get Scene component world transform
	// 3. Break transform to get Location and Rotation
	// 4. Return Success = true if valid

	if (IsValid(Scene))
	{
		FTransform SceneTransform = Scene->GetComponentTransform();
		OutLocation = SceneTransform.GetLocation();
		OutRotation = SceneTransform.GetRotation().Rotator();
		OutSuccess = true;

		UE_LOG(LogTemp, Log, TEXT("  Success - Location: %s, Rotation: %s"),
			*OutLocation.ToString(), *OutRotation.ToString());
	}
	else
	{
		// Fallback to actor location if Scene component not available
		OutLocation = GetActorLocation();
		OutRotation = GetActorRotation();
		OutSuccess = false;

		UE_LOG(LogTemp, Warning, TEXT("  Scene component not valid, using actor transform"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENTGRAPH: OnInteract
// Blueprint Logic:
//   Branch: IsActivated?
//     FALSE -> DiscoverPoint(InteractingActor)
//     TRUE  -> SittingActor = InteractingActor
//              Cast to BPI_Player -> OnRest(this)
//              Delay 1.0s -> PositionSittingActor()
// ═══════════════════════════════════════════════════════════════════════════════
void AB_RestingPoint::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::OnInteract - %s, IsActivated: %d"),
		*GetName(), IsActivated);

	if (!IsActivated)
	{
		// First discovery - activate the resting point
		DiscoverPoint(InteractingActor);
	}
	else
	{
		// Already activated - player sits down to rest
		SittingActor = InteractingActor;

		// Call OnRest on player via BPI_Player interface
		if (IsValid(SittingActor) && SittingActor->GetClass()->ImplementsInterface(USLFPlayerInterface::StaticClass()))
		{
			ISLFPlayerInterface::Execute_OnRest(SittingActor, this);
			UE_LOG(LogTemp, Log, TEXT("  Called OnRest on player"));
		}

		// Delay 1.0 second then position the sitting actor
		// Blueprint uses Delay node with 1.0 second
		GetWorld()->GetTimerManager().SetTimer(
			PositionTimerHandle,
			this,
			&AB_RestingPoint::PositionSittingActor,
			1.0f,  // 1 second delay from Blueprint
			false  // Don't loop
		);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENTGRAPH: OnTraced
// Blueprint Logic:
//   Parent: OnTraced(TracedBy)
//   Select Text (IsActivated):
//     TRUE  -> "Rest"
//     FALSE -> "Discover Resting Point"
//   Set InteractionText
// ═══════════════════════════════════════════════════════════════════════════════
void AB_RestingPoint::OnTraced_Implementation(AActor* TracedBy)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::OnTraced - %s, IsActivated: %d"),
		*GetName(), IsActivated);

	// Call parent implementation if it exists
	// Note: AB_Interactable doesn't have OnTraced_Implementation, but interface does
	// Super::OnTraced_Implementation(TracedBy);

	// Set interaction text based on activation state
	if (IsActivated)
	{
		InteractionText = FText::FromString(TEXT("Rest"));
	}
	else
	{
		InteractionText = FText::FromString(TEXT("Discover Resting Point"));
	}

	UE_LOG(LogTemp, Log, TEXT("  InteractionText set to: %s"), *InteractionText.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENTGRAPH: DiscoverPoint (Custom Event)
// Blueprint Logic:
//   1. Activate UnlockEffect (Niagara)
//   2. Set Collision Object Type (already interactable via parent)
//   3. Async Load DiscoverInteractionMontage
//   4. Cast to AnimMontage -> Cast InteractingActor to BPI_Player
//      -> Call DiscoverRestingPoint(Montage, this)
//   5. Set IsActivated = true
//   6. Get Player Controller -> Cast to BPI_Controller
//      -> SerializeAllDataForSaving(SaveBehavior: SaveInstantly)
// ═══════════════════════════════════════════════════════════════════════════════
void AB_RestingPoint::DiscoverPoint(AActor* DiscoveringActor)
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::DiscoverPoint - %s"), *GetName());

	// 1. Activate unlock effect (Niagara)
	if (IsValid(UnlockEffect))
	{
		UnlockEffect->Activate(false);
		UE_LOG(LogTemp, Log, TEXT("  Activated UnlockEffect"));
	}

	// 2. Collision is already set on parent's InteractableSM component

	// 3 & 4. Load discovery montage and call DiscoverRestingPoint on player
	if (!DiscoverInteractionMontage.IsNull())
	{
		UAnimMontage* Montage = DiscoverInteractionMontage.LoadSynchronous();
		if (Montage)
		{
			// Call BPI_Player::DiscoverRestingPoint
			if (IsValid(DiscoveringActor) && DiscoveringActor->GetClass()->ImplementsInterface(USLFPlayerInterface::StaticClass()))
			{
				ISLFPlayerInterface::Execute_DiscoverRestingPoint(DiscoveringActor, Montage, this);
				UE_LOG(LogTemp, Log, TEXT("  Called DiscoverRestingPoint with montage: %s"), *Montage->GetName());
			}
		}
	}

	// 5. Mark as activated
	IsActivated = true;
	UE_LOG(LogTemp, Log, TEXT("  IsActivated = true"));

	// 5b. Register for fast travel
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (IsValid(PC))
	{
		if (USaveLoadManagerComponent* SaveMgr = PC->FindComponentByClass<USaveLoadManagerComponent>())
		{
			FSLFRestPointSaveInfo RestInfo;
			RestInfo.RestPointId = ID; // FGuid from AB_Interactable base
			RestInfo.LocationName = LocationName;
			RestInfo.WorldLocation = GetActorLocation();
			bool bSpawnSuccess = false;
			GetRestingPointSpawnPosition(bSpawnSuccess, RestInfo.SpawnLocation, RestInfo.SpawnRotation);
			if (!bSpawnSuccess)
			{
				RestInfo.SpawnLocation = GetActorLocation();
				RestInfo.SpawnRotation = GetActorRotation();
			}
			SaveMgr->RegisterDiscoveredRestPoint(RestInfo);
		}

		// 6. Trigger save via BPI_Controller::SerializeAllDataForSaving
		if (PC->GetClass()->ImplementsInterface(USLFControllerInterface::StaticClass()))
		{
			ISLFControllerInterface::Execute_SerializeAllDataForSaving(PC, ESLFSaveBehavior::SaveInstantly);
			UE_LOG(LogTemp, Log, TEXT("  Called SerializeAllDataForSaving with SaveInstantly"));
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER: PositionSittingActor
// Blueprint Logic (after 1.0s delay from OnInteract):
//   1. Get SittingZone world location
//   2. SetActorLocation(SittingActor, SittingZone.WorldLocation)
//   3. If ForceFaceSittingActor:
//        Calculate rotation from actor to SittingZone
//        SetActorRotation(SittingActor, LookAtRotation)
//   4. Broadcast OnReady dispatcher
// ═══════════════════════════════════════════════════════════════════════════════
void AB_RestingPoint::PositionSittingActor()
{
	UE_LOG(LogTemp, Log, TEXT("AB_RestingPoint::PositionSittingActor"));

	if (!IsValid(SittingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("  SittingActor is not valid"));
		return;
	}

	if (!IsValid(SittingZone))
	{
		UE_LOG(LogTemp, Warning, TEXT("  SittingZone is not valid"));
		// Still broadcast OnReady even if positioning fails
		OnReady.Broadcast();
		return;
	}

	// Move actor to sitting position
	FVector SittingLocation = SittingZone->GetComponentLocation();
	SittingActor->SetActorLocation(SittingLocation);
	UE_LOG(LogTemp, Log, TEXT("  Positioned SittingActor at: %s"), *SittingLocation.ToString());

	// Rotate to face camera if configured
	if (ForceFaceSittingActor)
	{
		// Blueprint: Find Look At Rotation from actor to sitting zone
		// This makes the actor face the camera position
		FVector CameraLocation = LookatCamera ? LookatCamera->GetComponentLocation() : SittingZone->GetComponentLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			SittingActor->GetActorLocation(),
			CameraLocation
		);
		SittingActor->SetActorRotation(LookAtRotation);
		UE_LOG(LogTemp, Log, TEXT("  Rotated SittingActor to face camera: %s"), *LookAtRotation.ToString());
	}

	// Broadcast OnReady - rest menu can now open
	OnReady.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("  Broadcast OnReady"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTION: OnConstruction (UserConstructionScript)
// Blueprint Logic:
//   CameraArm->SetRelativeRotation(0, SittingAngle + 180, 0)
//   CameraArm->TargetArmLength = CameraDistance
//   LookatCamera->SetRelativeRotation(CameraRotation)
//   LookatCamera->SetRelativeLocation(CameraOffset)
// ═══════════════════════════════════════════════════════════════════════════════
void AB_RestingPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Find components by name if not already found (OnConstruction runs before BeginPlay)
	if (!CameraArm || !LookatCamera)
	{
		TArray<UActorComponent*> Components;
		GetComponents(Components);

		for (UActorComponent* Comp : Components)
		{
			FString CompName = Comp->GetName();

			if (CompName == TEXT("CameraArm") && !CameraArm)
			{
				CameraArm = Cast<USpringArmComponent>(Comp);
			}
			else if (CompName == TEXT("LookatCamera") && !LookatCamera)
			{
				LookatCamera = Cast<UCameraComponent>(Comp);
			}
		}
	}

	// Setup camera arm based on configured values
	if (IsValid(CameraArm))
	{
		// Blueprint: Set Relative Rotation with Yaw = SittingAngle + 180
		FRotator ArmRotation = FRotator(0.0, SittingAngle + 180.0, 0.0);
		CameraArm->SetRelativeRotation(ArmRotation);
		CameraArm->TargetArmLength = CameraDistance;
	}

	// Setup lookat camera
	if (IsValid(LookatCamera))
	{
		LookatCamera->SetRelativeRotation(CameraRotation);
		LookatCamera->SetRelativeLocation(CameraOffset);
	}
}
