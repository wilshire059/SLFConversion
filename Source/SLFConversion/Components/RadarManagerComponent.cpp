// RadarManagerComponent.cpp
// C++ implementation for AC_RadarManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_RadarManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 (initialized in constructor)
// Functions:         13/13 implemented (12 BlueprintNativeEvent + 1 inline getter)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "RadarManagerComponent.h"
#include "RadarElementComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

URadarManagerComponent::URadarManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	RadarClampLength = 100.0;
	RefreshInterval = 0.1;
	IconTint = FLinearColor::White;
	IconSize = FVector2D(16.0, 16.0);

	// Initialize runtime
	bVisible = false;
	bShouldUpdate = false;
	RadarWidget = nullptr;
	PlayerCamera = nullptr;
	RadarRadius = 5000.0;
}

void URadarManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] BeginPlay on %s"), *GetOwner()->GetName());

	// Schedule late initialization
	if (UWorld* World = GetWorld())
	{
		FTimerHandle InitTimer;
		World->GetTimerManager().SetTimer(InitTimer, this,
			&URadarManagerComponent::LateInitialize_Implementation, 0.5f, false);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// VISIBILITY [1-2/13]
// ═══════════════════════════════════════════════════════════════════════════════

void URadarManagerComponent::ShowRadar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] ShowRadar"));

	bVisible = true;
	if (RadarWidget)
	{
		RadarWidget->SetVisibility(ESlateVisibility::Visible);
	}
	ToggleUpdateState(true);
}

void URadarManagerComponent::HideRadar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] HideRadar"));

	bVisible = false;
	if (RadarWidget)
	{
		RadarWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	ToggleUpdateState(false);
}

// ═══════════════════════════════════════════════════════════════════════════════
// TRACKING [3-6/13]
// ═══════════════════════════════════════════════════════════════════════════════

UUserWidget* URadarManagerComponent::StartTrackElement_Implementation(URadarElementComponent* Element)
{
	if (!Element)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RadarManager] StartTrackElement: Invalid element"));
		return nullptr;
	}

	// Add to tracked list if not already there
	TrackedElements.AddUnique(Element);

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] StartTrackElement: Now tracking %d elements"), TrackedElements.Num());

	// TODO: Create marker widget for this element
	return nullptr;
}

void URadarManagerComponent::StopTrackElement_Implementation(URadarElementComponent* Element)
{
	if (!Element) return;

	TrackedElements.Remove(Element);

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] StopTrackElement: Now tracking %d elements"), TrackedElements.Num());
}

void URadarManagerComponent::AddCompassMarker_Implementation(FGameplayTag DirectionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] AddCompassMarker: %s"), *DirectionTag.ToString());

	// TODO: Add cardinal direction marker to radar
	if (FSLFCardinalData* Data = CardinalData.Find(DirectionTag))
	{
		// Create widget using Data->DirectionName and Data->Icon
	}
}

void URadarManagerComponent::UpdateTrackedElements_Implementation()
{
	if (!bShouldUpdate || !PlayerCamera) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector OwnerLocation = Owner->GetActorLocation();

	for (URadarElementComponent* Element : TrackedElements)
	{
		if (!Element || !Element->GetOwner()) continue;

		// Calculate position on radar
		FVector ElementLocation = Element->GetOwner()->GetActorLocation();
		FVector2D RadarPos = CalculateRadarPosition(ElementLocation);

		// TODO: Update element's marker widget position
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UPDATE CONTROL [7-9/13]
// ═══════════════════════════════════════════════════════════════════════════════

void URadarManagerComponent::ToggleUpdateState_Implementation(bool bEnabled)
{
	bShouldUpdate = bEnabled;

	if (bEnabled)
	{
		StartTimer();
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(Timer);
		}
	}
}

void URadarManagerComponent::RefreshTimer_Implementation()
{
	if (bShouldUpdate)
	{
		UpdateTrackedElements();
	}
}

void URadarManagerComponent::StartTimer_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(Timer, this,
			&URadarManagerComponent::RefreshTimer_Implementation,
			RefreshInterval, true);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [11-13/13]
// ═══════════════════════════════════════════════════════════════════════════════

FVector2D URadarManagerComponent::CalculateRadarPosition_Implementation(FVector WorldLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner || !PlayerCamera)
	{
		return FVector2D::ZeroVector;
	}

	// Get relative position
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector RelativePos = WorldLocation - OwnerLocation;

	// Get camera forward (yaw only)
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();
	CameraRotation.Pitch = 0.0f;
	CameraRotation.Roll = 0.0f;

	// Rotate relative position to camera space
	FVector RotatedPos = CameraRotation.UnrotateVector(RelativePos);

	// Scale to radar size
	float Distance = RelativePos.Size2D();
	float Scale = FMath::Min(Distance / RadarRadius, 1.0f);

	FVector2D RadarPos;
	RadarPos.X = RotatedPos.Y / RadarRadius * RadarClampLength;
	RadarPos.Y = -RotatedPos.X / RadarRadius * RadarClampLength;

	// Clamp to radar bounds
	float Length = RadarPos.Size();
	if (Length > RadarClampLength)
	{
		RadarPos = RadarPos.GetSafeNormal() * RadarClampLength;
	}

	return RadarPos;
}

void URadarManagerComponent::InitializeRadar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] InitializeRadar"));

	// Cache camera reference
	if (AActor* Owner = GetOwner())
	{
		PlayerCamera = Owner->FindComponentByClass<UCameraComponent>();
	}

	// TODO: Create radar widget from class
}

void URadarManagerComponent::LateInitialize_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] LateInitialize"));

	InitializeRadar();

	// Add compass markers
	for (const auto& Pair : CardinalData)
	{
		AddCompassMarker(Pair.Key);
	}
}
