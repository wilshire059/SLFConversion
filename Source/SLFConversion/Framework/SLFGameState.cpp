// SLFGameState.cpp
#include "SLFGameState.h"

ASLFGameState::ASLFGameState()
{
}

void ASLFGameState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SLFGameState] BeginPlay"));
}
