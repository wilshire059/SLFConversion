// B_Ladder.cpp
// C++ implementation for Blueprint B_Ladder
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Ladder.json
// Logic traced from JSON node-by-node

#include "Blueprints/B_Ladder.h"
#include "Engine/StaticMesh.h"

AB_Ladder::AB_Ladder()
{
	// Initialize default values from Blueprint
	LadderHeight = 500.0;
	BarOffset = 50.0;
	OvershootCount = 2;
	ConnectionCount = 3;
	bBarAtZero = true;
	bAddFinishBar = true;
	TopCollisionExtent = FVector(50.0, 50.0, 50.0);
	TopCollisionOffsetX = 0.0;
	TopCollisionOffsetZ = 0.0;
	BottomCollisionExtent = FVector(50.0, 50.0, 50.0);
	BottomCollisionOffsetX = 0.0;
	BottomCollisionOffsetZ = 0.0;
	TopdownCollisionExtent = FVector(50.0, 50.0, 50.0);
	TopdownCollisionOffsetX = 0.0;
	TopdownCollisionOffsetZ = 0.0;
	Cache_LastBarIndex = 0;

	// Create components
	Barz = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Barz"));
	Pole = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Pole"));
	Connections = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Connections"));
	ClimbingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbingCollision"));
	BottomCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomCollision"));
	TopCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TopCollision"));
	TopdownCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TopdownCollision"));

	// Attach components to root
	if (RootComponent)
	{
		Barz->SetupAttachment(RootComponent);
		Pole->SetupAttachment(RootComponent);
		Connections->SetupAttachment(RootComponent);
		ClimbingCollision->SetupAttachment(RootComponent);
		BottomCollision->SetupAttachment(RootComponent);
		TopCollision->SetupAttachment(RootComponent);
		TopdownCollision->SetupAttachment(RootComponent);
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::Constructor - Components created"));
}

void AB_Ladder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Blueprint calls CreateLadder in construction script
	CreateLadder();
}

double AB_Ladder::GetPoleHeight_Implementation()
{
	// Logic from JSON GetPoleHeight graph:
	// 1. Get Pole component
	// 2. Get StaticMesh from Pole
	// 3. Call GetBounds() on StaticMesh
	// 4. Break BoxSphereBounds to get BoxExtent_Z
	// 5. Multiply by 2.0 (extent is half-size, we need full height)
	// 6. Return result

	if (Pole && Pole->GetStaticMesh())
	{
		FBoxSphereBounds Bounds = Pole->GetStaticMesh()->GetBounds();
		double PoleZ = Bounds.BoxExtent.Z * 2.0;

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::GetPoleHeight - PoleZ: %f"), PoleZ);
		return PoleZ;
	}

	UE_LOG(LogTemp, Warning, TEXT("AB_Ladder::GetPoleHeight - Pole or StaticMesh is null, returning 0"));
	return 0.0;
}

void AB_Ladder::CreateLadder_Implementation()
{
	// Logic from JSON CreateLadder graph:
	// Sequence of operations:
	// 1. Clear all ISM instances (Barz, Pole, Connections)
	// 2. Calculate pole height and bar count
	// 3. For loop to add bar instances
	// 4. For loop to add pole instances
	// 5. For loop to add connection instances
	// 6. Set collision box extents and positions

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Starting ladder creation"));

	// Step 1: Clear existing instances
	if (Barz)
	{
		Barz->ClearInstances();
	}
	if (Pole)
	{
		Pole->ClearInstances();
	}
	if (Connections)
	{
		Connections->ClearInstances();
	}

	// Step 2: Calculate pole height
	double PoleHeight = GetPoleHeight();
	if (PoleHeight <= 0.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AB_Ladder::CreateLadder - PoleHeight is 0, cannot create ladder"));
		return;
	}

	// Calculate number of bars based on ladder height and bar offset
	int32 BarCount = 0;
	if (BarOffset > 0.0)
	{
		BarCount = FMath::FloorToInt(LadderHeight / BarOffset);
	}

	// Add overshoot bars
	int32 TotalBars = BarCount + OvershootCount;

	// Store last bar index for caching
	Cache_LastBarIndex = TotalBars;

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - PoleHeight: %f, BarCount: %d, TotalBars: %d"),
		PoleHeight, BarCount, TotalBars);

	// Step 3: Add bar instances
	if (Barz)
	{
		int32 StartIndex = bBarAtZero ? 0 : 1;
		int32 EndIndex = TotalBars;

		// Add finish bar if enabled
		if (bAddFinishBar)
		{
			EndIndex++;
		}

		for (int32 i = StartIndex; i <= EndIndex; i++)
		{
			double BarZ = static_cast<double>(i) * BarOffset;
			FTransform BarTransform;
			BarTransform.SetLocation(FVector(0.0, 0.0, BarZ));
			BarTransform.SetRotation(FQuat::Identity);
			BarTransform.SetScale3D(FVector::OneVector);

			Barz->AddInstance(BarTransform, true);
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d bar instances"), EndIndex - StartIndex + 1);
	}

	// Step 4: Add pole instances
	// Calculate how many poles needed based on ladder height and pole height
	if (Pole && PoleHeight > 0.0)
	{
		double LastPoleZ = 0.0;
		int32 PoleCount = FMath::CeilToInt(LadderHeight / PoleHeight);

		for (int32 i = 0; i < PoleCount; i++)
		{
			FTransform PoleTransform;
			PoleTransform.SetLocation(FVector(0.0, 0.0, LastPoleZ));
			PoleTransform.SetRotation(FQuat::Identity);
			PoleTransform.SetScale3D(FVector::OneVector);

			Pole->AddInstance(PoleTransform, true);
			LastPoleZ += PoleHeight;
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d pole instances"), PoleCount);
	}

	// Step 5: Add connection instances
	if (Connections && ConnectionCount > 0)
	{
		double ConnectionSpacing = LadderHeight / static_cast<double>(ConnectionCount + 1);

		for (int32 i = 1; i <= ConnectionCount; i++)
		{
			double ConnectionZ = static_cast<double>(i) * ConnectionSpacing;
			FTransform ConnectionTransform;
			ConnectionTransform.SetLocation(FVector(0.0, 0.0, ConnectionZ));
			ConnectionTransform.SetRotation(FQuat::Identity);
			ConnectionTransform.SetScale3D(FVector::OneVector);

			Connections->AddInstance(ConnectionTransform, true);
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d connection instances"), ConnectionCount);
	}

	// Step 6: Set collision box positions and extents
	// Climbing collision - spans the full ladder height
	if (ClimbingCollision)
	{
		ClimbingCollision->SetBoxExtent(FVector(50.0, 50.0, LadderHeight / 2.0));
		ClimbingCollision->SetRelativeLocation(FVector(0.0, 0.0, LadderHeight / 2.0));
	}

	// Bottom collision
	if (BottomCollision)
	{
		BottomCollision->SetBoxExtent(BottomCollisionExtent);
		BottomCollision->SetRelativeLocation(FVector(BottomCollisionOffsetX, 0.0, BottomCollisionOffsetZ));
	}

	// Top collision
	if (TopCollision)
	{
		TopCollision->SetBoxExtent(TopCollisionExtent);
		TopCollision->SetRelativeLocation(FVector(TopCollisionOffsetX, 0.0, LadderHeight + TopCollisionOffsetZ));
	}

	// Topdown collision
	if (TopdownCollision)
	{
		TopdownCollision->SetBoxExtent(TopdownCollisionExtent);
		TopdownCollision->SetRelativeLocation(FVector(TopdownCollisionOffsetX, 0.0, LadderHeight + TopdownCollisionOffsetZ));
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Ladder creation complete"));
}
