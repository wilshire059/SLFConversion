// SLFPlayerState.cpp
#include "SLFPlayerState.h"

ASLFPlayerState::ASLFPlayerState()
{
}

void ASLFPlayerState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerState] BeginPlay"));
}
