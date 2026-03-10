// SLFCaveRoomSelector.cpp

#include "SLFCaveRoomSelector.h"
#include "Core/Markers/DungeonMarker.h"

bool USLFCaveRoomSelector::SelectNode_Implementation(UDungeonModel* Model, UDungeonConfig* Config,
	UDungeonBuilder* Builder, UDungeonQuery* Query,
	const FRandomStream& RandomStream, const FDungeonMarkerInstance& MarkerInstance)
{
	// Check marker tags for room type classification
	const FDungeonMarkerInstanceTags& Tags = MarkerInstance.Tags;

	bool bHasBossTag = Tags.Tags.Contains(FName(TEXT("BossRoom")));
	bool bHasEntranceTag = Tags.Tags.Contains(FName(TEXT("Entrance")));
	bool bHasCorridorTag = Tags.Tags.Contains(FName(TEXT("Corridor")));

	if (bBossRoomOnly && !bHasBossTag) return false;
	if (bEntranceRoomOnly && !bHasEntranceTag) return false;
	if (bExcludeEntranceRoom && bHasEntranceTag) return false;
	if (bCorridorOnly && !bHasCorridorTag) return false;

	return true;
}
