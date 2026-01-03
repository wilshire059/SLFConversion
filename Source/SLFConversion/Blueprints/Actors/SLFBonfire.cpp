// SLFBonfire.cpp
#include "SLFBonfire.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

ASLFBonfire::ASLFBonfire()
{
	LocationType = ESLFLocationType::Bonfire;




	UE_LOG(LogTemp, Log, TEXT("[Bonfire] Created"));
}

void ASLFBonfire::BeginPlay()
{
	Super::BeginPlay();
	UpdateFireVisuals();
	UE_LOG(LogTemp, Log, TEXT("[Bonfire] BeginPlay - Lit: %s, Kindling: %d/%d"),
		bIsLit ? TEXT("Yes") : TEXT("No"), KindlingLevel, MaxKindlingLevel);
}

void ASLFBonfire::LightBonfire_Implementation(AActor* Player)
{
	if (!bIsLit)
	{
		bIsLit = true;
		bIsUnlocked = true;

		UpdateFireVisuals();

		if (LightSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LightSound, GetActorLocation());
		}

		OnBonfireLit.Broadcast(Player);
		UE_LOG(LogTemp, Log, TEXT("[Bonfire] Lit by: %s"), Player ? *Player->GetName() : TEXT("None"));
	}
}

void ASLFBonfire::Rest_Implementation(AActor* Player)
{
	if (!bIsLit)
	{
		LightBonfire(Player);
	}

	UE_LOG(LogTemp, Log, TEXT("[Bonfire] Player resting: %s"), Player ? *Player->GetName() : TEXT("None"));

	if (RestSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RestSound, GetActorLocation());
	}

	// TODO: Restore player health/FP, respawn enemies, etc.

	OnBonfireRested.Broadcast(Player);
}

void ASLFBonfire::Kindle_Implementation(AActor* Player)
{
	if (CanKindle())
	{
		KindlingLevel++;
		UE_LOG(LogTemp, Log, TEXT("[Bonfire] Kindled to level %d by: %s"),
			KindlingLevel, Player ? *Player->GetName() : TEXT("None"));

		// Update fire intensity based on kindling level
		if (FireLight)
		{
			float IntensityMultiplier = 1.0f + (KindlingLevel * 0.5f);
			FireLight->SetIntensity(3000.0f * IntensityMultiplier);
		}
	}
}

void ASLFBonfire::OpenBonfireMenu_Implementation(AActor* Player)
{
	// Override in Blueprint to open bonfire UI (level up, travel, etc.)
	UE_LOG(LogTemp, Log, TEXT("[Bonfire] Opening menu for: %s"), Player ? *Player->GetName() : TEXT("None"));
}

void ASLFBonfire::UpdateFireVisuals()
{
	if (FireLight)
	{
		FireLight->SetVisibility(bIsLit);
	}

	if (FireEffect)
	{
		if (bIsLit)
		{
			FireEffect->Activate(true);
		}
		else
		{
			FireEffect->Deactivate();
		}
	}
}
