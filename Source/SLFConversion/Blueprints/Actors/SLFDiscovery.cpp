// SLFDiscovery.cpp
#include "SLFDiscovery.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ASLFDiscovery::ASLFDiscovery()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[Discovery] Created"));
}

void ASLFDiscovery::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[Discovery] BeginPlay - Area: %s, Tag: %s"),
		*AreaName.ToString(), *DiscoveryTag.ToString());
}

void ASLFDiscovery::OnPlayerEnter_Implementation(AActor* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[Discovery] Player entered: %s"), Player ? *Player->GetName() : TEXT("None"));

	if (!bAlreadyDiscovered)
	{
		TriggerDiscovery(Player);
	}
}

void ASLFDiscovery::TriggerDiscovery_Implementation(AActor* Discoverer)
{
	if (!bAlreadyDiscovered)
	{
		bAlreadyDiscovered = true;
		UE_LOG(LogTemp, Log, TEXT("[Discovery] Area discovered: %s"), *AreaName.ToString());

		if (DiscoverySound)
		{
			UGameplayStatics::PlaySound2D(this, DiscoverySound);
		}

		if (bShowNotification)
		{
			ShowDiscoveryUI();
		}

		OnAreaDiscovered.Broadcast(AreaName, Discoverer);

		// TODO: Save discovery to save game
	}
}

void ASLFDiscovery::ShowDiscoveryUI_Implementation()
{
	// Override in Blueprint to show UI notification
	UE_LOG(LogTemp, Log, TEXT("[Discovery] Showing UI for: %s"), *AreaName.ToString());
}
