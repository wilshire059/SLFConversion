// SLFStatusEffectOneShot.cpp
#include "SLFStatusEffectOneShot.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

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

	// TODO: Find status effect component and apply instant buildup
	// This should be enough to immediately trigger the status effect

	if (bDestroyOnTrigger)
	{
		Destroy();
	}
}
