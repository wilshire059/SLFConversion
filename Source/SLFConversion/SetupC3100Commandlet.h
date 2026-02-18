#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SetupC3100Commandlet.generated.h"

UCLASS()
class USetupC3100Commandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	USetupC3100Commandlet();
	virtual int32 Main(const FString& Params) override;
};
