// SLFEnums.h
// C++ versions of all Blueprint UserDefinedEnums
// Generated from JSON exports - ready for migration when needed
//
// Migration process for each enum:
// 1. Rename Blueprint enum to XXX_DEPRECATED
// 2. Update all Blueprint references to use the C++ version
// 3. Delete the deprecated Blueprint enum

#pragma once

#include "CoreMinimal.h"
#include "SLFEnums.generated.h"

//////////////////////////////////////////////////////////////////////////
// ESLFCompareOperation
// Simple comparison operations for AI/BT services
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFCompareOperation : uint8
{
	Equal = 0			UMETA(DisplayName = "Equal"),
	NotEqual = 1		UMETA(DisplayName = "Not Equal"),
	LessThan = 2		UMETA(DisplayName = "Less Than"),
	LessThanOrEqual = 3	UMETA(DisplayName = "Less Than Or Equal"),
	GreaterThan = 4		UMETA(DisplayName = "Greater Than"),
	GreaterThanOrEqual = 5	UMETA(DisplayName = "Greater Than Or Equal")
};

//////////////////////////////////////////////////////////////////////////
// ESLFActionWeaponSlot
// Replaces: /Game/SoulslikeFramework/Enums/E_ActionWeaponSlot
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFActionWeaponSlot : uint8
{
	Null = 0		UMETA(DisplayName = "Null"),
	Right = 1		UMETA(DisplayName = "Right"),
	Left = 2		UMETA(DisplayName = "Left"),
	Dual = 3		UMETA(DisplayName = "Dual")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAIBossEncounterType
// Replaces: /Game/SoulslikeFramework/Enums/E_AI_BossEncounterType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAIBossEncounterType : uint8
{
	Perception = 0			UMETA(DisplayName = "Perception"),
	CollisionTrigger = 1	UMETA(DisplayName = "Collision Trigger"),
	OnDamaged = 2			UMETA(DisplayName = "On Damaged"),
	OnOverlap = 3			UMETA(DisplayName = "On Overlap")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAISenses
// Replaces: /Game/SoulslikeFramework/Enums/E_AI_Senses
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAISenses : uint8
{
	None = 0		UMETA(DisplayName = "-"),
	Sight = 1		UMETA(DisplayName = "Sight"),
	Hearing = 2		UMETA(DisplayName = "Hearing")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAIStateHandle
// Replaces: /Game/SoulslikeFramework/Enums/E_AI_StateHandle
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAIStateHandle : uint8
{
	Current = 0		UMETA(DisplayName = "Current"),
	Previous = 1	UMETA(DisplayName = "Previous")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAIStates
// Replaces: /Game/SoulslikeFramework/Enums/E_AI_States
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAIStates : uint8
{
	Idle = 0			UMETA(DisplayName = "Idle"),
	RandomRoam = 1		UMETA(DisplayName = "Random Roam"),
	Patrolling = 2		UMETA(DisplayName = "Patrolling"),
	Investigating = 3	UMETA(DisplayName = "Investigating"),
	Combat = 4			UMETA(DisplayName = "Combat"),
	PoiseBroken = 5		UMETA(DisplayName = "Poise Broken"),
	Uninterruptable = 6	UMETA(DisplayName = "Uninterruptable"),
	OutOfBounds = 7		UMETA(DisplayName = "Out of Bounds")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAIStrafeLocations
// Replaces: /Game/SoulslikeFramework/Enums/E_AI_StrafeLocations
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAIStrafeLocations : uint8
{
	Back = 0	UMETA(DisplayName = "Back"),
	Left = 1	UMETA(DisplayName = "Left"),
	Right = 2	UMETA(DisplayName = "Right")
};

//////////////////////////////////////////////////////////////////////////
// ESLFAttackPower
// Replaces: /Game/SoulslikeFramework/Enums/E_AttackPower
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFAttackPower : uint8
{
	Physical = 0	UMETA(DisplayName = "Physical"),
	Magic = 1		UMETA(DisplayName = "Magic"),
	Fire = 2		UMETA(DisplayName = "Fire"),
	Frost = 3		UMETA(DisplayName = "Frost"),
	Lightning = 4	UMETA(DisplayName = "Lightning"),
	Holy = 5		UMETA(DisplayName = "Holy")
};

//////////////////////////////////////////////////////////////////////////
// ESLFCalculationType
// Replaces: /Game/SoulslikeFramework/Enums/E_CalculationType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFCalculationType : uint8
{
	Multiply = 0	UMETA(DisplayName = "Multiply")
};

//////////////////////////////////////////////////////////////////////////
// ESLFDebugWindowScaleType
// Replaces: /Game/SoulslikeFramework/Enums/E_DebugWindowScaleType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFDebugWindowScaleType : uint8
{
	None = 0		UMETA(DisplayName = "-"),
	CornerRight = 1	UMETA(DisplayName = "Corner Right"),
	CornerLeft = 2	UMETA(DisplayName = "Corner Left"),
	Right = 3		UMETA(DisplayName = "Right"),
	Left = 4		UMETA(DisplayName = "Left"),
	Bottom = 5		UMETA(DisplayName = "Bottom")
};

//////////////////////////////////////////////////////////////////////////
// ESLFDirection
// Replaces: /Game/SoulslikeFramework/Enums/E_Direction
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFDirection : uint8
{
	Idle = 0		UMETA(DisplayName = "Idle"),
	Fwd = 1			UMETA(DisplayName = "Fwd"),
	FwdLeft = 2		UMETA(DisplayName = "Fwd Left"),
	FwdRight = 3	UMETA(DisplayName = "Fwd Right"),
	Left = 4		UMETA(DisplayName = "Left"),
	Right = 5		UMETA(DisplayName = "Right"),
	Bwd = 6			UMETA(DisplayName = "Bwd"),
	BwdLeft = 7		UMETA(DisplayName = "Bwd Left"),
	BwdRight = 8	UMETA(DisplayName = "Bwd Right")
};

//////////////////////////////////////////////////////////////////////////
// ESLFDotProductThreshold
// Replaces: /Game/SoulslikeFramework/Enums/E_DotProductThreshold
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFDotProductThreshold : uint8
{
	Low = 0			UMETA(DisplayName = "Low"),
	Medium = 1		UMETA(DisplayName = "Medium"),
	High = 2		UMETA(DisplayName = "High"),
	Generous = 3	UMETA(DisplayName = "Generous")
};

//////////////////////////////////////////////////////////////////////////
// ESLFEnclosureLevel
// Replaces: /Game/SoulslikeFramework/Enums/E_EnclosureLevel
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFEnclosureLevel : uint8
{
	Closed = 0		UMETA(DisplayName = "Closed"),
	Partial = 1		UMETA(DisplayName = "Partial"),
	Open = 2		UMETA(DisplayName = "Open")
};

//////////////////////////////////////////////////////////////////////////
// ESLFExecutionType
// Replaces: /Game/SoulslikeFramework/Enums/E_ExecutionType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFExecutionType : uint8
{
	None = 0		UMETA(DisplayName = "-"),
	Backstab = 1	UMETA(DisplayName = "Backstab"),
	Execution = 2	UMETA(DisplayName = "Execution")
};

//////////////////////////////////////////////////////////////////////////
// ESLFFadeTypes
// Replaces: /Game/SoulslikeFramework/Enums/E_FadeTypes
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFFadeTypes : uint8
{
	FadeIn = 0		UMETA(DisplayName = "Fade In"),
	FadeOut = 1		UMETA(DisplayName = "Fade Out"),
	FadeInOut = 2	UMETA(DisplayName = "Fade In n Out")
};

//////////////////////////////////////////////////////////////////////////
// ESLFHitReactType
// Replaces: /Game/SoulslikeFramework/Enums/E_HitReactType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFHitReactType : uint8
{
	None = 0	UMETA(DisplayName = "-"),
	Light = 1	UMETA(DisplayName = "Light"),
	Heavy = 2	UMETA(DisplayName = "Heavy"),
	Montage = 3	UMETA(DisplayName = "Montage"),
	IK = 4		UMETA(DisplayName = "IK"),
	Both = 5	UMETA(DisplayName = "Both")
};

//////////////////////////////////////////////////////////////////////////
// ESLFInputAction
// Replaces: /Game/SoulslikeFramework/Enums/E_InputAction
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFInputAction : uint8
{
	None = 0			UMETA(DisplayName = "-"),
	Jump = 1			UMETA(DisplayName = "Jump"),
	Dodge = 2			UMETA(DisplayName = "Dodge"),
	Block = 3			UMETA(DisplayName = "Block"),
	LightAttack = 4		UMETA(DisplayName = "Light Attack"),
	HeavyAttack = 5		UMETA(DisplayName = "Heavy Attack"),
	JumpAttack = 6		UMETA(DisplayName = "Jump Attack"),
	SpecialAttack = 7	UMETA(DisplayName = "Special Attack"),
	DrinkFlask = 8		UMETA(DisplayName = "Drink Flask")
};

//////////////////////////////////////////////////////////////////////////
// ESLFInventoryAmountedActionType
// Replaces: /Game/SoulslikeFramework/Enums/E_InventoryAmountedActionType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFInventoryAmountedActionType : uint8
{
	LeaveAmount = 0		UMETA(DisplayName = "Leave Amount"),
	DiscardAmount = 1	UMETA(DisplayName = "Discard Amount"),
	StoreAmount = 2		UMETA(DisplayName = "Store Amount"),
	RetrieveAmount = 3	UMETA(DisplayName = "Retrieve Amount")
};

//////////////////////////////////////////////////////////////////////////
// ESLFInventorySlotType
// Replaces: /Game/SoulslikeFramework/Enums/E_InventorySlotType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFInventorySlotType : uint8
{
	InventorySlot = 0	UMETA(DisplayName = "Inventory Slot"),
	StorageSlot = 1		UMETA(DisplayName = "Storage Slot")
};

//////////////////////////////////////////////////////////////////////////
// ESLFItemCategory
// Replaces: /Game/SoulslikeFramework/Enums/E_ItemCategory
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFItemCategory : uint8
{
	None = 0		UMETA(DisplayName = "-"),
	Tools = 1		UMETA(DisplayName = "Tools"),
	Crafting = 2	UMETA(DisplayName = "Crafting"),
	Bolstering = 3	UMETA(DisplayName = "Bolstering"),
	KeyItems = 4	UMETA(DisplayName = "Key Items"),
	Abilities = 5	UMETA(DisplayName = "Abilities"),
	Weapons = 6		UMETA(DisplayName = "Weapons"),
	Shields = 7		UMETA(DisplayName = "Shields"),
	Armor = 8		UMETA(DisplayName = "Armor")
};

//////////////////////////////////////////////////////////////////////////
// ESLFItemSubCategory
// Replaces: /Game/SoulslikeFramework/Enums/E_ItemSubCategory
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFItemSubCategory : uint8
{
	Flasks = 0			UMETA(DisplayName = "Flasks"),
	Consumables = 1		UMETA(DisplayName = "Consumables"),
	Projectiles = 2		UMETA(DisplayName = "Projectiles"),
	Enchants = 3		UMETA(DisplayName = "Enchants"),
	Miscellaneous = 4	UMETA(DisplayName = "Miscellaneous"),
	Runes = 5			UMETA(DisplayName = "Runes"),
	Head = 6			UMETA(DisplayName = "Head"),
	Chest = 7			UMETA(DisplayName = "Chest"),
	Arms = 8			UMETA(DisplayName = "Arms"),
	Legs = 9			UMETA(DisplayName = "Legs"),
	Talismans = 10		UMETA(DisplayName = "Talismans"),
	InfoItems = 11		UMETA(DisplayName = "Info Items"),
	Sword = 12			UMETA(DisplayName = "Sword"),
	Katana = 13			UMETA(DisplayName = "Katana"),
	Axe = 14			UMETA(DisplayName = "Axe"),
	Mace = 15			UMETA(DisplayName = "Mace"),
	Staff = 16			UMETA(DisplayName = "Staff"),
	Dagger = 17			UMETA(DisplayName = "Dagger"),
	Spear = 18			UMETA(DisplayName = "Spear"),
	Hammer = 19			UMETA(DisplayName = "Hammer"),
	Greatsword = 20		UMETA(DisplayName = "Greatsword"),
	Bow = 21			UMETA(DisplayName = "Bow"),
	Crossbow = 22		UMETA(DisplayName = "Crossbow"),
	Catalyst = 23		UMETA(DisplayName = "Catalyst"),
	BossWeapon = 24		UMETA(DisplayName = "Boss Weapon")
};

//////////////////////////////////////////////////////////////////////////
// ESLFKeyType
// Replaces: /Game/SoulslikeFramework/Enums/E_KeyType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFKeyType : uint8
{
	Bool = 0	UMETA(DisplayName = "Bool"),
	Class = 1	UMETA(DisplayName = "Class"),
	Enum = 2	UMETA(DisplayName = "Enum"),
	Float = 3	UMETA(DisplayName = "Float"),
	Int = 4		UMETA(DisplayName = "Int"),
	Name = 5	UMETA(DisplayName = "Name"),
	Object = 6	UMETA(DisplayName = "Object"),
	Rotator = 7	UMETA(DisplayName = "Rotator"),
	String = 8	UMETA(DisplayName = "String"),
	Vector = 9	UMETA(DisplayName = "Vector")
};

//////////////////////////////////////////////////////////////////////////
// ESLFLadderClimbState
// Replaces: /Game/SoulslikeFramework/Enums/E_LadderClimbState
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFLadderClimbState : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	Idle = 1				UMETA(DisplayName = "Idle"),
	ClimbingUp = 2			UMETA(DisplayName = "Climbing Up"),
	ClimbingDown = 3		UMETA(DisplayName = "Climbing Down"),
	MountTop = 4			UMETA(DisplayName = "Mount Top"),
	MountBottom = 5			UMETA(DisplayName = "Mount Bottom"),
	DismountTop = 6			UMETA(DisplayName = "Dismount Top"),
	DismountBottom = 7		UMETA(DisplayName = "Dismount Bottom"),
	ClimbLoop = 8			UMETA(DisplayName = "Climb Loop"),
	ClimbOutFromTop = 9		UMETA(DisplayName = "Climb Out From Top"),
	ClimbIntoFromTop = 10	UMETA(DisplayName = "Climb Into Ladder From Top")
};

//////////////////////////////////////////////////////////////////////////
// ESLFLightningMode
// Replaces: /Game/SoulslikeFramework/Enums/E_LightningMode
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFLightningMode : uint8
{
	Exterior = 0	UMETA(DisplayName = "Exterior Lightning"),
	Interior = 1	UMETA(DisplayName = "Interior Lightning")
};

//////////////////////////////////////////////////////////////////////////
// ESLFMontageSection
// Replaces: /Game/SoulslikeFramework/Enums/E_MontageSection
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFMontageSection : uint8
{
	Light_01 = 0,
	Light_02 = 1,
	Light_03 = 2,
	Light_04 = 3,
	Light_05 = 4,
	Light_06 = 5,
	Light_07 = 6,
	Light_08 = 7,
	Light_09 = 8,
	Light_10 = 9,
	Light_11 = 10,
	Light_12 = 11,
	Light_13 = 12,
	Light_14 = 13,
	Light_15 = 14,
	Heavy_01 = 15,
	Heavy_02 = 16,
	Heavy_03 = 17,
	Heavy_04 = 18,
	Heavy_05 = 19,
	Heavy_06 = 20,
	Heavy_07 = 21,
	Heavy_08 = 22,
	Heavy_09 = 23,
	Heavy_10 = 24,
	Heavy_11 = 25,
	Heavy_12 = 26,
	Heavy_13 = 27,
	Heavy_14 = 28,
	Heavy_15 = 29,
	Charge_01 = 30,
	Charge_02 = 31,
	Charge_03 = 32,
	Charge_04 = 33,
	Charge_05 = 34,
	Charge_06 = 35,
	Charge_07 = 36,
	Charge_08 = 37,
	Charge_09 = 38,
	Charge_10 = 39
};

//////////////////////////////////////////////////////////////////////////
// ESLFMovementType
// Replaces: /Game/SoulslikeFramework/Enums/E_MovementType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFMovementType : uint8
{
	Walk = 0	UMETA(DisplayName = "Walk"),
	Run = 1		UMETA(DisplayName = "Run"),
	Sprint = 2	UMETA(DisplayName = "Sprint")
};

//////////////////////////////////////////////////////////////////////////
// ESLFNpcState
// Replaces: /Game/SoulslikeFramework/Enums/E_NpcState
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFNpcState : uint8
{
	NotSpawned = 0	UMETA(DisplayName = "NotSpawned"),
	Spawned = 1		UMETA(DisplayName = "Spawned"),
	Inactive = 2	UMETA(DisplayName = "Inactive"),
	Active = 3		UMETA(DisplayName = "Active"),
	Hostile = 4		UMETA(DisplayName = "Hostile"),
	Friendly = 5	UMETA(DisplayName = "Friendly")
};

//////////////////////////////////////////////////////////////////////////
// ESLFOverlayState
// Replaces: /Game/SoulslikeFramework/Enums/E_OverlayState
// CRITICAL: Values MUST match AnimGraph BlendListByEnum BlendPose indices:
//   BlendPose_0 = Unarmed, BlendPose_1 = OneHanded,
//   BlendPose_2 = TwoHanded, BlendPose_3 = Shield
// NOTE: This differs from E_OverlayState.json because AnimGraph wiring
//       was designed with this order, not the Blueprint enum order.
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFOverlayState : uint8
{
	Unarmed = 0		UMETA(DisplayName = "Unarmed"),
	OneHanded = 1	UMETA(DisplayName = "OneHanded"),
	TwoHanded = 2	UMETA(DisplayName = "TwoHanded"),
	Shield = 3		UMETA(DisplayName = "Shield"),
	DualWield = 4	UMETA(DisplayName = "DualWield")
};

//////////////////////////////////////////////////////////////////////////
// ESLFProgress
// Replaces: /Game/SoulslikeFramework/Enums/E_Progress
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFProgress : uint8
{
	NotStarted = 0	UMETA(DisplayName = "NotStarted"),
	InProgress = 1	UMETA(DisplayName = "InProgress"),
	Completed = 2	UMETA(DisplayName = "Completed")
};

//////////////////////////////////////////////////////////////////////////
// ESLFSaveBehavior
// Replaces: /Game/SoulslikeFramework/Enums/E_SaveBehavior
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFSaveBehavior : uint8
{
	TriggerAutosaveTimer = 0	UMETA(DisplayName = "Trigger Autosave Timer"),
	SaveInstantly = 1			UMETA(DisplayName = "Save Instantly")
};

//////////////////////////////////////////////////////////////////////////
// ESLFSettingCategory
// Replaces: /Game/SoulslikeFramework/Enums/E_SettingCategory
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFSettingCategory : uint8
{
	GameOptions = 0		UMETA(DisplayName = "Game Options"),
	CameraOptions = 1	UMETA(DisplayName = "Camera Options"),
	DisplayOptions = 2	UMETA(DisplayName = "Display Options"),
	SoundOptions = 3	UMETA(DisplayName = "Sound Options"),
	KeybindOptions = 4	UMETA(DisplayName = "Keybind Options"),
	QuitGame = 5		UMETA(DisplayName = "Quit Game")
};

//////////////////////////////////////////////////////////////////////////
// ESLFSettingEntry
// Replaces: /Game/SoulslikeFramework/Enums/E_SettingEntry
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFSettingEntry : uint8
{
	DropDown = 0		UMETA(DisplayName = "Drop Down"),
	SingleButton = 1	UMETA(DisplayName = "Single Button"),
	DoubleButton = 2	UMETA(DisplayName = "Double Button"),
	Slider = 3			UMETA(DisplayName = "Slider"),
	InputKeySelector = 4 UMETA(DisplayName = "Input Key Selector")
};

//////////////////////////////////////////////////////////////////////////
// ESLFStatCategory
// Replaces: /Game/SoulslikeFramework/Enums/E_StatCategory
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFStatCategory : uint8
{
	None = 0		UMETA(DisplayName = "None"),
	Primary = 1		UMETA(DisplayName = "Primary"),
	Secondary = 2	UMETA(DisplayName = "Secondary"),
	Attack = 3		UMETA(DisplayName = "Attack"),
	Magic = 4		UMETA(DisplayName = "Magic"),
	Defense = 5		UMETA(DisplayName = "Defense"),
	Misc = 6		UMETA(DisplayName = "Misc")
};

//////////////////////////////////////////////////////////////////////////
// ESLFStatScaling
// Replaces: /Game/SoulslikeFramework/Enums/E_StatScaling
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFStatScaling : uint8
{
	S = 0	UMETA(DisplayName = "S"),
	A = 1	UMETA(DisplayName = "A"),
	B = 2	UMETA(DisplayName = "B"),
	C = 3	UMETA(DisplayName = "C"),
	D = 4	UMETA(DisplayName = "D"),
	E = 5	UMETA(DisplayName = "E")
};

//////////////////////////////////////////////////////////////////////////
// ESLFTraceType
// Replaces: /Game/SoulslikeFramework/Enums/E_TraceType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFTraceType : uint8
{
	RightHand = 0	UMETA(DisplayName = "Right Hand"),
	LeftHand = 1	UMETA(DisplayName = "Left Hand"),
	Both = 2		UMETA(DisplayName = "Both")
};

//////////////////////////////////////////////////////////////////////////
// ESLFVendorType
// Replaces: /Game/SoulslikeFramework/Enums/E_VendorType
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFVendorType : uint8
{
	Buy = 0		UMETA(DisplayName = "Buy"),
	Sell = 1	UMETA(DisplayName = "Sell")
};

//////////////////////////////////////////////////////////////////////////
// ESLFWeaponAbilityHandle
// Replaces: /Game/SoulslikeFramework/Enums/E_WeaponAbilityHandle
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFWeaponAbilityHandle : uint8
{
	RightHand = 0	UMETA(DisplayName = "Right Hand"),
	LeftHand = 1	UMETA(DisplayName = "Left Hand")
};

//////////////////////////////////////////////////////////////////////////
// ESLFWorldMeshStyle
// Replaces: /Game/SoulslikeFramework/Enums/E_WorldMeshStyle
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFWorldMeshStyle : uint8
{
	ShowMesh = 0	UMETA(DisplayName = "Show Mesh"),
	ShowEffect = 1	UMETA(DisplayName = "Show Effect")
};

//////////////////////////////////////////////////////////////////////////
// ESLFComparisonMethod
// For AI rule comparisons (matches Blueprint EComparisonMethod)
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFComparisonMethod : uint8
{
	Equal = 0			UMETA(DisplayName = "Equal"),
	NotEqual = 1		UMETA(DisplayName = "Not Equal"),
	Less = 2			UMETA(DisplayName = "Less"),
	LessOrEqual = 3		UMETA(DisplayName = "Less Or Equal"),
	Greater = 4			UMETA(DisplayName = "Greater"),
	GreaterOrEqual = 5	UMETA(DisplayName = "Greater Or Equal")
};

//////////////////////////////////////////////////////////////////////////

// Type aliases for Blueprint enum name compatibility
// These allow generated code to use Blueprint names (E_*)
// while referencing the C++ enums (ESLF*)
// NOTE: Only aliases for DEFINED enums are included
//////////////////////////////////////////////////////////////////////////

using E_ActionWeaponSlot = ESLFActionWeaponSlot;
using E_AI_BossEncounterType = ESLFAIBossEncounterType;
using E_AI_Senses = ESLFAISenses;
using E_AI_StateHandle = ESLFAIStateHandle;
using E_AI_States = ESLFAIStates;
using E_AI_StrafeLocations = ESLFAIStrafeLocations;
using E_AttackPower = ESLFAttackPower;
using E_Direction = ESLFDirection;
using E_DotProductThreshold = ESLFDotProductThreshold;
using E_HitReactType = ESLFHitReactType;
using E_LightningMode = ESLFLightningMode;
using E_OverlayState = ESLFOverlayState;
using E_WeaponAbilityHandle = ESLFWeaponAbilityHandle;
using E_WorldMeshStyle = ESLFWorldMeshStyle;
using E_ComparisonMethod = ESLFComparisonMethod;
using E_VendorType = ESLFVendorType;
using E_Progress = ESLFProgress;
using E_MovementType = ESLFMovementType;
using E_TraceType = ESLFTraceType;
using E_MontageSection = ESLFMontageSection;
using E_StatCategory = ESLFStatCategory;
using E_DebugWindowScaleType = ESLFDebugWindowScaleType;
using E_ItemCategory = ESLFItemCategory;
using E_InventorySlotType = ESLFInventorySlotType;
using E_SaveBehavior = ESLFSaveBehavior;
using E_EnclosureLevel = ESLFEnclosureLevel;

//////////////////////////////////////////////////////////////////////////
// ESLFZoneDifficulty
// Difficulty rating for game zones
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFZoneDifficulty : uint8
{
	Tutorial = 0	UMETA(DisplayName = "Tutorial"),
	Easy = 1		UMETA(DisplayName = "Easy"),
	Medium = 2		UMETA(DisplayName = "Medium"),
	Hard = 3		UMETA(DisplayName = "Hard"),
	VeryHard = 4	UMETA(DisplayName = "Very Hard"),
	Final = 5		UMETA(DisplayName = "Final")
};

//////////////////////////////////////////////////////////////////////////
// ESLFFaction
// World factions that control different zones
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFFaction : uint8
{
	None = 0			UMETA(DisplayName = "None"),
	Withered = 1		UMETA(DisplayName = "The Withered"),
	IronCovenant = 2	UMETA(DisplayName = "The Iron Covenant"),
	Veilborn = 3		UMETA(DisplayName = "The Veilborn"),
	AshenOrder = 4		UMETA(DisplayName = "The Ashen Order"),
	Hollow = 5			UMETA(DisplayName = "The Hollow"),
	Neutral = 6			UMETA(DisplayName = "Neutral")
};

//////////////////////////////////////////////////////////////////////////
// ESLFEnemyRank
// Enemy rank within a zone (affects loot, difficulty scaling)
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFEnemyRank : uint8
{
	Regular = 0		UMETA(DisplayName = "Regular"),
	Elite = 1		UMETA(DisplayName = "Elite"),
	MiniBoss = 2	UMETA(DisplayName = "Mini-Boss"),
	Boss = 3		UMETA(DisplayName = "Boss")
};

using E_ZoneDifficulty = ESLFZoneDifficulty;
using E_Faction = ESLFFaction;
using E_EnemyRank = ESLFEnemyRank;
