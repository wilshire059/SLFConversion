// SLFLocationActor.cpp
#include "SLFLocationActor.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"

ASLFLocationActor::ASLFLocationActor()
{
	PrimaryActorTick.bCanEverTick = false;




	UE_LOG(LogTemp, Log, TEXT("[LocationActor] Created"));
}

void ASLFLocationActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[LocationActor] BeginPlay - Name: %s, Tag: %s, Type: %d"),
		*LocationName.ToString(), *LocationTag.ToString(), (int32)LocationType);
}

FTransform ASLFLocationActor::GetSpawnTransform() const
{
	return GetActorTransform();
}

void ASLFLocationActor::UnlockLocation_Implementation()
{
	if (!bIsUnlocked)
	{
		bIsUnlocked = true;
		UE_LOG(LogTemp, Log, TEXT("[LocationActor] Location unlocked: %s"), *LocationName.ToString());
	}
}

void ASLFLocationActor::TeleportActorHere_Implementation(AActor* ActorToTeleport)
{
	if (ActorToTeleport)
	{
		FTransform SpawnTransform = GetSpawnTransform();
		ActorToTeleport->SetActorTransform(SpawnTransform);
		UE_LOG(LogTemp, Log, TEXT("[LocationActor] Teleported %s to %s"),
			*ActorToTeleport->GetName(), *LocationName.ToString());
	}
}
