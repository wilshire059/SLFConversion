// SLFCaveEventListener.h
// Elden Ring-style cave dungeon gameplay placement
// Boss door visible near entrance (locked), lever deep in dungeon,
// dead ends with loot/ambush/traps, shortcut back to entrance

#pragma once

#include "CoreMinimal.h"
#include "Core/DungeonEventListener.h"
#include "SLFCaveEventListener.generated.h"

UCLASS()
class SLFCONVERSION_API USLFCaveEventListener : public UDungeonEventListener
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Cave Event")
	int32 DungeonIndex = 0;

	/** Base damage for traps (scales with dungeon index) */
	UPROPERTY(EditAnywhere, Category = "Cave Event")
	float TrapBaseDamage = 30.0f;

	virtual void OnPostDungeonBuild_Implementation(ADungeon* Dungeon) override;
};
