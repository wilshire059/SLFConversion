// AC_RadarElement.cpp
// C++ component implementation for AC_RadarElement
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_RadarElement.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_RadarElement.h"
#include "AC_RadarManager.h"
#include "Kismet/GameplayStatics.h"

UAC_RadarElement::UAC_RadarElement()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize defaults
	IconTint = FLinearColor::White;
	IconSize = FVector2D(16.0, 16.0);
	MarkerWidget = nullptr;
}

void UAC_RadarElement::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarElement::BeginPlay"));

	// Register with player's radar manager
	RegisterWithRadarManager();
}

void UAC_RadarElement::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from radar manager
	UnregisterFromRadarManager();

	Super::EndPlay(EndPlayReason);
}

void UAC_RadarElement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * RegisterWithRadarManager - Find player's radar and register this element
 */
void UAC_RadarElement::RegisterWithRadarManager()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarElement::RegisterWithRadarManager"));

	// Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	// Get radar manager from player
	UAC_RadarManager* RadarManager = PlayerPawn->FindComponentByClass<UAC_RadarManager>();
	if (IsValid(RadarManager))
	{
		UW_Radar_TrackedElement* Widget1 = nullptr;
		UW_Radar_TrackedElement* Widget2 = nullptr;
		RadarManager->StartTrackElement(this, Widget1, Widget2);
		MarkerWidget = Widget1;
		UE_LOG(LogTemp, Log, TEXT("  Registered with radar manager"));
	}
}

/**
 * UnregisterFromRadarManager - Remove this element from player's radar
 */
void UAC_RadarElement::UnregisterFromRadarManager()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarElement::UnregisterFromRadarManager"));

	// Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	// Get radar manager from player
	UAC_RadarManager* RadarManager = PlayerPawn->FindComponentByClass<UAC_RadarManager>();
	if (IsValid(RadarManager))
	{
		RadarManager->StopTrackElement(this);
		MarkerWidget = nullptr;
		UE_LOG(LogTemp, Log, TEXT("  Unregistered from radar manager"));
	}
}

