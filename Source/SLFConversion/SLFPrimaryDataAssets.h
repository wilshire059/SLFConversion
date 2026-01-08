// SLFPrimaryDataAssets.h
// Primary Data Asset base classes for SoulslikeFramework
// These are stub classes that will be replaced with full implementations during migration

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "SLFGameTypes.h"
#include "SLFEnums.h"
#include "SLFPrimaryDataAssets.generated.h"

// Forward declarations
class UNiagaraSystem;
class USoundCue;
class UAnimMontage;
class UDataTable;
class UCurveFloat;
class USLFActionBase;
class UTexture2D;

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
class SLFCONVERSION_API UPDA_ActionBase : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** The action class to instantiate when this action is triggered (soft reference) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TSoftClassPtr<USLFActionBase> ActionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TSoftObjectPtr<UAnimMontage> ActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	double StaminaCost = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bCanBeInterrupted = true;

	/** Generic data container for action-specific data (e.g., FMontage for pickup actions) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FInstancedStruct RelevantData;

	/** Required stat tag that must meet RequiredStatAmount to execute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FGameplayTag RequiredStatTag;

	/** Required stat amount for RequiredStatTag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	double RequiredStatAmount = 0.0;

	/** Dodge montages - migrated from FInstancedStruct<FDodgeMontages> for direct C++ access */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action|Dodge")
	FSLFDodgeMontages DodgeMontages;
};

//////////////////////////////////////////////////////////////////////////
// Item Primary Data Asset
// Migrated from PDA_Item Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

// Event dispatcher for crafting unlocked
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftingUnlocked);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Item : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Main item information struct - matches Blueprint "ItemInformation" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSLFItemInfo ItemInformation;

	/** Whether crafting is unlocked for this item - matches Blueprint "CrafingUnlocked?" (note: typo preserved from BP) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Crafting", meta = (DisplayName = "CrafingUnlocked?"))
	bool bCrafingUnlocked = false;

	/** Sell price in currency - matches Blueprint "SellPrice" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 SellPrice = 0;

	/** Event dispatcher when crafting is unlocked - matches Blueprint "OnCraftingUnlocked" */
	UPROPERTY(BlueprintAssignable, Category = "Private")
	FOnCraftingUnlocked OnCraftingUnlocked;

	/** World pickup Niagara effect - convenience accessor for ItemInformation.WorldPickupInfo.WorldNiagaraSystem */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|WorldPickup")
	TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

	/** World pickup static mesh - convenience accessor for ItemInformation.WorldPickupInfo.WorldStaticMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|WorldPickup")
	TSoftObjectPtr<UStaticMesh> WorldStaticMesh;

	/** World pickup skeletal mesh - convenience accessor for ItemInformation.WorldPickupInfo.WorldSkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|WorldPickup")
	TSoftObjectPtr<USkeletalMesh> WorldSkeletalMesh;

	/**
	 * GetRequiredCraftingItems - Pure function
	 * Returns the required items map for crafting this item
	 * Blueprint logic: Checks CanBeCrafted, returns RequiredItems map from CraftingDetails
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crafting")
	TMap<UPDA_Item*, int32> GetRequiredCraftingItems();

	/**
	 * UnlockCrafting
	 * Unlocks crafting for this item if it can be crafted
	 * Blueprint logic: If CanBeCrafted -> Set CrafingUnlocked = true -> Broadcast OnCraftingUnlocked
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void UnlockCrafting();
};

//////////////////////////////////////////////////////////////////////////
// Buff Primary Data Asset
// Migrated from PDA_Buff Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Buff : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Gameplay tag identifying this buff - matches Blueprint "BuffTag" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag BuffTag;

	/** Icon to display in UI for this buff - matches Blueprint "BuffIcon" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	/** The buff logic class to spawn - matches Blueprint "Class" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (DisplayName = "Class"))
	TSoftClassPtr<UObject> BuffClass;

	/** Duration in seconds - matches Blueprint "Duration" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Duration = 0.0;

	/** Curve that returns multiplier based on rank (X=rank, Y=multiplier) - matches Blueprint "RankMultiplierCurve" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UCurveFloat* RankMultiplierCurve = nullptr;
};

//////////////////////////////////////////////////////////////////////////
// Status Effect Primary Data Asset
// Migrated from PDA_StatusEffect Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_StatusEffect : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Gameplay tag identifying this status effect - matches Blueprint "Tag" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identifier")
	FGameplayTag Tag;

	/** The effect class to spawn (B_StatusEffect or subclass) - matches Blueprint "Effect" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identifier")
	TSoftClassPtr<UObject> Effect;

	/** Icon for UI - matches Blueprint "Icon" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Stat tag that provides resistance to this effect - matches Blueprint "ResistiveStat" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance|Buildup Resistance")
	FGameplayTag ResistiveStat;

	/** Curve for resistance scaling by stat - matches Blueprint "ResistiveStatCurve" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance|Buildup Resistance")
	UCurveFloat* ResistiveStatCurve = nullptr;

	/** Items that mitigate this effect - matches Blueprint "MitigatedBy" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance")
	TArray<UPDA_Item*> MitigatedBy;

	/** Items that cure this effect - matches Blueprint "CuredBy" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistance")
	TArray<UPDA_Item*> CuredBy;

	/** Base buildup rate per second - matches Blueprint "BaseBuildupRate" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Up & Decay")
	double BaseBuildupRate = 0.0;

	/** Base decay rate per second - matches Blueprint "BaseDecayRate" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Up & Decay")
	double BaseDecayRate = 0.0;

	/** Delay before decay starts - matches Blueprint "DecayDelay" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Up & Decay")
	double DecayDelay = 0.0;

	/** Color for status bar UI - matches Blueprint "BarFillColor" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FLinearColor BarFillColor = FLinearColor::White;

	/** Whether effect can be retriggered while active - matches Blueprint "CanRetrigger?" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "On Trigger", meta = (DisplayName = "CanRetrigger?"))
	bool bCanRetrigger = false;

	/** Text displayed when effect triggers - matches Blueprint "TriggeredText" variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FText TriggeredText;
};

//////////////////////////////////////////////////////////////////////////
// Weapon Animset Primary Data Asset
// Migrated from PDA_WeaponAnimset Blueprint - property names use DisplayName
// to match Blueprint names since C++ identifiers can't start with numbers
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_WeaponAnimset : public UPDA_Base
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════
	// COMBO MONTAGES - Light attacks
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "1h_LightComboMontage_R"))
	TSoftObjectPtr<UAnimMontage> OneH_LightComboMontage_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "1h_LightComboMontage_L"))
	TSoftObjectPtr<UAnimMontage> OneH_LightComboMontage_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "2h_LightComboMontage"))
	TSoftObjectPtr<UAnimMontage> TwoH_LightComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages")
	TSoftObjectPtr<UAnimMontage> LightDualWieldMontage;

	// ═══════════════════════════════════════════════════════════════════
	// COMBO MONTAGES - Heavy attacks
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "1h_HeavyComboMontage_R"))
	TSoftObjectPtr<UAnimMontage> OneH_HeavyComboMontage_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "1h_HeavyComboMontage_L"))
	TSoftObjectPtr<UAnimMontage> OneH_HeavyComboMontage_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages", meta = (DisplayName = "2h_HeavyComboMontage"))
	TSoftObjectPtr<UAnimMontage> TwoH_HeavyComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages")
	TSoftObjectPtr<UAnimMontage> HeavyDualWieldMontage;

	// ═══════════════════════════════════════════════════════════════════
	// COMBO MONTAGES - Special attacks
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages")
	TSoftObjectPtr<UAnimMontage> JumpAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Montages")
	TSoftObjectPtr<UAnimMontage> SprintAttackMontage;

	// ═══════════════════════════════════════════════════════════════════
	// GUARDING
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guarding")
	UAnimSequenceBase* Guard_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guarding")
	UAnimMontage* Guard_R_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guarding")
	UAnimSequenceBase* Guard_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guarding")
	UAnimMontage* Guard_L_Hit;

	// ═══════════════════════════════════════════════════════════════════
	// BACKSTAB & EXECUTION
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstab & Execution")
	UPrimaryDataAsset* ExecutionAsset;

	// ═══════════════════════════════════════════════════════════════════
	// LEGACY - for backwards compatibility
	// ═══════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAnimset")
	TArray<TSoftObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAnimset")
	TSoftObjectPtr<UAnimMontage> HeavyAttackMontage;

	UPDA_WeaponAnimset()
		: Guard_R(nullptr)
		, Guard_R_Hit(nullptr)
		, Guard_L(nullptr)
		, Guard_L_Hit(nullptr)
		, ExecutionAsset(nullptr)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Weapon Ability Primary Data Asset
// Migrated from PDA_WeaponAbility Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_WeaponAbility : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Icon to display - matches Blueprint "Icon" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Display name - matches Blueprint "Name" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config", meta = (DisplayName = "Name"))
	FText AbilityName;

	/** Affected stat tag - matches Blueprint "Affected Stat" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config", meta = (DisplayName = "Affected Stat"))
	FGameplayTag AffectedStat;

	/** Cost to use ability - matches Blueprint "Cost" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config")
	double Cost = 0.0;

	/** Animation montage - matches Blueprint "Montage" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config")
	TSoftObjectPtr<UAnimMontage> Montage;

	/** Additional effect class to spawn - matches Blueprint "AdditionalEffectClass" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability Config")
	TSoftClassPtr<UObject> AdditionalEffectClass;

	/**
	 * SetupData function - matches Blueprint "Event SetupData" custom event
	 * Used by the Weapon Ability Creator tool to initialize this data asset
	 */
	UFUNCTION(BlueprintCallable, Category = "WeaponAbility")
	void SetupData(TSoftObjectPtr<UTexture2D> InIcon, FText InName, FGameplayTag InAffectedStat,
		double InCost, TSoftObjectPtr<UAnimMontage> InMontage, TSoftClassPtr<UObject> InAdditionalEffectClass)
	{
		Icon = InIcon;
		AbilityName = InName;
		AffectedStat = InAffectedStat;
		Cost = InCost;
		Montage = InMontage;
		AdditionalEffectClass = InAdditionalEffectClass;
	}
};

//////////////////////////////////////////////////////////////////////////
// AI Ability Primary Data Asset
// Migrated from PDA_AI_Ability Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_AI_Ability : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Animation montage for this ability - matches Blueprint "Montage" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UAnimMontage> Montage;

	/** Score value for AI decision making - matches Blueprint "Score" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Score = 0.0;

	/** Cooldown duration in seconds - matches Blueprint "Cooldown" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Cooldown = 0.0;

	/** AI Rules using FInstancedStruct for polymorphic behavior - matches Blueprint "AiRules" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FInstancedStruct> AiRules;

	/**
	 * Setup data function - matches Blueprint "Event SetupData" custom event
	 * Used by the AI Skill Creator tool to initialize this data asset
	 */
	UFUNCTION(BlueprintCallable, Category = "AI_Ability")
	void SetupData(TSoftObjectPtr<UAnimMontage> InMontage, double InScore, double InCooldown, const TArray<FInstancedStruct>& InAiRules)
	{
		Montage = InMontage;
		Score = InScore;
		Cooldown = InCooldown;
		AiRules = InAiRules;
	}
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
	/** Poise break start animation - matches Blueprint "PoiseBreak_Start" exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poise Break Config", meta = (DisplayName = "PoiseBreak_Start"))
	UAnimSequence* PoiseBreak_Start;

	/** Poise break loop animation - matches Blueprint "PoiseBreak_Loop" exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poise Break Config", meta = (DisplayName = "PoiseBreak_Loop"))
	UAnimSequence* PoiseBreak_Loop;

	UPDA_PoiseBreakAnimData()
		: PoiseBreak_Start(nullptr)
		, PoiseBreak_Loop(nullptr)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Dialog Primary Data Asset
// Migrated from PDA_Dialog Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Dialog : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Dialog requirements array - matches Blueprint "Requirement" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FSLFDialogRequirement> Requirement;

	/** Default dialog table - matches Blueprint "DefaultDialogTable" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Defaults")
	TSoftObjectPtr<UDataTable> DefaultDialogTable;

	/**
	 * Get dialog table based on progress - matches Blueprint function exactly
	 * Iterates through Requirements, checks progress conditions, returns appropriate table
	 * @param ProgressManager - The progress manager component to query
	 * @param Table - Output: The dialog table matching current progress
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void GetDialogTableBasedOnProgress(UActorComponent* ProgressManager, TSoftObjectPtr<UDataTable>& Table);
};

//////////////////////////////////////////////////////////////////////////
// Vendor Primary Data Asset
// Migrated from PDA_Vendor Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

// Event dispatcher for when item stock is updated
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemStockUpdated);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Vendor : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Available items for sale - matches Blueprint "Items" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor Config")
	TSet<FSLFVendorItems> Items;

	/** Default number of slots - matches Blueprint "DefaultSlotCount" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor Config")
	int32 DefaultSlotCount = 0;

	/** Default slots per row - matches Blueprint "DefaultSlotsPerRow" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor Config")
	int32 DefaultSlotsPerRow = 0;

	/** Cached items for runtime - matches Blueprint "CachedItems" variable exactly */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFVendorItems> CachedItems;

	/** Event fired when item stock changes - matches Blueprint "OnItemStockUpdated" event dispatcher */
	UPROPERTY(BlueprintAssignable, Category = "Runtime")
	FOnItemStockUpdated OnItemStockUpdated;

	/**
	 * Reduce item stock by delta amount - matches Blueprint "ReduceItemStock" function
	 * @param Item - The item to reduce stock for
	 * @param Delta - Amount to reduce by
	 */
	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void ReduceItemStock(UObject* Item, int32 Delta);
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

public:
	/** Invert camera X axis - matches Blueprint "Invert Cam X" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (DisplayName = "Invert Cam X"))
	bool bInvertCamX;

	/** Invert camera Y axis - matches Blueprint "Invert Cam Y" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (DisplayName = "Invert Cam Y"))
	bool bInvertCamY;

	/** Camera speed - matches Blueprint "CamSpeed" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	double CamSpeed;

	UPDA_CustomSettings()
		: bInvertCamX(false)
		, bInvertCamY(false)
		, CamSpeed(1.0)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Default Mesh Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_DefaultMeshData : public UPDA_Base
{
	GENERATED_BODY()

public:
	// Option B Migration: Direct mesh properties instead of struct (avoids F-prefix type mismatch)
	// These bypass the FSLFSkeletalMeshData struct which has serialization issues with Blueprint structs

	/** Head skeletal mesh for modular character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Data")
	TSoftObjectPtr<USkeletalMesh> HeadMesh;

	/** Upper body skeletal mesh for modular character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Data")
	TSoftObjectPtr<USkeletalMesh> UpperBodyMesh;

	/** Arms skeletal mesh for modular character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Data")
	TSoftObjectPtr<USkeletalMesh> ArmsMesh;

	/** Lower body skeletal mesh for modular character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Data")
	TSoftObjectPtr<USkeletalMesh> LowerBodyMesh;

	/** Physics asset to use for the merged mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information")
	UPhysicsAsset* FinalPhysicsAsset;

	// Legacy struct property - kept for compatibility but data should be in direct properties above
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (DeprecatedProperty, DeprecationMessage = "Use direct mesh properties instead"))
	FSLFSkeletalMeshData MeshData;
};

//////////////////////////////////////////////////////////////////////////
// Base Character Info Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_BaseCharacterInfo : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Default mesh data asset for this character class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UDataAsset* DefaultMeshAsset;

	UPDA_BaseCharacterInfo()
		: DefaultMeshAsset(nullptr)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Movement Speed Data Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_MovementSpeedData : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Walk speed - matches Blueprint "WalkSpeed" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double WalkSpeed;

	/** Run speed - matches Blueprint "RunSpeed" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double RunSpeed;

	/** Sprint speed - matches Blueprint "SprintSpeed" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double SprintSpeed;

	UPDA_MovementSpeedData()
		: WalkSpeed(200.0)
		, RunSpeed(400.0)
		, SprintSpeed(600.0)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Animation Data Primary Data Asset
// Migrated from PDA_AnimData Blueprint - Properties match Blueprint names
// for AnimBP PropertyAccess compatibility
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_AnimData : public UPDA_Base
{
	GENERATED_BODY()

public:
	// Blendspaces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blendspaces", meta = (DisplayName = "Walk Run Locomotion"))
	UBlendSpace* WalkRunLocomotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blendspaces", meta = (DisplayName = "Crouch Locomotion"))
	UBlendSpace* CrouchLocomotion;

	// Overlays and Additive
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "One Handed Weapon Left"))
	UAnimSequenceBase* OneHandedWeapon_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "One Handed Weapon Right"))
	UAnimSequenceBase* OneHandedWeapon_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "Dual Wield"))
	UAnimSequenceBase* DualWield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "Two Handed Weapon Right"))
	UAnimSequenceBase* TwoHandedWeapon_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "Two Handed Weapon Left"))
	UAnimSequenceBase* TwoHandedWeapon_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlays", meta = (DisplayName = "Shield Left"))
	UAnimSequenceBase* ShieldLeft;

	// Idles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idles")
	UAnimSequenceBase* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idles", meta = (DisplayName = "Crouch Idle"))
	UAnimSequenceBase* CrouchIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idles", meta = (DisplayName = "Resting Idle"))
	UAnimSequenceBase* RestingIdle;

	UPDA_AnimData()
		: WalkRunLocomotion(nullptr)
		, CrouchLocomotion(nullptr)
		, OneHandedWeapon_Left(nullptr)
		, OneHandedWeapon_Right(nullptr)
		, DualWield(nullptr)
		, TwoHandedWeapon_Right(nullptr)
		, TwoHandedWeapon_Left(nullptr)
		, ShieldLeft(nullptr)
		, Idle(nullptr)
		, CrouchIdle(nullptr)
		, RestingIdle(nullptr)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Ladder Animation Data Primary Data Asset
// Migrated from PDA_LadderAnimData Blueprint
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_LadderAnimData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	TSoftObjectPtr<UAnimMontage> ClimbUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	TSoftObjectPtr<UAnimMontage> ClimbDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	TSoftObjectPtr<UAnimMontage> ClimbOutAtTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	TSoftObjectPtr<UAnimMontage> ClimbInFromTop;
};

//////////////////////////////////////////////////////////////////////////
// Execution Animation Data Primary Data Asset
// Migrated from PDA_ExecutionAnimData Blueprint
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_ExecutionAnimData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSLFExecutionInfo ExecuteFront;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSLFExecutionInfo ExecuteBack;
};

//////////////////////////////////////////////////////////////////////////
// Credits Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Credits : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Credits entry list - matches Blueprint "EntryList" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entries")
	TArray<FSLFCreditsEntry> EntryList;

	/** Extra credits data - matches Blueprint "ExtraData" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extra Data")
	TArray<FSLFCreditsExtra> ExtraData;
};

//////////////////////////////////////////////////////////////////////////
// Loading Screens Primary Data Asset
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_LoadingScreens : public UPDA_Base
{
	GENERATED_BODY()

public:
	/** Loading screen textures - matches Blueprint "Textures" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<TSoftObjectPtr<UTexture2D>> Textures;

	/** Loading screen tips - matches Blueprint "Tips" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FSLFLoadingScreenTip> Tips;

	/** Get random loading screen texture and tip - matches Blueprint "GetRandomLoadingScreenData" */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loading Screens")
	void GetRandomLoadingScreenData(TSoftObjectPtr<UTexture2D>& Texture, FSLFLoadingScreenTip& Tip) const
	{
		if (Textures.Num() > 0)
		{
			const int32 TextureIndex = FMath::RandRange(0, Textures.Num() - 1);
			Texture = Textures[TextureIndex];
		}
		if (Tips.Num() > 0)
		{
			const int32 TipIndex = FMath::RandRange(0, Tips.Num() - 1);
			Tip = Tips[TipIndex];
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// Calculations Primary Data Asset
// Migrated from PDA_Calculations Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Calculations : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Calculation type - matches Blueprint "CalculationType" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	ESLFCalculationType CalculationType = ESLFCalculationType::Multiply;

	/**
	 * Calculate function - matches Blueprint "Calculate" function exactly
	 * Pure function that applies the calculation based on CalculationType
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calculations")
	void Calculate(double Modifier, double ChangeAmount, double& OutChange) const
	{
		switch (CalculationType)
		{
		case ESLFCalculationType::Multiply:
		default:
			OutChange = ChangeAmount * Modifier;
			break;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// MainMenu Data Primary Data Asset
// Migrated from PDA_MainMenuData Blueprint - variable names MUST match JSON exactly
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_MainMenuData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Loading screen asset reference - matches Blueprint "LoadingScreenAsset" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Config|Loading Screen")
	UObject* LoadingScreenAsset = nullptr;

	/** Minimum additional wait time - matches Blueprint "AdditionalWaitMin" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Config|Loading Screen")
	double AdditionalWaitMin = 0.0;

	/** Maximum additional wait time - matches Blueprint "AdditionalWaitMax" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Config|Loading Screen")
	double AdditionalWaitMax = 0.0;

	/** Remove loading screen on finish - matches Blueprint "RemoveOnFinish?" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Config|Loading Screen", meta = (DisplayName = "RemoveOnFinish?"))
	bool bRemoveOnFinish = false;

	/** Default level to load - matches Blueprint "DefaultLevelToLoad" variable exactly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Config")
	TSoftObjectPtr<UWorld> DefaultLevelToLoad;
};
