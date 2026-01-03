// AC_DebugCentral.h
// C++ component for AC_DebugCentral
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_DebugCentral.json
// Variables: 16 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_DebugCentral.generated.h"

// Forward declarations
class UAC_ActionManager;
class UAC_BuffManager;
class UAC_CombatManager;
class UAC_EquipmentManager;
class UAC_InputBuffer;
class UAC_InteractionManager;
class UAC_InventoryManager;
class UAC_ProgressManager;
class UAC_RadarManager;
class UAC_SaveLoadManager;
class UAC_StatManager;
class UAC_StatusEffectManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UW_Debug_HUD;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_DebugCentral : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_DebugCentral();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (16)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_StatusEffectManager* StatusEffectComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_StatManager* StatComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_CombatManager* CombatComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_ActionManager* ActionComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_InteractionManager* InteractionComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_BuffManager* BuffComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_InputBuffer* InputComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_EquipmentManager* EquipmentComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_InventoryManager* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_RadarManager* RadarComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_ProgressManager* ProgressComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_SaveLoadManager* SaveLoadComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	bool EnableDebugging;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FKey ToggleKey;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UW_Debug_HUD* HUD_Debug;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> Components;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * LateInitialize - Called via timer from BeginPlay to cache component refs
	 * Blueprint: Custom Event "LateInitialize"
	 *
	 * Checks if owner is B_Soulslike_Character, then caches all manager components
	 * and populates the Components map for debug access
	 */
	UFUNCTION(BlueprintCallable, Category = "AC_DebugCentral")
	void LateInitialize();

protected:
	// Helper functions
	void CacheComponentReferences();
	void PopulateComponentsMap();

	/** Timer handle for LateInitialize call */
	FTimerHandle LateInitializeTimer;
};
