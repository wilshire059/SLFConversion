// SLFInteractableBase.cpp
#include "SLFInteractableBase.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "InstancedStruct.h"

ASLFInteractableBase::ASLFInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component for Blueprint SCS components to attach to
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	// Create mesh components (matching B_Interactable's "Interactable SM" and "Interactable SK")
	InteractableSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable SM"));
	if (InteractableSM)
	{
		InteractableSM->SetupAttachment(DefaultSceneRoot);
		// Set collision to Interactable channel for trace detection
		InteractableSM->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
		InteractableSM->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		InteractableSM->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		InteractableSM->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	}

	InteractableSK = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Interactable SK"));
	if (InteractableSK)
	{
		InteractableSK->SetupAttachment(DefaultSceneRoot);
		InteractableSK->SetVisibility(false); // Hidden by default, enabled when skeletal mesh is set
	}
}

void ASLFInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] BeginPlay - Prompt: %s"), *InteractionPrompt.ToString());

	// Apply default mesh if set (from Blueprint CDO or C++ child class)
	if (InteractableSM && !DefaultInteractableMesh.IsNull())
	{
		UStaticMesh* MeshToApply = DefaultInteractableMesh.LoadSynchronous();
		if (MeshToApply)
		{
			InteractableSM->SetStaticMesh(MeshToApply);
			UE_LOG(LogTemp, Log, TEXT("[InteractableBase] Applied DefaultInteractableMesh: %s"), *MeshToApply->GetName());
		}
	}
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

// ============================================================
// ISLFInteractableInterface Implementation
// ============================================================

void ASLFInteractableBase::OnTraced_Implementation(AActor* TracedBy)
{
	UE_LOG(LogTemp, Verbose, TEXT("[InteractableBase] OnTraced by: %s"), TracedBy ? *TracedBy->GetName() : TEXT("None"));
	// Called when traced by interaction system - can be used for highlight effects etc.
}

void ASLFInteractableBase::OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData)
{
	UE_LOG(LogTemp, Log, TEXT("[InteractableBase] OnSpawnedFromSave - ID: %s"), *Id.ToString());
	// Called when spawned from save data - override in child classes
}

FSLFItemInfo ASLFInteractableBase::TryGetItemInfo_Implementation()
{
	UE_LOG(LogTemp, Verbose, TEXT("[InteractableBase] TryGetItemInfo - returning default"));
	// Return default item info - override in child classes (like SLFPickupItemBase)
	return FSLFItemInfo();
}
