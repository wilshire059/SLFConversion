// SLFCaveSpawnLogic.h
// Post-spawn initialization for cave actors (lights, particles, traps)

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/ThemeEngine/Rules/Spawn/DungeonSpawnLogic.h"
#include "SLFCaveSpawnLogic.generated.h"

UENUM(BlueprintType)
enum class ESLFCaveLightType : uint8
{
	Bioluminescent,   // Blue-green ambient
	Entrance,         // Warm amber
	BossRoom,         // Ominous purple
	Torch             // Orange-red
};

UCLASS()
class SLFCONVERSION_API USLFCaveSpawnLogic : public UDungeonSpawnLogic
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Cave Spawn")
	ESLFCaveLightType LightType = ESLFCaveLightType::Bioluminescent;

	UPROPERTY(EditAnywhere, Category = "Cave Spawn")
	float LightIntensity = 15000.0f;

	UPROPERTY(EditAnywhere, Category = "Cave Spawn")
	float LightAttenuation = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Cave Spawn")
	int32 DungeonTier = 0;

	virtual void OnItemSpawn_Implementation(AActor* Actor, const ADungeon* Dungeon) override;

private:
	FLinearColor GetLightColor() const;
};
