// SaveLoadManagerComponent.h
// C++ base class for AC_SaveLoadManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_SaveLoadManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 migrated (excluding 2 dispatchers counted separately)
// Functions:         23/23 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 2/2 migrated
// Graphs:            15 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_SaveLoadManager
//
// PURPOSE: Save/load system - manages save slots, serialization, autosave

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SaveLoadManagerComponent.generated.h"

// Forward declarations
class UStatManagerComponent;
class UInventoryManagerComponent;
class UEquipmentManagerComponent;
class UProgressManagerComponent;
class USaveGame;

// Types used from SLFGameTypes.h:
// - FSLFSaveGameInfo (defined below as it's the main struct for this component)

// Note: Keep this definition local as it's the authoritative definition for the save system
// Other components should use this header for FSLFSaveGameInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSaveGameInfoLocal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTimespan PlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform SpawnTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> StatsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> InventoryData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> EquipmentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> ProgressData;
};

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 2/2 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/2] Called when save operation completes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveCompleted);

/** [2/2] Called when data is loaded from save */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDataLoaded, const FSLFSaveGameInfo&, LoadedData);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API USaveLoadManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USaveLoadManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config (1) ---

	/** [1/13] Autosave interval in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float AutosaveTimer;

	// --- Runtime State (8) ---

	/** [2/13] Current save slot name */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString CurrentSaveSlot;

	/** [3/13] Current save data */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FSLFSaveGameInfo SaveData;

	/** [4/13] Whether autosave is needed */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bAutoSaveNeeded;

	/** [5/13] Whether resave is allowed */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bCanResave;

	/** [6/13] Autosave timer handle */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FTimerHandle AutosaveTimerHandle;

	/** [7/13] Cached data for pending save */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FInstancedStruct> DataCache;

	// --- Component References (6) ---

	/** [8/13] StatManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UStatManagerComponent* StatManager;

	/** [9/13] InventoryManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* InventoryManager;

	/** [10/13] EquipmentManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UActorComponent* EquipmentManager;

	/** [11/13] ProgressManager component reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime|Components")
	UProgressManagerComponent* ProgressManager;

	/** [12/13] Character save game object */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	USaveGame* SGO_Character;

	/** [13/13] Slots save game object */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	USaveGame* SGO_Slots;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Broadcast when save completes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSaveCompleted OnSaveCompleted;

	/** [2/2] Broadcast when data is loaded */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDataLoaded OnDataLoaded;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 23/23 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Save Operations (6) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void SaveToSlot(const FString& SlotName);
	virtual void SaveToSlot_Implementation(const FString& SlotName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void SaveToCheckpoint();
	virtual void SaveToCheckpoint_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void AutoSave();
	virtual void AutoSave_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void SerializeAllData();
	virtual void SerializeAllData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void AddToSaveData(FGameplayTag DataTag, const FInstancedStruct& Data);
	virtual void AddToSaveData_Implementation(FGameplayTag DataTag, const FInstancedStruct& Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Save")
	void UpdateSaveData(FGameplayTag DataTag);
	virtual void UpdateSaveData_Implementation(FGameplayTag DataTag);

	// --- Load Operations (5) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Load")
	void LoadFromSlot(const FString& SlotName);
	virtual void LoadFromSlot_Implementation(const FString& SlotName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Load")
	void LoadDataFromSave();
	virtual void LoadDataFromSave_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Load")
	void InitializeLoadedData();
	virtual void InitializeLoadedData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Load")
	void ApplyLoadedData();
	virtual void ApplyLoadedData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Load")
	bool DoesSaveExist(const FString& SlotName);
	virtual bool DoesSaveExist_Implementation(const FString& SlotName);

	// --- Slot Management (5) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Slots")
	void CreateNewSlot(const FString& SlotName);
	virtual void CreateNewSlot_Implementation(const FString& SlotName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Slots")
	void DeleteSlot(const FString& SlotName);
	virtual void DeleteSlot_Implementation(const FString& SlotName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Slots")
	void GetAllSlots(TArray<FString>& OutSlots);
	virtual void GetAllSlots_Implementation(TArray<FString>& OutSlots);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Slots")
	void SetActiveSlot(const FString& SlotName);
	virtual void SetActiveSlot_Implementation(const FString& SlotName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Slots")
	FString GetActiveSlot();
	virtual FString GetActiveSlot_Implementation();

	// --- Autosave Control (4) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Autosave")
	void StartAutosaveTimer();
	virtual void StartAutosaveTimer_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Autosave")
	void StopAutosaveTimer();
	virtual void StopAutosaveTimer_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Autosave")
	void OnAutosaveTimerTick();
	virtual void OnAutosaveTimerTick_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load|Autosave")
	void SetAutoSaveNeeded(bool bNeeded);
	virtual void SetAutoSaveNeeded_Implementation(bool bNeeded);

	// --- Initialization (3) ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load")
	void Initialize();
	virtual void Initialize_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load")
	void InitializeNewGame();
	virtual void InitializeNewGame_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Load")
	void CacheComponentReferences();
	virtual void CacheComponentReferences_Implementation();

	// ═══════════════════════════════════════════════════════════════════
	// ADDITIONAL EVENTS
	// ═══════════════════════════════════════════════════════════════════

	/** Attempt autosave if needed */
	UFUNCTION(BlueprintCallable, Category = "Save Load|Autosave")
	void EventAttemptAutosave();

	/** Check if save data is valid */
	UFUNCTION(BlueprintPure, Category = "Save Load")
	bool IsSaveDataValid() const;

	/** Set loaded data from save file */
	UFUNCTION(BlueprintCallable, Category = "Save Load")
	void EventSetLoadedData(USaveGame* InSaveGame);

	/** Load data asynchronously */
	UFUNCTION(BlueprintCallable, Category = "Save Load")
	void LoadDataAsync(const FString& SlotName);

	/** Try to preload data for a slot */
	UFUNCTION(BlueprintCallable, Category = "Save Load")
	void EventTryPreloadData(int32 SlotIndex);

	/** Update the active save slot */
	UFUNCTION(BlueprintCallable, Category = "Save Load")
	void EventUpdateActiveSlot(int32 SlotIndex);

	/** Remove a spawned actor from save data */
	UFUNCTION(BlueprintCallable, Category = "Save Load")
	void EventRemoveSpawnedActorFromSaveData(AActor* Actor);
};
