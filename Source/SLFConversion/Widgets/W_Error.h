// W_Error.h
// C++ Widget class for W_Error
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Error.json
// Parent: UUserWidget
// Variables: 8 | Functions: 0 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/Button.h"

#include "W_Error.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Error_OnClosed);

UCLASS()
class SLFCONVERSION_API UW_Error : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Error(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (8)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	double MessageWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FLinearColor BackgroundColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FLinearColor InnerBackgroundColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FButtonStyle ButtonStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FSlateColor TitleColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FText TitleText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FSlateColor MessageColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message Settings")
	FText MessageText;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Error_OnClosed OnClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Error")
	void EventSetMessage(const FText& Title, const FText& InMessage);
	virtual void EventSetMessage_Implementation(const FText& Title, const FText& InMessage);

protected:
	// Cache references
	void CacheWidgetReferences();
};
