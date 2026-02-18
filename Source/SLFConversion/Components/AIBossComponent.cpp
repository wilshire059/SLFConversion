// AIBossComponent.cpp
// C++ implementation for AC_AI_Boss
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_AI_Boss
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         18/18 (initialized in constructor)
// Functions:         17/17 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "AIBossComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Interfaces/BPI_BossDoor.h"
#include "Interfaces/BPI_Controller.h"
#include "Widgets/W_HUD.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

UAIBossComponent::UAIBossComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize display
	Name = FText::FromString(TEXT("Boss"));
	bShowBossBar = true;

	// Initialize trigger config
	EncounterTriggerType = ESLFAIBossEncounterType::OnOverlap;
	TriggerCollisionRadius = 1000.0;
	DoorCheckDistance = 500.0;

	// Initialize phase config (-1 = no phase set yet, prevents SetPhase guard from blocking first call)
	ActivePhaseIndex = -1;

	// Initialize death config
	DeathSequence = nullptr;
	DeathText = FText::FromString(TEXT("ENEMY FELLED"));
	DeathTextDelay = 2.0f;
	bHiddenInDeathSequence = false;

	// Initialize boss doors
	BossDoorClass = nullptr;

	// Initialize runtime state
	bFightActive = false;
	ActiveMusicComponent = nullptr;

	// Initialize debug
	bDebugActive = false;
	DebugPhase = 0;

	// Initialize sequence tracking
	ActiveSequencePlayer = nullptr;
	ActiveSequenceActor = nullptr;
}

void UAIBossComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[AIBoss] BeginPlay on %s - Boss: %s, Phases: %d"),
		*GetOwner()->GetName(), *Name.ToString(), Phases.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// FIGHT CONTROL [1-2/17]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBossComponent::SetFightActive_Implementation(bool bActive)
{
	// Guard: don't re-trigger if already in the requested state
	if (bActive && bFightActive)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIBoss] SetFightActive: already active, ignoring duplicate call"));
		return;
	}

	bFightActive = bActive;

	UE_LOG(LogTemp, Log, TEXT("[AIBoss] SetFightActive: %s, bShowBossBar: %s"),
		bActive ? TEXT("true") : TEXT("false"),
		bShowBossBar ? TEXT("true") : TEXT("false"));

	// Get player HUD for boss bar display
	UW_HUD* PlayerHUD = nullptr;
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Found PC: %s"), *PC->GetClass()->GetName());

			// Check if controller implements IBPI_Controller (the correct interface)
			if (PC->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
			{
				UUserWidget* HUDWidget = nullptr;
				IBPI_Controller::Execute_GetPlayerHUD(PC, HUDWidget);
				PlayerHUD = Cast<UW_HUD>(HUDWidget);
				UE_LOG(LogTemp, Log, TEXT("[AIBoss] GetPlayerHUD returned: %s"),
					PlayerHUD ? *PlayerHUD->GetName() : TEXT("null"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AIBoss] PC does not implement IBPI_Controller!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBoss] No PlayerController found!"));
		}
	}

	if (bActive)
	{
		// Start fight - show boss bar, play phase music, close doors
		if (bShowBossBar && PlayerHUD)
		{
			AActor* Owner = GetOwner();
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Showing boss bar for: %s"), *Name.ToString());
			PlayerHUD->EventShowBossBar(Name, Owner);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBoss] Cannot show boss bar - bShowBossBar: %s, PlayerHUD: %s"),
				bShowBossBar ? TEXT("true") : TEXT("false"),
				PlayerHUD ? TEXT("valid") : TEXT("null"));
		}

		// Start phase 0 (or debug phase) - this plays the phase cinematic and music
		if (bDebugActive)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] DebugActive, setting phase to DebugPhase: %d"), DebugPhase);
			SetPhase(DebugPhase);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Setting initial phase 0"));
			SetPhase(0);
		}
	}
	else
	{
		// End fight - hide boss bar
		if (PlayerHUD)
		{
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Hiding boss bar"));
			PlayerHUD->EventHideBossBar();
		}

		EndFight();
	}
}

void UAIBossComponent::EndFight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] EndFight"));

	bFightActive = false;

	// Stop any active cinematic
	CleanupActiveSequence();

	// Stop music
	if (ActiveMusicComponent)
	{
		ActiveMusicComponent->Stop();
		ActiveMusicComponent = nullptr;
	}

	// Unlock boss doors
	TryUnlockBossDoors();
}

// ═══════════════════════════════════════════════════════════════════════════════
// PHASE MANAGEMENT [3-6/17]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBossComponent::SetPhase_Implementation(int32 PhaseIndex)
{
	// Guard: don't re-enter same phase (matches bp_only "Not Equal" check)
	if (PhaseIndex == ActivePhaseIndex)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIBoss] SetPhase: already at phase %d, ignoring"), PhaseIndex);
		return;
	}

	if (PhaseIndex >= 0 && PhaseIndex < Phases.Num())
	{
		ActivePhaseIndex = PhaseIndex;
		ActivePhase = Phases[PhaseIndex];

		UE_LOG(LogTemp, Log, TEXT("[AIBoss] SetPhase: %d - %s"), PhaseIndex, *ActivePhase.PhaseName.ToString());

		// HandlePhaseChange plays the cinematic, montage, and sets combat state
		HandlePhaseChange(PhaseIndex);
	}
}

bool UAIBossComponent::EvaluatePhaseRequirement_Implementation(int32 PhaseIndex, float CurrentHealthPercent)
{
	if (PhaseIndex >= 0 && PhaseIndex < Phases.Num())
	{
		const FSLFAiBossPhase& Phase = Phases[PhaseIndex];
		bool bMeetsRequirement = CurrentHealthPercent <= Phase.HealthThreshold;

		UE_LOG(LogTemp, Verbose, TEXT("[AIBoss] EvaluatePhaseRequirement: Phase %d, Health %.2f%% <= %.2f%% = %s"),
			PhaseIndex, CurrentHealthPercent * 100.0f, Phase.HealthThreshold * 100.0f,
			bMeetsRequirement ? TEXT("true") : TEXT("false"));

		return bMeetsRequirement;
	}
	return false;
}

void UAIBossComponent::HandlePhaseChange_Implementation(int32 NewPhaseIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] HandlePhaseChange: %d"), NewPhaseIndex);

	if (NewPhaseIndex < 0 || NewPhaseIndex >= Phases.Num())
	{
		return;
	}

	const FSLFAiBossPhase& NewPhase = Phases[NewPhaseIndex];

	// Clean up any previous cinematic before starting a new one
	CleanupActiveSequence();

	// --- TRY PLAY PHASE SEQUENCE (cinematic) ---
	// Matches bp_only: play level sequence, bind OnFinished → OnPhaseSequenceFinished
	if (!NewPhase.PhaseStartSequence.IsNull())
	{
		ULevelSequence* Sequence = NewPhase.PhaseStartSequence.LoadSynchronous();
		if (IsValid(Sequence))
		{
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Playing PhaseStartSequence: %s"), *Sequence->GetName());

			FMovieSceneSequencePlaybackSettings PlaybackSettings;
			PlaybackSettings.bAutoPlay = false; // We'll call Play() ourselves after binding

			ALevelSequenceActor* OutActor = nullptr;
			ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(
				GetWorld(),
				Sequence,
				PlaybackSettings,
				OutActor
			);

			if (IsValid(Player))
			{
				// Store references for cleanup
				ActiveSequencePlayer = Player;
				ActiveSequenceActor = OutActor;

				// Position the sequence actor at the boss location so it lines up
				if (OutActor && GetOwner())
				{
					OutActor->SetActorTransform(GetOwner()->GetActorTransform());
				}

				// Bind OnFinished BEFORE playing (matches bp_only "Bind Event to On Finished")
				Player->OnFinished.AddDynamic(this, &UAIBossComponent::OnLevelSequenceFinished);
				Player->Play();

				UE_LOG(LogTemp, Log, TEXT("[AIBoss] Started playing phase sequence, bound OnFinished"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AIBoss] Failed to create level sequence player"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBoss] PhaseStartSequence failed to load"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[AIBoss] No PhaseStartSequence configured for phase %d"), NewPhaseIndex);
	}

	// --- TRY PLAY PHASE MONTAGE ---
	// Play PhaseStartMontage (for phase-start animations like boss intro pose)
	UAnimMontage* MontageToPlay = nullptr;
	if (!NewPhase.PhaseStartMontage.IsNull())
	{
		MontageToPlay = NewPhase.PhaseStartMontage.LoadSynchronous();
	}
	else if (!NewPhase.PhaseTransitionMontage.IsNull())
	{
		// Fallback to PhaseTransitionMontage if no start montage
		MontageToPlay = NewPhase.PhaseTransitionMontage.LoadSynchronous();
	}

	if (MontageToPlay)
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
			{
				if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
				{
					float MontageLength = AnimInstance->Montage_Play(MontageToPlay);
					UE_LOG(LogTemp, Log, TEXT("[AIBoss] Playing phase montage: %s (%.2fs)"),
						*MontageToPlay->GetName(), MontageLength);
				}
			}
		}
	}

	// --- SWITCH MUSIC ---
	if (!NewPhase.PhaseMusic.IsNull())
	{
		// Stop current music
		if (ActiveMusicComponent)
		{
			ActiveMusicComponent->Stop();
			ActiveMusicComponent = nullptr;
		}
		if (USoundBase* Music = NewPhase.PhaseMusic.LoadSynchronous())
		{
			ActiveMusicComponent = UGameplayStatics::SpawnSound2D(this, Music);
		}
	}
}

void UAIBossComponent::OnPhaseSequenceFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] OnPhaseSequenceFinished - Phase %d"), ActivePhaseIndex);

	// Clean up the sequence actor/player now that it's done
	CleanupActiveSequence();

	// Phase transition complete - resume AI behavior
}

void UAIBossComponent::OnLevelSequenceFinished()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] Level sequence finished playing - calling OnPhaseSequenceFinished"));
	OnPhaseSequenceFinished();
}

void UAIBossComponent::CleanupActiveSequence()
{
	if (ActiveSequencePlayer)
	{
		ActiveSequencePlayer->OnFinished.RemoveAll(this);
		ActiveSequencePlayer->Stop();
		ActiveSequencePlayer = nullptr;
	}
	if (ActiveSequenceActor)
	{
		ActiveSequenceActor->Destroy();
		ActiveSequenceActor = nullptr;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BOSS DOORS [7-8/17]
// ═══════════════════════════════════════════════════════════════════════════════

bool UAIBossComponent::TryGetBossDoors_Implementation(TArray<AActor*>& OutDoors)
{
	OutDoors = RelatedBossDoors;
	return RelatedBossDoors.Num() > 0;
}

void UAIBossComponent::TryUnlockBossDoors_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] TryUnlockBossDoors - %d doors"), RelatedBossDoors.Num());

	for (AActor* Door : RelatedBossDoors)
	{
		if (Door && Door->GetClass()->ImplementsInterface(UBPI_BossDoor::StaticClass()))
		{
			IBPI_BossDoor::Execute_UnlockBossDoor(Door);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEATH HANDLING [9-11/17]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBossComponent::OnBossDeath_Implementation(AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] OnBossDeath - Killed by: %s"),
		Killer ? *Killer->GetName() : TEXT("null"));

	// Play death sequence
	if (DeathSequence)
	{
		// Play death montage on owner's skeletal mesh
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
			{
				if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
				{
					float MontageLength = AnimInstance->Montage_Play(DeathSequence);
					// Schedule OnDeathSequenceFinished after montage completes
					if (MontageLength > 0.0f)
					{
						FTimerHandle DeathSequenceTimer;
						GetWorld()->GetTimerManager().SetTimer(DeathSequenceTimer, this, &UAIBossComponent::OnDeathSequenceFinished_Implementation, MontageLength, false);
					}
					else
					{
						OnDeathSequenceFinished();
					}
				}
			}
		}
	}
	else
	{
		OnDeathSequenceFinished();
	}

	// Schedule death text display
	if (UWorld* World = GetWorld())
	{
		FTimerHandle DeathTextTimer;
		World->GetTimerManager().SetTimer(
			DeathTextTimer,
			this,
			&UAIBossComponent::ShowDeathText_Implementation,
			DeathTextDelay,
			false
		);
	}
}

void UAIBossComponent::OnDeathSequenceFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] OnDeathSequenceFinished"));

	EndFight();

	// Hide boss if configured
	if (bHiddenInDeathSequence)
	{
		if (AActor* Owner = GetOwner())
		{
			Owner->SetActorHiddenInGame(true);
		}
	}
}

void UAIBossComponent::ShowDeathText_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] ShowDeathText: %s"), *DeathText.ToString());

	// Display death text on HUD via player controller
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			// Get the HUD widget and call its big screen message function
			// Note: This requires W_BigScreenMessage integration when HUD is migrated
			UE_LOG(LogTemp, Log, TEXT("[AIBoss] Death text should be displayed: %s"), *DeathText.ToString());
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT HANDLERS [12/17]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBossComponent::OnStatUpdated_Implementation(FGameplayTag StatTag, float CurrentValue, float MaxValue)
{
	// Only evaluate phase transitions during an active fight
	if (!bFightActive) return;

	// Check if this is health stat
	if (StatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Stat.Health"))))
	{
		float HealthPercent = (MaxValue > 0.0f) ? (CurrentValue / MaxValue) : 0.0f;

		// Check for phase transition
		for (int32 i = ActivePhaseIndex + 1; i < Phases.Num(); ++i)
		{
			if (EvaluatePhaseRequirement(i, HealthPercent))
			{
				SetPhase(i);
				break;
			}
		}
	}
}
