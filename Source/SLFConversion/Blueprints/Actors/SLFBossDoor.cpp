// SLFBossDoor.cpp
#include "SLFBossDoor.h"
#include "NiagaraComponent.h"

ASLFBossDoor::ASLFBossDoor()
{

	// Boss doors don't auto close
	bAutoClose = false;
	bIsLocked = false;

	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Created"));
}

void ASLFBossDoor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] BeginPlay - IsFogWall: %s"), bIsFogWall ? TEXT("Yes") : TEXT("No"));
}

void ASLFBossDoor::SealDoor_Implementation()
{
	if (!bSealed)
	{
		bSealed = true;
		bIsLocked = true;

		if (FogWallEffect)
		{
			FogWallEffect->Activate(true);
		}

		OnBossDoorSealed.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door sealed - boss fight started"));
	}
}

void ASLFBossDoor::UnsealDoor_Implementation()
{
	if (bSealed)
	{
		bSealed = false;
		bIsLocked = false;

		if (FogWallEffect)
		{
			FogWallEffect->Deactivate();
		}

		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Door unsealed"));
	}
}

void ASLFBossDoor::OnBossDefeated_Implementation()
{
	UnsealDoor();

	if (bDestroyOnBossDefeated)
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Boss defeated - destroying fog wall"));
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[BossDoor] Boss defeated - fog wall remains"));
	}
}

void ASLFBossDoor::OnPlayerEnterArena_Implementation(AActor* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[BossDoor] Player entered arena: %s"), Player ? *Player->GetName() : TEXT("None"));
	SealDoor();
}
