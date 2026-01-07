// BFL_Helper.h
// C++ Blueprint Function Library
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/BFL_Helper.json
// Parent: UBlueprintFunctionLibrary (CRITICAL: Must be static functions)
// Functions: 27

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "BFL_Helper.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UAC_EquipmentManager;
class UAC_InventoryManager;
class UAC_SaveLoadManager;
class UAC_ProgressManager;
class UAC_StatManager;
class UW_HUD;

/**
 * Blueprint Function Library - Helper functions for SoulslikeFramework
 * All functions are static and callable from any Blueprint
 */
UCLASS()
class SLFCONVERSION_API UBFL_Helper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// INPUT HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get all keys mapped to a specific Input Action in an Input Mapping Context
	 *  NOTE: Output parameter named "MappedKeys" to match Blueprint pin name exactly
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Input", meta = (WorldContext = "WorldContextObject"))
	static void GetKeysForIA(UInputMappingContext* InputMapping, UInputAction* TargetIA, const UObject* WorldContextObject, UPARAM(DisplayName = "Mapped Keys") TArray<FKey>& MappedKeys);

	/** Get the Enhanced Input User Settings */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Input", meta = (WorldContext = "WorldContextObject"))
	static UEnhancedInputUserSettings* GetInputUserSettings(const UObject* WorldContextObject);

	// ═══════════════════════════════════════════════════════════════════════
	// STAT HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get the current value from a StatInfo struct */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Stats", meta = (WorldContext = "WorldContextObject"))
	static void GetStatCurrentValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, UPARAM(DisplayName = "Current Value") double& CurrentValue);

	/** Get the max value from a StatInfo struct */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Stats", meta = (WorldContext = "WorldContextObject"))
	static void GetStatMaxValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, UPARAM(DisplayName = "Max Value") double& MaxValue);

	/** Get the percent value (current/max) from a StatInfo struct */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Stats", meta = (WorldContext = "WorldContextObject"))
	static void GetStatPercentValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, UPARAM(DisplayName = "Percent Value") double& PercentValue);

	/** Remove duplicate categories from an array */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Stats", meta = (WorldContext = "WorldContextObject"))
	static TArray<ESLFStatCategory> RemoveDuplicateCategories(const TArray<ESLFStatCategory>& Array, const UObject* WorldContextObject);

	/** Parse affected stats map to a formatted string */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Stats", meta = (WorldContext = "WorldContextObject"))
	static FString ParseAffectedStats(const TMap<FGameplayTag, FGameplayTag>& AffectedStats, const UObject* WorldContextObject);

	// ═══════════════════════════════════════════════════════════════════════
	// TAG/STRING HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Parse the last segment of a gameplay tag as display text */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Tags", meta = (WorldContext = "WorldContextObject"))
	static FText ParseOutTagName(const FGameplayTag& Tag, bool Shorten, int32 ShortenedLen, const UObject* WorldContextObject);

	/** Check if an actor has any of the specified tags */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Tags", meta = (WorldContext = "WorldContextObject"))
	static bool ActorTagMatches(AActor* Target, const TArray<FName>& Tags, const UObject* WorldContextObject);

	/** Compare two strings for equality */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Strings", meta = (WorldContext = "WorldContextObject"))
	static bool CompareStrings(const FString& String1, const FString& String2, const UObject* WorldContextObject);

	/** Parse a string to a specific length, returning both parsed and remaining portions */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Strings", meta = (WorldContext = "WorldContextObject"))
	static void ParseStringLength(const FString& String, int32 Length, const UObject* WorldContextObject, FString& OutParsedString, FString& OutRemainder);

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENT ACCESSORS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get Equipment Component from a controller's pawn */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Components", meta = (WorldContext = "WorldContextObject"))
	static void GetEquipmentComponent(AController* Target, const UObject* WorldContextObject, UAC_EquipmentManager*& OutEquipmentManager);

	/** Get Inventory Component from a controller's pawn */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Components", meta = (WorldContext = "WorldContextObject"))
	static void GetInventoryComponent(AController* Target, const UObject* WorldContextObject, UAC_InventoryManager*& OutInventoryManager);

	/** Get Save/Load Component from a controller's pawn */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Components", meta = (WorldContext = "WorldContextObject"))
	static void GetSaveLoadComponent(AController* Target, const UObject* WorldContextObject, UAC_SaveLoadManager*& OutSaveLoadManager);

	/** Get Progress Manager from the local player controller */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Components", meta = (WorldContext = "WorldContextObject"))
	static UAC_ProgressManager* GetLocalProgressManager(const UObject* WorldContextObject);

	// ═══════════════════════════════════════════════════════════════════════
	// MOVEMENT/POSITION HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Check if velocity indicates 2D movement above a threshold */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Movement", meta = (WorldContext = "WorldContextObject"))
	static bool GetIsMoving2D(const FVector& Velocity, double SpeedToCheck, const UObject* WorldContextObject);

	/** Check if an actor is behind another actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Movement", meta = (WorldContext = "WorldContextObject"))
	static bool GetIsBehindTarget(AActor* SelfActor, AActor* TargetActor, double PrecisionTolerance, const UObject* WorldContextObject);

	/** Get transform of a location actor by tag */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Location", meta = (WorldContext = "WorldContextObject"))
	static void GetLocationActorTransform(const FGameplayTag& LocationTag, const UObject* WorldContextObject, bool& OutSuccess, FTransform& OutTransform);

	// ═══════════════════════════════════════════════════════════════════════
	// COMBAT HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Calculate weapon damage with scaling factors */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Combat", meta = (WorldContext = "WorldContextObject"))
	static double CalculateWeaponDamage(const FSLFWeaponAttackPower& AttackPowerStats, double AdditionalModifier, const TMap<FGameplayTag, FGameplayTag>& ScalingFactor, UAC_StatManager* StatManager, AActor* DamagedActor, const UObject* WorldContextObject);

	/** Check if an enemy actor is dead */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Combat", meta = (WorldContext = "WorldContextObject"))
	static bool IsEnemyDead(AActor* Enemy, const UObject* WorldContextObject);

	/** Get hit direction from a hit result
	 *  NOTE: Output parameter named "HitDirection" to match Blueprint pin name exactly
	 */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Combat", meta = (WorldContext = "WorldContextObject"))
	static void GetDirectionFromHit(AActor* OwnerActor, const FHitResult& HitInfo, const UObject* WorldContextObject, UPARAM(DisplayName = "HitDirection") ESLFDirection& HitDirection);

	/** Get death tag based on hit direction */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Combat", meta = (WorldContext = "WorldContextObject"))
	static void GetDeathTagFromDirection(AActor* OwnerActor, const FHitResult& HitInfo, const UObject* WorldContextObject, FGameplayTag& OutDeathTag);

	// ═══════════════════════════════════════════════════════════════════════
	// UI HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Send an interaction error message to the player's HUD */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|UI", meta = (WorldContext = "WorldContextObject"))
	static void SendErrorMessage(AActor* PlayerActor, const FText& Message, double Time, const UObject* WorldContextObject);

	/** Try to get HUD widget for an actor */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|UI", meta = (WorldContext = "WorldContextObject"))
	static void TryGetHudForActor(AActor* Actor, const UObject* WorldContextObject, bool& OutSuccess, UW_HUD*& OutHUD);

	// ═══════════════════════════════════════════════════════════════════════
	// SAVE/LOAD HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get a unique save slot name for a character class */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|SaveGame", meta = (WorldContext = "WorldContextObject"))
	static FString GetUniqueSlotName(UPrimaryDataAsset* CharacterClass, int32 CheckDepth, const UObject* WorldContextObject);

	/** Get save data by tag from a save game info struct */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|SaveGame", meta = (WorldContext = "WorldContextObject"))
	static void GetSaveDataByTag(const FSLFSaveGameInfo& Data, const FGameplayTag& SaveGameTag, const UObject* WorldContextObject, bool& OutSuccess, FSLFSaveData& OutSaveData);

	// ═══════════════════════════════════════════════════════════════════════
	// CRAFTING HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Unlock craftable items in the progress manager */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Crafting", meta = (WorldContext = "WorldContextObject"))
	static void UnlockCraftableItems(const TArray<UPrimaryDataAsset*>& Items, const UObject* WorldContextObject);

	// ═══════════════════════════════════════════════════════════════════════
	// CLASS CHECKING (replaces BML_HelperMacros)
	// ═══════════════════════════════════════════════════════════════════════

	/** Check if Target's class is a child of the specified ParentClass
	 *  Replaces: BML_HelperMacros.IsClassChild (note: BP had typo "IsClassChlid")
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Class")
	static bool IsClassChild(UObject* Target, UClass* ParentClass);

	/** Check if Target's class is exactly equal to the specified Class
	 *  Replaces: BML_HelperMacros.IsClass
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Class")
	static bool IsClassEqual(UObject* Target, UClass* ClassToCheck);

	// ═══════════════════════════════════════════════════════════════════════
	// ARRAY HELPERS (replaces BML_HelperMacros)
	// ═══════════════════════════════════════════════════════════════════════

	/** Check if an array of objects is valid (not empty)
	 *  Replaces: BML_HelperMacros.IsValidArray
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Array")
	static bool IsValidObjectArray(const TArray<UObject*>& Array);

	/** Check if an array of integers is valid (not empty) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Array")
	static bool IsValidIntArray(const TArray<int32>& Array);

	/** Check if an array of gameplay tags is valid (not empty) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Array")
	static bool IsValidTagArray(const TArray<FGameplayTag>& Array);

	// ═══════════════════════════════════════════════════════════════════════
	// TIMER/COOLDOWN HELPERS (replaces BML_HelperMacros)
	// ═══════════════════════════════════════════════════════════════════════

	/** Start a cooldown timer on an object
	 *  Returns true if cooldown was started, false if already on cooldown
	 *  Replaces: BML_HelperMacros.DoCooldown
	 */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Cooldown", meta = (WorldContext = "WorldContextObject"))
	static bool StartCooldown(UObject* Object, FName CooldownKey, float Duration, const UObject* WorldContextObject);

	/** Check if an object is currently on cooldown */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BFL_Helper|Cooldown", meta = (WorldContext = "WorldContextObject"))
	static bool IsOnCooldown(UObject* Object, FName CooldownKey, const UObject* WorldContextObject);

	/** Clear a cooldown for an object */
	UFUNCTION(BlueprintCallable, Category = "BFL_Helper|Cooldown", meta = (WorldContext = "WorldContextObject"))
	static void ClearCooldown(UObject* Object, FName CooldownKey, const UObject* WorldContextObject);

private:
	/** Internal cooldown tracking - maps Object+Key to end time */
	static TMap<TPair<TWeakObjectPtr<UObject>, FName>, double> CooldownMap;
};
