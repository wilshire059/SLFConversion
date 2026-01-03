// SLFBossMalgareth.cpp
// C++ implementation for B_Soulslike_Boss_Malgareth

#include "SLFBossMalgareth.h"

ASLFBossMalgareth::ASLFBossMalgareth()
{
	// Components (Weapon_L, Weapon_R) stay in Blueprint to preserve configured values.
	// Access via FindComponentByClass<UChildActorComponent>() if needed in C++.
}

void ASLFBossMalgareth::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[BossMalgareth] BeginPlay: %s"), *GetName());
}
