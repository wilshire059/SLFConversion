// SLFDoorDemo.h
// C++ base for B_Door_Demo - Demo/Tutorial door with hints
#pragma once

#include "CoreMinimal.h"
#include "SLFDoorBase.h"
#include "SLFDoorDemo.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFDoorDemo : public ASLFDoorBase
{
	GENERATED_BODY()

public:
	ASLFDoorDemo();

	// ============================================================
	// MIGRATION SUMMARY: B_Door_Demo
	// Variables: 4 | Functions: 2 | Dispatchers: 0
	// ============================================================

	// Demo/Tutorial Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FText HintText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	bool bShowHintOnApproach = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	bool bShowControlPrompt = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	float HintDisplayTime = 5.0f;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Demo")
	void ShowHint();
	virtual void ShowHint_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Demo")
	void HideHint();
	virtual void HideHint_Implementation();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;
};
