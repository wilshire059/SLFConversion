// SLFContainerBase.cpp
// C++ implementation for B_Container
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Container
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         5/5 (initialized in constructor)
// Functions:         1/1 implemented (override only)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFContainerBase.h"
#include "Animation/AnimMontage.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/BPI_GenericCharacter.h"

ASLFContainerBase::ASLFContainerBase()
{
	// Initialize open config
	MoveDistance = 100.0;
	SpeedMultiplier = 1.0;

	// Initialize runtime
	CachedIntensity = 1.0f;

	// Update interaction text
	InteractionText = FText::FromString(TEXT("Open"));
}

void ASLFContainerBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Container] BeginPlay: %s"),
		*InteractableDisplayName.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERACTION OVERRIDE [1/1]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFContainerBase::OnInteract_Implementation(AActor* Interactor)
{
	Super::OnInteract_Implementation(Interactor);

	UE_LOG(LogTemp, Log, TEXT("[Container] OnInteract - Opening container"));

	// Load and play open montage on the interactor
	if (!OpenMontage.IsNull())
	{
		UAnimMontage* LoadedMontage = OpenMontage.LoadSynchronous();
		if (LoadedMontage && Interactor)
		{
			// Play montage on interactor via interface
			if (Interactor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				IBPI_GenericCharacter::Execute_PlayMontageReplicated(
					Interactor, LoadedMontage, SpeedMultiplier, 0.0, NAME_None);
				UE_LOG(LogTemp, Log, TEXT("[Container] Playing open montage: %s"), *LoadedMontage->GetName());
			}
		}
	}

	// Spawn open VFX
	if (!OpenVFX.IsNull())
	{
		UNiagaraSystem* LoadedVFX = OpenVFX.LoadSynchronous();
		if (LoadedVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this, LoadedVFX, GetActorLocation());
		}
	}

	// Loot handling:
	// - Show loot UI would be done via player HUD interface
	// - Give items would be done via InventoryManager->AddItem
	// This base class doesn't have loot - see SLFContainer for full loot implementation
	UE_LOG(LogTemp, Log, TEXT("[Container] Container opened - loot handled by derived class if any"));
}
