// SLFPlayerState.h
// C++ base for PS_SoulslikeFramework - Player State
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SLFPlayerState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASLFPlayerState();

	virtual void BeginPlay() override;
};
