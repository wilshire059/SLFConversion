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

	// Load and play open montage
	if (!OpenMontage.IsNull())
	{
		UAnimMontage* LoadedMontage = OpenMontage.LoadSynchronous();
		if (LoadedMontage && Interactor)
		{
			// TODO: Play montage on interactor's anim instance
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

	// TODO: Show loot UI
	// TODO: Give items to player
}
