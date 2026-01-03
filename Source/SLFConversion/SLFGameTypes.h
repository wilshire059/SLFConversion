// SLFGameTypes.h
// C++ versions of all Blueprint UserDefinedStructs
// Generated from JSON exports - ready for migration when needed
//
// NOTE: Some structs are already defined in SLFStatTypes.h:
// - FRegen, FAffectedStat, FAffectedStats, FStatBehavior, FStatInfo, FStatOverride
// - FStatChange, FStatChangePercent, FLevelChangeData
// - FStatusEffectStatChanges, FStatusEffectTick, FStatusEffectOneShotAndTick
//
// Migration process for each struct:
// 1. Rename Blueprint struct to XXX_DEPRECATED
// 2. Update all Blueprint references to use the C++ version
// 3. Delete the deprecated Blueprint struct

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "NiagaraSystem.h"
#include "LevelSequence.h"
#include "Engine/DataTable.h"
#include "SLFEnums.h"
#include "SLFStatTypes.h"
#include "SLFGameTypes.generated.h"

// Forward declarations for Blueprint classes (will be replaced during migration)
class UPDA_Action_C;
class UPDA_Item_C;
class UPDA_WeaponAbility_C;
class UPDA_WeaponAnimset_C;
class UPDA_BaseCharacterInfo_C;
class UPDA_Vendor_C;
class AB_PatrolPath_C;
class AB_Item_C;

//////////////////////////////////////////////////////////////////////////
// SECTION: Basic Wrapper Types
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFBool
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	bool bValue = false;

	FSLFBool() {}
	FSLFBool(bool InValue) : bValue(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFInt
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	int32 Value = 0;

	FSLFInt() {}
	FSLFInt(int32 InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFFloat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	double Value = 0.0;

	FSLFFloat() {}
	FSLFFloat(double InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFString
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	FString Value;

	FSLFString() {}
	FSLFString(const FString& InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFName
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	FName Value;

	FSLFName() {}
	FSLFName(FName InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFVector
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	FVector Value = FVector::ZeroVector;

	FSLFVector() {}
	FSLFVector(const FVector& InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFRotator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	FRotator Value = FRotator::ZeroRotator;

	FSLFRotator() {}
	FSLFRotator(const FRotator& InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFClass
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	UClass* Value = nullptr;

	FSLFClass() {}
	FSLFClass(UClass* InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	UObject* Value = nullptr;

	FSLFObject() {}
	FSLFObject(UObject* InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEnumByte
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrapper")
	uint8 Value = 0;

	FSLFEnumByte() {}
	FSLFEnumByte(uint8 InValue) : Value(InValue) {}
};

USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFActorClass
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor")
	UClass* ActorClass = nullptr;

	FSLFActorClass() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Action/Combat Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/Actions/FActionsData
// Inherits from FTableRowBase to be usable as Data Table row struct
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFActionsData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	UObject* ActionAsset = nullptr; // UPDA_Action_C* in Blueprint

	FSLFActionsData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Animation/FDodgeMontages
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDodgeMontages
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* Fwd = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* FwdLeft = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* FwdRight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* Left = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* Right = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* Bwd = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* BwdLeft = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* BwdRight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	UAnimMontage* Backstep = nullptr;

	FSLFDodgeMontages() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Animation/FAnimationData
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UBlendSpace* BlendSpace = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequenceBase* Idle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequenceBase* Jump = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequenceBase* FallingLoop = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequenceBase* Landed = nullptr;

	FSLFAnimationData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Animation/FMontage
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TSoftObjectPtr<UAnimMontage> AnimMontage;

	FSLFMontage() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Combat/FExecutionInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFExecutionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	TSoftObjectPtr<UAnimMontage> Animation;

	FSLFExecutionInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Combat/FExecutionAnimInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFExecutionAnimInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	UAnimMontage* Executor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	UAnimMontage* Receiver = nullptr;

	FSLFExecutionAnimInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Combat/FExecutionType
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFExecutionTypeWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	ESLFExecutionType Type = ESLFExecutionType::None;

	FSLFExecutionTypeWrapper() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Combat/FWeaponAttackPower
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFWeaponAttackPower
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Physical = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Magic = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Lightning = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Holy = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Frost = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackPower")
	double Fire = 0.0;

	FSLFWeaponAttackPower() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Stats/FSprintCost
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSprintCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	double TickInterval = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	double Change = 0.0;

	FSLFSprintCost() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Stats/FStatEntry
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	UClass* StatObject = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FGameplayTag ParentCategory;

	FSLFStatEntry() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Stats/FCardinalData
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCardinalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cardinal")
	FString UIDisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cardinal")
	double Value = 0.0;

	FSLFCardinalData() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: AI Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiRuleDistance
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiRuleDistance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ESLFComparisonMethod ComparisonType = ESLFComparisonMethod::Equal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	double Value = 0.0;

	FSLFAiRuleDistance() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiRuleStat
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiRuleStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	double Percent = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ESLFComparisonMethod ComparisonType = ESLFComparisonMethod::Equal;

	FSLFAiRuleStat() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiAttackEntry
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiAttackEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	double Weight = 0.0;

	FSLFAiAttackEntry() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiBossPhase
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiBossPhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName PhaseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HealthThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FSLFAiRuleStat RequiredStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<UObject*> PhaseAbilities; // TArray<UPDA_Action_C*>

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<ULevelSequence> PhaseStartSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<UAnimMontage> PhaseStartMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<UAnimMontage> PhaseTransitionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<USoundBase> PhaseMusic;

	FSLFAiBossPhase() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiPatrolPathInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiPatrolPathInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* PatrolPath = nullptr; // AB_PatrolPath_C*

	FSLFAiPatrolPathInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiSenseLocationInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiSenseLocationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FVector SenseLocation = FVector::ZeroVector;

	FSLFAiSenseLocationInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiSenseTargetInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiSenseTargetInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* SensedTarget = nullptr;

	FSLFAiSenseTargetInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/AI/FAiStrafeInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFAiStrafeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ESLFAIStrafeLocations StrafeMethod = ESLFAIStrafeLocations::Back;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	double Score = 0.0;

	FSLFAiStrafeInfo() {}

	bool operator==(const FSLFAiStrafeInfo& Other) const
	{
		return StrafeMethod == Other.StrafeMethod && FMath::IsNearlyEqual(Score, Other.Score);
	}

	friend uint32 GetTypeHash(const FSLFAiStrafeInfo& Info)
	{
		return HashCombine(GetTypeHash(static_cast<uint8>(Info.StrafeMethod)), GetTypeHash(Info.Score));
	}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Item/Equipment Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/Items/FItemCategory
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFItemCategoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ESLFItemCategory Category = ESLFItemCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ESLFItemSubCategory SubCategory = ESLFItemSubCategory::Miscellaneous;

	FSLFItemCategoryData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FEquipmentSocketInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentSocketInfo
{
	GENERATED_BODY()

	// Socket attachment info - expand as needed based on actual usage

	FSLFEquipmentSocketInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FEquipmentStat
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	double Delta = 0.0;

	FSLFEquipmentStat() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FEquipmentWeaponStatInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentWeaponStatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayName = "Has Stat Scaling?"))
	bool bHasStatScaling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, ESLFStatScaling> ScalingInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayName = "Has Stat Requirement?"))
	bool bHasStatRequirement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, int32> StatRequirementInfo;

	FSLFEquipmentWeaponStatInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FEquipmentSlot
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag SlotTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TSoftObjectPtr<UTexture2D> BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 Row = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 Column = 0;

	FSLFEquipmentSlot() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/World/FWorldMeshInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFWorldMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	ESLFWorldMeshStyle WorldDisplayStyle = ESLFWorldMeshStyle::ShowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TSoftObjectPtr<UStaticMesh> WorldStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TSoftObjectPtr<USkeletalMesh> WorldSkeletalMesh;

	FSLFWorldMeshInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FCraftingInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCraftingInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (DisplayName = "Can Be Crafted?"))
	bool bCanBeCrafted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TMap<FGameplayTag, int32> RequiredItems;

	FSLFCraftingInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FEquipmentInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTagContainer EquipSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTagContainer GrantedTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag WeaponOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UObject* WeaponAbility = nullptr; // UPDA_WeaponAbility_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FSLFEquipmentWeaponStatInfo WeaponStatInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, double> WeaponStatusEffectInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, FSLFEquipmentStat> StatChanges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, TSoftObjectPtr<USkeletalMesh>> SkeletalMeshInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FSLFEquipmentSocketInfo AttachmentSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UObject* MovesetWeapons = nullptr; // UPDA_WeaponAnimset_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FText ItemBuffDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<FGameplayTag, int32> GrantedBuffRank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	double StaminaMultiplier = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	double MinPoiseDamage = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	double MaxPoiseDamage = 0.0;

	FSLFEquipmentInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FItemInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFItemCategoryData Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ShortDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText LongDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DiscoveryDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Usable?"))
	bool bUsable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Rechargeable?"))
	bool bRechargeable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText OnUseDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> IconSmall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> IconLargeOptional;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTag ActionToTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftClassPtr<AActor> ItemClass; // AB_Item_C

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFWorldMeshInfo WorldPickupInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFEquipmentInfo EquipmentDetails;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFCraftingInfo CraftingDetails;

	FSLFItemInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FItemInfoCount
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFItemInfoCount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSLFItemInfo Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Count = 0;

	FSLFItemInfoCount() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Equipment/FCurrentEquipment
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCurrentEquipment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	UObject* ItemAsset = nullptr; // UPDA_Item_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGuid UniqueId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 StackCount = 1;

	FSLFCurrentEquipment() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Inventory/FInventoryCategory
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFInventoryCategory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	ESLFItemCategory Category = ESLFItemCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSoftObjectPtr<UTexture2D> CategoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText DisplayNameOverride;

	FSLFInventoryCategory() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Inventory/FInventoryItem
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UObject* ItemAsset = nullptr; // UPDA_Item_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Amount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FGuid UniqueId;

	FSLFInventoryItem() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FLootItem
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TSubclassOf<AActor> ItemClass; // Pickup actor class

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	UObject* Item = nullptr; // UPDA_Item_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 MinAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 MaxAmount = 1;

	FSLFLootItem() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FWeightedLoot
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFWeightedLoot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FSLFLootItem Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	double Weight = 1.0;

	FSLFWeightedLoot() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Items/FFlaskData
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFFlaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flask")
	TArray<FStatChangePercent> StatChangesPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flask")
	TSoftObjectPtr<UAnimMontage> DrinkingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flask")
	TSoftObjectPtr<UNiagaraSystem> VFX;

	FSLFFlaskData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Vendor/FVendorItems
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFVendorItems
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor")
	UObject* Item = nullptr; // UPDA_Item_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor", meta = (DisplayName = "Infinite Stock?"))
	bool bInfiniteStock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor")
	int32 StockAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vendor")
	int32 Price = 0;

	FSLFVendorItems() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/ItemWheel/FItemWheelNextSlotInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFItemWheelNextSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWheel", meta = (DisplayName = "Display Next Slots?"))
	bool bDisplayNextSlots = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWheel")
	int32 AdditionalDisplaySlotCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWheel")
	TEnumAsByte<EVerticalAlignment> Alignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWheel")
	FVector2D Sizing = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemWheel")
	double Spacing = 0.0;

	FSLFItemWheelNextSlotInfo() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Dialog Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/Dialog/FDialogGameplayEvent
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDialogGameplayEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FGameplayTag AdditionalTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FInstancedStruct CustomData;

	FSLFDialogGameplayEvent() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Dialog/FDialogEntry
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDialogEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FText Entry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FSLFDialogGameplayEvent> GameplayEvents;

	FSLFDialogEntry() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Dialog/FDialogProgress
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDialogProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FGameplayTagContainer ProgressContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	ESLFProgress State = ESLFProgress::NotStarted;

	FSLFDialogProgress() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Dialog/FDialogRequirement
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDialogRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FSLFDialogProgress> Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TSoftObjectPtr<UDataTable> RelatedDialogTable;

	FSLFDialogRequirement() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Save/Load Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/Save/FSaveData
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<uint8> Data;

	FSLFSaveData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FSaveGameInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSaveGameInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TMap<FString, FSLFSaveData> SaveGameEntry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTimespan PlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform SpawnTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> StatsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> InventoryData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> EquipmentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<FInstancedStruct> ProgressData;

	FSLFSaveGameInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FClassSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFClassSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	UObject* BaseCharacterClass = nullptr; // UPDA_BaseCharacterInfo_C*

	FSLFClassSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FWorldSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFWorldSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform WorldTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FRotator ControlRotation = FRotator::ZeroRotator;

	FSLFWorldSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FProgressSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFProgressSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGameplayTag ProgressTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	ESLFProgress State = ESLFProgress::NotStarted;

	FSLFProgressSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FInventoryItemsSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFInventoryItemsSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	UObject* Item = nullptr; // UPDA_Item_C*

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 Amount = 0;

	FSLFInventoryItemsSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FEquipmentItemsSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFEquipmentItemsSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGameplayTag SlotTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	UObject* AssignedItem = nullptr; // UPDA_Item_C*

	FSLFEquipmentItemsSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FItemWheelSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFItemWheelSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGameplayTag Identifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 CurrentIndex = 0;

	FSLFItemWheelSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FInteractableStateSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFInteractableStateSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save", meta = (DisplayName = "Can Be Traced?"))
	bool bCanBeTraced = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save", meta = (DisplayName = "Is Activated?"))
	bool bIsActivated = false;

	FSLFInteractableStateSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FNpcSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFNpcSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform Transform;

	FSLFNpcSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FNpcVendorSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFNpcVendorSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	UObject* VendorAsset = nullptr; // UPDA_Vendor_C*

	FSLFNpcVendorSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FSpawnedActorSaveInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSpawnedActorSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	UClass* Class = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FInstancedStruct CustomData;

	FSLFSpawnedActorSaveInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Save/FDoorLockInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDoorLockInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (DisplayName = "Is Locked?"))
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TMap<FGameplayTag, int32> RequiredItemAmount;

	FSLFDoorLockInfo() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Status Effect Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectVfxInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatusEffectVfxInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	TSoftObjectPtr<UNiagaraSystem> VFXSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FName AttachSocket;

	FSLFStatusEffectVfxInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectRankInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatusEffectRankInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FSLFStatusEffectVfxInfo TriggerVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FSLFStatusEffectVfxInfo LoopVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FInstancedStruct RelevantData;

	FSLFStatusEffectRankInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectApplication
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatusEffectApplication
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	int32 Rank = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double BuildupAmount = 0.0;

	FSLFStatusEffectApplication() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectFrostbiteExample
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatusEffectFrostbiteExample
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FStatusEffectStatChanges StatChanges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double SlowedSpeed = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double SlowedDuration = 0.0;

	FSLFStatusEffectFrostbiteExample() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectPlagueExample
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFStatusEffectPlagueExample
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	FStatusEffectStatChanges StatChanges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double NewScale = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
	double Duration = 0.0;

	FSLFStatusEffectPlagueExample() {}
};

//////////////////////////////////////////////////////////////////////////
// SECTION: Miscellaneous Types
//////////////////////////////////////////////////////////////////////////

// Replaces: /Game/SoulslikeFramework/Structures/DayNight/FDayNightInfo
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFDayNightInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	double FromTime = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	double ToTime = 0.0;

	FSLFDayNightInfo() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Progress/FProgress
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFProgressWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	ESLFProgress Progress = ESLFProgress::NotStarted;

	FSLFProgressWrapper() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Stats/FRequiredCurrencyForLevel
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFRequiredCurrencyForLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 RequiredCost = 0;

	FSLFRequiredCurrencyForLevel() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Mesh/FSkeletalMeshData
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFSkeletalMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<USkeletalMesh> HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<USkeletalMesh> UpperBodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<USkeletalMesh> ArmsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<USkeletalMesh> LowerBodyMesh;

	FSLFSkeletalMeshData() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Input/FKeyMappingCorrelation
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFKeyMappingCorrelation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TSoftObjectPtr<UTexture2D> KeyImage;

	FSLFKeyMappingCorrelation() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/UI/FLoadingScreenTip
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFLoadingScreenTip
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText TipTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText TipDescription;

	FSLFLoadingScreenTip() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Credits/FCreditsNames
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCreditsNames
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	TArray<FText> Names;

	FSLFCreditsNames() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Credits/FCreditsExtra
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCreditsExtra
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits", meta = (DisplayName = "Before Entry?"))
	bool bBeforeEntry = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	FVector2D ImageSizeOverride = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	double Score = 0.0;

	FSLFCreditsExtra() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Credits/FCreditsEntry
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFCreditsEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	FText Title;

	// Note: Blueprint uses TMap<FText, FCreditsNames> but FText can't be TMap key in C++
	// Using FString as key instead - may need conversion during migration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	TMap<FString, FSLFCreditsNames> Subtitle;

	FSLFCreditsEntry() {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Colour/S_Colour
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFColour
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colour")
	FColor Colour = FColor::White;

	FSLFColour() {}
	FSLFColour(const FColor& InColour) : Colour(InColour) {}
};

// Replaces: /Game/SoulslikeFramework/Structures/Room/S_RoomSettings
USTRUCT(BlueprintType)
struct SLFCONVERSION_API FSLFRoomSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	FText RoomName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 Length = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 Width = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 Height = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	ESLFEnclosureLevel WallDrop = ESLFEnclosureLevel::Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bPillar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bSubPillar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bRoof = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bEnclosedLeft = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bEnclosedRight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	double RoofDrop = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	ESLFLightningMode LightRoom = ESLFLightningMode::Exterior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	double PillarThickness = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	bool bSquareExterior = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	double InnerPanelPercent = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	double OuterPanelPercent = 0.9;

	// Convenience aliases for backward compatibility
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 RoomWidth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 RoomHeight = 0;

	FSLFRoomSettings() {}
};

//////////////////////////////////////////////////////////////////////////
// Type aliases for Blueprint struct name compatibility
// These allow generated code to use Blueprint names
// while referencing the C++ structs
//////////////////////////////////////////////////////////////////////////

// Item-related (NOTE: FItemInfo conflicts with Unreal's FItemInfo in STreeView.h, so skipped)
using FItemWheelSaveInfo = FSLFItemWheelSaveInfo;
using FItemWheelNextSlotInfo = FSLFItemWheelNextSlotInfo;

// Save-related
using FSaveGameInfo = FSLFSaveGameInfo;
using FSaveData = FSLFSaveData;

// Equipment-related
using FCurrentEquipment = FSLFCurrentEquipment;
using FEquipmentItemsSaveInfo = FSLFEquipmentItemsSaveInfo;

// Combat-related
using FWeaponAttackPower = FSLFWeaponAttackPower;
using FDodgeMontages = FSLFDodgeMontages;

// Door-related
using FDoorLockInfo = FSLFDoorLockInfo;

// Dialog-related
using FDialogEntry = FSLFDialogEntry;
using FDialogGameplayEvent = FSLFDialogGameplayEvent;

// Day/Night-related
using FDayNightInfo = FSLFDayNightInfo;

// AI-related
using FAiBossPhase = FSLFAiBossPhase;
using FAiRuleStat = FSLFAiRuleStat;
using FAiStrafeInfo = FSLFAiStrafeInfo;

// Inventory-related
using FInventoryCategory = FSLFInventoryCategory;
using FLootItem = FSLFLootItem;

// Credits-related
using FCreditsEntry = FSLFCreditsEntry;
using FCreditsExtra = FSLFCreditsExtra;

// Radar
using FCardinalData = FSLFCardinalData;
