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
#include "Widgets/W_Radar.h"
#include "Widgets/W_Radar_TrackedElement.h"
#include "Widgets/W_Radar_Cardinal.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagsManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

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
	bIsRadarInitialized = false;
}

void URadarManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] BeginPlay on %s"), *GetOwner()->GetName());

	// Populate default cardinal data if not set from Blueprint
	PopulateDefaultCardinalData();

	// Schedule late initialization
	if (UWorld* World = GetWorld())
	{
		FTimerHandle InitTimer;
		World->GetTimerManager().SetTimer(InitTimer, this,
			&URadarManagerComponent::LateInitialize_Implementation, 0.5f, false);
	}
}

void URadarManagerComponent::PopulateDefaultCardinalData()
{
	// Only populate if CardinalData is empty (not set from Blueprint)
	if (CardinalData.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] CardinalData already populated (%d entries), skipping defaults"), CardinalData.Num());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] Populating default CardinalData"));

	// Define cardinal directions with their angles
	// North = 0, East = 90, South = 180, West = 270
	// Only showing 4 main cardinals (N, E, S, W) to match bp_only behavior
	struct FCardinalEntry
	{
		const TCHAR* TagSuffix;
		const TCHAR* DisplayText;
		double Angle;
	};

	const FCardinalEntry Cardinals[] = {
		{ TEXT("N"), TEXT("N"), 0.0 },
		{ TEXT("E"), TEXT("E"), 90.0 },
		{ TEXT("S"), TEXT("S"), 180.0 },
		{ TEXT("W"), TEXT("W"), 270.0 },
	};

	for (const FCardinalEntry& Entry : Cardinals)
	{
		FString TagString = FString::Printf(TEXT("SoulslikeFramework.Radar.Cardinals.%s"), Entry.TagSuffix);
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);

		if (Tag.IsValid())
		{
			FSLFCardinalData Data;
			Data.UIDisplayText = Entry.DisplayText;
			Data.Value = Entry.Angle;
			CardinalData.Add(Tag, Data);
			UE_LOG(LogTemp, Log, TEXT("[RadarManager] Added cardinal: %s (%.0f degrees)"), Entry.DisplayText, Entry.Angle);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[RadarManager] Failed to find GameplayTag: %s"), *TagString);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] Populated %d cardinal entries"), CardinalData.Num());
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

UW_Radar_TrackedElement* URadarManagerComponent::StartTrackElement_Implementation(URadarElementComponent* Element)
{
	if (!Element)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RadarManager] StartTrackElement: Invalid element"));
		return nullptr;
	}

	// Check if already tracking this element
	if (UW_Radar_TrackedElement** ExistingWidget = TrackedElementWidgets.Find(Element))
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] StartTrackElement: Element already tracked"));
		return *ExistingWidget;
	}

	// Add to tracked list
	TrackedElements.AddUnique(Element);

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] StartTrackElement: Now tracking %d elements"), TrackedElements.Num());

	// Create widget marker via RadarWidget
	if (RadarWidget)
	{
		// Get icon from RadarElementComponent
		TSoftObjectPtr<UTexture2D> ElementIcon = Element->Icon;
		FLinearColor ElementTint = Element->IconTint;
		FVector2D ElementSize = Element->IconSize;

		// Create tracked element widget
		UW_Radar_TrackedElement* NewMarker = RadarWidget->AddTrackedElement(ElementIcon, ElementTint, ElementSize);
		if (NewMarker)
		{
			TrackedElementWidgets.Add(Element, NewMarker);
			UE_LOG(LogTemp, Log, TEXT("[RadarManager] Created marker widget for tracked element"));
			return NewMarker;
		}
	}

	return nullptr;
}

void URadarManagerComponent::StopTrackElement_Implementation(URadarElementComponent* Element)
{
	if (!Element) return;

	TrackedElements.Remove(Element);

	// Remove the widget from the radar
	if (UW_Radar_TrackedElement** MarkerWidget = TrackedElementWidgets.Find(Element))
	{
		if (RadarWidget && *MarkerWidget)
		{
			RadarWidget->RemoveTrackedElement(*MarkerWidget);
		}
		TrackedElementWidgets.Remove(Element);
	}

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] StopTrackElement: Now tracking %d elements"), TrackedElements.Num());
}

void URadarManagerComponent::AddCompassMarker_Implementation(FGameplayTag DirectionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] AddCompassMarker: %s"), *DirectionTag.ToString());

	// Add cardinal direction marker to radar
	if (FSLFCardinalData* Data = CardinalData.Find(DirectionTag))
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] Compass marker data: %s"), *Data->UIDisplayText);

		// Add cardinal to radar widget
		if (RadarWidget)
		{
			// MaxAllowedNameLength controls text truncation (e.g., "North" vs "N")
			int32 MaxNameLength = 1; // Single character for cardinals (N, S, E, W)
			RadarWidget->AddCardinal(*Data, MaxNameLength);
			UE_LOG(LogTemp, Log, TEXT("[RadarManager] Added cardinal %s to radar widget"), *Data->UIDisplayText);
		}
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

		// Update element marker widget position via render transform
		if (UW_Radar_TrackedElement** MarkerWidget = TrackedElementWidgets.Find(Element))
		{
			if (*MarkerWidget)
			{
				// Set render translation to position the marker on the radar
				(*MarkerWidget)->SetRenderTranslation(RadarPos);
			}
		}
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
		RefreshCardinals();
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

	// Cache camera reference - camera is on the Pawn, not the Controller
	AActor* Owner = GetOwner();
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		// Owner is a PlayerController, get camera from controlled Pawn
		if (APawn* Pawn = PC->GetPawn())
		{
			PlayerCamera = Pawn->FindComponentByClass<UCameraComponent>();
			UE_LOG(LogTemp, Log, TEXT("[RadarManager] Found camera on Pawn: %s"), PlayerCamera ? TEXT("Yes") : TEXT("No"));
		}
	}
	else if (Owner)
	{
		// Owner might be the Pawn directly
		PlayerCamera = Owner->FindComponentByClass<UCameraComponent>();
	}

	// RadarWidget is typically set from the HUD widget when it initializes
	// The HUD creates the radar widget and passes reference via SetRadarWidget
	if (RadarWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] Radar widget assigned - setting up player icon"));

		// Set player icon on the radar widget
		RadarWidget->SetPlayerIcon(PlayerIcon, IconTint, IconSize);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] Awaiting radar widget assignment from HUD"));
	}
}

void URadarManagerComponent::LateInitialize_Implementation()
{
	// Prevent duplicate initialization (can be called from both timer and EventInitializeRadar)
	if (bIsRadarInitialized)
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] LateInitialize - Already initialized, skipping"));
		return;
	}

	// Only proceed if RadarWidget is set
	if (!RadarWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[RadarManager] LateInitialize - No RadarWidget yet, skipping"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[RadarManager] LateInitialize - Initializing radar with %d cardinal entries"), CardinalData.Num());

	InitializeRadar();

	// Add compass markers
	for (const auto& Pair : CardinalData)
	{
		AddCompassMarker(Pair.Key);
	}

	bIsRadarInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("[RadarManager] LateInitialize - Completed, marked as initialized"));

	// Show radar and start updates
	ShowRadar();
}

void URadarManagerComponent::RefreshCardinals_Implementation()
{
	if (!RadarWidget || !PlayerCamera)
	{
		return;
	}

	// Get camera yaw (facing direction)
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();
	float CameraYaw = CameraRotation.Yaw;

	// Normalize to 0-360 range
	while (CameraYaw < 0.0f)
	{
		CameraYaw += 360.0f;
	}
	while (CameraYaw >= 360.0f)
	{
		CameraYaw -= 360.0f;
	}

	// Update each cardinal widget's position based on relative angle
	for (UW_Radar_Cardinal* Cardinal : RadarWidget->Cardinals)
	{
		if (!Cardinal)
		{
			continue;
		}

		// Get this cardinal's angle (from CardinalData.Value)
		float CardinalAngle = Cardinal->CardinalData.Value;

		// Calculate relative angle (how far from camera forward)
		float RelativeAngle = CardinalAngle - CameraYaw;

		// Normalize to -180 to +180 range
		while (RelativeAngle < -180.0f)
		{
			RelativeAngle += 360.0f;
		}
		while (RelativeAngle > 180.0f)
		{
			RelativeAngle -= 360.0f;
		}

		// Cardinals outside visible range (+/- 90 degrees) are hidden
		const float MaxVisibleAngle = 90.0f;
		if (FMath::Abs(RelativeAngle) > MaxVisibleAngle)
		{
			Cardinal->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Cardinal->SetVisibility(ESlateVisibility::Visible);

			// Calculate X position on radar
			// RelativeAngle 0 = center, -90 = left edge, +90 = right edge
			// Map angle to position: X = (RelativeAngle / MaxVisibleAngle) * RadarClampLength
			float XPos = (RelativeAngle / MaxVisibleAngle) * RadarClampLength;

			// Set render translation for positioning
			Cardinal->SetRenderTranslation(FVector2D(XPos, 0.0f));
		}
	}
}
