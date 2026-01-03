// SLFEnemyShowcase.cpp
// C++ implementation for B_Soulslike_Enemy_Showcase

#include "SLFEnemyShowcase.h"
#include "Components/ChildActorComponent.h"

ASLFEnemyShowcase::ASLFEnemyShowcase()
{
	// Weapon component stays in Blueprint to preserve configured values
	// Access via FindComponentByClass<UChildActorComponent>() if needed
}

void ASLFEnemyShowcase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[EnemyShowcase] BeginPlay: %s"), *GetName());
}
