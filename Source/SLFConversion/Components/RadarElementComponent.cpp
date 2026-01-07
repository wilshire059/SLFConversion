// RadarElementComponent.cpp
// C++ implementation for AC_RadarElement

#include "RadarElementComponent.h"
#include "RadarManagerComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRadarElement, Log, All);

URadarElementComponent::URadarElementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IconTint = FLinearColor::White;
	IconSize = FVector2D(32.0f, 32.0f);
	MarkerWidget = nullptr;
}

void URadarElementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogRadarElement, Verbose, TEXT("EndPlay - removing tracked element"));
	RemoveTrackedElement();
	Super::EndPlay(EndPlayReason);
}

void URadarElementComponent::AddTrackedElement_Implementation()
{
	UE_LOG(LogRadarElement, Log, TEXT("AddTrackedElement for %s"), *GetOwner()->GetName());

	// Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		UE_LOG(LogRadarElement, Warning, TEXT("  -> No valid player controller"));
		return;
	}

	// Get radar manager component from controller and register this element
	if (URadarManagerComponent* RadarManager = PC->FindComponentByClass<URadarManagerComponent>())
	{
		MarkerWidget = RadarManager->StartTrackElement(this);
		UE_LOG(LogRadarElement, Log, TEXT("  -> Registered with RadarManager"));
	}
	else
	{
		UE_LOG(LogRadarElement, Warning, TEXT("  -> No RadarManagerComponent on controller"));
	}
}

void URadarElementComponent::RemoveTrackedElement_Implementation()
{
	UE_LOG(LogRadarElement, Verbose, TEXT("RemoveTrackedElement for %s"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("nullptr"));

	if (!MarkerWidget)
	{
		return;
	}

	// Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		return;
	}

	// Unregister from RadarManager
	if (URadarManagerComponent* RadarManager = PC->FindComponentByClass<URadarManagerComponent>())
	{
		RadarManager->StopTrackElement(this);
		UE_LOG(LogRadarElement, Verbose, TEXT("  -> Unregistered from RadarManager"));
	}

	MarkerWidget = nullptr;
}
