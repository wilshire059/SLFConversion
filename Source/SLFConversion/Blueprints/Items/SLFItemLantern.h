// SLFItemLantern.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFItemBase.h"
#include "SLFItemLantern.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFItemLantern : public ASLFItemBase
{
	GENERATED_BODY()

public:
	ASLFItemLantern();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lantern")
	class UPointLightComponent* LanternLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lantern")
	bool bIsLit = false;

	UFUNCTION(BlueprintCallable, Category = "Lantern")
	void ToggleLight();
};
