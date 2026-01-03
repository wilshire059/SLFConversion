// W_LoadingScreen.h
// C++ Widget class for W_LoadingScreen
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
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


#include "W_LoadingScreen.generated.h"

// Forward declarations for widget types


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
	void EventOpenLevelByReferenceAndFadeOut(int32 Level, bool bAbsolute, const FString& Options);
	virtual void EventOpenLevelByReferenceAndFadeOut_Implementation(int32 Level, bool bAbsolute, const FString& Options);

protected:
	// Cache references
	void CacheWidgetReferences();
};
