// AC_SaveLoadManager.h
// C++ component for AC_SaveLoadManager
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Component/AC_SaveLoadManager.json
// Variables: 13 | Functions: 10 | Events: 9 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"

#include "AC_SaveLoadManager.generated.h"

// Forward declarations
class UAC_EquipmentManager;
class UAC_InventoryManager;
class UAC_ProgressManager;
class UAC_StatManager;
class USG_SaveSlots;
class USG_SoulslikeFramework;
class AB_Interactable;
class USaveGame;

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

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (13)
	// ═══════════════════════════════════════════════════════════════════════

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
	float AutosaveTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle AutosaveTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<FInstancedStruct> DataCache;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_SaveLoadManager_OnSaveCompleted OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_SaveLoadManager_OnDataLoaded OnDataLoaded;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (10)
	// ═══════════════════════════════════════════════════════════════════════

	/** Refresh cached save data from save game object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Data")
	void RefreshData();
	virtual void RefreshData_Implementation();

	/** Get save data for a specific gameplay tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Data")
	FSLFSaveData GetDataByTag(const FGameplayTag& SaveGameTag);
	virtual FSLFSaveData GetDataByTag_Implementation(const FGameplayTag& SaveGameTag);

	/** Serialize all game data for saving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	void SerializeAllDataForSaving();
	virtual void SerializeAllDataForSaving_Implementation();

	/** Serialize specific data category by tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	void SerializeDataForSaving(const FGameplayTag& SaveTag);
	virtual void SerializeDataForSaving_Implementation(const FGameplayTag& SaveTag);

	/** Serialize current level information (level name, player transform) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	TArray<FInstancedStruct> SerializeLevelData();
	virtual TArray<FInstancedStruct> SerializeLevelData_Implementation();

	/** Serialize character class information */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	TArray<FInstancedStruct> SerializeClassData();
	virtual TArray<FInstancedStruct> SerializeClassData_Implementation();

	/** Get the saved level name from save data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Data")
	void GetSavedLevelName(FName& OutLevelName, bool& OutSuccess);
	virtual void GetSavedLevelName_Implementation(FName& OutLevelName, bool& OutSuccess);

	/** Get the saved character class from save data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Data")
	UPrimaryDataAsset* GetSavedClass();
	virtual UPrimaryDataAsset* GetSavedClass_Implementation();

	/** Serialize states of all interactable objects in the world */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	TArray<FInstancedStruct> SerializeInteractableStates();
	virtual TArray<FInstancedStruct> SerializeInteractableStates_Implementation();

	/** Serialize vendor inventory states */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Serialization")
	TArray<FInstancedStruct> SerializeVendorData();
	virtual TArray<FInstancedStruct> SerializeVendorData_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// EVENTS (9) - Custom Events from Blueprint
	// ═══════════════════════════════════════════════════════════════════════

	/** Add data to save cache for a specific tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventAddToSaveData(const FGameplayTag& SaveGameTag, const FInstancedStruct& DataToSave);
	virtual void EventAddToSaveData_Implementation(const FGameplayTag& SaveGameTag, const FInstancedStruct& DataToSave);

	/** Update existing save data for a specific tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventUpdateSaveData(const FGameplayTag& SaveGameTag, const FSLFSaveData& SaveData);
	virtual void EventUpdateSaveData_Implementation(const FGameplayTag& SaveGameTag, const FSLFSaveData& SaveData);

	/** Remove spawned actor data by GUID */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventRemoveSpawnedActorFromSaveData(const FGuid& Guid);
	virtual void EventRemoveSpawnedActorFromSaveData_Implementation(const FGuid& Guid);

	/** Attempt to autosave if conditions are met */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventAttemptAutosave();
	virtual void EventAttemptAutosave_Implementation();

	/** Start the autosave timer */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventStartAutosaveTimer();
	virtual void EventStartAutosaveTimer_Implementation();

	/** Try to preload save data from slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventTryPreloadData();
	virtual void EventTryPreloadData_Implementation();

	/** Apply loaded data to game state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventSetLoadedData(const FSLFSaveGameInfo& LoadedData);
	virtual void EventSetLoadedData_Implementation(const FSLFSaveGameInfo& LoadedData);

	/** Save current data to the active slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventSaveData();
	virtual void EventSaveData_Implementation();

	/** Update the active save slot */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_SaveLoadManager|Events")
	void EventUpdateActiveSlot(const FString& NewSlotName);
	virtual void EventUpdateActiveSlot_Implementation(const FString& NewSlotName);

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

protected:
	/** Called when async save completes */
	void OnAsyncSaveCompleted(const FString& SlotName, int32 UserIndex, bool bSuccess);

	/** Called when async load completes */
	void OnAsyncLoadCompleted(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame);

	/** Autosave timer callback */
	void OnAutosaveTimerTick();

	/** Get player controller from owner */
	APlayerController* GetOwnerPlayerController() const;

	/** Get player pawn from owner */
	APawn* GetOwnerPawn() const;
};
