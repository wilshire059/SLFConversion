// SLFNPCShowcaseGuide.cpp
// C++ implementation for B_Soulslike_NPC_ShowcaseGuide

#include "SLFNPCShowcaseGuide.h"

ASLFNPCShowcaseGuide::ASLFNPCShowcaseGuide()
{
	// Showcase Guide has no additional settings beyond base NPC
}

void ASLFNPCShowcaseGuide::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseGuide] BeginPlay: %s"), *GetName());
}
