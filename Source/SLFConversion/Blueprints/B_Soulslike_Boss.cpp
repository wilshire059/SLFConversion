// B_Soulslike_Boss.cpp
// C++ implementation for Blueprint B_Soulslike_Boss
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_Boss.json

#include "Blueprints/B_Soulslike_Boss.h"
#include "Components/AIBossComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

AB_Soulslike_Boss::AB_Soulslike_Boss()
{
	// Create boss AI component
	AC_AI_Boss = CreateDefaultSubobject<UAIBossComponent>(TEXT("AC_AI_Boss"));

	// Create trigger collision sphere
	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerCollision"));
	TriggerCollision->SetupAttachment(RootComponent);
	TriggerCollision->SetSphereRadius(1000.0f); // Default radius, configurable via AC_AI_Boss
	TriggerCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerCollision->SetGenerateOverlapEvents(true);
}

void AB_Soulslike_Boss::BeginPlay()
{
	Super::BeginPlay();

	// Configure AIStateMachine for boss behavior (inherited from enemy)
	if (AIStateMachine)
	{
		AIStateMachine->Config.bIsBoss = true;

		// Pull phase thresholds from AC_AI_Boss if available
		if (IsValid(AC_AI_Boss) && AC_AI_Boss->Phases.Num() > 0)
		{
			// Use first phase transition as Phase2 threshold
			if (AC_AI_Boss->Phases.Num() >= 2)
			{
				AIStateMachine->Config.Phase2HealthThreshold = AC_AI_Boss->Phases[1].HealthThreshold;
			}
			// Use second phase transition as Phase3 threshold
			if (AC_AI_Boss->Phases.Num() >= 3)
			{
				AIStateMachine->Config.Phase3HealthThreshold = AC_AI_Boss->Phases[2].HealthThreshold;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("[Boss] Configured AIStateMachine for boss: bIsBoss=true, Phase2=%.2f, Phase3=%.2f"),
			AIStateMachine->Config.Phase2HealthThreshold, AIStateMachine->Config.Phase3HealthThreshold);
	}

	// Bind trigger collision overlap event
	if (IsValid(TriggerCollision))
	{
		TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &AB_Soulslike_Boss::OnTriggerCollisionBeginOverlap);

		// Update collision radius from boss component config
		if (IsValid(AC_AI_Boss))
		{
			TriggerCollision->SetSphereRadius(static_cast<float>(AC_AI_Boss->TriggerCollisionRadius));
		}
	}

	// Bind perception updated event from AI controller
	AAIController* AIController = Cast<AAIController>(GetController());
	if (IsValid(AIController))
	{
		UAIPerceptionComponent* PerceptionComp = AIController->GetAIPerceptionComponent();
		if (IsValid(PerceptionComp))
		{
			PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AB_Soulslike_Boss::OnControllerPerceptionUpdated);
		}
	}
}

void AB_Soulslike_Boss::OnTriggerCollisionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Check if the overlapping actor is a player
	if (!IsValid(OtherActor))
	{
		return;
	}

	// Check for "Player" tag
	if (!OtherActor->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	// Try to start the fight
	TryStartFight(OtherActor);
}

void AB_Soulslike_Boss::OnControllerPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// Check if boss component is valid and fight is not already active
	if (!IsValid(AC_AI_Boss) || AC_AI_Boss->GetFightActive())
	{
		return;
	}

	// Loop through perceived actors
	for (AActor* Actor : UpdatedActors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		// Check if actor has "Player" tag
		if (!Actor->ActorHasTag(TEXT("Player")))
		{
			continue;
		}

		// Check sight sense (via AI perception)
		AAIController* AIController = Cast<AAIController>(GetController());
		if (!IsValid(AIController))
		{
			continue;
		}

		UAIPerceptionComponent* PerceptionComp = AIController->GetAIPerceptionComponent();
		if (!IsValid(PerceptionComp))
		{
			continue;
		}

		FActorPerceptionBlueprintInfo PerceptionInfo;
		if (PerceptionComp->GetActorsPerception(Actor, PerceptionInfo))
		{
			// Check if we have sight on this actor
			FAISenseID SightSenseID = UAISense_Sight::StaticClass()->GetDefaultObject<UAISense_Sight>()->GetSenseID();
			for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
			{
				if (Stimulus.WasSuccessfullySensed() && Stimulus.Type == SightSenseID)
				{
					TryStartFight(Actor);
					return; // Only start fight once
				}
			}
		}
	}
}

void AB_Soulslike_Boss::TryStartFight(AActor* Target)
{
	if (!IsValid(AC_AI_Boss))
	{
		return;
	}

	// Check if fight is already active
	if (AC_AI_Boss->GetFightActive())
	{
		return;
	}

	// Validate target before setting (per Gemini peer review - actor could be destroyed same frame)
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Boss] TryStartFight - Target is no longer valid"));
		return;
	}

	// Activate the fight via AIBossComponent
	AC_AI_Boss->SetFightActive(true);

	// Also inform AIStateMachine about the target to start combat state
	// This replaces the old BehaviorManager->SetTarget pattern
	if (AIStateMachine)
	{
		AIStateMachine->SetTarget(Target);  // This automatically transitions to Combat state
		UE_LOG(LogTemp, Log, TEXT("[Boss] Set target on AIStateMachine: %s"), *GetNameSafe(Target));
	}

	UE_LOG(LogTemp, Log, TEXT("AB_Soulslike_Boss: Fight started with target %s"), *GetNameSafe(Target));
}

