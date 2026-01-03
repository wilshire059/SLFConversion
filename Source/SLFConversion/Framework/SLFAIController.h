// SLFAIController.h
// C++ base for AIC_SoulslikeFramework - AI Controller
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SLFAIController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASLFAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};
