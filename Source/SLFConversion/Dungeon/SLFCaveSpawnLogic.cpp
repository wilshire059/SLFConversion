// SLFCaveSpawnLogic.cpp

#include "SLFCaveSpawnLogic.h"
#include "Core/Dungeon.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"

void USLFCaveSpawnLogic::OnItemSpawn_Implementation(AActor* Actor, const ADungeon* Dungeon)
{
	if (!Actor) return;

	// Configure point lights based on light type
	APointLight* PointLight = Cast<APointLight>(Actor);
	if (PointLight && PointLight->PointLightComponent)
	{
		PointLight->PointLightComponent->SetIntensity(LightIntensity);
		PointLight->PointLightComponent->SetAttenuationRadius(LightAttenuation);
		PointLight->PointLightComponent->SetLightColor(GetLightColor());
		PointLight->PointLightComponent->SetCastShadows(LightType == ESLFCaveLightType::BossRoom);
	}
}

FLinearColor USLFCaveSpawnLogic::GetLightColor() const
{
	switch (LightType)
	{
	case ESLFCaveLightType::Bioluminescent:
		return FLinearColor(0.2f, 0.6f, 0.5f); // Blue-green
	case ESLFCaveLightType::Entrance:
		return FLinearColor(0.8f, 0.7f, 0.5f); // Warm amber
	case ESLFCaveLightType::BossRoom:
		return FLinearColor(0.4f, 0.2f, 0.6f); // Ominous purple
	case ESLFCaveLightType::Torch:
		return FLinearColor(1.0f, 0.5f, 0.2f); // Orange-red
	default:
		return FLinearColor::White;
	}
}
