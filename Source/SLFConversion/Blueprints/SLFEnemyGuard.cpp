// SLFEnemyGuard.cpp
// C++ implementation for B_Soulslike_Enemy_Guard

#include "SLFEnemyGuard.h"

ASLFEnemyGuard::ASLFEnemyGuard()
{
	// Components (RimLight, AreaLight, Lantern, Weapon) stay in Blueprint
	// to preserve configured values. Access via FindComponentByClass<>() if needed.
}

void ASLFEnemyGuard::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[EnemyGuard] BeginPlay: %s"), *GetName());
}
