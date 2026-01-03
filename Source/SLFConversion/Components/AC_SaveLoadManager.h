// AC_SaveLoadManager.h
// C++ component for AC_SaveLoadManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_SaveLoadManager.json
// Variables: 13 | Functions: 10 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_SaveLoadManager.generated.h"

// Forward declarations
class UAC_EquipmentManager;
class UAC_InventoryManager;
class UAC_ProgressManager;
class UAC_StatManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class USG_SaveSlots;
class USG_SoulslikeFramework;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_SaveLoadManager_OnSaveCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_SaveLoadManager_OnDataLoaded, FSLFSaveGameInfo, LoadedData);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_SaveLoadManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_SaveLoadManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// VARIABLES (13)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FString CurrentSaveSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FSLFSaveGameInfo Data;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_StatManager* StatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_InventoryManager* InventoryManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_EquipmentManager* EquipmentManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_ProgressManager* ProgressManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USG_SoulslikeFramework* SGO_Character;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USG_SaveSlots* SGOSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool AutoSaveNeeded;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool CanResave;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double AutosaveTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle AutosaveTimerHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<FInstancedStruct> DataCache;

	// EVENT DISPATCHERS (2)

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_SaveLoadManager_OnSaveCompleted OnSaveCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_SaveLoadManager_OnDataLoaded OnDataLoaded;

	// FUNCTIONS (10)

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void RefreshData();
	virtual void RefreshData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	FSLFSaveData GetDataByTag(const FGameplayTag& SaveGameTag);
	virtual FSLFSaveData GetDataByTag_Implementation(const FGameplayTag& SaveGameTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeAllDataForSaving();
	virtual void SerializeAllDataForSaving_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeDataForSaving(const FGameplayTag& SaveTag);
	virtual void SerializeDataForSaving_Implementation(const FGameplayTag& SaveTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeLevelData();
	virtual void SerializeLevelData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeClassData();
	virtual void SerializeClassData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void GetSavedLevelName(FName& OutLevelName, bool& OutSuccess, FName& OutLevelName_1, bool& OutSuccess_1);
	virtual void GetSavedLevelName_Implementation(FName& OutLevelName, bool& OutSuccess, FName& OutLevelName_1, bool& OutSuccess_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void GetSavedClass(UPrimaryDataAsset*& OutBaseCharacterClass, UPrimaryDataAsset*& OutBaseCharacterClass_1);
	virtual void GetSavedClass_Implementation(UPrimaryDataAsset*& OutBaseCharacterClass, UPrimaryDataAsset*& OutBaseCharacterClass_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeInteractableStates();
	virtual void SerializeInteractableStates_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager")
	void SerializeVendorData();
	virtual void SerializeVendorData_Implementation();
};
