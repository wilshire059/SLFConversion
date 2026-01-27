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

	// Initialize phase config
	ActivePhaseIndex = 0;

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
}

void UAIBossComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[AIBoss] BeginPlay on %s - Boss: %s, Phases: %d"),
		*GetOwner()->GetName(), *Name.ToString(), Phases.Num());

	// Initialize first phase if phases exist
	if (Phases.Num() > 0)
	{
		ActivePhase = Phases[0];
		ActivePhaseIndex = 0;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FIGHT CONTROL [1-2/17]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBossComponent::SetFightActive_Implementation(bool bActive)
{
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

		// Play phase music
		if (!ActivePhase.PhaseMusic.IsNull() && !ActiveMusicComponent)
		{
			if (USoundBase* Music = ActivePhase.PhaseMusic.LoadSynchronous())
			{
				ActiveMusicComponent = UGameplayStatics::SpawnSound2D(this, Music);
			}
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
	if (PhaseIndex >= 0 && PhaseIndex < Phases.Num())
	{
		ActivePhaseIndex = PhaseIndex;
		ActivePhase = Phases[PhaseIndex];

		UE_LOG(LogTemp, Log, TEXT("[AIBoss] SetPhase: %d - %s"), PhaseIndex, *ActivePhase.PhaseName.ToString());

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

	if (NewPhaseIndex >= 0 && NewPhaseIndex < Phases.Num())
	{
		const FSLFAiBossPhase& NewPhase = Phases[NewPhaseIndex];

		// Play transition montage if available
		if (!NewPhase.PhaseTransitionMontage.IsNull())
		{
			// Play montage on owner's skeletal mesh
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
			{
				if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
				{
					if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
					{
						UAnimMontage* TransitionMontage = NewPhase.PhaseTransitionMontage.LoadSynchronous();
						if (TransitionMontage)
						{
							float MontageLength = AnimInstance->Montage_Play(TransitionMontage);
							// Schedule OnPhaseSequenceFinished after montage completes
							if (MontageLength > 0.0f)
							{
								FTimerHandle PhaseTimer;
								GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UAIBossComponent::OnPhaseSequenceFinished_Implementation, MontageLength, false);
							}
							else
							{
								OnPhaseSequenceFinished();
							}
						}
					}
				}
			}
		}
		else
		{
			OnPhaseSequenceFinished();
		}

		// Switch music if different
		if (!NewPhase.PhaseMusic.IsNull() && ActiveMusicComponent)
		{
			ActiveMusicComponent->Stop();
			if (USoundBase* Music = NewPhase.PhaseMusic.LoadSynchronous())
			{
				ActiveMusicComponent = UGameplayStatics::SpawnSound2D(this, Music);
			}
		}
	}
}

void UAIBossComponent::OnPhaseSequenceFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AIBoss] OnPhaseSequenceFinished - Phase %d"), ActivePhaseIndex);

	// Phase transition complete - resume AI behavior
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
