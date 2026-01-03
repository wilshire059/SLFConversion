// W_LoadGame_Entry.h
// C++ Widget class for W_LoadGame_Entry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_LoadGame_Entry.json
// Parent: UUserWidget
// Variables: 5 | Functions: 1 | Dispatchers: 1

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


#include "W_LoadGame_Entry.generated.h"

// Forward declarations for widget types
class UW_LoadingScreen;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types
class USG_SoulslikeFramework;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_LoadGame_Entry_OnSaveSlotSelected, UW_LoadGame_Entry*, Card);

UCLASS()
class SLFCONVERSION_API UW_LoadGame_Entry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_LoadGame_Entry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString SaveSlotName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_LoadingScreen* LoadingScreen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	USG_SoulslikeFramework* SGO;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* LoadedClass;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LoadGame_Entry_OnSaveSlotSelected OnSaveSlotSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame_Entry")
	void SetSaveSlotSelected(bool InSelected);
	virtual void SetSaveSlotSelected_Implementation(bool InSelected);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame_Entry")
	void EventOnLoadEntryPressed();
	virtual void EventOnLoadEntryPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
