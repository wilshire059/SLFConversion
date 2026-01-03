// AC_LootDropManager.h
// C++ component for AC_LootDropManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_LootDropManager.json
// Variables: 2 | Functions: 2 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_LootDropManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_LootDropManager_OnItemReadyForSpawn, FSLFLootItem, Item);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_LootDropManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_LootDropManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UDataTable> LootTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSLFLootItem OverrideItem;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_LootDropManager_OnItemReadyForSpawn OnItemReadyForSpawn;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LootDropManager")
	FSLFLootItem GetRandomItemFromTable(UDataTable* InLootTable);
	virtual FSLFLootItem GetRandomItemFromTable_Implementation(UDataTable* InLootTable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LootDropManager")
	bool IsOverrideItemValid();
	virtual bool IsOverrideItemValid_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_LootDropManager")
	void PickItem();
	virtual void PickItem_Implementation();
};
