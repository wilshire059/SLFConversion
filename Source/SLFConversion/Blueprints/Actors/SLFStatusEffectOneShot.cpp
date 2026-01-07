// SLFStatusEffectOneShot.cpp
#include "SLFStatusEffectOneShot.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Components/StatusEffectManagerComponent.h"

ASLFStatusEffectOneShot::ASLFStatusEffectOneShot()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[StatusEffectOneShot] Created"));
}

void ASLFStatusEffectOneShot::BeginPlay()
{
	Super::BeginPlay();

	// Update collision radius
	if (TriggerSphere)
	{
		if (TriggerSphere) TriggerSphere->SetSphereRadius(TriggerRadius);
	}

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectOneShot] BeginPlay - Effect: %s, Buildup: %.1f"),
		*StatusEffectTag.ToString(), InstantBuildupAmount);
}

void ASLFStatusEffectOneShot::OnActorOverlap_Implementation(AActor* OtherActor)
{
	if (bHasTriggered)
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		TriggerEffect(OtherActor);
	}
}

void ASLFStatusEffectOneShot::TriggerEffect_Implementation(AActor* Target)
{
	if (bHasTriggered)
	{
		return;
	}

	bHasTriggered = true;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectOneShot] Triggering %s on %s with %.1f buildup"),
		*StatusEffectTag.ToString(), *Target->GetName(), InstantBuildupAmount);

	// Find status effect manager component on target
	UStatusEffectManagerComponent* StatusMgr = Target->FindComponentByClass<UStatusEffectManagerComponent>();
	if (StatusMgr)
	{
		// For instant/one-shot effects, we want to apply enough buildup to trigger immediately
		// AddOneShotBuildup applies instant buildup - high amount should trigger the effect
		UE_LOG(LogTemp, Log, TEXT("[StatusEffectOneShot] Applying %.1f %s instant buildup via StatusEffectManager"),
			InstantBuildupAmount, *StatusEffectTag.ToString());
		// StatusMgr->AddOneShotBuildup(StatusEffectData, 1, InstantBuildupAmount);
		// Note: Full implementation requires looking up PDA_StatusEffect from tag
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusEffectOneShot] Target %s has no StatusEffectManagerComponent"),
			*Target->GetName());
	}

	if (bDestroyOnTrigger)
	{
		Destroy();
	}
}
