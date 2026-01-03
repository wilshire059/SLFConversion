// SLFStatusEffectArea.cpp
#include "SLFStatusEffectArea.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"

ASLFStatusEffectArea::ASLFStatusEffectArea()
{
	PrimaryActorTick.bCanEverTick = true;




	UE_LOG(LogTemp, Log, TEXT("[StatusEffectArea] Created"));
}

void ASLFStatusEffectArea::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectArea] BeginPlay - Effect: %s, BuildupPerSec: %.1f"),
		*StatusEffectTag.ToString(), BuildupPerSecond);
}

void ASLFStatusEffectArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		for (AActor* Actor : ActorsInArea)
		{
			if (Actor && !Actor->IsPendingKillPending())
			{
				ApplyEffectTick(Actor, DeltaTime);
			}
		}
	}
}

void ASLFStatusEffectArea::OnActorEnter_Implementation(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	bool bIsPlayer = Character->IsPlayerControlled();

	if ((bIsPlayer && bAffectsPlayers) || (!bIsPlayer && bAffectsAI))
	{
		ActorsInArea.AddUnique(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectArea] Actor entered: %s"), *OtherActor->GetName());
	}
}

void ASLFStatusEffectArea::OnActorExit_Implementation(AActor* OtherActor)
{
	ActorsInArea.Remove(OtherActor);
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectArea] Actor exited: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}

void ASLFStatusEffectArea::ApplyEffectTick_Implementation(AActor* TargetActor, float DeltaTime)
{
	// Apply status effect buildup
	float BuildupThisTick = BuildupPerSecond * DeltaTime;
	UE_LOG(LogTemp, Verbose, TEXT("[StatusEffectArea] Applying %.2f buildup to %s"),
		BuildupThisTick, *TargetActor->GetName());

	// TODO: Call status effect component to add buildup
}
