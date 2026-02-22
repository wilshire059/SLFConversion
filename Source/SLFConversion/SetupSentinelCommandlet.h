#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SetupSentinelCommandlet.generated.h"

UCLASS()
class USetupSentinelCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	USetupSentinelCommandlet();
	virtual int32 Main(const FString& Params) override;
};
