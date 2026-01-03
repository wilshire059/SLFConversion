// SLFGameMode.cpp
#include "SLFGameMode.h"
#include "Blueprints/SLFSoulslikeCharacter.h"
#include "SLFPlayerController.h"

ASLFGameMode::ASLFGameMode()
{
	// Set DefaultPawnClass to the C++ class which now has camera and mesh components
	// The C++ class (SLFSoulslikeCharacter) has:
	// - CameraBoom (SpringArmComponent) for third-person camera
	// - FollowCamera (CameraComponent) attached to boom
	// - Default SKM_Manny mesh on CharacterMesh0
	DefaultPawnClass = ASLFSoulslikeCharacter::StaticClass();

	PlayerControllerClass = ASLFPlayerController::StaticClass();

	UE_LOG(LogTemp, Log, TEXT("[SLFGameMode] DefaultPawnClass: %s"), *DefaultPawnClass->GetName());
	UE_LOG(LogTemp, Log, TEXT("[SLFGameMode] PlayerControllerClass: %s"), *PlayerControllerClass->GetName());
}

void ASLFGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SLFGameMode] BeginPlay"));
}
