// SLFSequenceActor.cpp
#include "SLFSequenceActor.h"
#include "Components/BoxComponent.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"

ASLFSequenceActor::ASLFSequenceActor()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Created"));
}

void ASLFSequenceActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SequenceActor] BeginPlay - Sequence: %s, PlayOnEnter: %s"),
		LevelSequence ? *LevelSequence->GetName() : TEXT("None"),
		bPlayOnEnter ? TEXT("Yes") : TEXT("No"));
}

void ASLFSequenceActor::PlaySequence_Implementation()
{
	if (bIsPlaying)
	{
		UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Already playing"));
		return;
	}

	if (bPlayOnce && bHasPlayed)
	{
		UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Already played once"));
		return;
	}

	if (!LevelSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SequenceActor] No level sequence assigned"));
		return;
	}

	bIsPlaying = true;
	bHasPlayed = true;

	UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Playing sequence: %s"), *LevelSequence->GetName());

	// Create level sequence player
	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.bAutoPlay = true;
	PlaybackSettings.bPauseAtEnd = false;

	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		LevelSequence,
		PlaybackSettings,
		SequenceActor);

	if (SequencePlayer)
	{
		// Bind to finish event
		SequencePlayer->OnFinished.AddDynamic(this, &ASLFSequenceActor::OnSequenceComplete);
		SequencePlayer->Play();
		UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Started playing"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SequenceActor] Failed to create sequence player"));
		bIsPlaying = false;
	}

	OnSequenceStarted.Broadcast();
}

void ASLFSequenceActor::StopSequence_Implementation()
{
	if (bIsPlaying)
	{
		bIsPlaying = false;
		UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Stopped sequence"));

		// Stop the sequence player
		if (SequencePlayer)
		{
			SequencePlayer->Stop();
			SequencePlayer = nullptr;
		}

		OnSequenceFinished.Broadcast();
	}
}

void ASLFSequenceActor::OnSequenceComplete()
{
	UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Sequence completed"));
	bIsPlaying = false;
	SequencePlayer = nullptr;
	OnSequenceFinished.Broadcast();
}

void ASLFSequenceActor::SkipSequence_Implementation()
{
	if (bIsPlaying && bSkippable)
	{
		UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Skipping sequence"));
		StopSequence();
	}
}

void ASLFSequenceActor::OnPlayerEnter_Implementation(AActor* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[SequenceActor] Player entered: %s"), Player ? *Player->GetName() : TEXT("None"));

	if (bPlayOnEnter)
	{
		PlaySequence();
	}
}
