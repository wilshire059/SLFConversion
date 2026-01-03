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
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeBoss] Player entered trigger - starting boss encounter"));

		if (BossComponent)
		{
			BossComponent->SetFightActive(true);
		}
	}
}
