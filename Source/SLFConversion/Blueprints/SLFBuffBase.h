// SLFBuffBase.h
// C++ base class for B_Buff
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Buff
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         3/3 migrated
// Functions:         2/2 migrated (excluding EventGraph)
// Event Dispatchers: 0/0
// Graphs:            3 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff
//
// PURPOSE: Base buff class - passive stat modifiers from equipment/talismans
// CHILDREN: B_Buff_AttackPower, equipment-specific buffs

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SLFBuffBase.generated.h"

// Forward declarations
class UDataAsset;
class UStatManagerComponent;
class UPDA_Buff;

/**
 * Buff rank data - stat multipliers per rank
 */
USTRUCT(BlueprintType)
struct FSLFBuffRankData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	int32 Rank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float Multiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	TMap<FGameplayTag, float> StatModifiers;
};

/**
 * Buff data struct - matches data in PDA_Buff
 */
USTRUCT(BlueprintType)
struct FSLFBuffData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	FGameplayTag BuffTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	TArray<FSLFBuffRankData> RankData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool bIsPassive = true;
};

/**
 * Base buff object - passive stat modifier
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBuffBase : public UObject
{
	GENERATED_BODY()

public:
	USLFBuffBase();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 3/3 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/3] Current buff rank (1-based) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	int32 Rank;

	/** [2/3] Buff data asset (PDA_Buff) - typed from JSON as PDA_Buff_C */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPDA_Buff* BuffData;

	/** [3/3] Owner actor */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* OwnerActor;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Get stat manager from owner actor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	UStatManagerComponent* GetOwnerStatManager();
	virtual UStatManagerComponent* GetOwnerStatManager_Implementation();

	/** [2/2] Get multiplier value for current rank
	 * @return Multiplier for current rank (defaults to 1.0)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	float GetMultiplierForCurrentRank();
	virtual float GetMultiplierForCurrentRank_Implementation();

	// --- Lifecycle (called by BuffManager) ---

	/** Apply buff effects to owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	void ApplyBuff();
	virtual void ApplyBuff_Implementation();

	/** Remove buff effects from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	void RemoveBuff();
	virtual void RemoveBuff_Implementation();

	/** Called when buff is first granted */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	void OnGranted();
	virtual void OnGranted_Implementation();

	/** Called when buff is removed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	void OnRemoved();
	virtual void OnRemoved_Implementation();

	/** Set buff rank and reapply */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff")
	void SetRank(int32 NewRank);
	virtual void SetRank_Implementation(int32 NewRank);

	// --- Getters ---

	UFUNCTION(BlueprintPure, Category = "Buff|Getters")
	int32 GetRank() const { return Rank; }

	UFUNCTION(BlueprintPure, Category = "Buff|Getters")
	UPDA_Buff* GetBuffData() const { return BuffData; }
};
