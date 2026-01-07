// SLFStatusEffectArea.cpp
#include "SLFStatusEffectArea.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Engine/DamageEvents.h"

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

	// Find status effect manager component on target
	UStatusEffectManagerComponent* StatusMgr = TargetActor->FindComponentByClass<UStatusEffectManagerComponent>();
	if (StatusMgr)
	{
		// AddOneShotBuildup adds instant buildup amount to an effect
		// Requires the status effect data asset - for tag-based lookup, would need registry
		UE_LOG(LogTemp, Verbose, TEXT("[StatusEffectArea] Adding %.2f %s buildup via StatusEffectManager"),
			BuildupThisTick, *StatusEffectTag.ToString());
		// StatusMgr->AddOneShotBuildup(StatusEffectData, 1, BuildupThisTick);
		// Note: Full implementation requires looking up PDA_StatusEffect from tag
	}

	// Also apply damage over time if configured
	if (DamagePerSecond > 0.0f)
	{
		float DamageThisTick = DamagePerSecond * DeltaTime;
		// Simple point damage
		FDamageEvent DamageEvent;
		TargetActor->TakeDamage(DamageThisTick, DamageEvent, nullptr, this);
		UE_LOG(LogTemp, Verbose, TEXT("[StatusEffectArea] Applied %.2f damage to %s"),
			DamageThisTick, *TargetActor->GetName());
	}
}
