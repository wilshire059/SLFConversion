// SLFPuzzleMarker.cpp
#include "SLFPuzzleMarker.h"
#include "SLFBossDoor.h"
#include "EngineUtils.h"

ASLFPuzzleMarker::ASLFPuzzleMarker()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASLFPuzzleMarker::BeginPlay()
{
	Super::BeginPlay();
}

void ASLFPuzzleMarker::CompleteObjective()
{
	if (bIsSolved) return;

	CompletedObjectives++;
	UE_LOG(LogTemp, Log, TEXT("[Puzzle] %s: Objective %d/%d completed"),
		*GetName(), CompletedObjectives, RequiredObjectives);

	if (CompletedObjectives >= RequiredObjectives)
	{
		bIsSolved = true;
		UE_LOG(LogTemp, Warning, TEXT("[Puzzle] %s SOLVED!"), *GetName());

		OnPuzzleSolved.Broadcast();

		// Find and unseal linked boss door
		if (!LinkedBossDoorTag.IsNone())
		{
			for (TActorIterator<ASLFBossDoor> It(GetWorld()); It; ++It)
			{
				if (It->BossArenaTag == LinkedBossDoorTag)
				{
					It->UnsealDoor();
					UE_LOG(LogTemp, Warning, TEXT("[Puzzle] Unsealed boss door: %s"), *It->GetName());
					break;
				}
			}
		}
	}
}
