// SLFStatusEffectBuildup.cpp
#include "SLFStatusEffectBuildup.h"

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

	// TODO: Find status effect component and apply buildup
	// ISLFStatusEffectInterface* StatusEffectInterface = Cast<ISLFStatusEffectInterface>(Target);
	// if (StatusEffectInterface)
	// {
	//     StatusEffectInterface->ApplyStatusBuildup(StatusEffectTag, BuildupAmount, SourceActor);
	// }

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
