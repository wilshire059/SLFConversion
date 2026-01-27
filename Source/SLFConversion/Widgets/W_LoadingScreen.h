// W_LoadingScreen.h
// C++ Widget class for W_LoadingScreen
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated: 2026-01-20 - Implemented actual level loading logic
// Source: BlueprintDNA/WidgetBlueprint/W_LoadingScreen.json
// Parent: UUserWidget
// Variables: 3 | Functions: 0 | Dispatchers: 1

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
#include "Animation/WidgetAnimation.h"


#include "W_LoadingScreen.generated.h"

// Forward declarations for widget types
class UImage;
class UTextBlock;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LoadingScreen_OnLoadingScreenFinished);

UCLASS()
class SLFCONVERSION_API UW_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_LoadingScreen(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* LoadingScreenAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double AdditionalWaitDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool AutoRemove;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET ANIMATIONS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FadeOut;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* LoadingScreenImage;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TipTitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TipDescription;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LoadingScreen_OnLoadingScreenFinished OnLoadingScreenFinished;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadingScreen")
	void EventFadeOutAndNotifyListeners();
	virtual void EventFadeOutAndNotifyListeners_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadingScreen")
	void EventFadeOutAndOpenLevelByName();
	virtual void EventFadeOutAndOpenLevelByName_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadingScreen")
	void EventFadeOutAndOpenLevelByRef();
	virtual void EventFadeOutAndOpenLevelByRef_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadingScreen")
	void EventOpenLevelByNameAndFadeOut(FName LevelName, bool bAbsolute, const FString& Options);
	virtual void EventOpenLevelByNameAndFadeOut_Implementation(FName LevelName, bool bAbsolute, const FString& Options);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadingScreen")
	void EventOpenLevelByReferenceAndFadeOut(const TSoftObjectPtr<UWorld>& Level, bool bAbsolute, const FString& Options);
	virtual void EventOpenLevelByReferenceAndFadeOut_Implementation(const TSoftObjectPtr<UWorld>& Level, bool bAbsolute, const FString& Options);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Callbacks for animation completion
	UFUNCTION()
	void OnFadeOutFinished_OpenLevelByRef();

	UFUNCTION()
	void OnFadeOutFinished_OpenLevelByName();

	UFUNCTION()
	void OnFadeOutFinished_NotifyListeners();

	// Cached level loading parameters
	TSoftObjectPtr<UWorld> PendingLevelRef;
	FName PendingLevelName;
	bool bPendingAbsolute;
	FString PendingOptions;

	// Timer handles for additional wait
	FTimerHandle AdditionalWaitTimerHandle;
};
