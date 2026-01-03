// SLFDoorDemo.cpp
#include "SLFDoorDemo.h"

ASLFDoorDemo::ASLFDoorDemo()
{
	// Demo doors are typically unlocked
	bIsLocked = false;

	UE_LOG(LogTemp, Log, TEXT("[DoorDemo] Created"));
}

void ASLFDoorDemo::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[DoorDemo] BeginPlay - Hint: %s"), *HintText.ToString());
}

void ASLFDoorDemo::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[DoorDemo] Interact - showing hint first"));

	if (bShowHintOnApproach)
	{
		ShowHint();
	}

	Super::Interact_Implementation(Interactor);
}

void ASLFDoorDemo::ShowHint_Implementation()
{
	// Override in Blueprint to show UI hint
	UE_LOG(LogTemp, Log, TEXT("[DoorDemo] Showing hint: %s"), *HintText.ToString());
}

void ASLFDoorDemo::HideHint_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[DoorDemo] Hiding hint"));
}
