// RadarManagerComponent.h
// C++ base class for AC_RadarManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_RadarManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 migrated
// Functions:         13/13 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 0/0
// Graphs:            11 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_RadarManager
//
// PURPOSE: Minimap/radar system - tracks elements, updates positions, manages widget

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "RadarManagerComponent.generated.h"

// Forward declarations
class UCameraComponent;
class UUserWidget;
class URadarElementComponent;
class UTexture2D;
class UW_Radar;
class UW_Radar_TrackedElement;

// Types used from SLFGameTypes.h:
// - FSLFCardinalData

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API URadarManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URadarManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config Variables (6) ---

	/** [1/13] Cardinal direction data (N, S, E, W icons) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, FSLFCardinalData> CardinalData;

	/** [2/13] Maximum radar clamp distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double RadarClampLength;

	/** [3/13] Radar refresh interval in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double RefreshInterval;

	/** [4/13] Player icon texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	TSoftObjectPtr<UTexture2D> PlayerIcon;

	/** [5/13] Player icon tint color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	FLinearColor IconTint;

	/** [6/13] Player icon size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Icon")
	FVector2D IconSize;

	// --- Runtime Variables (7) ---

	/** [7/13] Whether radar is currently visible */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bVisible;

	/** [8/13] Whether radar should update positions */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bShouldUpdate;

	/** [9/13] Refresh timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FTimerHandle Timer;

	/** [10/13] Radar widget reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UW_Radar* RadarWidget;

	/** Map of tracked elements to their widget representations */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<URadarElementComponent*, UW_Radar_TrackedElement*> TrackedElementWidgets;

	/** [11/13] Player camera reference for orientation */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UCameraComponent* PlayerCamera;

	/** [12/13] Array of tracked radar elements */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<URadarElementComponent*> TrackedElements;

	/** [13/13] Radar detection radius */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	double RadarRadius;

	/** Flag to track if radar has been fully initialized (prevents duplicate cardinal markers) */
	UPROPERTY(Transient)
	bool bIsRadarInitialized;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Visibility (2) ---

	/** [1/13] Show radar widget */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void ShowRadar();
	virtual void ShowRadar_Implementation();

	/** [2/13] Hide radar widget */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void HideRadar();
	virtual void HideRadar_Implementation();

	// --- Tracking (4) ---

	/** [3/13] Start tracking a radar element
	 * @param Element - The radar element component to track
	 * @return The created marker widget
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager|Tracking")
	UW_Radar_TrackedElement* StartTrackElement(URadarElementComponent* Element);
	virtual UW_Radar_TrackedElement* StartTrackElement_Implementation(URadarElementComponent* Element);

	/** [4/13] Stop tracking a radar element
	 * @param Element - The radar element component to stop tracking
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager|Tracking")
	void StopTrackElement(URadarElementComponent* Element);
	virtual void StopTrackElement_Implementation(URadarElementComponent* Element);

	/** [5/13] Add compass marker to radar
	 * @param DirectionTag - Cardinal direction tag
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager|Tracking")
	void AddCompassMarker(FGameplayTag DirectionTag);
	virtual void AddCompassMarker_Implementation(FGameplayTag DirectionTag);

	/** [6/13] Update all tracked element positions */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager|Tracking")
	void UpdateTrackedElements();
	virtual void UpdateTrackedElements_Implementation();

	// --- Update Control (3) ---

	/** [7/13] Toggle whether radar updates */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void ToggleUpdateState(bool bEnabled);
	virtual void ToggleUpdateState_Implementation(bool bEnabled);

	/** [8/13] Internal timer callback for refresh */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void RefreshTimer();
	virtual void RefreshTimer_Implementation();

	/** [9/13] Start the refresh timer */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void StartTimer();
	virtual void StartTimer_Implementation();

	// --- Utility (4) ---

	/** [10/13] Get radar widget reference */
	UFUNCTION(BlueprintPure, Category = "Radar Manager")
	UW_Radar* GetRadarWidget() const { return RadarWidget; }

	/** [11/13] Calculate position on radar for world location
	 * @param WorldLocation - World position to convert
	 * @return 2D position on radar widget
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	FVector2D CalculateRadarPosition(FVector WorldLocation);
	virtual FVector2D CalculateRadarPosition_Implementation(FVector WorldLocation);

	/** [12/13] Initialize radar system */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void InitializeRadar();
	virtual void InitializeRadar_Implementation();

	/** [13/13] Late initialization after HUD is ready */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void LateInitialize();
	virtual void LateInitialize_Implementation();

	/** [14/13] Refresh cardinal positions based on camera rotation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radar Manager")
	void RefreshCardinals();
	virtual void RefreshCardinals_Implementation();

protected:
	/** Populate default cardinal data if not set from Blueprint */
	void PopulateDefaultCardinalData();
};
