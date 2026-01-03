// SLFPrimaryDataAssets.h
// Primary Data Asset base classes for SoulslikeFramework
// These are stub classes that will be replaced with full implementations during migration

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.generated.h"

// Forward declarations
class UNiagaraSystem;
class USoundCue;
class UAnimMontage;
class UDataTable;
class UCurveFloat;

//////////////////////////////////////////////////////////////////////////
// Base Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Base : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FName AssetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FText Description;
};

//////////////////////////////////////////////////////////////////////////
// Action Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Action : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TSoftObjectPtr<UAnimMontage> ActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	double StaminaCost = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bCanBeInterrupted = true;
};

//////////////////////////////////////////////////////////////////////////
// Item Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Item : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsConsumable = false;
};

//////////////////////////////////////////////////////////////////////////
// Buff Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Buff : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	FGameplayTag BuffTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	double Duration = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool bIsStackable = false;

	/** Curve that returns multiplier based on rank (X=rank, Y=multiplier) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	UCurveFloat* RankMultiplierCurve = nullptr;
};

//////////////////////////////////////////////////////////////////////////
// Status Effect Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_StatusEffect : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double Duration = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double TickInterval = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double DamagePerTick = 0.0;
};

//////////////////////////////////////////////////////////////////////////
// Weapon Animset Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_WeaponAnimset : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAnimset")
	TArray<TSoftObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAnimset")
	TSoftObjectPtr<UAnimMontage> HeavyAttackMontage;
};

//////////////////////////////////////////////////////////////////////////
// Weapon Ability Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_WeaponAbility : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAbility")
	TSoftObjectPtr<UAnimMontage> AbilityMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAbility")
	double FPCost = 0.0;
};

//////////////////////////////////////////////////////////////////////////
// AI Ability Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_AI_Ability : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI_Ability")
	TSoftObjectPtr<UAnimMontage> AbilityMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI_Ability")
	double Cooldown = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI_Ability")
	double Range = 0.0;
};

//////////////////////////////////////////////////////////////////////////
// Combat Reaction Anim Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_CombatReactionAnimData : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatReaction")
	TSoftObjectPtr<UAnimMontage> ReactionMontage;
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_CombatReactionAnimData_Player : public UPDA_CombatReactionAnimData
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Poise Break Anim Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_PoiseBreakAnimData : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoiseBreak")
	TSoftObjectPtr<UAnimMontage> PoiseBreakMontage;
};

//////////////////////////////////////////////////////////////////////////
// Dialog Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Dialog : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FText> DialogLines;
};

//////////////////////////////////////////////////////////////////////////
// Vendor Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Vendor : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor")
	UDataTable* VendorInventory;
};

//////////////////////////////////////////////////////////////////////////
// Day/Night Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_DayNight : public UPDA_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	double SunriseTime = 6.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	double SunsetTime = 18.0;
};

//////////////////////////////////////////////////////////////////////////
// Custom Settings Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_CustomSettings : public UPDA_Base
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Default Mesh Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_DefaultMeshData : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Modular skeletal mesh data (Head, UpperBody, Arms, LowerBody) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information")
	FSLFSkeletalMeshData MeshData;

	/** Physics asset to use for the merged mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information")
	UPhysicsAsset* FinalPhysicsAsset;
};

//////////////////////////////////////////////////////////////////////////
// Base Character Info Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_BaseCharacterInfo : public UPDA_Base
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Movement Speed Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_MovementSpeedData : public UPDA_Base
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Animation Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_AnimData : public UPDA_Base
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Credits Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Credits : public UPDA_Base
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////////
// Loading Screens Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_LoadingScreens : public UPDA_Base
{
	GENERATED_BODY()
};
