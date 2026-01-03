// SLFAbilityEffectBase.cpp
#include "SLFAbilityEffectBase.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

ASLFAbilityEffectBase::ASLFAbilityEffectBase()
{
	PrimaryActorTick.bCanEverTick = true;



	UE_LOG(LogTemp, Log, TEXT("[AbilityEffectBase] Created"));
}

void ASLFAbilityEffectBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[AbilityEffectBase] BeginPlay - Tag: %s, Duration: %.1f"),
		*AbilityTag.ToString(), Duration);

	if (bAutoActivate)
	{
		ActivateEffect();
	}
}

void ASLFAbilityEffectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		OnEffectTick(DeltaTime);

		if (Duration > 0.0f)
		{
			ElapsedTime += DeltaTime;
			if (ElapsedTime >= Duration)
			{
				DeactivateEffect();
			}
		}
	}
}

void ASLFAbilityEffectBase::ActivateEffect_Implementation()
{
	if (!bIsActive)
	{
		bIsActive = true;
		ElapsedTime = 0.0f;

		if (bAttachToOwner && OwnerActor)
		{
			AttachToActor(OwnerActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
		}

		if (EffectParticles)
		{
			EffectParticles->Activate(true);
		}

		if (ActivateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, GetActorLocation());
		}

		OnEffectActivated.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("[AbilityEffectBase] Effect activated: %s"), *AbilityTag.ToString());
	}
}

void ASLFAbilityEffectBase::DeactivateEffect_Implementation()
{
	if (bIsActive)
	{
		bIsActive = false;

		if (EffectParticles)
		{
			EffectParticles->Deactivate();
		}

		if (DeactivateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeactivateSound, GetActorLocation());
		}

		OnEffectDeactivated.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("[AbilityEffectBase] Effect deactivated: %s"), *AbilityTag.ToString());

		// Destroy after particles finish
		SetLifeSpan(2.0f);
	}
}

void ASLFAbilityEffectBase::SetOwnerActor_Implementation(AActor* NewOwner)
{
	OwnerActor = NewOwner;
	UE_LOG(LogTemp, Log, TEXT("[AbilityEffectBase] Owner set to: %s"), NewOwner ? *NewOwner->GetName() : TEXT("None"));
}

void ASLFAbilityEffectBase::OnEffectTick_Implementation(float DeltaTime)
{
	// Override in Blueprint for per-tick behavior
}
