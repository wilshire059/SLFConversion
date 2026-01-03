// SLFSaveGame.h
// C++ base for SG_SoulslikeFramework - Save Game
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLFSaveGame.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USLFSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FString SaveSlotName;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	int32 UserIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FTransform PlayerTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FName CurrentLevelName;
};
