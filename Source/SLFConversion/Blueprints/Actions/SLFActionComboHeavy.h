// SLFActionComboHeavy.h
// C++ base for B_Action_ComboHeavy - Heavy attack combo
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionComboHeavy.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionComboHeavy : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionComboHeavy();
	virtual void ExecuteAction_Implementation() override;
};
