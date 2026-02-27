// SLFGameMode.cpp
#include "SLFGameMode.h"
#include "Blueprints/SLFSoulslikeCharacter.h"
#include "SLFPlayerController.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Atmosphere/AtmosphericFog.h"
#include "EngineUtils.h"

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
	UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode] BeginPlay — Lighting diagnostics:"));

	UWorld* World = GetWorld();
	if (!World) return;

	// Count all actors
	int32 TotalActors = 0;
	for (TActorIterator<AActor> It(World); It; ++It) { TotalActors++; }
	UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   Total actors in world: %d"), TotalActors);

	// Directional lights
	int32 DirLightCount = 0;
	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* DL = *It;
		UDirectionalLightComponent* Comp = DL->GetComponent();
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   DirectionalLight: %s | Visible=%d Intensity=%.1f"),
			*DL->GetName(), DL->IsHidden() ? 0 : 1, Comp ? Comp->Intensity : -1.0f);
		DirLightCount++;
	}
	if (DirLightCount == 0) UE_LOG(LogTemp, Error, TEXT("[SLFGameMode]   NO DirectionalLight found!"));

	// Sky lights
	int32 SkyLightCount = 0;
	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		ASkyLight* SL = *It;
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   SkyLight: %s | Visible=%d"),
			*SL->GetName(), SL->IsHidden() ? 0 : 1);
		SkyLightCount++;
	}
	if (SkyLightCount == 0) UE_LOG(LogTemp, Error, TEXT("[SLFGameMode]   NO SkyLight found!"));

	// Exponential height fog
	for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
	{
		AExponentialHeightFog* Fog = *It;
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   ExponentialHeightFog: %s"), *Fog->GetName());
	}

	// Look for BP_TODManager by class name (Blueprint actor)
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		FString ClassName = It->GetClass()->GetName();
		if (ClassName.Contains(TEXT("TOD")) || ClassName.Contains(TEXT("DayNight")) || ClassName.Contains(TEXT("Sky")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   Lighting-related actor: %s (class: %s)"),
				*It->GetName(), *ClassName);
		}
	}

	// Player position
	if (APawn* Pawn = World->GetFirstPlayerController() ? World->GetFirstPlayerController()->GetPawn() : nullptr)
	{
		FVector Loc = Pawn->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("[SLFGameMode]   Player position: (%.0f, %.0f, %.0f)"), Loc.X, Loc.Y, Loc.Z);
	}
}
