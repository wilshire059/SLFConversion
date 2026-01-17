// SLFSoulslikeBoss.cpp
// C++ implementation for B_Soulslike_Boss
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Soulslike_Boss
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 (inherits from Enemy)
// Functions:         1/1 implemented (trigger overlap)
// Event Dispatchers: 0/0 (inherits from Enemy)
// Components:        2 (AC_AI_Boss, TriggerCollision)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFSoulslikeBoss.h"
#include "Components/AIBossComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SLFAIStateMachineComponent.h"

ASLFSoulslikeBoss::ASLFSoulslikeBoss()
{
	// Create AI Boss Component - use different FName than Blueprint SCS to avoid collision
	BossComponent = CreateDefaultSubobject<UAIBossComponent>(TEXT("BossComponent"));

	// Create Trigger Collision sphere for boss encounter activation - renamed to avoid collision
	TriggerCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerCollisionSphere"));
	if (TriggerCollisionSphere)
	{
		TriggerCollisionSphere->SetupAttachment(RootComponent);
		TriggerCollisionSphere->SetSphereRadius(1000.0f); // Large radius for boss encounter trigger
		TriggerCollisionSphere->SetCollisionProfileName(TEXT("Trigger"));
		TriggerCollisionSphere->SetGenerateOverlapEvents(true);

		// Bind overlap event
		TriggerCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASLFSoulslikeBoss::OnTriggerBeginOverlap);
	}
}

void ASLFSoulslikeBoss::BeginPlay()
{
	Super::BeginPlay();

	// Configure AIStateMachine for boss behavior (inherited from enemy)
	if (AIStateMachine)
	{
		AIStateMachine->Config.bIsBoss = true;

		// Pull phase thresholds from BossComponent if available
		if (BossComponent && BossComponent->Phases.Num() > 0)
		{
			// Use first phase transition as Phase2 threshold
			if (BossComponent->Phases.Num() >= 2)
			{
				AIStateMachine->Config.Phase2HealthThreshold = BossComponent->Phases[1].HealthThreshold;
			}
			// Use second phase transition as Phase3 threshold
			if (BossComponent->Phases.Num() >= 3)
			{
				AIStateMachine->Config.Phase3HealthThreshold = BossComponent->Phases[2].HealthThreshold;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("[SoulslikeBoss] Configured AIStateMachine for boss: bIsBoss=true, Phase2=%.2f, Phase3=%.2f"),
			AIStateMachine->Config.Phase2HealthThreshold, AIStateMachine->Config.Phase3HealthThreshold);
	}

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeBoss] BeginPlay: %s"), *GetName());
}

// ═══════════════════════════════════════════════════════════════════════════════
// TRIGGER OVERLAP CALLBACK
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeBoss::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// From Blueprint: B_Soulslike_Boss EventGraph
	// 1. Check if OtherActor has tag "Player"
	// 2. Check if EncounterTriggerType == TriggerCollision (NewEnumerator1)
	// 3. If both true, start fight

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// Check if player entered (actor must have "Player" tag)
	const bool bIsPlayer = OtherActor->ActorHasTag(FName("Player"));

	// Check if trigger type is CollisionTrigger (enum index 1)
	bool bIsTriggerEncounterType = false;
	if (BossComponent)
	{
		bIsTriggerEncounterType = (BossComponent->EncounterTriggerType == ESLFAIBossEncounterType::CollisionTrigger);
	}

	// If both conditions met, start the fight
	if (bIsPlayer && bIsTriggerEncounterType)
	{
		// Validate OtherActor before setting (per Gemini peer review - actor could be destroyed same frame)
		if (!IsValid(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeBoss] Player trigger - but actor no longer valid"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[SoulslikeBoss] Player entered trigger - starting boss encounter"));

		if (BossComponent)
		{
			BossComponent->SetFightActive(true);
		}

		// Also inform AIStateMachine about the target to start combat state
		// This replaces the old BehaviorManager->SetTarget pattern
		if (AIStateMachine)
		{
			AIStateMachine->SetTarget(OtherActor);  // This automatically transitions to Combat state
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeBoss] Set target on AIStateMachine: %s"), *GetNameSafe(OtherActor));
		}
	}
}
