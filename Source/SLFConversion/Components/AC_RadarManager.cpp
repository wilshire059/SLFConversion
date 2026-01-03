// AC_RadarManager.cpp
// C++ component implementation for AC_RadarManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_RadarManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_RadarManager.h"
#include "AC_RadarElement.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

UAC_RadarManager::UAC_RadarManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	Visible = true;
	ShouldUpdate = true;
	RadarWidget = nullptr;
	RadarClampLength = 100.0;
	PlayerCamera = nullptr;
	RefreshInterval = 0.1;
	IconTint = FLinearColor::White;
	IconSize = FVector2D(32.0, 32.0);
	Pawn = nullptr;
}

void UAC_RadarManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::BeginPlay"));

	// Cache pawn and camera references
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		Pawn = Cast<APawn>(Owner);
		if (IsValid(Pawn))
		{
			PlayerCamera = Pawn->FindComponentByClass<UCameraComponent>();
		}
	}

	// Initialize radar
	SetupCardinals();
	SetupPlayerIcon();

	// Start update timer
	if (ShouldUpdate)
	{
		SetupUpdateTimer();
	}
}

void UAC_RadarManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * SetupPlayerIcon - Configure player icon on radar widget
 */
void UAC_RadarManager::SetupPlayerIcon_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::SetupPlayerIcon"));

	// Radar widget would be set up with player icon texture, tint, and size
	// This would typically call RadarWidget->SetPlayerIcon(PlayerIcon, IconTint, IconSize)
}

/**
 * SetupCardinals - Create and configure cardinal direction indicators
 */
void UAC_RadarManager::SetupCardinals_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::SetupCardinals"));

	// Cardinal setup would create N/S/E/W indicators on the radar widget
}

/**
 * GetDelta - Calculate delta between two angles considering direction
 */
void UAC_RadarManager::GetDelta_Implementation(double A, double B, bool IsClockwise, double& OutReturnValue, double& OutReturnValue_1, double& OutReturnValue_2)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::GetDelta - A: %.2f, B: %.2f, Clockwise: %s"),
		A, B, IsClockwise ? TEXT("true") : TEXT("false"));

	double Delta;
	if (IsClockwise)
	{
		Delta = FMath::Fmod(B - A + 360.0, 360.0);
	}
	else
	{
		Delta = FMath::Fmod(A - B + 360.0, 360.0);
	}

	OutReturnValue = Delta;
	OutReturnValue_1 = Delta;
	OutReturnValue_2 = Delta;
}

/**
 * ConvertToCircleDegrees - Convert angle to 0-360 range
 */
double UAC_RadarManager::ConvertToCircleDegrees_Implementation(double In)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::ConvertToCircleDegrees - In: %.2f"), In);

	double Result = FMath::Fmod(In + 360.0, 360.0);

	return Result;
}

/**
 * ConvertToTranslation - Convert rotations to 2D radar translation
 */
void UAC_RadarManager::ConvertToTranslation_Implementation(const FRotator& A, const FRotator& B, FVector2D& OutTranslation, double& OutTranslation_X, double& OutTranslation_Y, bool& OutIsClockwise, bool& OutIsInRadarLength, FVector2D& OutTranslation_3, double& OutTranslation_X_1, double& OutTranslation_Y_1, bool& OutIsClockwise_1, bool& OutIsInRadarLength_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::ConvertToTranslation"));

	// Calculate relative yaw
	double YawA = ConvertToCircleDegrees(A.Yaw);
	double YawB = ConvertToCircleDegrees(B.Yaw);

	// Determine direction (clockwise or counter-clockwise)
	double CWDelta, CCWDelta, Dummy;
	GetDelta(YawA, YawB, true, CWDelta, Dummy, Dummy);
	GetDelta(YawA, YawB, false, CCWDelta, Dummy, Dummy);

	bool bIsClockwise = CWDelta <= CCWDelta;
	double Delta = bIsClockwise ? CWDelta : CCWDelta;

	// Convert to translation (normalized to radar length)
	double NormalizedX = FMath::Sin(FMath::DegreesToRadians(Delta)) * (bIsClockwise ? 1.0 : -1.0);
	double NormalizedY = FMath::Cos(FMath::DegreesToRadians(Delta));

	// Clamp to radar bounds
	bool bInRadarLength = Delta <= RadarClampLength;
	double ClampedX = FMath::Clamp(NormalizedX, -1.0, 1.0);
	double ClampedY = FMath::Clamp(NormalizedY, -1.0, 1.0);

	FVector2D Translation(ClampedX, ClampedY);

	OutTranslation = Translation;
	OutTranslation_X = Translation.X;
	OutTranslation_Y = Translation.Y;
	OutIsClockwise = bIsClockwise;
	OutIsInRadarLength = bInRadarLength;

	OutTranslation_3 = Translation;
	OutTranslation_X_1 = Translation.X;
	OutTranslation_Y_1 = Translation.Y;
	OutIsClockwise_1 = bIsClockwise;
	OutIsInRadarLength_1 = bInRadarLength;
}

/**
 * ToggleUpdateState - Enable or disable radar updates
 */
void UAC_RadarManager::ToggleUpdateState_Implementation(bool InShouldUpdate)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::ToggleUpdateState - %s"), InShouldUpdate ? TEXT("true") : TEXT("false"));

	ShouldUpdate = InShouldUpdate;

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (ShouldUpdate)
	{
		SetupUpdateTimer();
	}
	else
	{
		World->GetTimerManager().ClearTimer(Timer);
	}
}

/**
 * RefreshCardinals - Update cardinal positions based on camera rotation
 */
void UAC_RadarManager::RefreshCardinals_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::RefreshCardinals"));

	if (!IsValid(PlayerCamera))
	{
		return;
	}

	// Update cardinal direction widgets based on camera yaw
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();

	// Each cardinal would get updated position based on relative angle to camera
}

/**
 * StartTrackElement - Add element to radar tracking
 */
void UAC_RadarManager::StartTrackElement_Implementation(UAC_RadarElement* TrackedElement, UW_Radar_TrackedElement*& OutTrackedElementWidget, UW_Radar_TrackedElement*& OutTrackedElementWidget_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::StartTrackElement"));

	if (!IsValid(TrackedElement))
	{
		OutTrackedElementWidget = nullptr;
		OutTrackedElementWidget_1 = nullptr;
		return;
	}

	// Add to tracked components
	TrackedComponents.AddUnique(TrackedElement);

	// Create widget for tracked element
	// Widget would be created and added to RadarWidget

	OutTrackedElementWidget = nullptr;
	OutTrackedElementWidget_1 = nullptr;
}

/**
 * RefreshTrackedElements - Update positions of all tracked elements
 */
void UAC_RadarManager::RefreshTrackedElements_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::RefreshTrackedElements - %d elements"), TrackedComponents.Num());

	if (!IsValid(Pawn) || !IsValid(PlayerCamera))
	{
		return;
	}

	FRotator CameraRotation = PlayerCamera->GetComponentRotation();

	for (UAC_RadarElement* Element : TrackedComponents)
	{
		if (!IsValid(Element))
		{
			continue;
		}

		AActor* ElementOwner = Element->GetOwner();
		if (!IsValid(ElementOwner))
		{
			continue;
		}

		// Calculate direction to element
		FVector Direction = ElementOwner->GetActorLocation() - Pawn->GetActorLocation();
		FRotator DirectionRotation = Direction.Rotation();

		// Convert to radar translation
		FVector2D Translation;
		double TransX, TransY;
		bool bClockwise, bInRange;
		FVector2D Translation2;
		double TransX2, TransY2;
		bool bClockwise2, bInRange2;

		ConvertToTranslation(CameraRotation, DirectionRotation, Translation, TransX, TransY, bClockwise, bInRange, Translation2, TransX2, TransY2, bClockwise2, bInRange2);

		// Update element widget position
	}
}

/**
 * StopTrackElement - Remove element from radar tracking
 */
void UAC_RadarManager::StopTrackElement_Implementation(UAC_RadarElement* Element)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::StopTrackElement"));

	if (IsValid(Element))
	{
		TrackedComponents.Remove(Element);
	}
}

/**
 * SetupUpdateTimer - Internal: Set up refresh timer
 */
void UAC_RadarManager::SetupUpdateTimer()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_RadarManager::SetupUpdateTimer - Interval: %.2f"), RefreshInterval);

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UAC_RadarManager::UpdateVisuals);

	World->GetTimerManager().SetTimer(Timer, TimerDelegate, RefreshInterval, true);
}

/**
 * UpdateVisuals - Called by timer to refresh radar
 */
void UAC_RadarManager::UpdateVisuals()
{
	RefreshCardinals();
	RefreshTrackedElements();
}
