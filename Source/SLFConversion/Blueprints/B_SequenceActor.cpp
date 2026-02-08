// B_SequenceActor.cpp
// C++ implementation for Blueprint B_SequenceActor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_SequenceActor.json

#include "Blueprints/B_SequenceActor.h"
#include "Widgets/W_HUD.h"
#include "Framework/SLFPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
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
	CanBeSkipped = false;
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

	// Load input actions if not set (fallback loading)
	if (!IA_AnyKey)
	{
		IA_AnyKey = LoadObject<UInputAction>(nullptr,
			TEXT("/Game/SoulslikeFramework/Input/Actions/IA_AnyKey.IA_AnyKey"));
		UE_LOG(LogTemp, Log, TEXT("[B_SequenceActor] Loaded IA_AnyKey: %s"), IA_AnyKey ? TEXT("OK") : TEXT("FAILED"));
	}
	if (!IA_GameMenu)
	{
		IA_GameMenu = LoadObject<UInputAction>(nullptr,
			TEXT("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu.IA_GameMenu"));
		UE_LOG(LogTemp, Log, TEXT("[B_SequenceActor] Loaded IA_GameMenu: %s"), IA_GameMenu ? TEXT("OK") : TEXT("FAILED"));
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

	// Enter cinematic mode on HUD (hides MainHUD, shows CinematicHUD)
	// NOTE: HUDWidgetRef may not exist yet at BeginPlay time (HUD is created later in PC's BeginPlay)
	// Use a deferred call to handle this timing issue
	EnterCinematicMode();

	// bp_only: CanBeSkipped is set to TRUE after showing notification
	CanBeSkipped = true;

	// Setup input bindings for skip
	SetupInputBindings();

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

		// Show skip notification on HUD via PlayerController
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (ASLFPlayerController* SoulslikePC = Cast<ASLFPlayerController>(PC))
		{
			if (SoulslikePC->HUDWidgetRef)
			{
				SoulslikePC->HUDWidgetRef->EventShowSkipCinematicNotification();
				UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Called EventShowSkipCinematicNotification on HUD"));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Skip prompt shown"));
	}
}

void AB_SequenceActor::OnGameMenuInput()
{
	// bp_only: Check CanBeSkipped, then check IsPlaying, then stop
	if (CanBeSkipped && IsValid(ActiveSequencePlayer) && ActiveSequencePlayer->IsPlaying())
	{
		SkipSequence();
	}
}

void AB_SequenceActor::OnSequenceFinishedCallback()
{
	UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Sequence finished naturally"));
	EventFinished();
}

void AB_SequenceActor::EventFinished()
{
	// Restore HUD from cinematic mode
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (ASLFPlayerController* SoulslikePC = Cast<ASLFPlayerController>(PC))
	{
		if (SoulslikePC->HUDWidgetRef)
		{
			SoulslikePC->HUDWidgetRef->EventToggleCinematicMode(false, false);
			UE_LOG(LogTemp, Log, TEXT("AB_SequenceActor: Restored HUD from cinematic mode"));
		}
	}

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

	// bp_only: Stop the sequence player, set to nullptr, then call EventFinished
	if (IsValid(ActiveSequencePlayer))
	{
		ActiveSequencePlayer->Stop();
		ActiveSequencePlayer = nullptr;
	}

	// Trigger finished event
	EventFinished();
}

void AB_SequenceActor::EnterCinematicMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ASLFPlayerController* SoulslikePC = Cast<ASLFPlayerController>(PC);

	if (!SoulslikePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_SequenceActor] Failed to cast PC to ASLFPlayerController (PC class: %s)"),
			PC ? *PC->GetClass()->GetName() : TEXT("NULL"));
		return;
	}

	if (SoulslikePC->HUDWidgetRef)
	{
		// HUD is ready - enter cinematic mode immediately
		SoulslikePC->HUDWidgetRef->EventToggleCinematicMode(true, false);
		SoulslikePC->HUDWidgetRef->EventShowSkipCinematicNotification();
		GetWorldTimerManager().ClearTimer(CinematicModeTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[B_SequenceActor] Entered cinematic mode and showed skip notification"));
	}
	else
	{
		// HUD not ready yet - retry on next tick
		UE_LOG(LogTemp, Log, TEXT("[B_SequenceActor] HUDWidgetRef not ready, deferring cinematic mode entry..."));
		GetWorldTimerManager().SetTimer(CinematicModeTimerHandle, this,
			&AB_SequenceActor::EnterCinematicMode, 0.05f, false);
	}
}
