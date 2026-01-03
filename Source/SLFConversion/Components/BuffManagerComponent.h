// BuffManagerComponent.h
// C++ base class for AC_BuffManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_BuffManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 migrated (Cache_Buff, ActiveBuffs)
// Functions:         8/8 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 1/1 migrated (OnBuffDetected)
// Graphs:            7 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_BuffManager
//
// PURPOSE: Manages active buffs on a character (stacking, removal, detection)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "BuffManagerComponent.generated.h"

// Forward declarations
class UDataAsset;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when a buff is added or removed
 * @param Buff - The buff data asset (PDA_Buff)
 * @param bAdded - True if buff was added, false if removed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffDetected, UDataAsset*, Buff, bool, bAdded);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UBuffManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffManagerComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Array of active buff instances (B_Buff objects) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<UObject*> ActiveBuffs;

	/** [2/2] Cached buff for async loading */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UObject* Cache_Buff;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Broadcast when a buff is detected (added or removed) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuffDetected OnBuffDetected;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 8/8 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Query Functions (1) ---

	/** [1/8] Get all buffs with a specific tag
	 * @param Tag - Gameplay tag to filter by
	 * @param OutBuffs - Array of matching buff objects
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Query")
	void GetBuffsWithTag(FGameplayTag Tag, TArray<UObject*>& OutBuffs);
	virtual void GetBuffsWithTag_Implementation(FGameplayTag Tag, TArray<UObject*>& OutBuffs);

	// --- Removal by Tag (2 functions) ---

	/** [2/8] Remove one buff with a specific tag
	 * @param Tag - Gameplay tag of buff to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Remove")
	void RemoveBuffWithTag(FGameplayTag Tag);
	virtual void RemoveBuffWithTag_Implementation(FGameplayTag Tag);

	/** [3/8] Remove all buffs with a specific tag
	 * @param Tag - Gameplay tag of buffs to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Remove")
	void RemoveAllBuffsWithTag(FGameplayTag Tag);
	virtual void RemoveAllBuffsWithTag_Implementation(FGameplayTag Tag);

	// --- Removal by Type (2 functions) ---

	/** [4/8] Remove one buff of a specific type
	 * @param Buff - Buff data asset (PDA_Buff) to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Remove")
	void RemoveBuffOfType(UDataAsset* Buff);
	virtual void RemoveBuffOfType_Implementation(UDataAsset* Buff);

	/** [5/8] Remove all buffs of a specific type
	 * @param Buff - Buff data asset (PDA_Buff) to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Remove")
	void RemoveAllBuffsOfType(UDataAsset* Buff);
	virtual void RemoveAllBuffsOfType_Implementation(UDataAsset* Buff);

	// --- Add/Apply Functions (2 functions) ---

	/** [6/8] Try to add a buff to the character
	 * Handles stacking logic and broadcasts OnBuffDetected
	 * @param Buff - Buff data asset (PDA_Buff) to add
	 * @param Rank - Buff rank/level (affects potency)
	 * @param bIsLoading - True if loading from save (skip some effects)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Add")
	void TryAddBuff(UDataAsset* Buff, int32 Rank, bool bIsLoading);
	virtual void TryAddBuff_Implementation(UDataAsset* Buff, int32 Rank, bool bIsLoading);

	/** [7/8] Delayed buff removal (for timed buffs)
	 * @param DelayDuration - Time before removal in seconds
	 * @param Buff - Buff data asset to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Remove")
	void DelayedRemoveBuff(float DelayDuration, UDataAsset* Buff);
	virtual void DelayedRemoveBuff_Implementation(float DelayDuration, UDataAsset* Buff);

	// --- Internal Callbacks (1 function) ---

	/** [8/8] Callback when async class load completes
	 * @param Loaded - The loaded class (B_Buff subclass)
	 */
	UFUNCTION()
	void OnLoaded_B4EA76A7416C76054179AE97AB1F528E(UClass* Loaded);
};
