// DebugCentralComponent.h
// C++ base class for AC_DebugCentral
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_DebugCentral
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         16/16 migrated
// Functions:         3/3 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 0/0
// Graphs:            1 (EventGraph with tick-based key detection)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_DebugCentral
//
// PURPOSE: Debug overlay that shows component references and state info via HUD

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "DebugCentralComponent.generated.h"

// Forward declarations
class UUserWidget;

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UDebugCentralComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDebugCentralComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 16/16 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Component References (12) - Cached from owner character ---

	/** [1/16] StatusEffectManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* StatusEffectComponent;

	/** [2/16] StatManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* StatComponent;

	/** [3/16] CombatManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* CombatComponent;

	/** [4/16] ActionManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* ActionComponent;

	/** [5/16] InteractionManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* InteractionComponent;

	/** [6/16] BuffManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* BuffComponent;

	/** [7/16] InputBuffer component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* InputComponent;

	/** [8/16] EquipmentManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* EquipmentComponent;

	/** [9/16] InventoryManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* InventoryComponent;

	/** [10/16] RadarManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* RadarComponent;

	/** [11/16] ProgressManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* ProgressComponent;

	/** [12/16] SaveLoadManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* SaveLoadComponent;

	// --- Config Variables (2) ---

	/** [13/16] Master toggle for debug functionality */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bEnableDebugging;

	/** [14/16] Key to toggle debug HUD visibility */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FKey ToggleKey;

	// --- Runtime Variables (2) ---

	/** [15/16] Debug HUD widget instance */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UUserWidget* HUD_Debug;

	/** [16/16] Map of tags to component references for lookup */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UActorComponent*> Components;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] Late initialization - called via timer after BeginPlay
	 * Caches all component references from owner
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Debug Central")
	void LateInitialize();
	virtual void LateInitialize_Implementation();

	// [2/3] ReceiveBeginPlay -> BeginPlay override
	// [3/3] ReceiveTick -> TickComponent override (checks ToggleKey input)
};
