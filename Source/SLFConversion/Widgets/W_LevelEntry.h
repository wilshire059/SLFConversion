// W_LevelEntry.h
// C++ Widget class for W_LevelEntry

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "W_LevelEntry.generated.h"

// Forward declarations
class UTextBlock;
class UStatManagerComponent;
class UW_StatEntry_StatName;

UCLASS()
class SLFCONVERSION_API UW_LevelEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_LevelEntry(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 LocalPlayerLevel;

	UPROPERTY()
	UTextBlock* ValueTextBlock;

	UPROPERTY()
	UW_StatEntry_StatName* NameWidget;

	UPROPERTY()
	UStatManagerComponent* CachedStatManager;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LevelEntry")
	void EventOnLevelUpdated(int32 NewLevel);
	virtual void EventOnLevelUpdated_Implementation(int32 NewLevel);

protected:
	void CacheWidgetReferences();
	void UpdateLevelDisplay();
};
