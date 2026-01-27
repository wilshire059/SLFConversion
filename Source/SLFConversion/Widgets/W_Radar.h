// W_Radar.h
// C++ Widget class for W_Radar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Radar.json
// Parent: UUserWidget
// Variables: 2 | Functions: 4 | Dispatchers: 0

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


#include "W_Radar.generated.h"

// Forward declarations for widget types
class UW_Radar_Cardinal;
class UW_Radar_TrackedElement;
class UOverlay;
class UImage;
class USizeBox;
class UHorizontalBox;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Radar : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Radar(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Radar_Cardinal*> Cardinals;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Radar_TrackedElement*> Elements;

	// Widget references (from UMG designer)
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	class UOverlay* RadarOL;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	class UImage* PlayerIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	class USizeBox* IconSizer;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	class UHorizontalBox* CardinalContainer;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Radar")
	void AddCardinal(const FSLFCardinalData& InData, int32 MaxAllowedNameLength);
	virtual void AddCardinal_Implementation(const FSLFCardinalData& InData, int32 MaxAllowedNameLength);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Radar")
	void SetPlayerIcon(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& Tint, const FVector2D& Scale);
	virtual void SetPlayerIcon_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& Tint, const FVector2D& Scale);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Radar")
	UW_Radar_TrackedElement* AddTrackedElement(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& IconTint, const FVector2D& IconSize);
	virtual UW_Radar_TrackedElement* AddTrackedElement_Implementation(const TSoftObjectPtr<UTexture2D>& Icon, const FLinearColor& IconTint, const FVector2D& IconSize);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Radar")
	void RemoveTrackedElement(UW_Radar_TrackedElement* Element);
	virtual void RemoveTrackedElement_Implementation(UW_Radar_TrackedElement* Element);

protected:
	// Cache references
	void CacheWidgetReferences();
};
