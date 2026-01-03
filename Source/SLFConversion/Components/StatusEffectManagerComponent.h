// StatusEffectManagerComponent.h
// C++ base class for AC_StatusEffectManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_StatusEffectManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 migrated (ActiveStatusEffects)
// Functions:         8/8 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 3/3 migrated
// Graphs:            8 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_StatusEffectManager
//
// PURPOSE: Manages status effects (poison, bleed, frostbite, etc.) buildup and triggers

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StatusEffectManagerComponent.generated.h"

// Forward declarations
class UDataAsset;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 3/3 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/3] Called when a status effect is added */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectAdded, UObject*, StatusEffect);

/** [2/3] Called when a status effect is removed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectRemoved, UDataAsset*, StatusEffect);

/** [3/3] Called when a status effect triggers (e.g., poison damage tick) - displays text */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectTextTriggered, FText, TriggeredText);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UStatusEffectManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusEffectManagerComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Active status effect instances by tag */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UObject*> ActiveStatusEffects;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] Broadcast when a status effect is added */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusEffectAdded OnStatusEffectAdded;

	/** [2/3] Broadcast when a status effect is removed */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusEffectRemoved OnStatusEffectRemoved;

	/** [3/3] Broadcast when a status effect triggers (with text) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatusEffectTextTriggered OnStatusEffectTextTriggered;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 8/8 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Query Functions (1) ---

	/** [1/8] Check if a status effect is currently active
	 * @param StatusEffectTag - Tag of the status effect to check
	 * @return True if effect is active
	 */
	UFUNCTION(BlueprintPure, Category = "Status Effect Manager|Query")
	bool IsStatusEffectActive(FGameplayTag StatusEffectTag) const;

	// --- Buildup Functions (3) ---

	/** [2/8] Add a one-shot buildup amount to a status effect
	 * @param StatusEffect - Status effect data asset (PDA_StatusEffect)
	 * @param EffectRank - Rank/potency of the effect
	 * @param Delta - Amount of buildup to add
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager|Buildup")
	void AddOneShotBuildup(UDataAsset* StatusEffect, int32 EffectRank, double Delta);
	virtual void AddOneShotBuildup_Implementation(UDataAsset* StatusEffect, int32 EffectRank, double Delta);

	/** [3/8] Start continuous buildup for a status effect
	 * @param StatusEffect - Status effect data asset
	 * @param Rank - Rank/potency
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager|Buildup")
	void StartBuildup(UDataAsset* StatusEffect, int32 Rank);
	virtual void StartBuildup_Implementation(UDataAsset* StatusEffect, int32 Rank);

	/** [4/8] Stop buildup for a status effect
	 * @param StatusEffect - Status effect data asset
	 * @param bApplyDecayDelay - Whether to apply decay delay before clearing
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager|Buildup")
	void StopBuildup(UDataAsset* StatusEffect, bool bApplyDecayDelay);
	virtual void StopBuildup_Implementation(UDataAsset* StatusEffect, bool bApplyDecayDelay);

	// --- Add/Trigger Functions (2) ---

	/** [5/8] Try to add a status effect to the character
	 * @param EffectClass - Status effect data asset (PDA_StatusEffect)
	 * @param Rank - Effect rank/potency
	 * @param bStartBuildup - Whether to start buildup immediately
	 * @param StartAmount - Initial buildup amount
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager|Add")
	void TryAddStatusEffect(UDataAsset* EffectClass, int32 Rank, bool bStartBuildup, double StartAmount);
	virtual void TryAddStatusEffect_Implementation(UDataAsset* EffectClass, int32 Rank, bool bStartBuildup, double StartAmount);

	/** [6/8] Called when a status effect finishes/expires
	 * @param StatusEffectTag - Tag of the finished effect
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager")
	void OnStatusEffectFinished(FGameplayTag StatusEffectTag);
	virtual void OnStatusEffectFinished_Implementation(FGameplayTag StatusEffectTag);

	/** [7/8] Called when a status effect triggers its effect
	 * @param TriggeredText - Display text for the trigger
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status Effect Manager")
	void OnStatusEffectTriggeredEvent(const FText& TriggeredText);
	virtual void OnStatusEffectTriggeredEvent_Implementation(const FText& TriggeredText);

	// --- Internal Callbacks (1) ---

	/** [8/8] Callback when async class load completes */
	UFUNCTION()
	void OnLoaded_185D3AEC4B5162C1F2C50C87BF007D3F(UClass* Loaded);
};
