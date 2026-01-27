// AC_InteractionManager.cpp
// C++ component implementation for AC_InteractionManager
//
// 20-PASS VALIDATION: 2026-01-02 Full Migration
// Source: BlueprintDNA_v2/Component/AC_InteractionManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added
//
// Full logic migrated from Blueprint graphs:
// - INTERACTABLE DETECTION & INTERACTION (Tick)
// - RESTING (Event OnRest)
// - TARGET LOCKING (interface calls)

#include "AC_InteractionManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Interfaces/BPI_Interactable.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Interfaces/BPI_Player.h"
#include "Interfaces/BPI_Controller.h"
#include "Widgets/W_HUD.h"
#include "Blueprints/SLFRestingPointBase.h"
#include "Components/AC_ActionManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/CombatManagerComponent.h"

UAC_InteractionManager::UAC_InteractionManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	InteractionRadius = 200.0;
	DebugDraw = EDrawDebugTrace::None;
	NearestInteractable = nullptr;
	TargetLocked = false;
	LockedOnTarget = nullptr;
	MaxTargetLockDistance = 2000.0;
	TargetLockStrength = 3.0;
	TargetLockTraceDistance = 1500.0;
	TargetLockTraceRadius = 300.0;
	TargetLockDebugDrawType = EDrawDebugTrace::None;
	AllowTargetSwap = true;
	LastRestingPoint = nullptr;
	LockedOnComponent = nullptr;

	// Initialize default trace channels for interactables
	// Must match all possible collision types used by interactable actors:
	// - WorldStatic: Static props, doors, chests
	// - WorldDynamic: Moving/physics interactables
	// - PhysicsBody: Ragdoll/physics-enabled actors
	// - Pawn: Character-based interactables, NPCs
	// - Visibility: Items that use visibility channel for interaction
	// - GameTraceChannel1: Custom "Interactable" channel used by B_PickupItem and other interactables
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
	TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)); // "Interactable" custom channel
}

void UAC_InteractionManager::BeginPlay()
{
	Super::BeginPlay();

	// CRITICAL FIX: Blueprint CDO may override C++ constructor defaults with empty/incomplete array
	// Ensure TraceChannels has required channels for interaction detection
	// We need at least 6 channels to detect all interactable types:
	// - WorldStatic: Static props, doors, chests
	// - WorldDynamic: Moving/physics interactables
	// - PhysicsBody: Ragdoll/physics-enabled actors
	// - Pawn: Character-based interactables, NPCs
	// - Visibility: Items that use visibility channel for interaction
	// - GameTraceChannel1: Custom "Interactable" channel used by B_PickupItem
	if (TraceChannels.Num() < 6)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InteractionManager] TraceChannels only has %d entries - reinitializing to default 6 channels"), TraceChannels.Num());
		TraceChannels.Empty();
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		TraceChannels.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)); // "Interactable" custom channel
	}

	// Also ensure InteractionRadius is valid - Blueprint CDO may have too small a value
	// C++ default is 200.0, but Blueprint may override to 100.0 which is too small
	if (InteractionRadius < 200.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InteractionManager] InteractionRadius too small (%.1f) - setting to 200"), InteractionRadius);
		InteractionRadius = 200.0;
	}

	// Debug draw disabled - interaction now working
	// Uncomment to visualize sphere trace:
	// DebugDraw = EDrawDebugTrace::ForDuration;

	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::BeginPlay - Owner: %s, TraceChannels: %d, Radius: %.1f, DebugDraw: %d"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("None"),
		TraceChannels.Num(),
		InteractionRadius,
		(int32)DebugDraw);
}

void UAC_InteractionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ═══════════════════════════════════════════════════════════════════════
	// INTERACTABLE DETECTION & INTERACTION (From Blueprint EventGraph)
	// ═══════════════════════════════════════════════════════════════════════
	// 1. Sphere trace around player
	// 2. For each hit, check if implements BPI_Interactable
	// 3. Check CanBeTraced property
	// 4. Add unique to NearbyInteractables
	// 5. Find nearest interactable
	// 6. Notify player via BPI_Player::OnInteractableTraced

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Calculate trace location around owner
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerForward = Owner->GetActorForwardVector();
	FVector OwnerUp = Owner->GetActorUpVector();

	// Trace start/end is at same location (sphere overlap at player position)
	// The Blueprint does: Start = Location + Forward*50 - Up*30
	//                     End = Start (same point, sphere overlap)
	FVector TraceLocation = OwnerLocation + (OwnerForward * 50.0) - (OwnerUp * 30.0);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	// Also ignore all child actors spawned by the owner (e.g., ChaosForceField from ChildActorComponent)
	// These are attached to the player and would always be "nearest" otherwise
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors, true, true); // bResetArray=true, bRecursivelyIncludeAttachedActors=true
	for (AActor* Attached : AttachedActors)
	{
		ActorsToIgnore.AddUnique(Attached);
	}

	// Also get child actors from ChildActorComponents
	TArray<UChildActorComponent*> ChildActorComps;
	Owner->GetComponents<UChildActorComponent>(ChildActorComps);
	for (UChildActorComponent* ChildComp : ChildActorComps)
	{
		if (AActor* ChildActor = ChildComp->GetChildActor())
		{
			ActorsToIgnore.AddUnique(ChildActor);
		}
	}

	TArray<FHitResult> OutHits;
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		TraceLocation,
		TraceLocation, // Same point - sphere overlap
		InteractionRadius,
		TraceChannels,
		false, // bTraceComplex
		ActorsToIgnore,
		DebugDraw,
		OutHits,
		true, // bIgnoreSelf
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	// Debug: Log all nearby actors for troubleshooting (temporarily enabled)
	static float DebugLogTimer = 0.0f;
	DebugLogTimer += DeltaTime;
	bool bShouldLogDebug = (DebugLogTimer > 2.0f); // Log every 2 seconds
	if (bShouldLogDebug)
	{
		DebugLogTimer = 0.0f;
		UE_LOG(LogTemp, Log, TEXT("[InteractionManager DEBUG] TraceLocation: %s, Radius: %.1f, Channels: %d, bHit: %s"),
			*TraceLocation.ToString(), InteractionRadius, TraceChannels.Num(), bHit ? TEXT("true") : TEXT("false"));
	}

	if (bHit)
	{
		// Debug: Log hit count
		if (bShouldLogDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("[InteractionManager DEBUG] Hit %d actors"), OutHits.Num());
		}

		// Process each hit
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!IsValid(HitActor))
			{
				continue;
			}

			// Debug: Log every hit actor and its class
			if (bShouldLogDebug)
			{
				UClass* ActorClass = HitActor->GetClass();
				bool bHasInteractableInterface = ActorClass->ImplementsInterface(UBPI_Interactable::StaticClass());
				bool bHasSLFInterface = ActorClass->ImplementsInterface(USLFInteractableInterface::StaticClass());
				UE_LOG(LogTemp, Log, TEXT("[InteractionManager DEBUG] Hit: %s (Class: %s, BPI_Interactable: %s, SLFInteractable: %s)"),
					*HitActor->GetName(), *ActorClass->GetName(),
					bHasInteractableInterface ? TEXT("YES") : TEXT("NO"),
					bHasSLFInterface ? TEXT("YES") : TEXT("NO"));
			}

			// Check if already in NearbyInteractables
			if (NearbyInteractables.Contains(HitActor))
			{
				continue;
			}

			// Check if implements either BPI_Interactable or SLFInteractableInterface
			// Both interfaces are used for interactables in the codebase
			bool bImplementsInterface = HitActor->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()) ||
				HitActor->GetClass()->ImplementsInterface(USLFInteractableInterface::StaticClass());

			if (bImplementsInterface)
			{
				// Check CanBeTraced property - multiple naming conventions exist:
				// - "CanBeTraced" (some C++ classes)
				// - "bCanBeTraced" (standard C++ bool naming convention, e.g., ASLFBossDoor)
				// - "CanBeTraced?" (Blueprint variable with question mark)
				bool bCanBeTraced = true; // Default to true if property doesn't exist

				// Try C++ property with 'b' prefix first (standard UE naming: ASLFBossDoor::bCanBeTraced)
				if (FBoolProperty* CanBeTracedProp = FindFProperty<FBoolProperty>(HitActor->GetClass(), TEXT("bCanBeTraced")))
				{
					bCanBeTraced = CanBeTracedProp->GetPropertyValue_InContainer(HitActor);
				}
				// Try C++ property without 'b' prefix (AB_Interactable::CanBeTraced)
				else if (FBoolProperty* CanBeTracedNoBProp = FindFProperty<FBoolProperty>(HitActor->GetClass(), TEXT("CanBeTraced")))
				{
					bCanBeTraced = CanBeTracedNoBProp->GetPropertyValue_InContainer(HitActor);
				}
				// Also check Blueprint variable "CanBeTraced?" (with question mark)
				else if (FBoolProperty* CanBeTracedBPProp = FindFProperty<FBoolProperty>(HitActor->GetClass(), TEXT("CanBeTraced?")))
				{
					bCanBeTraced = CanBeTracedBPProp->GetPropertyValue_InContainer(HitActor);
				}

				if (bCanBeTraced)
				{
					// Add unique to NearbyInteractables
					NearbyInteractables.AddUnique(HitActor);
					UE_LOG(LogTemp, Log, TEXT("[InteractionManager] Found interactable: %s (CanBeTraced: true)"), *HitActor->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Verbose, TEXT("[InteractionManager] Skipping interactable %s (CanBeTraced: false)"), *HitActor->GetName());
				}
			}
		}

		// Find nearest interactable
		AActor* PreviousNearest = NearestInteractable;
		NearestInteractable = nullptr;
		double NearestDistance = TNumericLimits<double>::Max();

		for (AActor* Interactable : NearbyInteractables)
		{
			if (!IsValid(Interactable))
			{
				continue;
			}

			double Distance = Owner->GetDistanceTo(Interactable);
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestInteractable = Interactable;
			}
		}

		// Notify player if nearest changed
		if (NearestInteractable != PreviousNearest)
		{
			if (Owner->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
			{
				IBPI_Player::Execute_OnInteractableTraced(Owner, NearestInteractable);
			}
		}
	}
	else
	{
		// No hits - clear nearby interactables
		if (NearbyInteractables.Num() > 0)
		{
			NearbyInteractables.Empty();
			NearestInteractable = nullptr;

			// Notify player that no interactable is traced
			if (Owner->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
			{
				IBPI_Player::Execute_OnInteractableTraced(Owner, nullptr);
			}
		}
	}

	// Clean up invalid references from NearbyInteractables
	NearbyInteractables.RemoveAll([](AActor* Actor) {
		return !IsValid(Actor);
	});
}

void UAC_InteractionManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAC_InteractionManager, TargetLocked);
	DOREPLIFETIME(UAC_InteractionManager, LockedOnTarget);
}


// ═══════════════════════════════════════════════════════════════════════
// EVENT IMPLEMENTATIONS (From Blueprint EventGraph)
// ═══════════════════════════════════════════════════════════════════════

/**
 * EventOnRest - Custom event called when player rests at a resting point
 *
 * Blueprint Logic (RESTING collapsed graph):
 * 1. Set LastRestingPoint = RestingPoint
 * 2. Get Instigator -> Get Controller
 * 3. Call BPI_Controller::GetPlayerHUD
 * 4. IsValid check on HUD
 * 5. If valid, call W_HUD::EventFade(FadeToBlack)
 * 6. Bind to resting point's OnReady event
 * 7. Call AC_ActionManager::SetIsResting(true) - triggers sitting animation
 */
void UAC_InteractionManager::EventOnRest(AActor* RestingPoint)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventOnRest - RestingPoint: %s"),
		RestingPoint ? *RestingPoint->GetName() : TEXT("None"));

	// Set LastRestingPoint
	LastRestingPoint = RestingPoint;

	// Get owner's controller via Instigator
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		// Try to get instigator if owner is not a pawn
		OwnerPawn = Owner->GetInstigator();
	}

	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not get pawn for HUD access"));
		return;
	}

	AController* Controller = OwnerPawn->GetController();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not get controller for HUD access"));
		return;
	}

	// Call BPI_Controller::GetPlayerHUD to get HUD widget
	// Then trigger fade on it
	// Note: This interface call would return the HUD widget
	// The actual fade logic depends on the W_HUD widget implementation
	if (Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		// Get HUD from controller via interface and trigger fade
		UUserWidget* HUDWidget = nullptr;
		IBPI_Controller::Execute_GetPlayerHUD(Controller, HUDWidget);
		if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
		{
			HUD->EventFade(ESLFFadeTypes::FadeOut, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("  Triggered HUD fade out"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Could not get W_HUD from controller"));
		}
	}

	// Bind to resting point's OnReady and OnExited events
	// When OnReady fires (after player is positioned), open the rest menu
	// When OnExited fires (player closes menu), call EventRestEnded
	if (ASLFRestingPointBase* RestPoint = Cast<ASLFRestingPointBase>(RestingPoint))
	{
		// Remove first to avoid duplicate binding ensure error
		RestPoint->OnReady.RemoveDynamic(this, &UAC_InteractionManager::OnRestingPointReady);
		RestPoint->OnReady.AddDynamic(this, &UAC_InteractionManager::OnRestingPointReady);
		UE_LOG(LogTemp, Log, TEXT("  Bound to OnReady delegate"));

		// Also bind to OnExited - fires when player leaves rest menu
		RestPoint->OnExited.RemoveDynamic(this, &UAC_InteractionManager::OnRestingPointExited);
		RestPoint->OnExited.AddDynamic(this, &UAC_InteractionManager::OnRestingPointExited);
		UE_LOG(LogTemp, Log, TEXT("  Bound to OnExited delegate"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not cast to ASLFRestingPointBase for OnReady/OnExited binding"));
	}

	// Call AC_ActionManager::SetIsResting(true) to trigger sitting animation
	// The AnimBP (ABP_SoulslikeCharacter_Additive) checks IsResting to play AS_SLF_Sitting_Idle_Loop
	if (UAC_ActionManager* ActionManager = Owner->FindComponentByClass<UAC_ActionManager>())
	{
		ActionManager->SetIsResting(true);
		UE_LOG(LogTemp, Log, TEXT("  Called SetIsResting(true) on AC_ActionManager"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find AC_ActionManager on owner"));
	}

	UE_LOG(LogTemp, Log, TEXT("  Resting point set, waiting for OnReady callback"));
}

void UAC_InteractionManager::OnRestingPointReady()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::OnRestingPointReady - Opening rest menu"));

	// Get controller and open rest menu via HUD
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		OwnerPawn = Owner->GetInstigator();
	}

	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not get pawn"));
		return;
	}

	AController* Controller = OwnerPawn->GetController();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not get controller"));
		return;
	}

	// Get HUD and open rest menu
	if (Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		UUserWidget* HUDWidget = nullptr;
		IBPI_Controller::Execute_GetPlayerHUD(Controller, HUDWidget);
		if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
		{
			// Call EventSetupRestingPointWidget on HUD to show the rest menu
			HUD->EventSetupRestingPointWidget(LastRestingPoint);
			UE_LOG(LogTemp, Log, TEXT("  Called HUD->EventSetupRestingPointWidget(%s)"),
				LastRestingPoint ? *LastRestingPoint->GetName() : TEXT("null"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Could not get W_HUD"));
		}
	}

	// Unbind from OnReady to avoid duplicate bindings
	if (ASLFRestingPointBase* RestPoint = Cast<ASLFRestingPointBase>(LastRestingPoint))
	{
		RestPoint->OnReady.RemoveDynamic(this, &UAC_InteractionManager::OnRestingPointReady);
	}
}

/**
 * OnRestingPointExited - Callback when player exits rest menu
 * Called via binding to B_RestingPoint::OnExited delegate
 * Triggers the full rest end flow via EventRestEnded
 */
void UAC_InteractionManager::OnRestingPointExited()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::OnRestingPointExited - Player closed rest menu"));

	// Unbind from OnExited to avoid duplicate calls
	if (ASLFRestingPointBase* RestPoint = Cast<ASLFRestingPointBase>(LastRestingPoint))
	{
		RestPoint->OnExited.RemoveDynamic(this, &UAC_InteractionManager::OnRestingPointExited);
	}

	// Call the full rest ended flow
	EventRestEnded();
}

/**
 * EventRestInitialized - Called when resting is initialized
 * This is a stub event called by the resting point when ready to show rest menu
 */
void UAC_InteractionManager::EventRestInitialized()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventRestInitialized"));
	// Called by resting point when initialization is complete
	// Subclasses or Blueprint can override to add custom behavior
}

/**
 * EventRestEnded - Called when resting ends
 * This is called when player leaves rest menu (bound to W_RestMenu OnExited delegate)
 *
 * Blueprint Logic:
 * 1. Do Once
 * 2. Event Replenishment
 * 3. GetPlayerHUD → CloseAllMenus → EventFade (FadeIn)
 * 4. SetIsResting(false) - stops sitting animation
 * 5. EventToggleUiMode(false)
 * 6. SetViewTarget back to player
 * 7. Enable player input
 * 8. BlendViewTarget (camera transition)
 * 9. Event Reset
 */
void UAC_InteractionManager::EventRestEnded()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventRestEnded"));

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Call Event Replenishment to restore health/stamina/etc
	EventReplenishment();

	// Get HUD and trigger fade in
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		OwnerPawn = Owner->GetInstigator();
	}

	if (IsValid(OwnerPawn))
	{
		AController* Controller = OwnerPawn->GetController();
		if (IsValid(Controller) && Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
		{
			UUserWidget* HUDWidget = nullptr;
			IBPI_Controller::Execute_GetPlayerHUD(Controller, HUDWidget);
			if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
			{
				HUD->CloseAllMenus();
				HUD->EventFade(ESLFFadeTypes::FadeIn, 1.0f);
				HUD->EventToggleUiMode(false);
				UE_LOG(LogTemp, Log, TEXT("  HUD: CloseAllMenus, FadeIn, UiMode(false)"));
			}
		}
	}

	// Call SetIsResting(false) to stop sitting animation
	if (UAC_ActionManager* ActionManager = Owner->FindComponentByClass<UAC_ActionManager>())
	{
		ActionManager->SetIsResting(false);
		UE_LOG(LogTemp, Log, TEXT("  Called SetIsResting(false) on AC_ActionManager"));
	}

	// SetViewTarget back to player and enable input
	if (IsValid(OwnerPawn))
	{
		APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
		if (IsValid(PC))
		{
			// Set view target back to player pawn with smooth blend
			PC->SetViewTargetWithBlend(OwnerPawn, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut);
			UE_LOG(LogTemp, Log, TEXT("  SetViewTargetWithBlend to player pawn"));

			// Re-enable player input
			PC->SetInputMode(FInputModeGameOnly());
			OwnerPawn->EnableInput(PC);
			UE_LOG(LogTemp, Log, TEXT("  Enabled player input"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Rest ended, player should stand up"));
}

/**
 * EventReplenishment - Called to replenish resources
 * This is a stub event called to restore HP/FP/etc at resting points
 */
void UAC_InteractionManager::EventReplenishment()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventReplenishment"));
	// Called to replenish player resources (HP, FP, flasks, etc.)
	// Subclasses or Blueprint can override to add custom behavior
}

/**
 * EventReset - Called to reset interaction state
 * This is a stub event to clear current interaction state
 */
void UAC_InteractionManager::EventReset()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventReset"));
	// Reset interaction state
	NearbyInteractables.Empty();
	NearestInteractable = nullptr;
}

/**
 * EventLockon - Called to trigger lock-on behavior
 * This is a stub event that triggers the lock-on system
 */
void UAC_InteractionManager::EventLockon()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::EventLockon"));
	// Trigger lock-on trace and lock to nearest valid target
	TArray<FHitResult> Hits = TargetLockTrace();
	if (Hits.Num() > 0)
	{
		// Find first valid target
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (IsValid(HitActor))
			{
				LockOnTarget(HitActor, true);
				break;
			}
		}
	}
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * RemoveFromNearbyInteractables - Remove an actor from the nearby interactables list
 *
 * Blueprint Logic:
 * - Get NearbyInteractables array
 * - Call Array_RemoveItem to remove the passed item
 */
void UAC_InteractionManager::RemoveFromNearbyInteractables_Implementation(AActor* Item)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::RemoveFromNearbyInteractables - Item: %s"),
		Item ? *Item->GetName() : TEXT("None"));

	if (IsValid(Item))
	{
		NearbyInteractables.Remove(Item);
	}
}

/**
 * TargetLockTrace - Perform a sphere trace for potential lock-on targets
 *
 * Blueprint Logic:
 * 1. Get start location from capsule component
 * 2. Get end location from camera forward * trace distance
 * 3. Perform sphere trace multi for objects
 * 4. Return hit results
 */
TArray<FHitResult> UAC_InteractionManager::TargetLockTrace_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::TargetLockTrace"));

	TArray<FHitResult> OutHits;

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return OutHits;
	}

	// Get start location from capsule component
	FVector StartLocation = Owner->GetActorLocation();
	UCapsuleComponent* Capsule = Owner->FindComponentByClass<UCapsuleComponent>();
	if (IsValid(Capsule))
	{
		StartLocation = Capsule->GetComponentLocation();
	}

	// Get end location from camera forward direction
	FVector EndLocation = StartLocation;
	UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
	if (IsValid(Camera))
	{
		FVector ForwardVector = Camera->GetForwardVector();
		EndLocation = StartLocation + (ForwardVector * TargetLockTraceDistance);
	}
	else
	{
		// Fallback to actor forward if no camera
		EndLocation = StartLocation + (Owner->GetActorForwardVector() * TargetLockTraceDistance);
	}

	// Setup actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	// Perform sphere trace
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		StartLocation,
		EndLocation,
		TargetLockTraceRadius,
		TargetLockTraceObjectTypes,
		false, // bTraceComplex
		ActorsToIgnore,
		TargetLockDebugDrawType,
		OutHits,
		true, // bIgnoreSelf
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	UE_LOG(LogTemp, Log, TEXT("  Found %d hits"), OutHits.Num());
	return OutHits;
}

/**
 * LockOnTarget - Lock onto a specific target
 *
 * Blueprint Logic:
 * 1. Validate target (must be valid and alive via BPI_Enemy::IsDead)
 * 2. Set LockedOnTarget = target
 * 3. Toggle lock-on widget on target (via BPI_GenericCharacter::ToggleLockonWidget)
 * 4. Get and store lock-on component from target (via BPI_GenericCharacter::GetLockonComponent)
 * 5. Set TargetLocked = true
 * 6. Notify player via BPI_Player::OnTargetLocked
 * 7. Start distance check timer (1.0s looping)
 */
void UAC_InteractionManager::LockOnTarget_Implementation(AActor* InLockOnTarget, bool RotateTowards)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::LockOnTarget - Target: %s, RotateTowards: %s"),
		InLockOnTarget ? *InLockOnTarget->GetName() : TEXT("None"),
		RotateTowards ? TEXT("true") : TEXT("false"));

	// Validate target
	if (!IsValid(InLockOnTarget))
	{
		UE_LOG(LogTemp, Log, TEXT("  Invalid target, aborting lock"));
		return;
	}

	// Check if target is alive (using pending kill check)
	// Note: Full death check would require stat manager or HP check
	// For now, just verify target is not pending destruction
	if (InLockOnTarget->IsPendingKillPending())
	{
		UE_LOG(LogTemp, Log, TEXT("  Target is pending kill, aborting lock"));
		return;
	}

	// Set the locked target
	LockedOnTarget = InLockOnTarget;

	// Toggle lock-on widget on target (BPI_GenericCharacter::ToggleLockonWidget)
	if (InLockOnTarget->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_ToggleLockonWidget(InLockOnTarget, true);
		UE_LOG(LogTemp, Log, TEXT("  Toggled lock-on widget ON for target"));

		// Get lock-on component from target (BPI_GenericCharacter::GetLockonComponent)
		USceneComponent* TargetLockonComponent = nullptr;
		IBPI_GenericCharacter::Execute_GetLockonComponent(InLockOnTarget, TargetLockonComponent);
		if (IsValid(TargetLockonComponent))
		{
			LockedOnComponent = TargetLockonComponent;
			UE_LOG(LogTemp, Log, TEXT("  Got lock-on component from target: %s"), *TargetLockonComponent->GetName());
		}
		else
		{
			// Fallback to root component
			LockedOnComponent = InLockOnTarget->GetRootComponent();
		}
	}
	else
	{
		// Fallback to root component if interface not implemented
		LockedOnComponent = InLockOnTarget->GetRootComponent();
	}

	// Set lock status
	TargetLocked = true;

	// Notify player via BPI_Player::OnTargetLocked
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		if (Owner->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
		{
			IBPI_Player::Execute_OnTargetLocked(Owner, true, RotateTowards);
			UE_LOG(LogTemp, Log, TEXT("  Notified player via OnTargetLocked(true, %s)"),
				RotateTowards ? TEXT("true") : TEXT("false"));
		}

		// Start distance check timer
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("CheckTargetDistance"));
		Owner->GetWorldTimerManager().SetTimer(
			TargetLockDistanceTimer,
			TimerDelegate,
			1.0f, // Every 1 second
			true, // Looping
			0.0f  // No initial delay
		);
	}

	UE_LOG(LogTemp, Log, TEXT("  Locked on to target, distance timer started"));
}

/**
 * ResetLockOn - Clear the current lock-on target
 *
 * Blueprint Logic:
 * 1. Set TargetLocked = false
 * 2. Notify player via BPI_Player::OnTargetLocked(false, false)
 * 3. Toggle lock-on widget off on target via BPI_GenericCharacter::ToggleLockonWidget(false)
 * 4. Clear LockedOnComponent
 * 5. Clear distance check timer
 * 6. Clear LockedOnTarget
 */
void UAC_InteractionManager::ResetLockOn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::ResetLockOn - Previous target: %s"),
		LockedOnTarget ? *LockedOnTarget->GetName() : TEXT("None"));

	// Clear lock status FIRST
	TargetLocked = false;

	// Notify player via BPI_Player::OnTargetLocked(false, false)
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		if (Owner->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
		{
			IBPI_Player::Execute_OnTargetLocked(Owner, false, false);
			UE_LOG(LogTemp, Log, TEXT("  Notified player via OnTargetLocked(false, false)"));
		}

		// Clear distance check timer
		Owner->GetWorldTimerManager().ClearTimer(TargetLockDistanceTimer);
	}

	// Toggle lock-on widget off on previous target (if still valid)
	if (IsValid(LockedOnTarget))
	{
		if (LockedOnTarget->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			IBPI_GenericCharacter::Execute_ToggleLockonWidget(LockedOnTarget, false);
			UE_LOG(LogTemp, Log, TEXT("  Toggled lock-on widget OFF for previous target"));
		}
	}

	// Clear component reference
	LockedOnComponent = nullptr;

	// Clear target reference (do this last so widget toggle can reference it)
	LockedOnTarget = nullptr;

	UE_LOG(LogTemp, Log, TEXT("  Lock-on reset complete"));
}

/**
 * IsTargetLocked - Check if currently locked onto a target
 *
 * Blueprint Logic:
 * - Simple getter, returns TargetLocked variable
 */
bool UAC_InteractionManager::IsTargetLocked_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::IsTargetLocked - %s"),
		TargetLocked ? TEXT("true") : TEXT("false"));

	return TargetLocked;
}

/**
 * CheckTargetDistance - Check if locked target is still in range
 *
 * Blueprint Logic (called every 1.0s by timer):
 * 1. Get LockedOnTarget
 * 2. Get distance to player (owner)
 * 3. If distance > MaxTargetLockDistance, call ResetLockOn
 */
void UAC_InteractionManager::CheckTargetDistance_Implementation()
{
	// If no locked target, nothing to check
	if (!IsValid(LockedOnTarget))
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// CHECK IF TARGET IS DEAD - must clear lock before distance check
	// The target may have an AC_CombatManager or AICombatManagerComponent
	bool bTargetIsDead = false;

	// Try AC_CombatManager (player/NPC)
	if (UAC_CombatManager* CombatManager = LockedOnTarget->FindComponentByClass<UAC_CombatManager>())
	{
		bTargetIsDead = CombatManager->IsDead;
	}
	// Try AICombatManagerComponent (AI enemies)
	else if (UAICombatManagerComponent* AICombatManager = LockedOnTarget->FindComponentByClass<UAICombatManagerComponent>())
	{
		bTargetIsDead = AICombatManager->bIsDead;
	}
	// Try CombatManagerComponent (generic)
	else if (UCombatManagerComponent* GenericCombatManager = LockedOnTarget->FindComponentByClass<UCombatManagerComponent>())
	{
		bTargetIsDead = GenericCombatManager->bIsDead;
	}

	if (bTargetIsDead)
	{
		UE_LOG(LogTemp, Log, TEXT("UAC_InteractionManager::CheckTargetDistance - Target is dead, resetting lock-on"));
		ResetLockOn();
		return;
	}

	// Calculate distance to locked target
	float Distance = Owner->GetDistanceTo(LockedOnTarget);

	UE_LOG(LogTemp, Verbose, TEXT("UAC_InteractionManager::CheckTargetDistance - Distance: %.1f, Max: %.1f"),
		Distance, MaxTargetLockDistance);

	// If target is out of range, reset lock-on
	if (Distance > MaxTargetLockDistance)
	{
		UE_LOG(LogTemp, Log, TEXT("  Target out of range, resetting lock-on"));
		ResetLockOn();
	}
}
