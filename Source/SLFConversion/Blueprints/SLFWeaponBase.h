// SLFWeaponBase.h
// C++ base class for B_Item_Weapon
//
// ===================================================================
// MIGRATION SUMMARY - B_Item_Weapon
// ===================================================================
// Variables:         7/7 migrated
// Functions:         8/8 migrated (excluding UserConstructionScript base call)
// Components:        2/2 migrated (WeaponMesh, TrailComponent)
// Event Dispatchers: 0/0
// Graphs:            9 (logic in functions)
// Interface:         IBPI_Item (OnWeaponEquip, OnWeaponUnequip)
// ===================================================================
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
#include "Interfaces/BPI_Item.h"
#include "SLFWeaponBase.generated.h"

// Forward declarations
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UStaticMeshComponent;

// Types used from SLFGameTypes.h:
// - FSLFWeaponAttackPower

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
 * Implements IBPI_Item interface for OnWeaponEquip/OnWeaponUnequip events
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFWeaponBase : public ASLFItemBase, public IBPI_Item
{
	GENERATED_BODY()

public:
	ASLFWeaponBase();

protected:
	virtual void BeginPlay() override;

public:
	// ===================================================================
	// IBPI_Item INTERFACE IMPLEMENTATION
	// Called by AC_EquipmentManager when weapon is equipped/unequipped
	// ===================================================================

	/** Called when weapon is equipped - handles attachment to character socket */
	virtual void OnWeaponEquip_Implementation(bool bRightHand) override;

	/** Called when weapon is unequipped */
	virtual void OnWeaponUnequip_Implementation() override;

	/** Called when item is used (not typically called for weapons) */
	virtual void OnUse_Implementation() override;

	// ===================================================================
	// COMPONENTS: 2/2 migrated
	// ===================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TrailComponent;

	// ===================================================================
	// DEFAULT MESH - Serializable property for Blueprint children
	// ===================================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TSoftObjectPtr<UStaticMesh> DefaultWeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Mesh")
	FVector DefaultMeshRelativeLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Mesh")
	FRotator DefaultMeshRelativeRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Attachment")
	FRotator DefaultAttachmentRotationOffset;

	// ===================================================================
	// VARIABLES: 7/7 migrated
	// ===================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|VFX")
	UNiagaraSystem* TrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|VFX")
	FName TrailWidthParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Impact")
	UNiagaraSystem* EnvironmentImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Impact")
	USoundBase* EnvironmentImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Guard")
	USoundBase* GuardSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Guard")
	USoundBase* PerfectGuardSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Debug")
	bool bDebugVisualizeTrace;

	// ===================================================================
	// FUNCTIONS: 8/8 migrated
	// ===================================================================

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponAttackPower> GetAttackPowerStats();
	virtual TArray<FSLFWeaponAttackPower> GetAttackPowerStats_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	float GetWeaponStat(FGameplayTag StatTag);
	virtual float GetWeaponStat_Implementation(FGameplayTag StatTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponStatScaling> GetWeaponStatScaling();
	virtual TArray<FSLFWeaponStatScaling> GetWeaponStatScaling_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Stats")
	TArray<FSLFWeaponAttackPower> GetWeaponAttackPowerData();
	virtual TArray<FSLFWeaponAttackPower> GetWeaponAttackPowerData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Damage")
	bool TryApplyDamage(AActor* Target, const FHitResult& HitResult, float AttackMultiplier);
	virtual bool TryApplyDamage_Implementation(AActor* Target, const FHitResult& HitResult, float AttackMultiplier);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Damage")
	float GetWeaponPoiseDamage();
	virtual float GetWeaponPoiseDamage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|StatusEffects")
	TArray<FSLFWeaponStatusEffectData> GetWeaponStatusEffectData();
	virtual TArray<FSLFWeaponStatusEffectData> GetWeaponStatusEffectData_Implementation();

	virtual void SetupItem_Implementation() override;

protected:
	UPROPERTY()
	UCollisionManagerComponent* CollisionManager;

	UFUNCTION()
	void OnActorTraced(AActor* Actor, FHitResult Hit, double Multiplier);

	void AttachToOwnerSocket(bool bRightHand);
	bool IsRightHandWeapon() const;
	void ApplyMeshSettings();

	bool bHasBeenEquipped = false;
};
