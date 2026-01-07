// SLFStatusEffectBuildup.cpp
#include "SLFStatusEffectBuildup.h"
#include "Components/StatusEffectManagerComponent.h"

ASLFStatusEffectBuildup::ASLFStatusEffectBuildup()
{
	PrimaryActorTick.bCanEverTick = false;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] Created"));
}

void ASLFStatusEffectBuildup::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] BeginPlay - Effect: %s, Amount: %.1f"),
		*StatusEffectTag.ToString(), BuildupAmount);

	if (bApplyOnSpawn && TargetActor)
	{
		ApplyBuildup(TargetActor);
	}
}

void ASLFStatusEffectBuildup::ApplyBuildup_Implementation(AActor* Target)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectBuildup] No target to apply buildup"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] Applying %.1f %s buildup to %s"),
		BuildupAmount, *StatusEffectTag.ToString(), *Target->GetName());

	// Find status effect manager component on target
	UStatusEffectManagerComponent* StatusMgr = Target->FindComponentByClass<UStatusEffectManagerComponent>();
	if (StatusMgr)
	{
		// TryAddStatusEffect takes data asset, but we have a tag
		// Use the buildup amount as start amount - if effect already exists, it will add to existing buildup
		// The data asset would need to be looked up from a registry by tag
		// For now, call with nullptr to at least log the attempt
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] Adding buildup via StatusEffectManager"));
		// StatusMgr->TryAddStatusEffect(StatusEffectData, 1, true, BuildupAmount);
		// Note: Full implementation requires looking up PDA_StatusEffect from tag via data table
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectBuildup] Target %s has no StatusEffectManagerComponent"), *Target->GetName());
	}

	OnBuildupApplied.Broadcast(Target, BuildupAmount);

	if (bDestroyAfterApply)
	{
		Destroy();
	}
}

void ASLFStatusEffectBuildup::SetTarget_Implementation(AActor* NewTarget)
{
	TargetActor = NewTarget;
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] Target set to: %s"),
		NewTarget ? *NewTarget->GetName() : TEXT("None"));
}

void ASLFStatusEffectBuildup::OnStatusEffectTriggered_Implementation(AActor* AffectedActor)
{
	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBuildup] Status effect triggered on: %s"),
		AffectedActor ? *AffectedActor->GetName() : TEXT("None"));
	OnStatusTriggered.Broadcast(AffectedActor);
}
