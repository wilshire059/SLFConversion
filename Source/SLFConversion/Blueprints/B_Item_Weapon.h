// B_Item_Weapon.h
// C++ class for Blueprint B_Item_Weapon
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Item_Weapon.json
// Parent: B_Item_C -> AB_Item
// Variables: 7 | Functions: 8 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Item.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_Item_Weapon.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Item_Weapon : public AB_Item
{
	GENERATED_BODY()

public:
	AB_Item_Weapon();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|Trail")
	UNiagaraSystem* TrailEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|Trail")
	FName TrailWidthParameterName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config")
	UNiagaraSystem* EnvironmentImpactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|SFX")
	USoundBase* EnvironmentImpactSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|SFX")
	USoundBase* GuardSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|SFX")
	USoundBase* PerfectGuardSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "World Actor Config|Debug")
	bool DebugVisualizeTrace;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (8)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	FSLFWeaponAttackPower GetAttackPowerStats();
	virtual FSLFWeaponAttackPower GetAttackPowerStats_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	void GetWeaponStat(const FGameplayTag& StatTag, double& OutStatValue, double& OutStatValue1);
	virtual void GetWeaponStat_Implementation(const FGameplayTag& StatTag, double& OutStatValue, double& OutStatValue1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	TMap<FGameplayTag, FGameplayTag> GetWeaponStatScaling();
	virtual TMap<FGameplayTag, FGameplayTag> GetWeaponStatScaling_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	FSLFWeaponAttackPower GetWeaponAttackPowerData();
	virtual FSLFWeaponAttackPower GetWeaponAttackPowerData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	void TryApplyDamage(AActor* DamagedActor, const FHitResult& Hit, double AdditionalModifier);
	virtual void TryApplyDamage_Implementation(AActor* DamagedActor, const FHitResult& Hit, double AdditionalModifier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	double GetWeaponPoiseDamage();
	virtual double GetWeaponPoiseDamage_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Item_Weapon")
	TMap<FGameplayTag, UPrimaryDataAsset*> GetWeaponStatusEffectData();
	virtual TMap<FGameplayTag, UPrimaryDataAsset*> GetWeaponStatusEffectData_Implementation();
};
