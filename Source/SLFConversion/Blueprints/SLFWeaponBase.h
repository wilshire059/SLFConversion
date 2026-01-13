// SLFWeaponBase.h
// C++ base class for B_Item_Weapon
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Item_Weapon
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 migrated
// Functions:         8/8 migrated (excluding UserConstructionScript base call)
// Components:        2/2 migrated (WeaponMesh, TrailComponent)
// Event Dispatchers: 0/0
// Graphs:            9 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item_Weapon
//
// PURPOSE: Base weapon actor - melee weapons with damage, trails, hit detection
// CHILDREN: B_Item_Weapon_Sword, B_Item_Weapon_Greatsword, B_Item_Weapon_Shield, etc.

#pragma once

#include "CoreMinimal.h"
#include "SLFItemBase.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "Components/CollisionManagerComponent.h"
#include "SLFWeaponBase.generated.h"

// Forward declarations
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UStaticMeshComponent;

// Types used from SLFGameTypes.h:
// - FSLFWeaponAttackPower

// Local types (not in shared headers)
/**
 * Weapon stat scaling data
 */
USTRUCT(BlueprintType)
struct FSLFWeaponStatScaling
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ScalingMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText ScalingGrade; // S, A, B, C, D, E
};

/**
 * Weapon status effect application data
 */
USTRUCT(BlueprintType)
struct FSLFWeaponStatusEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FGameplayTag StatusEffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BuildupAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 EffectRank = 1;
};

/**
 * Base weapon actor - melee weapon with damage calculation and effects
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFWeaponBase : public ASLFItemBase
{
	GENERATED_BODY()

public:
	ASLFWeaponBase();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS: 2/2 migrated
	// Names use "Weapon" prefix to avoid collision with Blueprint SCS
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Static mesh component for weapon visual */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	/** [2/2] Niagara trail component for weapon swing effects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TrailComponent;

	// ═══════════════════════════════════════════════════════════════════
	// DEFAULT MESH - Serializable property for Blueprint children
	// This property persists to Blueprint and is applied in BeginPlay
	// ═══════════════════════════════════════════════════════════════════

	/** Default mesh to apply to WeaponMesh component. Set per Blueprint child. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TSoftObjectPtr<UStaticMesh> DefaultWeaponMesh;

	/** Default mesh relative location offset.
	 * Applied to WeaponMesh component to position mesh correctly relative to actor root.
	 * Values from original Blueprint SCS component settings. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Mesh")
	FVector DefaultMeshRelativeLocation;

	/** Default mesh relative rotation offset.
	 * Applied to WeaponMesh component to orient mesh correctly relative to actor root.
	 * Values from original Blueprint SCS component settings. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Mesh")
	FRotator DefaultMeshRelativeRotation;

	/** Default rotation offset for socket attachment.
	 * Applied when the item data asset doesn't specify an offset.
	 * Useful for AI weapons that need different orientations on different skeletons. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Attachment")
	FRotator DefaultAttachmentRotationOffset;

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 7/7 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- VFX (2) ---

	/** [1/7] Trail effect for weapon swings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|VFX")
	UNiagaraSystem* TrailEffect;

	/** [2/7] Trail width parameter name for Niagara */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|VFX")
	FName TrailWidthParameterName;

	// --- Environment Impact (2) ---

	/** [3/7] VFX for hitting environment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Impact")
	UNiagaraSystem* EnvironmentImpactEffect;

	/** [4/7] Sound for hitting environment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Impact")
	USoundBase* EnvironmentImpactSound;

	// --- Guard Sounds (2) ---

	/** [5/7] Sound when blocking attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Guard")
	USoundBase* GuardSound;

	/** [6/7] Sound for perfect guard */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Guard")
	USoundBase* PerfectGuardSound;

	// --- Debug (1) ---

	/** [7/7] Whether to visualize trace debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Debug")
	bool bDebugVisualizeTrace;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 8/8 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Stat Queries (4) ---

	/** [1/8] Get attack power stats from weapon data
	 * @return Array of attack power per damage type
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponAttackPower> GetAttackPowerStats();
	virtual TArray<FSLFWeaponAttackPower> GetAttackPowerStats_Implementation();

	/** [2/8] Get specific weapon stat value
	 * @param StatTag - Tag of stat to retrieve
	 * @return Stat value
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	float GetWeaponStat(FGameplayTag StatTag);
	virtual float GetWeaponStat_Implementation(FGameplayTag StatTag);

	/** [3/8] Get stat scaling data for weapon
	 * @return Array of scaling info per stat
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponStatScaling> GetWeaponStatScaling();
	virtual TArray<FSLFWeaponStatScaling> GetWeaponStatScaling_Implementation();

	/** [4/8] Get all attack power data from data asset
	 * @return Array of attack power structs
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponAttackPower> GetWeaponAttackPowerData();
	virtual TArray<FSLFWeaponAttackPower> GetWeaponAttackPowerData_Implementation();

	// --- Damage (2) ---

	/** [5/8] Try to apply damage to target
	 * @param Target - Actor to damage
	 * @param HitResult - Hit information
	 * @param AttackMultiplier - Damage multiplier from attack type
	 * @return True if damage was applied
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Damage")
	bool TryApplyDamage(AActor* Target, const FHitResult& HitResult, float AttackMultiplier);
	virtual bool TryApplyDamage_Implementation(AActor* Target, const FHitResult& HitResult, float AttackMultiplier);

	/** [6/8] Get weapon poise damage
	 * @return Poise damage value
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Damage")
	float GetWeaponPoiseDamage();
	virtual float GetWeaponPoiseDamage_Implementation();

	// --- Status Effects (1) ---

	/** [7/8] Get status effect application data
	 * @return Array of status effects this weapon can apply
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|StatusEffects")
	TArray<FSLFWeaponStatusEffectData> GetWeaponStatusEffectData();
	virtual TArray<FSLFWeaponStatusEffectData> GetWeaponStatusEffectData_Implementation();

	// --- Construction (1) ---

	/** [8/8] Setup weapon (called from construction) */
	virtual void SetupItem_Implementation() override;

protected:
	/** Cached reference to collision manager component (from Blueprint SCS) */
	UPROPERTY()
	UCollisionManagerComponent* CollisionManager;

	/** Handler for OnActorTraced event from CollisionManager */
	UFUNCTION()
	void OnActorTraced(AActor* Actor, FHitResult Hit, double Multiplier);

	/** Attach weapon to owner's skeletal mesh socket */
	void AttachToOwnerSocket();

	/** Check if this is a right-hand weapon based on equipment slot */
	bool IsRightHandWeapon() const;
};
