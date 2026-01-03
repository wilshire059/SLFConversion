// SLFPlayerController.cpp
#include "SLFPlayerController.h"

ASLFPlayerController::ASLFPlayerController()
{
}

void ASLFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] BeginPlay"));
}

void ASLFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SetupInputComponent"));
}
