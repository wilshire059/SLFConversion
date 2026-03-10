// SLFCaveRoomSelector.h
// Room-type selection logic for cave theme — filters markers by room role
// Uses actor tags on the marker instance to determine room type.

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/ThemeEngine/Rules/Selector/GenericDungeonSelectorLogic.h"
#include "SLFCaveRoomSelector.generated.h"

UCLASS()
class SLFCONVERSION_API USLFCaveRoomSelector : public UGenericDungeonSelectorLogic
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Cave Selector")
	bool bBossRoomOnly = false;

	UPROPERTY(EditAnywhere, Category = "Cave Selector")
	bool bEntranceRoomOnly = false;

	UPROPERTY(EditAnywhere, Category = "Cave Selector")
	bool bExcludeEntranceRoom = false;

	UPROPERTY(EditAnywhere, Category = "Cave Selector")
	bool bCorridorOnly = false;

	virtual bool SelectNode_Implementation(UDungeonModel* Model, UDungeonConfig* Config,
		UDungeonBuilder* Builder, UDungeonQuery* Query,
		const FRandomStream& RandomStream, const FDungeonMarkerInstance& MarkerInstance) override;
};
