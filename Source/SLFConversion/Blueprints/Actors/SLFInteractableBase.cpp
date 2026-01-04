// SLFInteractableBase.cpp
#include "SLFInteractableBase.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

ASLFInteractableBase::ASLFInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// NOTE: ALL components (DefaultSceneRoot, Interactable SM, Interactable SK, World Niagara)
	// are defined in Blueprint's SCS and preserved via KEEP_VARS_MAP.
	// DO NOT create components here - they would conflict with preserved SCS components.
	// B_PickupItem's "World Niagara" references B_Interactable's "DefaultSceneRoot" as parent.
}

void ASLFInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] BeginPlay - Prompt: %s"), *InteractionPrompt.ToString());
}

void ASLFInteractableBase::Interact_Implementation(AActor* Interactor)
{
	if (!CanBeInteractedWith(Interactor))
	{
		UE_LOG(LogTemp, Log, TEXT("[InteractableBase] Cannot interact"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] Interacted by: %s"), Interactor ? *Interactor->GetName() : TEXT("None"));

	if (bOneTimeUse)
	{
		bHasBeenUsed = true;
		bCanInteract = false;
	}

	OnInteracted.Broadcast(Interactor);
}

bool ASLFInteractableBase::CanBeInteractedWith_Implementation(AActor* Interactor)
{
	if (!bCanInteract)
	{
		return false;
	}

	if (bOneTimeUse && bHasBeenUsed)
	{
		return false;
	}

	return true;
}

void ASLFInteractableBase::EnableInteraction_Implementation()
{
	bCanInteract = true;
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] Interaction enabled"));
}

void ASLFInteractableBase::DisableInteraction_Implementation()
{
	bCanInteract = false;
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] Interaction disabled"));
}

void ASLFInteractableBase::OnInteract_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] OnInteract - override in child classes"));
	// Base implementation - override in child classes
}

void ASLFInteractableBase::SetupInteractable_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] SetupInteractable - override in child classes"));
	// Base implementation - override in child classes
}
