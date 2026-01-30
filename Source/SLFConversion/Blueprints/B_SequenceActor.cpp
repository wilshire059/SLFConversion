// B_SequenceActor.cpp
// C++ implementation for Blueprint B_SequenceActor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_SequenceActor.json

#include "Blueprints/B_SequenceActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

AB_SequenceActor::AB_SequenceActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// COMPONENT OWNERSHIP: Blueprint SCS owns all components.
	// C++ only caches references at runtime. See CLAUDE.md for pattern.
	// Billboard component is created by Blueprint SCS, cached in BeginPlay.
	CachedBillboard = nullptr;

	// Default values
	SequenceToPlay = nullptr;
	ActiveSequencePlayer = nullptr;
	CanBeSkipped = true;
	IA_AnyKey = nullptr;
	IA_GameMenu = nullptr;
	bSkipPromptVisible = false;
}

void AB_SequenceActor::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("[B_SequenceActor] BeginPlay called - SequenceToPlay: %s"),
		SequenceToPlay ? *SequenceToPlay->GetName() : TEXT("NULL"));

	Super::BeginPlay();

	// Cache Billboard component from Blueprint SCS
	if (!CachedBillboard)
	{
		TArray<UBillboardComponent*> BillboardComponents;
		GetComponents<UBillboardComponent>(BillboardComponents);
		if (BillboardComponents.Num() > 0)
		{
			CachedBillboard = BillboardComponents[0];
		}
	}

	// Check if we have a sequence to play
	if (!IsValid(SequenceToPlay))
	{
		UE_LOG(LogTemp, Error, TEXT("[B_SequenceActor] ERROR: No SequenceToPlay assigned! The Level Blueprint should set this via ExposeOnSpawn."));
		UE_LOG(LogTemp, Error, TEXT("[B_SequenceActor] Check that the SpawnActor node has 'Sequence to Play' pin connected to a LevelSequence asset."));
		EventDestroy();
		return;
	}

	// Create the level sequence player
	ALevelSequenceActor* OutActor = nullptr;
	ActiveSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		SequenceToPlay,
		Settings,
		OutActor
	);

	if (!IsValid(ActiveSequencePlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("AB_SequenceActor: Failed to create LevelSequencePlayer"));
		EventDestroy();
		return;
	}

	// Bind the OnFinished event
	ActiveSequencePlayer->OnFinished.AddDynamic(this, &AB_SequenceActor::OnSequenceFinishedCallback);

	// Setup input bindings if sequence can be skipped
	if (CanBeSkipped)
	{
		SetupInputBindings();
	}

	// Start playback
	ActiveSequencePlayer->Play();

	UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Started playing sequence %s"), *GetNameSafe(SequenceToPlay));
}

void AB_SequenceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unbind from sequence player
	if (IsValid(ActiveSequencePlayer))
	{
		ActiveSequencePlayer->OnFinished.RemoveDynamic(this, &AB_SequenceActor::OnSequenceFinishedCallback);
	}

	Super::EndPlay(EndPlayReason);
}

void AB_SequenceActor::SetupInputBindings()
{
	// Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC))
	{
		return;
	}

	// Get enhanced input component
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PC->InputComponent);
	if (!IsValid(EnhancedInput))
	{
		return;
	}

	// Bind AnyKey action (shows skip prompt)
	if (IsValid(IA_AnyKey))
	{
		EnhancedInput->BindAction(IA_AnyKey, ETriggerEvent::Triggered, this, &AB_SequenceActor::OnAnyKeyInput);
	}

	// Bind GameMenu action (skips sequence)
	if (IsValid(IA_GameMenu))
	{
		EnhancedInput->BindAction(IA_GameMenu, ETriggerEvent::Triggered, this, &AB_SequenceActor::OnGameMenuInput);
	}
}

void AB_SequenceActor::OnAnyKeyInput()
{
	// Show skip prompt on first key press
	if (!bSkipPromptVisible)
	{
		bSkipPromptVisible = true;
		// The skip prompt UI is typically handled by a separate widget
		// that listens for this state change
		UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Skip prompt shown"));
	}
}

void AB_SequenceActor::OnGameMenuInput()
{
	// Skip only if prompt is visible (requires double-press pattern)
	if (bSkipPromptVisible && CanBeSkipped)
	{
		SkipSequence();
	}
	else if (CanBeSkipped)
	{
		// First press shows prompt
		OnAnyKeyInput();
	}
}

void AB_SequenceActor::OnSequenceFinishedCallback()
{
	UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Sequence finished naturally"));
	EventFinished();
}

void AB_SequenceActor::EventFinished()
{
	// Broadcast the finished event
	OnSequenceFinished.Broadcast();

	// Cleanup and destroy
	EventDestroy();
}

void AB_SequenceActor::EventDestroy()
{
	// Stop playback if still active
	if (IsValid(ActiveSequencePlayer))
	{
		ActiveSequencePlayer->Stop();
		ActiveSequencePlayer = nullptr;
	}

	// Destroy this actor
	Destroy();
}

void AB_SequenceActor::SkipSequence()
{
	if (!CanBeSkipped)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Sequence skipped by player"));

	// Stop the sequence
	if (IsValid(ActiveSequencePlayer))
	{
		// Jump to end
		ActiveSequencePlayer->GoToEndAndStop();
	}

	// Trigger finished event
	EventFinished();
}
