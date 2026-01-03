// SLFActionGuardStart.h
// C++ base for B_Action_GuardStart - Begin blocking
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionGuardStart.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionGuardStart : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionGuardStart();
	virtual void ExecuteAction_Implementation() override;
};
