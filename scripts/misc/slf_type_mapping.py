"""
Shared type mapping module for SoulslikeFramework Blueprint-to-C++ migration.
All generators should import and use these mappings for consistent type conversion.
"""

# Blueprint enum name -> C++ enum name mapping (UHT doesn't support using aliases)
ENUM_MAP = {
    "E_ActionWeaponSlot": "ESLFActionWeaponSlot",
    "E_AI_BossEncounterType": "ESLFAIBossEncounterType",
    "E_AI_Senses": "ESLFAISenses",
    "E_AI_StateHandle": "ESLFAIStateHandle",
    "E_AI_States": "ESLFAIStates",
    "E_AI_StrafeLocations": "ESLFAIStrafeLocations",
    "E_AttackPower": "ESLFAttackPower",
    "E_CalculationType": "ESLFCalculationType",
    "E_DebugWindowScaleType": "ESLFDebugWindowScaleType",
    "E_Direction": "ESLFDirection",
    "E_DotProductThreshold": "ESLFDotProductThreshold",
    "E_EnclosureLevel": "ESLFEnclosureLevel",
    "E_ExecutionType": "ESLFExecutionType",
    "E_FadeTypes": "ESLFFadeTypes",
    "E_HitReactType": "ESLFHitReactType",
    "E_InputAction": "ESLFInputAction",
    "E_InventoryAmountedActionType": "ESLFInventoryAmountedActionType",
    "E_InventorySlotType": "ESLFInventorySlotType",
    "E_ItemCategory": "ESLFItemCategory",
    "E_ItemSubCategory": "ESLFItemSubCategory",
    "E_KeyType": "ESLFKeyType",
    "E_LadderClimbState": "ESLFLadderClimbState",
    "E_LightningMode": "ESLFLightningMode",
    "E_MontageSection": "ESLFMontageSection",
    "E_MovementType": "ESLFMovementType",
    "E_NpcState": "ESLFNpcState",
    "E_OverlayState": "ESLFOverlayState",
    "E_Progress": "ESLFProgress",
    "E_SaveBehavior": "ESLFSaveBehavior",
    "E_SettingCategory": "ESLFSettingCategory",
    "E_SettingEntry": "ESLFSettingEntry",
    "E_StatCategory": "ESLFStatCategory",
    "E_StatScaling": "ESLFStatScaling",
    "E_TraceType": "ESLFTraceType",
    "E_VendorType": "ESLFVendorType",
    "E_WeaponAbilityHandle": "ESLFWeaponAbilityHandle",
    "E_WorldMeshStyle": "ESLFWorldMeshStyle",
    "E_ComparisonMethod": "ESLFComparisonMethod",
    "E_ValueType": "ESLFValueType",
    "ECompareOperation1Enum": "ESLFCompareOperation",
}

# Blueprint struct name -> C++ struct name mapping
# Include both with and without F prefix since JSON exports vary
STRUCT_MAP = {
    # Without F prefix
    "Bool": "FSLFBool",
    "Int": "FSLFInt",
    "Float": "FSLFFloat",
    "String": "FSLFString",
    "Name": "FSLFName",
    "Class": "FSLFClass",
    "Object": "FSLFObject",
    "EnumByte": "FSLFEnumByte",
    "ActorClass": "FSLFActorClass",
    "ActionsData": "FSLFActionsData",
    "DodgeMontages": "FSLFDodgeMontages",
    "AnimationData": "FSLFAnimationData",
    "Montage": "FSLFMontage",
    "ExecutionInfo": "FSLFExecutionInfo",
    "ExecutionAnimInfo": "FSLFExecutionAnimInfo",
    "ExecutionTypeWrapper": "FSLFExecutionTypeWrapper",
    "WeaponAttackPower": "FSLFWeaponAttackPower",
    "SprintCost": "FSLFSprintCost",
    "StatEntry": "FSLFStatEntry",
    "CardinalData": "FSLFCardinalData",
    "AiRuleDistance": "FSLFAiRuleDistance",
    "AiRuleStat": "FSLFAiRuleStat",
    "AiAttackEntry": "FSLFAiAttackEntry",
    "AiBossPhase": "FSLFAiBossPhase",
    "AiPatrolPathInfo": "FSLFAiPatrolPathInfo",
    "AiSenseLocationInfo": "FSLFAiSenseLocationInfo",
    "AiSenseTargetInfo": "FSLFAiSenseTargetInfo",
    "AiStrafeInfo": "FSLFAiStrafeInfo",
    "ItemCategoryData": "FSLFItemCategoryData",
    "EquipmentSocketInfo": "FSLFEquipmentSocketInfo",
    "EquipmentStat": "FSLFEquipmentStat",
    "EquipmentWeaponStatInfo": "FSLFEquipmentWeaponStatInfo",
    "EquipmentSlot": "FSLFEquipmentSlot",
    "WorldMeshInfo": "FSLFWorldMeshInfo",
    "CraftingInfo": "FSLFCraftingInfo",
    "EquipmentInfo": "FSLFEquipmentInfo",
    "ItemInfo": "FSLFItemInfo",
    "ItemInfoCount": "FSLFItemInfoCount",
    "CurrentEquipment": "FSLFCurrentEquipment",
    "InventoryCategory": "FSLFInventoryCategory",
    "InventoryItem": "FSLFInventoryItem",
    "LootItem": "FSLFLootItem",
    "WeightedLoot": "FSLFWeightedLoot",
    "FlaskData": "FSLFFlaskData",
    "VendorItems": "FSLFVendorItems",
    "ItemWheelNextSlotInfo": "FSLFItemWheelNextSlotInfo",
    "DialogGameplayEvent": "FSLFDialogGameplayEvent",
    "DialogEntry": "FSLFDialogEntry",
    "DialogProgress": "FSLFDialogProgress",
    "DialogRequirement": "FSLFDialogRequirement",
    "SaveData": "FSLFSaveData",
    "SaveGameInfo": "FSLFSaveGameInfo",
    "ClassSaveInfo": "FSLFClassSaveInfo",
    "WorldSaveInfo": "FSLFWorldSaveInfo",
    "ProgressSaveInfo": "FSLFProgressSaveInfo",
    "InventoryItemsSaveInfo": "FSLFInventoryItemsSaveInfo",
    "EquipmentItemsSaveInfo": "FSLFEquipmentItemsSaveInfo",
    "ItemWheelSaveInfo": "FSLFItemWheelSaveInfo",
    "InteractableStateSaveInfo": "FSLFInteractableStateSaveInfo",
    "NpcSaveInfo": "FSLFNpcSaveInfo",
    "NpcVendorSaveInfo": "FSLFNpcVendorSaveInfo",
    "SpawnedActorSaveInfo": "FSLFSpawnedActorSaveInfo",
    "DoorLockInfo": "FSLFDoorLockInfo",
    "StatusEffectVfxInfo": "FSLFStatusEffectVfxInfo",
    "StatusEffectRankInfo": "FSLFStatusEffectRankInfo",
    "StatusEffectApplication": "FSLFStatusEffectApplication",
    "StatusEffectFrostbiteExample": "FSLFStatusEffectFrostbiteExample",
    "StatusEffectPlagueExample": "FSLFStatusEffectPlagueExample",
    "DayNightInfo": "FSLFDayNightInfo",
    "ProgressWrapper": "FSLFProgressWrapper",
    "RequiredCurrencyForLevel": "FSLFRequiredCurrencyForLevel",
    "SkeletalMeshData": "FSLFSkeletalMeshData",
    "KeyMappingCorrelation": "FSLFKeyMappingCorrelation",
    "LoadingScreenTip": "FSLFLoadingScreenTip",
    "CreditsNames": "FSLFCreditsNames",
    "CreditsExtra": "FSLFCreditsExtra",
    "CreditsEntry": "FSLFCreditsEntry",
    "Colour": "FSLFColour",
    "RoomSettings": "FSLFRoomSettings",
    "S_RoomSettings": "FSLFRoomSettings",
    # With F prefix (from JSON exports)
    "FDodgeMontages": "FSLFDodgeMontages",
    "FWeaponAttackPower": "FSLFWeaponAttackPower",
    "FDoorLockInfo": "FSLFDoorLockInfo",
    "FSaveGameInfo": "FSLFSaveGameInfo",
    "FSaveData": "FSLFSaveData",
    "FDayNightInfo": "FSLFDayNightInfo",
    "FAiStrafeInfo": "FSLFAiStrafeInfo",
    "FAiBossPhase": "FSLFAiBossPhase",
    "FAiRuleStat": "FSLFAiRuleStat",
    "FDialogEntry": "FSLFDialogEntry",
    "FDialogGameplayEvent": "FSLFDialogGameplayEvent",
    "FItemInfo": "FSLFItemInfo",
    "FInventoryCategory": "FSLFInventoryCategory",
    "FInventoryItemsSaveInfo": "FSLFInventoryItemsSaveInfo",
    "FEquipmentItemsSaveInfo": "FSLFEquipmentItemsSaveInfo",
    "FLootItem": "FSLFLootItem",
    "FCurrentEquipment": "FSLFCurrentEquipment",
    "FS_RoomSettings": "FSLFRoomSettings",
    # Stat types from SLFStatTypes.h (no prefix change needed)
    "Regen": "FRegen",
    "AffectedStat": "FAffectedStat",
    "AffectedStats": "FAffectedStats",
    "StatBehavior": "FStatBehavior",
    "StatInfo": "FStatInfo",
    "StatOverride": "FStatOverride",
    "StatChange": "FStatChange",
    "StatChangePercent": "FStatChangePercent",
    "LevelChangeData": "FLevelChangeData",
    "StatusEffectStatChanges": "FStatusEffectStatChanges",
    "StatusEffectTick": "FStatusEffectTick",
    "StatusEffectOneShotAndTick": "FStatusEffectOneShotAndTick",
}

# Engine struct types (from UE core)
ENGINE_STRUCT_MAP = {
    "Transform": "FTransform",
    "Vector": "FVector",
    "Vector2D": "FVector2D",
    "Rotator": "FRotator",
    "GameplayTag": "FGameplayTag",
    "GameplayTagContainer": "FGameplayTagContainer",
    "LinearColor": "FLinearColor",
    "Color": "FColor",
    "Guid": "FGuid",
    "DateTime": "FDateTime",
    "Timespan": "FTimespan",
    "IntPoint": "FIntPoint",
    "SoftObjectPath": "FSoftObjectPath",
    "SoftClassPath": "FSoftClassPath",
    "InstancedStruct": "FInstancedStruct",
    "TableRowHandle": "FDataTableRowHandle",
    "PrimaryAssetId": "FPrimaryAssetId",
    "PrimaryAssetType": "FPrimaryAssetType",
}

# Known Actor-based Blueprint classes (for A prefix)
# These extend AActor or its subclasses (APawn, ACharacter, etc.)
ACTOR_BASED_BLUEPRINTS = {
    # Characters
    "B_Soulslike_Character", "B_Soulslike_Player", "B_Soulslike_Enemy", "B_Soulslike_NPC", "B_Soulslike_Boss",
    # Interactables (Actors)
    "B_RestingPoint", "B_PatrolPath", "B_Interactable", "B_PickupItem", "B_Door", "B_Ladder", "B_Container",
    # Effects and projectiles
    "B_BaseProjectile", "B_SkyManager", "B_WorldItem", "B_SpawnedActor", "B_EffectActor", "B_AbilityEffectBase",
    # Game framework
    "AIC_SoulslikeFramework", "GM_Soulslike", "GS_Soulslike", "PC_Soulslike", "PS_Soulslike",
    # Item actors (inherit from Actor)
    "B_Item", "B_Item_Armor", "B_Item_Weapon", "B_Item_Flask", "B_Item_Consumable",
    "B_Item_Key", "B_Item_Rune", "B_Item_Equipment", "B_Item_Equipment_Attachment",
    # Demo/utility actors
    "B_DemoDisplay", "B_DemoRoom", "B_CombatWall", "B_Teleporter", "B_Lever",
    # Showcase actors
    "B_ShowcaseEnemy_StatDisplay",
}

# Known UObject-based Blueprint classes (for U prefix)
# These extend UObject (not AActor)
UOBJECT_BASED_BLUEPRINTS = {
    # Gameplay data classes (inherit from Object)
    "B_Stat", "B_Action", "B_Buff", "B_StatusEffect",
    # Action subclasses (inherit from B_Action -> Object)
    "B_Action_Backstab", "B_Action_ComboHeavy", "B_Action_ComboLight_L", "B_Action_ComboLight_R",
    "B_Action_Crouch", "B_Action_Dodge", "B_Action_DrinkFlask_HP", "B_Action_DualWieldAttack",
    "B_Action_Execute", "B_Action_GuardCancel", "B_Action_GuardEnd", "B_Action_GuardStart",
    "B_Action_HeavyAttack", "B_Action_Jump", "B_Action_Kick", "B_Action_LadderClimb",
    "B_Action_LightAttack", "B_Action_OpeningAttack_L", "B_Action_OpeningAttack_R",
    "B_Action_Riposte", "B_Action_Roll", "B_Action_Sprint", "B_Action_Toggle2H",
    "B_Action_WeaponAbility", "B_Action_WingedLightAttack",
    # Primary Data Assets (use UPrimaryDataAsset as fallback)
    "PDA_Item", "PDA_Action", "PDA_Buff", "PDA_StatusEffect", "PDA_WeaponAnimset", "PDA_WeaponAbility",
    "PDA_AI_Ability", "PDA_CombatReactionAnimData", "PDA_Dialog", "PDA_Vendor", "PDA_DayNight",
    "PDA_CustomSettings", "PDA_DefaultMeshData", "PDA_BaseCharacterInfo", "PDA_MovementSpeedData",
    "PDA_AnimData", "PDA_Credits", "PDA_LoadingScreens", "PDA_LadderAnimData", "PDA_MainMenuData",
    # Widgets
    "W_Inventory", "W_InventorySlot", "W_HUD", "W_Equipment",
    # Save Games
    "SG_SoulslikeFramework", "SG_SaveSlots",
    # Blueprint Function Libraries
    "BFL_Helper", "BFL_Calculations",
}

# Primary Data Asset classes - use UPrimaryDataAsset as the C++ type
# since these are Blueprint PDAs without C++ counterparts yet
PDA_CLASSES = {
    "PDA_Item", "PDA_Action", "PDA_Buff", "PDA_StatusEffect", "PDA_WeaponAnimset", "PDA_WeaponAbility",
    "PDA_AI_Ability", "PDA_CombatReactionAnimData", "PDA_Dialog", "PDA_Vendor", "PDA_DayNight",
    "PDA_CustomSettings", "PDA_DefaultMeshData", "PDA_BaseCharacterInfo", "PDA_MovementSpeedData",
    "PDA_AnimData", "PDA_Credits", "PDA_LoadingScreens", "PDA_LadderAnimData", "PDA_MainMenuData",
}

# Type to include mapping for engine types that need explicit includes
# NOTE: Module includes use module name without "Public/" - that's for source paths
TYPE_INCLUDES = {
    # BehaviorTree module
    "UBehaviorTree": "BehaviorTree/BehaviorTree.h",
    "UBlackboardData": "BehaviorTree/BlackboardData.h",
    "UBlackboardComponent": "BehaviorTree/BlackboardComponent.h",
    # EnhancedInput module
    "UInputMappingContext": "InputMappingContext.h",
    "UInputAction": "InputAction.h",
    # LevelSequence module
    "ULevelSequencePlayer": "LevelSequencePlayer.h",
    "ULevelSequence": "LevelSequence.h",
    "ALevelSequenceActor": "LevelSequenceActor.h",
    # Kismet
    "EDrawDebugTrace": "Kismet/KismetSystemLibrary.h",
    # Engine
    "FSkeletalMeshMergeParams": "SkeletalMeshMerge.h",
    # MovieScene module
    "FMovieSceneSequencePlaybackSettings": "MovieSceneSequencePlaybackSettings.h",
    # Materials
    "UMaterialInterface": "Materials/MaterialInterface.h",
    "UMaterialInstanceDynamic": "Materials/MaterialInstanceDynamic.h",
    # Niagara module
    "UNiagaraSystem": "NiagaraSystem.h",
    "UNiagaraComponent": "NiagaraComponent.h",
    # Sound
    "USoundBase": "Sound/SoundBase.h",
    "USoundCue": "Sound/SoundCue.h",
    # Components
    "UWidgetComponent": "Components/WidgetComponent.h",
    "UCameraComponent": "Camera/CameraComponent.h",
    "USpringArmComponent": "GameFramework/SpringArmComponent.h",
    "UCharacterMovementComponent": "GameFramework/CharacterMovementComponent.h",
    "UCapsuleComponent": "Components/CapsuleComponent.h",
    "USkeletalMeshComponent": "Components/SkeletalMeshComponent.h",
    "UStaticMeshComponent": "Components/StaticMeshComponent.h",
    "UBoxComponent": "Components/BoxComponent.h",
    "USphereComponent": "Components/SphereComponent.h",
    "UTimelineComponent": "Components/TimelineComponent.h",
    "UAudioComponent": "Components/AudioComponent.h",
    "UArrowComponent": "Components/ArrowComponent.h",
    "UDecalComponent": "Components/DecalComponent.h",
    "UPointLightComponent": "Components/PointLightComponent.h",
    "USpotLightComponent": "Components/SpotLightComponent.h",
    "UTextRenderComponent": "Components/TextRenderComponent.h",
    "UBillboardComponent": "Components/BillboardComponent.h",
    "UParticleSystemComponent": "Particles/ParticleSystemComponent.h",
    "UCurveFloat": "Curves/CurveFloat.h",
    "UCurveLinearColor": "Curves/CurveLinearColor.h",
    "UCurveVector": "Curves/CurveVector.h",
    "UUserWidget": "Blueprint/UserWidget.h",
}


def get_enum_cpp_name(blueprint_name: str, wrap_engine_enums: bool = True) -> str:
    """Convert Blueprint enum name to C++ enum name.

    Args:
        blueprint_name: The enum name from Blueprint JSON
        wrap_engine_enums: If True, engine enums get TEnumAsByte wrapper for UPROPERTY use
    """
    # Check for engine enums needing TEnumAsByte wrapper
    if wrap_engine_enums and blueprint_name in ENGINE_ENUMS_NEEDING_WRAPPER:
        return ENGINE_ENUMS_NEEDING_WRAPPER[blueprint_name]

    # Direct lookup in our enum map
    if blueprint_name in ENUM_MAP:
        return ENUM_MAP[blueprint_name]

    # Try with E_ prefix
    e_name = f"E_{blueprint_name}" if not blueprint_name.startswith("E_") else blueprint_name
    if e_name in ENUM_MAP:
        return ENUM_MAP[e_name]

    # Return as-is if already ESL-prefixed or unknown
    if blueprint_name.startswith("ESLF"):
        return blueprint_name

    # Unknown - return with E prefix
    return blueprint_name if blueprint_name.startswith("E") else f"E{blueprint_name}"


def get_struct_cpp_name(blueprint_name: str) -> str:
    """Convert Blueprint struct name to C++ struct name."""
    # Engine types first
    if blueprint_name in ENGINE_STRUCT_MAP:
        return ENGINE_STRUCT_MAP[blueprint_name]

    # Our custom types
    if blueprint_name in STRUCT_MAP:
        return STRUCT_MAP[blueprint_name]

    # Already has F prefix
    if blueprint_name.startswith("F"):
        # Check if it's in STRUCT_MAP without the F
        without_f = blueprint_name[1:]
        if without_f in STRUCT_MAP:
            return STRUCT_MAP[without_f]
        return blueprint_name

    # Unknown - return with F prefix
    return f"F{blueprint_name}"


def get_class_prefix(blueprint_name: str) -> str:
    """Determine if a Blueprint class should use A or U prefix."""
    # Strip _C suffix if present
    name = blueprint_name[:-2] if blueprint_name.endswith("_C") else blueprint_name

    if name in ACTOR_BASED_BLUEPRINTS:
        return "A"
    if name in UOBJECT_BASED_BLUEPRINTS:
        return "U"

    # Default based on naming patterns
    if name.startswith("B_") or name.startswith("AIC_") or name.startswith("GM_") or name.startswith("GS_") or name.startswith("PC_") or name.startswith("PS_"):
        return "A"
    if name.startswith("W_") or name.startswith("PDA_") or name.startswith("UPDA_"):
        return "U"

    return "U"  # Default to UObject


# Engine enums that require TEnumAsByte wrapper (old-style UE enums)
# These cannot be used directly as UPROPERTY types
ENGINE_ENUMS_NEEDING_WRAPPER = {
    "EObjectTypeQuery": "TEnumAsByte<EObjectTypeQuery>",
    "ECollisionChannel": "TEnumAsByte<ECollisionChannel>",
    "EDrawDebugTrace": "TEnumAsByte<EDrawDebugTrace::Type>",
    "EPhysicalSurface": "TEnumAsByte<EPhysicalSurface>",
    "EVisibilityBasedAnimTickOption": "TEnumAsByte<EVisibilityBasedAnimTickOption>",
    "ECollisionEnabled": "TEnumAsByte<ECollisionEnabled::Type>",
    "EComponentMobility": "TEnumAsByte<EComponentMobility::Type>",
    # UMG/Slate enums
    "EHorizTextAligment": "TEnumAsByte<EHorizTextAligment>",
    "EVerticalTextAligment": "TEnumAsByte<EVerticalTextAligment>",
    "EHorizontalAlignment": "TEnumAsByte<EHorizontalAlignment>",
    "EVerticalAlignment": "TEnumAsByte<EVerticalAlignment>",
    "ETextJustify": "TEnumAsByte<ETextJustify::Type>",
    "ESlateVisibility": "TEnumAsByte<ESlateVisibility>",
    # Namespace enums (Type is inside namespace)
    "EWindowMode": "EWindowMode::Type",
    # UMG animation namespace enum (Type is inside namespace)
    "EUMGSequencePlayMode": "EUMGSequencePlayMode::Type",
    # Text alignment enums - UHT requires TEnumAsByte wrapper for old-style enums
    "EHorizTextAligment": "TEnumAsByte<ETextJustify::Type>",  # Blueprint uses old name
}


# Engine class name mappings (for correct A/U prefix)
ENGINE_CLASS_MAP = {
    # Actor classes
    "Controller": "AController",
    "AIController": "AAIController",
    "PlayerController": "APlayerController",
    "Actor": "AActor",
    "Pawn": "APawn",
    "Character": "ACharacter",
    "GameModeBase": "AGameModeBase",
    "GameStateBase": "AGameStateBase",
    "PlayerState": "APlayerState",
    # UObject classes
    "Object": "UObject",
    "GameInstance": "UGameInstance",
    "AnimInstance": "UAnimInstance",
    "UserWidget": "UUserWidget",
    "ActorComponent": "UActorComponent",
    "SceneComponent": "USceneComponent",
    "PrimitiveComponent": "UPrimitiveComponent",
    "StaticMeshComponent": "UStaticMeshComponent",
    "SkeletalMeshComponent": "USkeletalMeshComponent",
    "GeometryCollection": "UGeometryCollection",
    "GeometryCollectionComponent": "UGeometryCollectionComponent",
}

def get_class_cpp_name(blueprint_name: str, is_pointer: bool = True) -> str:
    """Convert Blueprint class reference to C++ class name."""
    # Strip _C suffix if present
    name = blueprint_name[:-2] if blueprint_name.endswith("_C") else blueprint_name

    # Check engine class mapping first
    if name in ENGINE_CLASS_MAP:
        cpp_name = ENGINE_CLASS_MAP[name]
        return f"{cpp_name}*" if is_pointer else cpp_name

    # Primary Data Assets - use base class since no C++ counterpart exists
    if name.startswith("PDA_") and name in PDA_CLASSES:
        cpp_name = "UPrimaryDataAsset"
        return f"{cpp_name}*" if is_pointer else cpp_name

    prefix = get_class_prefix(name)
    cpp_name = f"{prefix}{name}"
    return f"{cpp_name}*" if is_pointer else cpp_name


def get_default_return_value(ret_type: str) -> str:
    """Get the appropriate default return value for a C++ type.

    Args:
        ret_type: The C++ return type string

    Returns:
        A string representing the default return value for that type
    """
    if ret_type == "void":
        return ""
    # IMPORTANT: Check containers FIRST because TArray<UObject*> contains "*"
    elif ret_type.startswith("TArray<") or ret_type.startswith("TMap<") or ret_type.startswith("TSet<"):
        return "{}"  # Empty container
    elif "*" in ret_type:
        return "nullptr"
    elif ret_type == "bool":
        return "false"
    elif ret_type == "FString":
        return "FString()"
    elif ret_type == "FText":
        return "FText::GetEmpty()"
    elif ret_type == "FName":
        return "NAME_None"
    elif ret_type == "FGameplayTag":
        return "FGameplayTag()"
    elif ret_type == "FGameplayTagContainer":
        return "FGameplayTagContainer()"
    elif ret_type.startswith("TEnumAsByte<"):
        # Extract inner enum type and cast
        inner = ret_type.replace("TEnumAsByte<", "").rstrip(">")
        return f"TEnumAsByte<{inner}>(static_cast<{inner}>(0))"
    elif ret_type.startswith("E"):
        # Enum type - need explicit cast
        return f"static_cast<{ret_type}>(0)"
    elif ret_type.startswith("F"):
        return f"{ret_type}()"  # Struct default constructor
    elif ret_type in ("int32", "int64", "uint8", "uint32", "float", "double"):
        return "0"
    else:
        return "0"
