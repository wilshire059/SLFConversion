// SLFBossDoor.cpp
// C++ implementation for B_BossDoor - Boss arena fog gate
//
// CRITICAL COMPONENT SETUP (from bp_only analysis):
// - FogGateMesh (fog gate mesh): Scale 3.25, 0.1, 4.25 | Z=200 | Yaw=90
// - PortalEffectFront: Y=-100, Yaw=-90, attached to FogGateMesh
// - PortalEffectBack: Y=100, Yaw=-90, attached to FogGateMesh

#include "SLFBossDoor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Interfaces/BPI_GenericCharacter.h"

ASLFBossDoor::ASLFBossDoor()
{
	// Boss doors don't auto close
	bAutoClose = false;
	bIsLocked = false;

	// Set default interaction text - "Enter" for fog gates
	InteractionText = FText::FromString(TEXT("Enter"));

	// ============================================================
	// CREATE INTERACTABLE SM (Fog Gate Mesh)
	// CRITICAL: Scale must be 3.25, 0.1, 4.25 to fill doorway
	// ============================================================
	FogGateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable SM"));
	FogGateMesh->SetupAttachment(RootComponent);
	FogGateMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	FogGateMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	FogGateMesh->SetRelativeScale3D(FVector(3.25f, 0.1f, 4.25f)); // CRITICAL SCALE
	FogGateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FogGateMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	FogGateMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	FogGateMesh->SetGenerateOverlapEvents(true);
	FogGateMesh->SetVisibility(true);

	// Load cube mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		FogGateMesh->SetStaticMesh(CubeMeshFinder.Object);
	}

	// Make the mesh translucent/invisible (fog effect replaces visual)
	FogGateMesh->SetVisibility(false);

	// ============================================================
	// PORTAL NIAGARA COMPONENTS
	// DO NOT create these in C++ - Blueprint SCS has Portal_Front and Portal_Back.
	// These pointers will be found and assigned in BeginPlay.
	// Creating them here causes duplicates because Blueprint SCS components
	// don't merge with C++ components when parent hierarchies differ.
	// ============================================================
	PortalEffectFront = nullptr;
	PortalEffectBack = nullptr;

	// ============================================================
	// CREATE DEATH CURRENCY DROP LOCATION
	// ============================================================
	CurrencySpawnLocation = CreateDefaultSubobject<UBillboardComponent>(TEXT("CurrencySpawnLocation"));
	CurrencySpawnLocation->SetupAttachment(RootComponent);
	CurrencySpawnLocation->SetRelativeLocation(FVector(400.0f, 0.0f, 60.0f));

	// ============================================================
	// CREATE MOVETO COMPONENT - Player teleport destination
	// Position is updated dynamically in OnInteract based on approach direction
	// ============================================================
	MoveTo = CreateDefaultSubobject<UBillboardComponent>(TEXT("MoveTo"));
	MoveTo->SetupAttachment(RootComponent);
	MoveTo->SetRelativeLocation(FVector(MoveToDistance, 0.0f, 0.0f));

	// ============================================================
	// CREATE DOOR FRAME COMPONENT
	// Decorative arch/frame around the fog gate opening
	// Inherited from B_Door - provides visual framing
	// From bp_only: SM_Door mesh at scale (1,1,1), Yaw=90
	// ============================================================
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame"));
	DoorFrame->SetupAttachment(RootComponent);
	DoorFrame->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	DoorFrame->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f)); // Rotated to face forward (same as fog gate)
	DoorFrame->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); // Default scale - mesh is already correct size
	DoorFrame->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Load default door frame mesh (SM_Door - the full door frame mesh)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorFrameMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SM/Door/SM_Door.SM_Door"));
	if (DoorFrameMeshFinder.Succeeded())
	{
		DoorFrame->SetStaticMesh(DoorFrameMeshFinder.Object);
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door Frame mesh set to SM_Door, Scale: (1, 1, 1)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossDoor] Could not find SM_Door mesh"));
	}

	// Default Niagara system path
	PortalNiagaraSystem = TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_Portal.NS_Portal")));

	// ============================================================
	// SET DEFAULT MONTAGES FOR BOSS DOOR
	// Boss doors use AM_SLF_OpenDoor_Boss for the fog gate pass-through animation
	// ============================================================
	InteractMontage_RH = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Demo/_Animations/Interaction/Door/AM_SLF_OpenDoor_Boss.AM_SLF_OpenDoor_Boss")));
	InteractMontage_LH = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Demo/_Animations/Interaction/Door/AM_SLF_OpenDoor_Boss.AM_SLF_OpenDoor_Boss")));

	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Created - FogGateMesh Scale: (3.25, 0.1, 4.25), Montages set to AM_SLF_OpenDoor_Boss"));
}

void ASLFBossDoor::BeginPlay()
{
	Super::BeginPlay();

	// CRITICAL: Ensure door starts UNSEALED (Blueprint CDO may have stale sealed state)
	// Boss doors should always start open and seal AFTER player enters arena
	bSealed = false;
	bIsActivated = false;
	bIsLocked = false;
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] BeginPlay - Reset to unsealed state"));

	// Apply configurable fog gate mesh properties (scale and offset)
	// These can be adjusted per-instance in the level editor
	if (FogGateMesh)
	{
		FogGateMesh->SetRelativeScale3D(FogGateScale);
		FogGateMesh->SetRelativeLocation(FVector(0.0f, 0.0f, FogGateZOffset));
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Applied FogGateScale: (%.2f, %.2f, %.2f), ZOffset: %.2f"),
			FogGateScale.X, FogGateScale.Y, FogGateScale.Z, FogGateZOffset);
	}

	// ============================================================
	// CRITICAL: Find and configure portal Niagara components
	// Blueprint SCS has Portal_Front and Portal_Back - we need to:
	// 1. Find them by name
	// 2. Reattach to FogGateMesh (they may be attached to DefaultSceneRoot)
	// 3. Set correct transforms: Yaw=-90 relative to FogGateMesh (which has Yaw=90)
	//    Result: World Yaw = 90 + (-90) = 0, facing through doorway
	// 4. Destroy any duplicates/extras
	// ============================================================
	TArray<UNiagaraComponent*> NiagaraComps;
	GetComponents<UNiagaraComponent>(NiagaraComps);

	UE_LOG(LogTemp, Warning, TEXT("[BossDoor] Found %d Niagara components, processing..."), NiagaraComps.Num());

	// First pass: find the REAL Portal_Front and Portal_Back (prefer ones named exactly)
	UNiagaraComponent* FoundFront = nullptr;
	UNiagaraComponent* FoundBack = nullptr;

	for (UNiagaraComponent* NiagaraComp : NiagaraComps)
	{
		if (!NiagaraComp) continue;

		FString CompName = NiagaraComp->GetName();

		// Exact name matches take priority
		if (CompName == TEXT("Portal_Front") && !FoundFront)
		{
			FoundFront = NiagaraComp;
		}
		else if (CompName == TEXT("Portal_Back") && !FoundBack)
		{
			FoundBack = NiagaraComp;
		}
	}

	// Second pass: destroy extras and configure the ones we're keeping
	for (UNiagaraComponent* NiagaraComp : NiagaraComps)
	{
		if (!NiagaraComp) continue;

		FString CompName = NiagaraComp->GetName();

		if (NiagaraComp == FoundFront)
		{
			// Configure Portal_Front
			NiagaraComp->AttachToComponent(FogGateMesh, FAttachmentTransformRules::KeepRelativeTransform);
			NiagaraComp->SetRelativeLocation(FVector(0.0f, -100.0f, 0.0f));
			NiagaraComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
			PortalEffectFront = NiagaraComp;
			UE_LOG(LogTemp, Log, TEXT("[BossDoor] Configured Portal_Front: attached to FogGateMesh, Yaw=-90"));
		}
		else if (NiagaraComp == FoundBack)
		{
			// Configure Portal_Back
			NiagaraComp->AttachToComponent(FogGateMesh, FAttachmentTransformRules::KeepRelativeTransform);
			NiagaraComp->SetRelativeLocation(FVector(0.0f, 100.0f, 0.0f));
			NiagaraComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
			PortalEffectBack = NiagaraComp;
			UE_LOG(LogTemp, Log, TEXT("[BossDoor] Configured Portal_Back: attached to FogGateMesh, Yaw=-90"));
		}
		else
		{
			// Duplicate or unknown component - destroy it
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor] Destroying duplicate/unknown Niagara: %s"), *CompName);
			NiagaraComp->DestroyComponent();
		}
	}

	// Load and apply Niagara system to portal components
	if (!PortalNiagaraSystem.IsNull())
	{
		UNiagaraSystem* NiagaraAsset = PortalNiagaraSystem.LoadSynchronous();
		if (NiagaraAsset)
		{
			if (PortalEffectFront)
			{
				PortalEffectFront->SetAsset(NiagaraAsset);
				PortalEffectFront->Activate(true);
			}
			if (PortalEffectBack)
			{
				PortalEffectBack->SetAsset(NiagaraAsset);
				PortalEffectBack->Activate(true);
			}
			UE_LOG(LogTemp, Log, TEXT("[BossDoor] Niagara portals activated"));
		}
	}

	// ============================================================
	// DEBUG: List ALL remaining Niagara components AFTER fixes
	// ============================================================
	TArray<UNiagaraComponent*> AllNiagaraComponents;
	GetComponents<UNiagaraComponent>(AllNiagaraComponents);
	UE_LOG(LogTemp, Warning, TEXT("[BossDoor] ===== NIAGARA COMPONENT DEBUG (AFTER FIX) ====="));
	UE_LOG(LogTemp, Warning, TEXT("[BossDoor] Total Niagara components remaining: %d"), AllNiagaraComponents.Num());
	for (UNiagaraComponent* NiagaraComp : AllNiagaraComponents)
	{
		if (IsValid(NiagaraComp))
		{
			FVector RelLoc = NiagaraComp->GetRelativeLocation();
			FRotator RelRot = NiagaraComp->GetRelativeRotation();
			FRotator WorldRot = NiagaraComp->GetComponentRotation();
			USceneComponent* Parent = NiagaraComp->GetAttachParent();
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]   Niagara: %s"), *NiagaraComp->GetName());
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]     Parent: %s"), Parent ? *Parent->GetName() : TEXT("None"));
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]     RelativeLoc: (%.2f, %.2f, %.2f)"), RelLoc.X, RelLoc.Y, RelLoc.Z);
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]     RelativeRot: (P=%.2f, Y=%.2f, R=%.2f)"), RelRot.Pitch, RelRot.Yaw, RelRot.Roll);
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]     WorldRot: (P=%.2f, Y=%.2f, R=%.2f)"), WorldRot.Pitch, WorldRot.Yaw, WorldRot.Roll);
			UE_LOG(LogTemp, Warning, TEXT("[BossDoor]     IsPortalFront: %s, IsPortalBack: %s"),
				(NiagaraComp == PortalEffectFront) ? TEXT("YES") : TEXT("no"),
				(NiagaraComp == PortalEffectBack) ? TEXT("YES") : TEXT("no"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("[BossDoor] ============================================="));

	// Debug output
	UE_LOG(LogTemp, Warning, TEXT("[BossDoor] BeginPlay - %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("  Class: %s"), *GetClass()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("  bCanBeTraced: %s"), bCanBeTraced ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("  bIsActivated: %s"), bIsActivated ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("  bIsFogWall: %s"), bIsFogWall ? TEXT("true") : TEXT("false"));

	if (FogGateMesh)
	{
		FVector Scale = FogGateMesh->GetRelativeScale3D();
		FVector Location = FogGateMesh->GetRelativeLocation();
		FVector WorldLocation = FogGateMesh->GetComponentLocation();
		ECollisionChannel ObjectType = FogGateMesh->GetCollisionObjectType();
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh Scale: (%.2f, %.2f, %.2f)"), Scale.X, Scale.Y, Scale.Z);
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh Location (Relative): (%.2f, %.2f, %.2f)"), Location.X, Location.Y, Location.Z);
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh Location (World): (%.2f, %.2f, %.2f)"), WorldLocation.X, WorldLocation.Y, WorldLocation.Z);
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh CollisionEnabled: %d, ObjectType: %d"), (int32)FogGateMesh->GetCollisionEnabled(), (int32)ObjectType);
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh GenerateOverlapEvents: %s"), FogGateMesh->GetGenerateOverlapEvents() ? TEXT("true") : TEXT("false"));

		// Log actor world location and scale
		UE_LOG(LogTemp, Warning, TEXT("  Actor World Location: (%.2f, %.2f, %.2f)"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		UE_LOG(LogTemp, Warning, TEXT("  Actor World Scale3D: (%.2f, %.2f, %.2f)"), GetActorScale3D().X, GetActorScale3D().Y, GetActorScale3D().Z);

		// Log world scale of FogGateMesh component (component scale * actor scale)
		FVector WorldScale = FogGateMesh->GetComponentScale();
		UE_LOG(LogTemp, Warning, TEXT("  FogGateMesh World Scale: (%.2f, %.2f, %.2f)"), WorldScale.X, WorldScale.Y, WorldScale.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("  FogGateMesh is NULL!"));
	}

	// Fog gate collision:
	// - Always starts ON to block physical passage
	// - Player must INTERACT to pass through (triggers OnInteract)
	// - During boss fight: still blocks exit
	// - After boss defeated: player can interact to exit
	SetFogGateCollision(true);
}

void ASLFBossDoor::SetFogGateCollision(bool bEnableCollision)
{
	if (FogGateMesh)
	{
		if (bEnableCollision)
		{
			FogGateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			UE_LOG(LogTemp, Log, TEXT("[BossDoor] Fog gate collision ENABLED"));
		}
		else
		{
			FogGateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Log, TEXT("[BossDoor] Fog gate collision DISABLED"));
		}
	}
}

// ============================================================
// ISLFInteractableInterface Implementation
// ============================================================

void ASLFBossDoor::OnTraced_Implementation(AActor* TracedBy)
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] OnTraced by: %s"), TracedBy ? *TracedBy->GetName() : TEXT("None"));
}

void ASLFBossDoor::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] OnInteract - Actor: %s, IsActivated: %s, Sealed: %s"),
		InteractingActor ? *InteractingActor->GetName() : TEXT("None"),
		bIsActivated ? TEXT("true") : TEXT("false"),
		bSealed ? TEXT("true") : TEXT("false"));

	if (!IsValid(InteractingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossDoor] Invalid InteractingActor!"));
		return;
	}

	// If sealed during boss fight, player cannot pass
	if (bSealed)
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door is sealed - cannot pass during boss fight"));
		return;
	}

	// ============================================================
	// STEP 1: Determine player approach direction
	// Dot product: positive = in front, negative = behind
	// ============================================================
	FVector DoorForward = GetActorForwardVector();
	FVector ToPlayer = InteractingActor->GetActorLocation() - GetActorLocation();
	ToPlayer.Normalize();

	float DotProduct = FVector::DotProduct(DoorForward, ToPlayer);
	bOpenForwards = (DotProduct >= 0.0f);

	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Player approach - DotProduct: %.2f, OpenForwards: %s"),
		DotProduct, bOpenForwards ? TEXT("true") : TEXT("false"));

	// ============================================================
	// STEP 2: Select montage based on approach direction
	// ============================================================
	if (bOpenForwards)
	{
		SelectedMontage = InteractMontage_RH.LoadSynchronous();
	}
	else
	{
		SelectedMontage = InteractMontage_LH.LoadSynchronous();
	}

	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Selected montage: %s"),
		SelectedMontage ? *SelectedMontage->GetName() : TEXT("None"));

	// ============================================================
	// STEP 3: Update MoveTo position based on approach direction
	// MoveTo should be on the OTHER side of the door from where player is coming
	// ============================================================
	if (MoveTo)
	{
		// CRITICAL: Player needs to move THROUGH the door to the OTHER side
		// - If player is in front (bOpenForwards=true), move them to BACK of door (-Forward)
		// - If player is in back (bOpenForwards=false), move them to FRONT of door (+Forward)
		FVector TargetOffset = bOpenForwards ?
			(-DoorForward * MoveToDistance) :  // In front -> move to back
			(DoorForward * MoveToDistance);    // In back -> move to front

		FVector WorldMoveToLocation = GetActorLocation() + TargetOffset;
		MoveTo->SetWorldLocation(WorldMoveToLocation);

		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door at: %s, Forward: %s"),
			*GetActorLocation().ToString(), *DoorForward.ToString());
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] MoveTo world position: %s (offset: %s)"),
			*WorldMoveToLocation.ToString(), *TargetOffset.ToString());
	}

	// ============================================================
	// STEP 4: CRITICAL - Call OpenDoor on the PLAYER via BPI_GenericCharacter
	// This triggers the player's door-opening montage and movement
	// ============================================================
	if (InteractingActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Calling OpenDoor on player via BPI_GenericCharacter"));
		IBPI_GenericCharacter::Execute_OpenDoor(InteractingActor, SelectedMontage, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossDoor] InteractingActor does NOT implement BPI_GenericCharacter!"));
	}

	// ============================================================
	// STEP 5: Disable collision to let player pass through
	// ============================================================
	SetFogGateCollision(false);
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Disabled collision for pass-through"));

	// ============================================================
	// STEP 6: Handle boss arena entry/exit logic
	// ============================================================
	if (!bIsActivated)
	{
		// Player is entering boss arena
		// OnPlayerEnterArena will seal the door after a delay (allowing player to fully enter)
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this, InteractingActor]()
		{
			OnPlayerEnterArena(InteractingActor);
		});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.5f, false);
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Entry triggered - door will seal in 1.5 seconds"));
	}
	else
	{
		// Boss is defeated, player is exiting - re-enable collision after delay
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			SetFogGateCollision(true);
		});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Exit triggered - collision will re-enable in 2.0 seconds"));
	}
}

void ASLFBossDoor::OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData)
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] OnSpawnedFromSave - ID: %s"), *Id.ToString());
}

FSLFItemInfo ASLFBossDoor::TryGetItemInfo_Implementation()
{
	// Boss doors don't have item info
	return FSLFItemInfo();
}

// ============================================================
// Boss Door Functions
// ============================================================

void ASLFBossDoor::SealDoor_Implementation()
{
	if (!bSealed)
	{
		bSealed = true;
		bIsLocked = true;
		bIsActivated = true;

		// Activate portal effects
		if (PortalEffectFront)
		{
			PortalEffectFront->Activate(true);
		}
		if (PortalEffectBack)
		{
			PortalEffectBack->Activate(true);
		}

		// Enable collision
		SetFogGateCollision(true);

		OnBossDoorSealed.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door sealed - boss fight started"));
	}
}

void ASLFBossDoor::UnsealDoor_Implementation()
{
	if (bSealed)
	{
		bSealed = false;
		bIsLocked = false;
		bIsActivated = false;

		// Deactivate portal effects
		if (PortalEffectFront)
		{
			PortalEffectFront->Deactivate();
		}
		if (PortalEffectBack)
		{
			PortalEffectBack->Deactivate();
		}

		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door unsealed"));
	}
}

void ASLFBossDoor::OnBossDefeated_Implementation()
{
	UnlockBossDoor();

	if (bDestroyOnBossDefeated)
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Boss defeated - destroying fog wall"));
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Boss defeated - fog wall remains"));
	}
}

void ASLFBossDoor::OnPlayerEnterArena_Implementation(AActor* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Player entered arena: %s"), Player ? *Player->GetName() : TEXT("None"));
	SealDoor();
}

void ASLFBossDoor::UnlockBossDoor_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] UnlockBossDoor - Destroying fog gates"));

	// Deactivate portal effects
	if (IsValid(PortalEffectFront))
	{
		PortalEffectFront->DestroyComponent();
		PortalEffectFront = nullptr;
	}

	if (IsValid(PortalEffectBack))
	{
		PortalEffectBack->DestroyComponent();
		PortalEffectBack = nullptr;
	}

	// Mark as unlocked
	bCanBeTraced = true;
	bIsActivated = false;
	bSealed = false;
	bIsLocked = false;

	OnBossDoorUnlocked.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Boss door unlocked, fog gates destroyed"));
}

USceneComponent* ASLFBossDoor::GetDeathCurrencySpawnPoint_Implementation()
{
	return CurrencySpawnLocation;
}

void ASLFBossDoor::InitializeLoadedStates(bool bInCanBeTraced, bool bInIsActivated)
{
	bCanBeTraced = bInCanBeTraced;
	bIsActivated = bInIsActivated;

	UE_LOG(LogTemp, Log, TEXT("[BossDoor] InitializeLoadedStates - CanBeTraced: %s, IsActivated: %s"),
		bInCanBeTraced ? TEXT("true") : TEXT("false"),
		bInIsActivated ? TEXT("true") : TEXT("false"));

	// If activated, run activation logic (show fog gates)
	if (bIsActivated)
	{
		if (IsValid(PortalEffectFront))
		{
			PortalEffectFront->SetVisibility(true);
			PortalEffectFront->Activate();
		}
		if (IsValid(PortalEffectBack))
		{
			PortalEffectBack->SetVisibility(true);
			PortalEffectBack->Activate();
		}
		SetFogGateCollision(true);
	}
	else
	{
		// Not activated - hide the portal effects but keep collision
		if (IsValid(PortalEffectFront))
		{
			PortalEffectFront->SetVisibility(false);
			PortalEffectFront->Deactivate();
		}
		if (IsValid(PortalEffectBack))
		{
			PortalEffectBack->SetVisibility(false);
			PortalEffectBack->Deactivate();
		}
	}
}
