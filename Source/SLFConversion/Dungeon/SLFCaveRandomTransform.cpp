// SLFCaveRandomTransform.cpp

#include "SLFCaveRandomTransform.h"
#include "Core/Dungeon.h"
#include "Core/Markers/DungeonMarker.h"

FTransform USLFCaveRandomTransform::Execute(ADungeon* InDungeon, const FRandomStream& InRandom,
	const FDungeonMarkerInstance& InMarker)
{
	FVector Translation = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	float Scale = FMath::Lerp(MinScale, MaxScale, InRandom.FRand());

	// Random yaw rotation for organic variety
	if (bRandomYaw)
	{
		Rotation.Yaw = InRandom.FRandRange(0.0f, 360.0f);
	}

	// Position jitter
	if (PositionJitter > 0.0f)
	{
		Translation.X = InRandom.FRandRange(-PositionJitter, PositionJitter);
		Translation.Y = InRandom.FRandRange(-PositionJitter, PositionJitter);
		Translation.Z = InRandom.FRandRange(-PositionJitter * 0.25f, PositionJitter * 0.25f);
	}

	// Flip for ceiling-mounted props (stalactites)
	FVector ScaleVec(Scale, Scale, Scale);
	if (bFlipForCeiling)
	{
		Rotation.Pitch = 180.0f;
		// Extend Z stretch for stalactites
		ScaleVec.Z *= FMath::Lerp(1.5f, 4.0f, InRandom.FRand());
	}

	return FTransform(Rotation, Translation, ScaleVec);
}
