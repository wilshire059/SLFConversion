// SLFDeathTrigger.cpp
#include "SLFDeathTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASLFDeathTrigger::ASLFDeathTrigger()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[DeathTrigger] Created"));
}

void ASLFDeathTrigger::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[DeathTrigger] BeginPlay - Players: %s, AI: %s"),
		bAffectsPlayers ? TEXT("Yes") : TEXT("No"),
		bAffectsAI ? TEXT("Yes") : TEXT("No"));
}

void ASLFDeathTrigger::OnActorEnter_Implementation(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[DeathTrigger] Actor entered: %s"), *OtherActor->GetName());

	// Check if this is a character
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	// Check if player or AI
	bool bIsPlayer = Character->IsPlayerControlled();

	if ((bIsPlayer && bAffectsPlayers) || (!bIsPlayer && bAffectsAI))
	{
		KillActor(OtherActor);
	}
}

void ASLFDeathTrigger::KillActor_Implementation(AActor* ActorToKill)
{
	if (ActorToKill)
	{
		UE_LOG(LogTemp, Log, TEXT("[DeathTrigger] Killing actor: %s"), *ActorToKill->GetName());

		// Apply massive damage
		UGameplayStatics::ApplyDamage(ActorToKill, DamageAmount, nullptr, this, DamageType);

		OnActorKilled.Broadcast(ActorToKill);
	}
}
