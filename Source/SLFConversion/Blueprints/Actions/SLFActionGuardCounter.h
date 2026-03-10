// SLFActionGuardCounter.h
// Guard counter — after blocking, brief window to press heavy attack

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionGuardCounter.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionGuardCounter : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionGuardCounter();

	/** Window after blocking where guard counter is available (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard Counter")
	float CounterWindow = 0.3f;

	/** Damage multiplier for guard counter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard Counter")
	float DamageMultiplier = 1.8f;

	/** Poise damage multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard Counter")
	float PoiseDamageMultiplier = 2.0f;

	/** Whether the counter window is currently open */
	UPROPERTY(BlueprintReadOnly, Category = "Guard Counter")
	bool bCounterWindowOpen = false;

	/** Open the counter window (called when a block succeeds) */
	UFUNCTION(BlueprintCallable, Category = "Guard Counter")
	void OpenCounterWindow();

	/** Close the counter window */
	UFUNCTION(BlueprintCallable, Category = "Guard Counter")
	void CloseCounterWindow();

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;

private:
	FTimerHandle WindowTimerHandle;
};
