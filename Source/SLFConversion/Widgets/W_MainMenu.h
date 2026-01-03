// W_MainMenu.h
// C++ Widget class for W_MainMenu
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_MainMenu.json
// Parent: UW_Navigable
// Variables: 6 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_MainMenu.generated.h"

// Forward declarations for widget types
class UW_LoadingScreen;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_MainMenu : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_MainMenu(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UMediaPlayer* MediaPlayer;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Default")
	TArray<UWidget*> Buttons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool CanContinueGame;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_LoadingScreen* LoadingScreen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* MainMenuAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void OnButtonSelected(UW_MainMenu_Button* Button);
	virtual void OnButtonSelected_Implementation(UW_MainMenu_Button* Button);


	// Event Handlers (4 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventFadeInMenu(double PlaybackSpeed);
	virtual void EventFadeInMenu_Implementation(double PlaybackSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_MainMenu")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
