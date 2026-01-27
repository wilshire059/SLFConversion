// RadarElementComponent.h
// C++ base class for AC_RadarElement
// Variables: 4, Functions: 4
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_RadarElement
//
// PURPOSE: Marks actors to appear on the radar/minimap

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "RadarElementComponent.generated.h"

// Forward declarations
class UUserWidget;
class UW_Radar_TrackedElement;

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API URadarElementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URadarElementComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 4/4 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** Icon texture to display on radar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Tint color for the radar icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	FLinearColor IconTint;

	/** Size of the icon on the radar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	FVector2D IconSize;

	/** Reference to the created marker widget (W_Radar_TrackedElement) */
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	UW_Radar_TrackedElement* MarkerWidget;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 3/3 migrated (excluding ExecuteUbergraph)
	// ═══════════════════════════════════════════════════════════════════

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/**
	 * Register this element with the radar manager
	 * Creates the marker widget and adds to radar
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar")
	void AddTrackedElement();
	virtual void AddTrackedElement_Implementation();

	/**
	 * Unregister this element from the radar manager
	 * Removes the marker widget from radar
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar")
	void RemoveTrackedElement();
	virtual void RemoveTrackedElement_Implementation();
};
