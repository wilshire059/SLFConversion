// SLFTorch.cpp
#include "SLFTorch.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"

ASLFTorch::ASLFTorch()
{
	PrimaryActorTick.bCanEverTick = false;





	UE_LOG(LogTemp, Log, TEXT("[Torch] Created"));
}

void ASLFTorch::BeginPlay()
{
	Super::BeginPlay();

	bIsLit = bStartLit;
	UpdateTorchVisuals();

	UE_LOG(LogTemp, Log, TEXT("[Torch] BeginPlay - Lit: %s"), bIsLit ? TEXT("Yes") : TEXT("No"));
}

void ASLFTorch::LightTorch_Implementation()
{
	if (!bIsLit)
	{
		bIsLit = true;
		UpdateTorchVisuals();
		OnTorchStateChanged.Broadcast(true);
		UE_LOG(LogTemp, Log, TEXT("[Torch] Lit"));
	}
}

void ASLFTorch::ExtinguishTorch_Implementation()
{
	if (bIsLit)
	{
		bIsLit = false;
		UpdateTorchVisuals();
		OnTorchStateChanged.Broadcast(false);
		UE_LOG(LogTemp, Log, TEXT("[Torch] Extinguished"));
	}
}

void ASLFTorch::ToggleTorch_Implementation()
{
	if (bIsLit)
	{
		ExtinguishTorch();
	}
	else
	{
		LightTorch();
	}
}

void ASLFTorch::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[Torch] Interact by: %s"), Interactor ? *Interactor->GetName() : TEXT("None"));

	if (bCanBeToggled)
	{
		ToggleTorch();
	}
}

void ASLFTorch::UpdateTorchVisuals()
{
	if (TorchLight)
	{
		TorchLight->SetVisibility(bIsLit);
	}

	if (FlameEffect)
	{
		if (bIsLit)
		{
			FlameEffect->Activate();
		}
		else
		{
			FlameEffect->Deactivate();
		}
	}
}
