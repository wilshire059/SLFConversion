// SLFActionUseEquippedTool.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionUseEquippedTool.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionUseEquippedTool : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionUseEquippedTool();
	virtual void ExecuteAction_Implementation() override;
};
