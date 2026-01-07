// W_Navigable_InputReader.h
// C++ Widget class for W_Navigable_InputReader
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Navigable_InputReader.json
// Parent: UW_Navigable
// Variables: 2 | Functions: 2 | Dispatchers: 0

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


#include "W_Navigable_InputReader.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Navigable_InputReader : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_Navigable_InputReader(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	EHardwareDevicePrimaryType ActiveDeviceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* KeyCorrelationTable;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	// GetInputIconForKey - looks up icon for a given key
	// Output param: Icon - the texture icon for this key
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Navigable_InputReader")
	void GetInputIconForKey(const FKey& Key, TSoftObjectPtr<UTexture2D>& Icon);
	virtual void GetInputIconForKey_Implementation(const FKey& Key, TSoftObjectPtr<UTexture2D>& Icon);


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Navigable_InputReader")
	void EventInitializeInputDevice();
	virtual void EventInitializeInputDevice_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Navigable_InputReader")
	void EventOnHardwareDeviceDetected(FPlatformUserId UserId, FInputDeviceId DeviceId);
	virtual void EventOnHardwareDeviceDetected_Implementation(FPlatformUserId UserId, FInputDeviceId DeviceId);

protected:
	// Cache references
	void CacheWidgetReferences();
};
