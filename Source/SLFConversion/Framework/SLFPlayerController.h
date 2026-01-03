// SLFPlayerController.h
// C++ base for PC_SoulslikeFramework - Player Controller
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SLFPlayerController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASLFPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
