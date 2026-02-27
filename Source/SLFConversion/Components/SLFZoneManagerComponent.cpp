// SLFZoneManagerComponent.cpp

#include "Components/SLFZoneManagerComponent.h"

USLFZoneManagerComponent::USLFZoneManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLFZoneManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ZoneConfigs.Num() == 0)
	{
		InitializeDefaultZones();
	}
}

void USLFZoneManagerComponent::SetCurrentZone(FGameplayTag NewZone)
{
	if (CurrentZone != NewZone)
	{
		FGameplayTag OldZone = CurrentZone;
		CurrentZone = NewZone;
		OnZoneChanged.Broadcast(OldZone, NewZone);

		// Auto-discover on entry
		DiscoverZone(NewZone);
	}
}

void USLFZoneManagerComponent::DiscoverZone(FGameplayTag ZoneTag)
{
	if (!ZoneTag.IsValid()) return;

	if (!DiscoveredZones.HasTag(ZoneTag))
	{
		DiscoveredZones.AddTag(ZoneTag);
		OnZoneDiscovered.Broadcast(ZoneTag);
		UE_LOG(LogTemp, Log, TEXT("Zone discovered: %s"), *ZoneTag.ToString());
	}
}

void USLFZoneManagerComponent::DefeatBoss(FGameplayTag BossTag)
{
	if (!BossTag.IsValid()) return;

	if (!DefeatedBosses.HasTag(BossTag))
	{
		DefeatedBosses.AddTag(BossTag);
		OnBossDefeated.Broadcast(BossTag);
		UE_LOG(LogTemp, Log, TEXT("Boss defeated: %s"), *BossTag.ToString());
	}
}

bool USLFZoneManagerComponent::IsZoneDiscovered(FGameplayTag ZoneTag) const
{
	return DiscoveredZones.HasTag(ZoneTag);
}

bool USLFZoneManagerComponent::IsBossDefeated(FGameplayTag BossTag) const
{
	return DefeatedBosses.HasTag(BossTag);
}

bool USLFZoneManagerComponent::GetZoneConfig(FGameplayTag ZoneTag, FSLFZoneConfig& OutConfig) const
{
	for (const FSLFZoneConfig& Config : ZoneConfigs)
	{
		if (Config.ZoneTag == ZoneTag)
		{
			OutConfig = Config;
			return true;
		}
	}
	return false;
}

float USLFZoneManagerComponent::GetCurrentZoneDifficultyMultiplier() const
{
	FSLFZoneConfig Config;
	if (GetZoneConfig(CurrentZone, Config))
	{
		// Scale difficulty multiplier based on zone difficulty enum
		switch (Config.Difficulty)
		{
		case ESLFZoneDifficulty::Tutorial: return 0.5f;
		case ESLFZoneDifficulty::Easy: return 1.0f;
		case ESLFZoneDifficulty::Medium: return 1.5f;
		case ESLFZoneDifficulty::Hard: return 2.0f;
		case ESLFZoneDifficulty::VeryHard: return 3.0f;
		case ESLFZoneDifficulty::Final: return 4.0f;
		default: return 1.0f;
		}
	}
	return 1.0f;
}

bool USLFZoneManagerComponent::IsCoreAccessible() const
{
	// Check all 4 main boss tags
	static const FGameplayTag Broodmother = FGameplayTag::RequestGameplayTag(FName("Boss.Broodmother"), false);
	static const FGameplayTag Forgemaster = FGameplayTag::RequestGameplayTag(FName("Boss.Forgemaster"), false);
	static const FGameplayTag Convergent = FGameplayTag::RequestGameplayTag(FName("Boss.Convergent"), false);
	static const FGameplayTag Hierophant = FGameplayTag::RequestGameplayTag(FName("Boss.Hierophant"), false);

	return DefeatedBosses.HasTag(Broodmother)
		&& DefeatedBosses.HasTag(Forgemaster)
		&& DefeatedBosses.HasTag(Convergent)
		&& DefeatedBosses.HasTag(Hierophant);
}

TArray<FSLFEnemySpawnEntry> USLFZoneManagerComponent::GetSpawnEntriesForZone(FGameplayTag ZoneTag) const
{
	TArray<FSLFEnemySpawnEntry> Result;
	for (const auto& Pair : ZoneEnemyTables)
	{
		if (Pair.Key.MatchesTag(ZoneTag))
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

void USLFZoneManagerComponent::GetZoneSaveData(FGameplayTag& OutCurrentZone, FGameplayTagContainer& OutDiscovered, FGameplayTagContainer& OutBossesDefeated) const
{
	OutCurrentZone = CurrentZone;
	OutDiscovered = DiscoveredZones;
	OutBossesDefeated = DefeatedBosses;
}

void USLFZoneManagerComponent::LoadZoneSaveData(FGameplayTag InCurrentZone, const FGameplayTagContainer& InDiscovered, const FGameplayTagContainer& InBossesDefeated)
{
	CurrentZone = InCurrentZone;
	DiscoveredZones = InDiscovered;
	DefeatedBosses = InBossesDefeated;
}

void USLFZoneManagerComponent::InitializeDefaultZones()
{
	auto MakeTag = [](const FName& Name) -> FGameplayTag
	{
		return FGameplayTag::RequestGameplayTag(Name, false);
	};

	// Hub
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Hub"));
		Z.DisplayName = FText::FromString(TEXT("Safe Haven"));
		Z.Difficulty = ESLFZoneDifficulty::Tutorial;
		Z.Faction = ESLFFaction::Neutral;
		Z.SuggestedLevelMin = 1;
		Z.SuggestedLevelMax = 80;
		Z.CurrencyMultiplier = 0.0f;
		ZoneConfigs.Add(Z);
	}

	// Zone 1: The Ashfields
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Ashfields"));
		Z.DisplayName = FText::FromString(TEXT("The Ashfields"));
		Z.Difficulty = ESLFZoneDifficulty::Easy;
		Z.Faction = ESLFFaction::Withered;
		Z.SuggestedLevelMin = 1;
		Z.SuggestedLevelMax = 15;
		Z.CurrencyMultiplier = 1.0f;
		Z.BossProgressTag = MakeTag(FName("Boss.Broodmother"));
		ZoneConfigs.Add(Z);
	}

	// Zone 2: Ironworks
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Ironworks"));
		Z.DisplayName = FText::FromString(TEXT("Ironworks"));
		Z.Difficulty = ESLFZoneDifficulty::Medium;
		Z.Faction = ESLFFaction::IronCovenant;
		Z.SuggestedLevelMin = 15;
		Z.SuggestedLevelMax = 30;
		Z.CurrencyMultiplier = 1.5f;
		Z.BossProgressTag = MakeTag(FName("Boss.Forgemaster"));
		ZoneConfigs.Add(Z);
	}

	// Zone 3: The Rift
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Rift"));
		Z.DisplayName = FText::FromString(TEXT("The Rift"));
		Z.Difficulty = ESLFZoneDifficulty::Medium;
		Z.Faction = ESLFFaction::Veilborn;
		Z.SuggestedLevelMin = 30;
		Z.SuggestedLevelMax = 50;
		Z.CurrencyMultiplier = 2.0f;
		Z.BossProgressTag = MakeTag(FName("Boss.Convergent"));
		ZoneConfigs.Add(Z);
	}

	// Zone 4: Sanctum of Ash
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Sanctum"));
		Z.DisplayName = FText::FromString(TEXT("Sanctum of Ash"));
		Z.Difficulty = ESLFZoneDifficulty::Hard;
		Z.Faction = ESLFFaction::AshenOrder;
		Z.SuggestedLevelMin = 30;
		Z.SuggestedLevelMax = 50;
		Z.CurrencyMultiplier = 2.0f;
		Z.BossProgressTag = MakeTag(FName("Boss.Hierophant"));
		ZoneConfigs.Add(Z);
	}

	// Zone 5: The Undercroft
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Undercroft"));
		Z.DisplayName = FText::FromString(TEXT("The Undercroft"));
		Z.Difficulty = ESLFZoneDifficulty::VeryHard;
		Z.Faction = ESLFFaction::Hollow;
		Z.SuggestedLevelMin = 50;
		Z.SuggestedLevelMax = 70;
		Z.CurrencyMultiplier = 3.0f;
		Z.BossProgressTag = MakeTag(FName("Boss.Primordial"));
		ZoneConfigs.Add(Z);
	}

	// Zone 6: The Core
	{
		FSLFZoneConfig Z;
		Z.ZoneTag = MakeTag(FName("Zone.Core"));
		Z.DisplayName = FText::FromString(TEXT("The Core"));
		Z.Difficulty = ESLFZoneDifficulty::Final;
		Z.Faction = ESLFFaction::None;
		Z.SuggestedLevelMin = 70;
		Z.SuggestedLevelMax = 80;
		Z.CurrencyMultiplier = 4.0f;
		Z.BossProgressTag = MakeTag(FName("Boss.Convergence"));
		ZoneConfigs.Add(Z);
	}
}
