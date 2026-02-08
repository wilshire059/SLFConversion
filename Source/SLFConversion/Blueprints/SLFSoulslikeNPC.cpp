// SLFSoulslikeNPC.cpp
// C++ implementation for B_Soulslike_NPC
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Soulslike_NPC
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         3/3 (initialized in constructor)
// Functions:         2/2 implemented
// Interface Funcs:   4/4 implemented
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFSoulslikeNPC.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Interfaces/BPI_Player.h"
#include "UObject/ConstructorHelpers.h"

ASLFSoulslikeNPC::ASLFSoulslikeNPC()
{
	// Initialize NPC settings (renamed to avoid Blueprint collision)
	bIsTraceable = true;
	CachedLookAtComponent = nullptr;
	SavedNpcId = FGuid::NewGuid();

	// NPCs don't need to tick by default
	PrimaryActorTick.bCanEverTick = false;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS - Created here since Blueprint SCS was lost during migration
	// These match the original B_Soulslike_NPC Blueprint components
	// ═══════════════════════════════════════════════════════════════════

	// Create AI Interaction Manager - handles NPC dialog, vendor data, name
	AIInteractionManager = CreateDefaultSubobject<UAIInteractionManagerComponent>(TEXT("AC_AI_InteractionManager"));

	// Create Look At Radius sphere - for targeting/look-at system
	// In Blueprint: attached to CollisionCylinder (CapsuleComponent)
	LookAtRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Look At Radius"));
	if (LookAtRadius && GetCapsuleComponent())
	{
		LookAtRadius->SetupAttachment(GetCapsuleComponent());
		LookAtRadius->SetSphereRadius(200.0f); // Default radius
		LookAtRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LookAtRadius->SetHiddenInGame(true);
	}

	// Point LookAtComponent to our LookAtRadius
	LookAtComponent = LookAtRadius;

	// Default AnimClass - ABP_SoulslikeNPC
	// Note: Use ConstructorHelpers to load the AnimBP class
	static ConstructorHelpers::FClassFinder<UAnimInstance> DefaultNPCAnimBP(
		TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC"));
	if (DefaultNPCAnimBP.Succeeded())
	{
		NPCAnimClass = DefaultNPCAnimBP.Class;
	}
}

void ASLFSoulslikeNPC::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] BeginPlay: %s (ID: %s)"),
		*GetName(), *SavedNpcId.ToString());

	// Cache look-at component - use our LookAtRadius
	CachedLookAtComponent = LookAtRadius;

	// Log component status
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] Components: AIInteractionManager=%s, LookAtRadius=%s"),
		AIInteractionManager ? TEXT("VALID") : TEXT("NULL"),
		LookAtRadius ? TEXT("VALID") : TEXT("NULL"));

	// Apply AnimClass to the mesh - always ensure correct AnimClass is set
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp && NPCAnimClass)
	{
		UClass* CurrentAnimClass = MeshComp->GetAnimClass();

		// Always apply if no AnimClass set, or if it's different from what we want
		if (!CurrentAnimClass || CurrentAnimClass != NPCAnimClass)
		{
			MeshComp->SetAnimInstanceClass(NPCAnimClass);
			MeshComp->InitAnim(true); // Force initialize the anim instance
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] Applied AnimClass %s to mesh (was: %s)"),
				*NPCAnimClass->GetName(),
				CurrentAnimClass ? *CurrentAnimClass->GetName() : TEXT("NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] Mesh already has correct AnimClass: %s"),
				*MeshComp->GetAnimClass()->GetName());
		}
	}
	else if (!NPCAnimClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeNPC] No NPCAnimClass set for %s!"), *GetName());
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [2/2]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeNPC::GetLookAtLocation_Implementation(FVector& Location)
{
	// From JSON: if look-at component valid, return its world location
	// Otherwise return zero vector (original Blueprint behavior)
	if (CachedLookAtComponent)
	{
		Location = CachedLookAtComponent->GetComponentLocation();
	}
	else
	{
		Location = FVector::ZeroVector;
	}
}

void ASLFSoulslikeNPC::TeleportToLocation_Implementation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] TeleportToLocation: %s -> %s"),
		*GetName(), *NewLocation.ToString());

	ETeleportType TeleportType = bTeleport ? ETeleportType::TeleportPhysics : ETeleportType::None;
	SetActorLocationAndRotation(NewLocation, NewRotation, bSweep, nullptr, TeleportType);
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERFACE FUNCTIONS: ISLFInteractableInterface [4/4]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeNPC::OnTraced_Implementation(AActor* TracedBy)
{
	// Base NPC implementation - does nothing
	// Derived classes can override to add behavior
	UE_LOG(LogTemp, Verbose, TEXT("[SoulslikeNPC] OnTraced: %s traced by %s"),
		*GetName(), TracedBy ? *TracedBy->GetName() : TEXT("nullptr"));
}

void ASLFSoulslikeNPC::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] OnInteract: %s interacted by %s"),
		*GetName(), InteractingActor ? *InteractingActor->GetName() : TEXT("nullptr"));

	// Use our AIInteractionManager component
	if (!AIInteractionManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeNPC] OnInteract: %s has no AIInteractionManager component!"), *GetName());
		return;
	}

	// Call OnDialogStarted on the player via IBPI_Player
	// The player must implement IBPI_Player (BPI_Player interface)
	if (InteractingActor && InteractingActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] Calling OnDialogStarted on player with AIInteractionManager %s (Name: %s)"),
			*AIInteractionManager->GetName(),
			*AIInteractionManager->Name.ToString());
		IBPI_Player::Execute_OnDialogStarted(InteractingActor, AIInteractionManager);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeNPC] OnInteract: InteractingActor does not implement IBPI_Player!"));
	}
}

void ASLFSoulslikeNPC::OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData)
{
	// Set the NPC ID from the save data
	SavedNpcId = Id;

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] OnSpawnedFromSave: %s with ID %s"),
		*GetName(), *SavedNpcId.ToString());
}

FSLFItemInfo ASLFSoulslikeNPC::TryGetItemInfo_Implementation()
{
	// Base NPC returns an empty item info
	// Derived classes (like vendors) can override to return actual item info
	return FSLFItemInfo();
}
