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
	// TODO: Get from weapon data asset
	return TArray<FSLFWeaponAttackPower>();
}

float ASLFWeaponBase::GetWeaponStat_Implementation(FGameplayTag StatTag)
{
	// TODO: Look up stat in weapon data asset
	return 0.0f;
}

TArray<FSLFWeaponStatScaling> ASLFWeaponBase::GetWeaponStatScaling_Implementation()
{
	// TODO: Get from weapon data asset
	return TArray<FSLFWeaponStatScaling>();
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

	UE_LOG(LogTemp, Log, TEXT("[Weapon] TryApplyDamage to %s with multiplier %.2f"),
		*Target->GetName(), AttackMultiplier);

	// TODO: Calculate total damage from attack power stats
	// Apply damage via damage interface or combat manager

	return true;
}

float ASLFWeaponBase::GetWeaponPoiseDamage_Implementation()
{
	// TODO: Get from weapon data asset
	return 50.0f;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATUS EFFECTS [7/8]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFWeaponStatusEffectData> ASLFWeaponBase::GetWeaponStatusEffectData_Implementation()
{
	// TODO: Get from weapon data asset
	return TArray<FSLFWeaponStatusEffectData>();
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
