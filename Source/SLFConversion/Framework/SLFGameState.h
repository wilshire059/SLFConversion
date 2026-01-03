// SLFGameState.h
// C++ base for GS_SoulslikeFramework - Game State
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SLFGameState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASLFGameState();

	virtual void BeginPlay() override;
};
