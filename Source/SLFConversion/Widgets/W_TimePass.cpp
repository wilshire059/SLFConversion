// W_TimePass.cpp
// C++ Widget implementation for W_TimePass
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_TimePass.h"
#include "TimerManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

UW_TimePass::UW_TimePass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ClockInnerWidget(nullptr)
	, CurrentAngle(0.0)
	, AngleIncrement(5.0)  // Degrees per tick
{
}

void UW_TimePass::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_TimePass::NativeConstruct"));
}

void UW_TimePass::NativeDestruct()
{
	Super::NativeDestruct();

	// Clear timer if still running
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Timer);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_TimePass::NativeDestruct"));
}

void UW_TimePass::CacheWidgetReferences()
{
	// Find ClockInner widget
	if (WidgetTree)
	{
		ClockInnerWidget = WidgetTree->FindWidget(FName(TEXT("ClockInner")));
		UE_LOG(LogTemp, Log, TEXT("UW_TimePass::CacheWidgetReferences - ClockInner: %s"),
			ClockInnerWidget ? TEXT("found") : TEXT("null"));
	}
}

void UW_TimePass::EventAnimate_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_TimePass::EventAnimate - Animating from angle %f to %f"), FromAngle, ToAngle);

	// Set initial angle
	CurrentAngle = FromAngle;

	// Set initial rotation on ClockInner
	if (ClockInnerWidget)
	{
		ClockInnerWidget->SetRenderTransformAngle(static_cast<float>(CurrentAngle));
	}

	// Calculate angle direction and wrap handling
	// If ToAngle < FromAngle, we need to animate through 360
	double AngleDifference = ToAngle - FromAngle;
	if (AngleDifference < 0)
	{
		AngleDifference += 360.0;
	}

	// Calculate increment to complete in about 2 seconds at 60fps
	// 2 seconds * 60 fps = 120 ticks
	AngleIncrement = AngleDifference / 120.0;
	if (AngleIncrement < 1.0)
	{
		AngleIncrement = 3.0;  // Minimum visible increment
	}

	// Start repeating timer for animation (30fps = 0.033 seconds)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			Timer,
			this,
			&UW_TimePass::AnimationTick,
			0.033f,  // ~30 fps
			true     // Loop
		);
	}
}

void UW_TimePass::AnimationTick()
{
	// Increment angle
	CurrentAngle += AngleIncrement;

	// Wrap at 360
	if (CurrentAngle >= 360.0)
	{
		CurrentAngle -= 360.0;
	}

	// Set rotation on ClockInner widget
	if (ClockInnerWidget)
	{
		ClockInnerWidget->SetRenderTransformAngle(static_cast<float>(CurrentAngle));
	}

	// Check if we've reached or passed the target angle
	// Handle wrap-around: if ToAngle < FromAngle (crossing midnight)
	bool ReachedTarget = false;
	if (ToAngle >= FromAngle)
	{
		// Normal case: FromAngle < ToAngle
		ReachedTarget = CurrentAngle >= ToAngle;
	}
	else
	{
		// Wrap case: ToAngle < FromAngle (e.g., from 20:00 to 6:00)
		// We've completed if we've passed 360 and are now >= ToAngle
		ReachedTarget = (CurrentAngle >= ToAngle && CurrentAngle < FromAngle);
	}

	if (ReachedTarget)
	{
		// Set final angle exactly
		if (ClockInnerWidget)
		{
			ClockInnerWidget->SetRenderTransformAngle(static_cast<float>(ToAngle));
		}

		// Stop timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(Timer);
		}

		UE_LOG(LogTemp, Log, TEXT("UW_TimePass - Animation complete, broadcasting OnTimePassEnd"));
		OnTimePassEnd.Broadcast();
	}
}

void UW_TimePass::EventInitialize_Implementation(double InFromTime, double InToTime)
{
	UE_LOG(LogTemp, Log, TEXT("UW_TimePass::EventInitialize - FromTime: %f, ToTime: %f"), InFromTime, InToTime);

	// Store the from/to times
	FromTime = InFromTime;
	ToTime = InToTime;

	// Calculate angles for the clock visual (0-360 degrees based on 0-24 hours)
	FromAngle = (FromTime / 24.0) * 360.0;
	ToAngle = (ToTime / 24.0) * 360.0;

	// BUGFIX: Handle edge case where ToTime=24.0 results in ToAngle=360.0
	// Since we wrap angles at 360, use 359.9 to avoid infinite loop
	if (ToAngle >= 360.0)
	{
		ToAngle = 359.9;
	}
	if (FromAngle >= 360.0)
	{
		FromAngle = 0.0;
	}

	UE_LOG(LogTemp, Log, TEXT("UW_TimePass::EventInitialize - FromAngle: %f, ToAngle: %f"), FromAngle, ToAngle);

	// Start the animation
	EventAnimate();
}
