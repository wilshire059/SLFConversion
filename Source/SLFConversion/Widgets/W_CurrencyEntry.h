// W_CurrencyEntry.h
// C++ Widget class for W_CurrencyEntry

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "W_CurrencyEntry.generated.h"

// Forward declarations
class UTextBlock;
class UInventoryManagerComponent;
class UW_StatEntry_StatName;

UCLASS()
class SLFCONVERSION_API UW_CurrencyEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_CurrencyEntry(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalCurrency;

	UPROPERTY()
	UTextBlock* ValueTextBlock;

	UPROPERTY()
	UW_StatEntry_StatName* NameWidget;

	UPROPERTY()
	UInventoryManagerComponent* CachedInventoryManager;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CurrencyEntry")
	void EventOnCurrencyUpdated(int32 NewCurrency);
	virtual void EventOnCurrencyUpdated_Implementation(int32 NewCurrency);

protected:
	void CacheWidgetReferences();
	void UpdateCurrencyDisplay();
};
