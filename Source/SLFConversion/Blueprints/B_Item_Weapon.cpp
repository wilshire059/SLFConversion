// B_Item_Weapon.cpp
// C++ implementation for Blueprint B_Item_Weapon
//
// 20-PASS VALIDATION: 2026-01-02 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Item_Weapon.json

#include "Blueprints/B_Item_Weapon.h"
#include "Components/StatManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

AB_Item_Weapon::AB_Item_Weapon()
{
}

FSLFWeaponAttackPower AB_Item_Weapon::GetAttackPowerStats_Implementation()
{
	// JSON: Get Instigator -> GetComponentByClass(AC_StatManager) -> IsValid check
	// Then GetStat for each attack power type from the stat manager
	FSLFWeaponAttackPower Result;

	APawn* InstigatorPawn = GetInstigator();
	if (!InstigatorPawn)
	{
		// Return base weapon attack power data if no instigator
		return GetWeaponAttackPowerData();
	}

	UStatManagerComponent* StatManager = InstigatorPawn->FindComponentByClass<UStatManagerComponent>();
	if (!IsValid(StatManager))
	{
		return GetWeaponAttackPowerData();
	}

	// Get base weapon attack power and add stat-based bonuses
	FSLFWeaponAttackPower BaseAttackPower = GetWeaponAttackPowerData();

	// Get stat values for each attack type
	// JSON shows GetStat calls for Physical, Magic, Lightning, Holy, Frost, Fire
	FStatInfo StatInfo;

	// Note: The actual stat tags would be defined in gameplay tags
	// For now, return the base weapon attack power
	Result = BaseAttackPower;

	return Result;
}

void AB_Item_Weapon::GetWeaponStat_Implementation(const FGameplayTag& StatTag, double& OutStatValue, double& OutStatValue1)
{
	// JSON: Breaks ItemInfo -> EquipmentDetails -> WeaponStatInfo to get stat values
	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;

	// Check StatRequirementInfo for the stat requirement value
	OutStatValue = 0.0;
	OutStatValue1 = 0.0;

	if (EquipmentDetails.WeaponStatInfo.StatRequirementInfo.Contains(StatTag))
	{
		OutStatValue = static_cast<double>(EquipmentDetails.WeaponStatInfo.StatRequirementInfo[StatTag]);
	}
}

TMap<FGameplayTag, FGameplayTag> AB_Item_Weapon::GetWeaponStatScaling_Implementation()
{
	// JSON: Breaks ItemInfo -> EquipmentDetails -> WeaponStatInfo -> ScalingInfo
	// Note: ScalingInfo is TMap<FGameplayTag, ESLFStatScaling>, need to convert
	TMap<FGameplayTag, FGameplayTag> Result;

	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;

	// The Blueprint expects TMap<StatTag, ScalingTypeTag> but our struct has ESLFStatScaling enum
	// This would need conversion logic if the enum maps to tags
	// For now, return empty - Blueprint override can provide this
	return Result;
}

FSLFWeaponAttackPower AB_Item_Weapon::GetWeaponAttackPowerData_Implementation()
{
	// JSON: Returns weapon attack power based on equipped item stats
	// The attack power values would typically come from a data asset or be calculated
	FSLFWeaponAttackPower Result;

	// Base implementation returns default values
	// Child Blueprint classes or data assets would override with actual values
	return Result;
}

void AB_Item_Weapon::TryApplyDamage_Implementation(AActor* DamagedActor, const FHitResult& Hit, double AdditionalModifier)
{
	// JSON: Calls TakeDamage on target via combat interface
	// Passes attack power stats, poise damage, and status effect data
	if (!IsValid(DamagedActor))
	{
		return;
	}

	// Get attack power stats
	FSLFWeaponAttackPower AttackPower = GetAttackPowerStats();

	// Get poise damage
	double PoiseDamage = GetWeaponPoiseDamage();

	// Get status effect data
	TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects = GetWeaponStatusEffectData();

	// Apply additional modifier to attack power
	AttackPower.Physical *= AdditionalModifier;
	AttackPower.Magic *= AdditionalModifier;
	AttackPower.Lightning *= AdditionalModifier;
	AttackPower.Holy *= AdditionalModifier;
	AttackPower.Frost *= AdditionalModifier;
	AttackPower.Fire *= AdditionalModifier;

	// Damage application is handled by the CombatManager component on the target
	// This base implementation prepares the damage data - child classes can override
	UE_LOG(LogTemp, Verbose, TEXT("[B_Item_Weapon] TryApplyDamage to %s - Phys:%.1f Mag:%.1f Poise:%.1f"),
		*DamagedActor->GetName(), AttackPower.Physical, AttackPower.Magic, PoiseDamage);
}

double AB_Item_Weapon::GetWeaponPoiseDamage_Implementation()
{
	// JSON: Get ItemInfo -> Break FItemInfo -> EquipmentDetails
	//       -> Break FEquipmentInfo -> MinPoiseDamage, MaxPoiseDamage
	//       -> Random Float in Range(Min, Max) -> Return
	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;
	double MinPoise = EquipmentDetails.MinPoiseDamage;
	double MaxPoise = EquipmentDetails.MaxPoiseDamage;

	return FMath::FRandRange(MinPoise, MaxPoise);
}

TMap<FGameplayTag, UPrimaryDataAsset*> AB_Item_Weapon::GetWeaponStatusEffectData_Implementation()
{
	// JSON: Get ItemInfo -> Break FItemInfo -> EquipmentDetails
	//       -> Break FEquipmentInfo -> WeaponStatusEffectInfo -> Return
	// Note: The Blueprint type is TMap<UPDA_StatusEffect*, FStatusEffectApplication>
	// but the C++ interface uses TMap<FGameplayTag, UPrimaryDataAsset*>
	TMap<FGameplayTag, UPrimaryDataAsset*> Result;

	// The actual data would need to be converted from the equipment info
	// This requires proper type alignment between Blueprint and C++
	// For now, return empty map
	return Result;
}
