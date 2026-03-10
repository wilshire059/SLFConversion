// SLFCaveRandomTransform.h
// Organic variation transforms for cave decoration props

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/ThemeEngine/Rules/Transformer/ProceduralDungeonTransformLogic.h"
#include "SLFCaveRandomTransform.generated.h"

UCLASS()
class SLFCONVERSION_API USLFCaveRandomTransform : public UProceduralDungeonTransformLogic
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Cave Transform")
	bool bRandomYaw = true;

	UPROPERTY(EditAnywhere, Category = "Cave Transform")
	float MinScale = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Cave Transform")
	float MaxScale = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Cave Transform")
	float PositionJitter = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Cave Transform")
	bool bFlipForCeiling = false;

	virtual FTransform Execute(ADungeon* InDungeon, const FRandomStream& InRandom,
		const FDungeonMarkerInstance& InMarker) override;
};
