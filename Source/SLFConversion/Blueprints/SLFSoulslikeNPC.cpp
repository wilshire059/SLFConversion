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

ASLFSoulslikeNPC::ASLFSoulslikeNPC()
{
	// Initialize NPC settings (renamed to avoid Blueprint collision)
	bIsTraceable = true;
	CachedLookAtComponent = nullptr;
	SavedNpcId = FGuid::NewGuid();

	// NPCs don't need to tick by default
	PrimaryActorTick.bCanEverTick = false;

	// Components (LookAtRadius, AC_AI_InteractionManager) stay in Blueprint
	// to preserve configured values. Access via FindComponentByClass if needed.
}

void ASLFSoulslikeNPC::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeNPC] BeginPlay: %s (ID: %s)"),
		*GetName(), *SavedNpcId.ToString());

	// Cache look-at component - find from Blueprint's LookAtRadius sphere
	if (!CachedLookAtComponent)
	{
		CachedLookAtComponent = FindComponentByClass<USphereComponent>();
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
	// Base NPC implementation - does nothing
	// Derived classes can override to add behavior
	UE_LOG(LogTemp, Verbose, TEXT("[SoulslikeNPC] OnInteract: %s interacted by %s"),
		*GetName(), InteractingActor ? *InteractingActor->GetName() : TEXT("nullptr"));
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
