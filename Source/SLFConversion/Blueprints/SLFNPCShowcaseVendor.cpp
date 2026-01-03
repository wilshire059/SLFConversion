// SLFNPCShowcaseVendor.cpp
// C++ implementation for B_Soulslike_NPC_ShowcaseVendor

#include "SLFNPCShowcaseVendor.h"

ASLFNPCShowcaseVendor::ASLFNPCShowcaseVendor()
{
	// Components (Head, Body, Arms, Legs) stay in Blueprint
	// to preserve configured mesh values. Access via FindComponentByClass<>() if needed.
}

void ASLFNPCShowcaseVendor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] BeginPlay: %s"), *GetName());
}
