// SLFSaveSlots.h
// C++ base for SG_SaveSlots - Save slot management
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLFSaveSlots.generated.h"

USTRUCT(BlueprintType)
struct FSLFSaveSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString SlotName;

	UPROPERTY(BlueprintReadWrite)
	FDateTime SaveTime;

	UPROPERTY(BlueprintReadWrite)
	int32 PlayerLevel = 1;

	UPROPERTY(BlueprintReadWrite)
	FString LocationName;
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFSaveSlots : public USaveGame
{
	GENERATED_BODY()

public:
	USLFSaveSlots();

	UPROPERTY(BlueprintReadWrite, Category = "Slots")
	TArray<FSLFSaveSlotInfo> SaveSlots;

	UPROPERTY(BlueprintReadWrite, Category = "Slots")
	int32 MaxSlots = 10;
};
