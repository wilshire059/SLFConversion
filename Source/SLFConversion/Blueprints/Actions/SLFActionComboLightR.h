// SLFActionComboLightR.h
// C++ base for B_Action_ComboLight_R - Right-hand light attack combo
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionComboLightR.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionComboLightR : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionComboLightR();
	virtual void ExecuteAction_Implementation() override;
};
