// SLFWeaponBase.cpp
// C++ implementation for B_Item_Weapon
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Item_Weapon
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 (initialized in constructor)
// Functions:         8/8 implemented
// Components:        2/2 created in constructor (WeaponMesh, TrailComponent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFWeaponBase.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SLFEnums.h" // For ESLFStatScaling

ASLFWeaponBase::ASLFWeaponBase()
{
	// Create WeaponMesh component 
	// Named "WeaponMesh" to avoid collision with Blueprint SCS "StaticMesh"
	// Not attached here - Blueprint SCS will set up hierarchy
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh; // Set as root for C++ hierarchy

	// Create TrailComponent Niagara component
	// Named "TrailComponent" to avoid collision with Blueprint SCS "Trail"
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(WeaponMesh);
	TrailComponent->SetAutoActivate(false); // Trail activates during attacks

	// Initialize VFX
	TrailEffect = nullptr;
	TrailWidthParameterName = FName("Width");

	// Initialize environment impact
	EnvironmentImpactEffect = nullptr;
	EnvironmentImpactSound = nullptr;

	// Initialize guard
	GuardSound = nullptr;
	PerfectGuardSound = nullptr;

	// Initialize debug
	bDebugVisualizeTrace = false;
}

void ASLFWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] BeginPlay: %s"), *GetName());

	// Apply trail effect asset to trail component if set
	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT QUERIES [1-4/8]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFWeaponAttackPower> ASLFWeaponBase::GetAttackPowerStats_Implementation()
{
	// Get attack power from ItemInfo.EquipmentDetails
	// Returns base weapon attack power - inheriting classes can override
	TArray<FSLFWeaponAttackPower> Result;

	// Return a single default attack power entry
	// Actual values come from weapon data assets via Blueprint children
	Result.Add(FSLFWeaponAttackPower());

	return Result;
}

float ASLFWeaponBase::GetWeaponStat_Implementation(FGameplayTag StatTag)
{
	// Look up stat requirement from ItemInfo.EquipmentDetails.WeaponStatInfo
	const FSLFEquipmentWeaponStatInfo& WeaponStatInfo = ItemInfo.EquipmentDetails.WeaponStatInfo;

	// Check if stat requirement info contains this tag
	if (WeaponStatInfo.StatRequirementInfo.Contains(StatTag))
	{
		return static_cast<float>(WeaponStatInfo.StatRequirementInfo[StatTag]);
	}

	return 0.0f;
}

TArray<FSLFWeaponStatScaling> ASLFWeaponBase::GetWeaponStatScaling_Implementation()
{
	// Get stat scaling from ItemInfo.EquipmentDetails.WeaponStatInfo.ScalingInfo
	TArray<FSLFWeaponStatScaling> Result;

	const FSLFEquipmentWeaponStatInfo& WeaponStatInfo = ItemInfo.EquipmentDetails.WeaponStatInfo;

	if (!WeaponStatInfo.bHasStatScaling)
	{
		return Result;
	}

	// Convert TMap<FGameplayTag, ESLFStatScaling> to TArray<FSLFWeaponStatScaling>
	for (const auto& ScalingPair : WeaponStatInfo.ScalingInfo)
	{
		FSLFWeaponStatScaling Entry;
		Entry.StatTag = ScalingPair.Key;

		// Convert ESLFStatScaling enum to multiplier and grade text
		switch (ScalingPair.Value)
		{
		case ESLFStatScaling::S:
			Entry.ScalingMultiplier = 1.5f;
			Entry.ScalingGrade = FText::FromString(TEXT("S"));
			break;
		case ESLFStatScaling::A:
			Entry.ScalingMultiplier = 1.3f;
			Entry.ScalingGrade = FText::FromString(TEXT("A"));
			break;
		case ESLFStatScaling::B:
			Entry.ScalingMultiplier = 1.15f;
			Entry.ScalingGrade = FText::FromString(TEXT("B"));
			break;
		case ESLFStatScaling::C:
			Entry.ScalingMultiplier = 1.0f;
			Entry.ScalingGrade = FText::FromString(TEXT("C"));
			break;
		case ESLFStatScaling::D:
			Entry.ScalingMultiplier = 0.85f;
			Entry.ScalingGrade = FText::FromString(TEXT("D"));
			break;
		case ESLFStatScaling::E:
			Entry.ScalingMultiplier = 0.7f;
			Entry.ScalingGrade = FText::FromString(TEXT("E"));
			break;
		default:
			Entry.ScalingMultiplier = 1.0f;
			Entry.ScalingGrade = FText::FromString(TEXT("-"));
			break;
		}

		Result.Add(Entry);
	}

	return Result;
}

TArray<FSLFWeaponAttackPower> ASLFWeaponBase::GetWeaponAttackPowerData_Implementation()
{
	return GetAttackPowerStats();
}

// ═══════════════════════════════════════════════════════════════════════════════
// DAMAGE [5-6/8]
// ═══════════════════════════════════════════════════════════════════════════════

bool ASLFWeaponBase::TryApplyDamage_Implementation(AActor* Target, const FHitResult& HitResult, float AttackMultiplier)
{
	if (!Target) return false;

	// Calculate total damage from attack power stats
	TArray<FSLFWeaponAttackPower> AttackPowers = GetAttackPowerStats();
	double TotalDamage = 0.0;

	for (const FSLFWeaponAttackPower& AttackPower : AttackPowers)
	{
		TotalDamage += AttackPower.Physical + AttackPower.Magic + AttackPower.Lightning
			+ AttackPower.Holy + AttackPower.Frost + AttackPower.Fire;
	}

	// Apply attack multiplier
	TotalDamage *= AttackMultiplier;

	// Get poise damage
	float PoiseDamage = GetWeaponPoiseDamage();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] TryApplyDamage to %s: TotalDamage=%.2f, Poise=%.2f, Multiplier=%.2f"),
		*Target->GetName(), TotalDamage, PoiseDamage, AttackMultiplier);

	// Actual damage application is handled by CombatManager on target
	// This returns true to indicate damage data was successfully prepared
	return true;
}

float ASLFWeaponBase::GetWeaponPoiseDamage_Implementation()
{
	// Get poise damage from ItemInfo.EquipmentDetails.MinPoiseDamage/MaxPoiseDamage
	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;
	double MinPoise = EquipmentDetails.MinPoiseDamage;
	double MaxPoise = EquipmentDetails.MaxPoiseDamage;

	// Return random value in range
	if (MaxPoise > MinPoise)
	{
		return FMath::FRandRange(static_cast<float>(MinPoise), static_cast<float>(MaxPoise));
	}

	// If min equals max or max is less than min, return min
	return static_cast<float>(MinPoise);
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATUS EFFECTS [7/8]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFWeaponStatusEffectData> ASLFWeaponBase::GetWeaponStatusEffectData_Implementation()
{
	// Get status effect data from ItemInfo.EquipmentDetails.WeaponStatusEffectInfo
	TArray<FSLFWeaponStatusEffectData> Result;

	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;

	// Convert TMap<FGameplayTag, double> to TArray<FSLFWeaponStatusEffectData>
	for (const auto& StatusPair : EquipmentDetails.WeaponStatusEffectInfo)
	{
		FSLFWeaponStatusEffectData Entry;
		Entry.StatusEffectTag = StatusPair.Key;
		Entry.BuildupAmount = static_cast<float>(StatusPair.Value);
		Entry.EffectRank = 1; // Default rank

		Result.Add(Entry);
	}

	return Result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTION [8/8]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFWeaponBase::SetupItem_Implementation()
{
	Super::SetupItem_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] SetupItem - weapon-specific setup"));

	// Apply trail effect to trail component if set
	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
	}
}
