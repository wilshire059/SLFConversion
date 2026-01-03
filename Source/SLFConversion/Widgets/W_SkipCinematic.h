// W_SkipCinematic.h
// C++ Widget class for W_SkipCinematic
//
// 20-PASS VALIDATION: 2026-01-02 Autonomous Session
// Source: BlueprintDNA_v2/WidgetBlueprint/W_SkipCinematic.json
// Parent: UW_Navigable_InputReader
// Variables: 3 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"

#include "W_SkipCinematic.generated.h"

// Forward declarations for widget types
class UTextBlock;
class USizeBox;
class UImage;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_SkipCinematic : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_SkipCinematic(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* RelatedMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector2D IconSize;

	// Target Input Action for skip (IA_GameMenu)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputAction* TargetInputAction;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MessageText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USizeBox* IconSizer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* KeyIcon;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Called when input device changes - updates the input icon based on current device
	 * Flow:
	 * 1. Get keys for IA_GameMenu from RelatedMapping
	 * 2. Check if MappedKeys is not empty
	 * 3. Select index based on ActiveDeviceType (Gamepad = 1, else 0)
	 * 4. Get key at that index
	 * 5. Get input icon for that key
	 * 6. If valid, set KeyIcon brush from soft texture
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_SkipCinematic")
	void OnInputDeviceChanged();
	virtual void OnInputDeviceChanged_Implementation();

	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_SkipCinematic")
	void EventInitializeInputIcon();
	virtual void EventInitializeInputIcon_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Timer handle for delayed initialization
	FTimerHandle InitializeInputIconTimerHandle;
};
