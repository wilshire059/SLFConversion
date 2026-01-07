// SLFBonfire.cpp
#include "SLFBonfire.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StatManagerComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "SLFGameTypes.h"

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

	// Restore player stats (HP, FP, Stamina)
	if (UStatManagerComponent* StatManager = Player->FindComponentByClass<UStatManagerComponent>())
	{
		// Reset HP to max
		FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("Stat.HP"), false);
		if (HPTag.IsValid())
		{
			StatManager->ResetStat(HPTag);
			UE_LOG(LogTemp, Log, TEXT("[Bonfire] Restored HP"));
		}

		// Reset FP to max
		FGameplayTag FPTag = FGameplayTag::RequestGameplayTag(FName("Stat.FP"), false);
		if (FPTag.IsValid())
		{
			StatManager->ResetStat(FPTag);
			UE_LOG(LogTemp, Log, TEXT("[Bonfire] Restored FP"));
		}

		// Reset Stamina to max
		FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("Stat.Stamina"), false);
		if (StaminaTag.IsValid())
		{
			StatManager->ResetStat(StaminaTag);
			UE_LOG(LogTemp, Log, TEXT("[Bonfire] Restored Stamina"));
		}
	}

	// Replenish consumable items (flasks, etc.)
	if (UInventoryManagerComponent* InventoryManager = Player->FindComponentByClass<UInventoryManagerComponent>())
	{
		// Replenish rechargeable items - iterate through inventory and replenish each
		// Full implementation would call ReplenishItem for each rechargeable item
		// For now, log that replenishment would happen
		UE_LOG(LogTemp, Log, TEXT("[Bonfire] Replenishing items via InventoryManager"));
		// InventoryManager->ReplenishItem(Item, -1); // -1 = replenish to max
		// Note: Would need to iterate inventory for all rechargeable items
	}

	// Note: Enemy respawning would be handled by a game mode or level manager
	// that listens to OnBonfireRested and respawns enemies in the area

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
