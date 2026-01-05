// SLFLadderBase.cpp
// C++ implementation for B_Ladder

#include "SLFLadderBase.h"
#include "Components/SceneComponent.h"

ASLFLadderBase::ASLFLadderBase()
{
	// DefaultSceneRoot is created by parent SLFInteractableBase

	// Initialize basic config
	LadderHeight = 400.0;
	BarOffset = 40.0;
	OvershootCount = 2;
	ConnectionCount = 2;
	bBarAtZero = true;
	bAddFinishBar = true;

	// Initialize top collision
	TopCollisionExtent = FVector(50.0, 30.0, 30.0);
	TopCollisionOffsetX = 50.0;
	TopCollisionOffsetZ = 0.0;

	// Initialize bottom collision
	BottomCollisionExtent = FVector(50.0, 30.0, 30.0);
	BottomCollisionOffsetX = 50.0;
	BottomCollisionOffsetZ = 50.0;

	// Initialize topdown collision
	TopdownCollisionExtent = FVector(30.0, 30.0, 50.0);
	TopdownCollisionOffsetX = 30.0;
	TopdownCollisionOffsetZ = -30.0;

	// Initialize runtime
	Cache_LastBarIndex = 0;

	// Update interaction prompt (inherited from SLFInteractableBase)
	InteractionPrompt = FText::FromString(TEXT("Climb"));
}

void ASLFLadderBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[Ladder] BeginPlay - Height: %.2f"), LadderHeight);
}

double ASLFLadderBase::GetPoleHeight_Implementation()
{
	return LadderHeight + (OvershootCount * BarOffset);
}

void ASLFLadderBase::CreateLadder_Implementation()
{
	int32 NumBars = FMath::FloorToInt(LadderHeight / BarOffset);
	if (bBarAtZero) NumBars++;
	if (bAddFinishBar) NumBars++;
	Cache_LastBarIndex = NumBars;
}
