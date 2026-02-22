// SLFAutomationLibrary.h
// C++ Blueprint automation for Shell Pattern migration

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFAutomationLibrary.generated.h"

UCLASS()
class SLFCONVERSION_API USLFAutomationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// QUERY FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintVariables(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintFunctions(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintEventDispatchers(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static FString GetBlueprintParentClass(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static TArray<FString> GetBlueprintSCSComponents(UObject* BlueprintAsset);

	// SCS COMPONENT OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveSCSComponentByClass(UObject* BlueprintAsset, const FString& ComponentClassName);

	// REPARENTING
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ReparentBlueprint(UObject* BlueprintAsset, const FString& NewParentClassPath);

	// VARIABLE OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveVariable(UObject* BlueprintAsset, const FString& VariableName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveVariables(UObject* BlueprintAsset, const TArray<FString>& VariableNames);

	// Remove ALL Blueprint variables (for AnimBP reparenting to C++)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveAllVariables(UObject* BlueprintAsset);

	// FUNCTION OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveFunction(UObject* BlueprintAsset, const FString& FunctionName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveFunctions(UObject* BlueprintAsset, const TArray<FString>& FunctionNames);

	// EVENT DISPATCHER OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RemoveEventDispatcher(UObject* BlueprintAsset, const FString& DispatcherName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveEventDispatchers(UObject* BlueprintAsset, const TArray<FString>& DispatcherNames);

	// GRAPH CLEARING
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearEventGraphs(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearFunctionGraphs(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogic(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearAllBlueprintLogicNoCompile(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariables(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearGraphsKeepVariablesNoCompile(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool ClearConstructionScript(UObject* BlueprintAsset);

	// Clear widget delegate bindings from WidgetBlueprint (e.g., ComboBox OnGenerateItemWidget)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ClearWidgetDelegateBindings(UObject* WidgetBlueprintAsset);

	// COMPREHENSIVE MIGRATION
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool MigrateToShell(UObject* BlueprintAsset, const FString& NewParentClassPath, const TArray<FString>& VariablesToRemove, const TArray<FString>& FunctionsToRemove, const TArray<FString>& DispatchersToRemove);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool CompileAndSave(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RemoveImplementedInterfaces(UObject* BlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ReplaceVariableReferences(UObject* BlueprintAsset, FName OldVarName, FName NewVarName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool RenameVariable(UObject* BlueprintAsset, const FString& OldVarName, const FString& NewVarName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 RenameVariables(UObject* BlueprintAsset, const TArray<FString>& OldNames, const TArray<FString>& NewNames);

	// DATA ASSET OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyEquipSlotsToItem(const FString& ItemAssetPath, const TArray<FString>& SlotTags);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplySkeletalMeshInfoToItem(const FString& ItemAssetPath, const TArray<FString>& CharacterAssetNames, const TArray<FString>& MeshPaths);

	/**
	 * Apply Death montages TMap to a PDA_CombatReactionAnimData asset
	 * @param AnimsetPath - Path to the reaction animset data asset
	 * @param DirectionNames - Array of direction names (Fwd, Bwd, Left, Right, etc.)
	 * @param MontagePaths - Array of corresponding montage asset paths
	 * @return true if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyDeathMontagesToAnimset(const FString& AnimsetPath, const TArray<FString>& DirectionNames, const TArray<FString>& MontagePaths);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static FString GetWeaponOverlayTag(const FString& ItemAssetPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyWeaponOverlayTag(const FString& ItemAssetPath, const FString& OverlayTagString);

	/**
	 * Apply WeaponStatusEffectInfo TMap to a PDA_Item
	 * @param ItemAssetPath - Path to the item data asset
	 * @param StatusEffectPaths - Array of status effect data asset paths
	 * @param Ranks - Array of corresponding rank values (int32)
	 * @param BuildupAmounts - Array of corresponding buildup amounts (double)
	 * @return true if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyWeaponStatusEffects(const FString& ItemAssetPath, const TArray<FString>& StatusEffectPaths, const TArray<int32>& Ranks, const TArray<double>& BuildupAmounts);

	// ANIMBP FIX FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixPropertyAccessPaths(UObject* AnimBlueprintAsset, const FString& OldPropertyName, const FString& NewPropertyName);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixLinkedAnimLayers(UObject* AnimBlueprintAsset, const FString& InterfaceName, const FString& LinkedAnimGraphPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseAnimGraphNodes(UObject* AnimBlueprintAsset);

	// AAA-Quality LinkedAnimLayer functions using proper UE5 APIs
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseLinkedAnimLayerNodes(UObject* AnimBlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixLinkedAnimLayerGuid(UObject* AnimBlueprintAsset);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString GetBlueprintCompileErrors(UObject* BlueprintAsset);

	// BLUEPRINT STATE EXPORT/DIFF
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExportBlueprintState(UObject* BlueprintAsset, const FString& OutputFilePath = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExportAnimGraphState(UObject* AnimBlueprintAsset, const FString& OutputFilePath = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString DiffBlueprintStates(const FString& BeforeJsonPath, const FString& AfterJsonPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAnimBPFromReference(UObject* AnimBlueprintAsset, const FString& ReferenceJsonPath);

	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString GetNodeDetails(UObject* BlueprintAsset, const FString& NodeName);

	// AnimBP Diagnostic - finds all broken nodes, state machine states/transitions
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString DiagnoseAnimBP(UObject* AnimBlueprintAsset);

	// Get state machine structure (states and transitions)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString GetStateMachineStructure(UObject* AnimBlueprintAsset, const FString& StateMachineName);

	// Inspect transition graphs in detail (show all nodes for transitions matching keyword)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString InspectTransitionGraph(UObject* AnimBlueprintAsset, const FString& Keyword);

	// Fix broken transition boolean inputs by connecting to named variable
	// Example: FixTransitionBooleanInput(bp, "Transition_2", "A", "bIsAccelerating")
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixTransitionBooleanInput(UObject* AnimBlueprintAsset, const FString& TransitionGraphNameContains, const FString& BoolPinName, const FString& VariableName);

	// Auto-fix all broken transition inputs based on variable mapping
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAllBrokenTransitions(UObject* AnimBlueprintAsset);

	// Repair disconnected VariableGet nodes in transition graphs
	// Finds VariableGet nodes whose outputs are not connected and wires them to appropriate targets
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 RepairTransitionWiring(UObject* AnimBlueprintAsset);

	// Rebuild broken VariableGet nodes that reference invalid variables
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixBrokenVariableGetNodes(UObject* AnimBlueprintAsset);

	// Fix BlendListByEnum nodes by binding their ActiveEnumValue to a variable
	// NodeIdentifier can be partial match on node name, position, or index
	// VariableName is the Blueprint variable to bind to (e.g., "LeftHandOverlayState_0")
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixBlendListByEnumBinding(UObject* AnimBlueprintAsset, int32 NodeIndex, const FString& VariableName);

	// Auto-fix all BlendListByEnum nodes for overlay states
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAllBlendListByEnumBindings(UObject* AnimBlueprintAsset);

	// Redirect property bindings from old Blueprint variables (_0 suffix) to C++ properties
	// Fixes compiler errors like "Invalid field 'LeftHandOverlayState_0' found in property path"
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 RedirectOverlayStateBindings(UObject* AnimBlueprintAsset);

	// Migrate BlendListByEnum nodes from Blueprint enum to C++ enum
	// This properly replaces nodes: saves connections, clears enum, sets new enum, reconstructs, restores connections
	// OldEnumPath: e.g., "/Game/SoulslikeFramework/Enums/E_OverlayState.E_OverlayState"
	// NewEnumPath: e.g., "/Script/SLFConversion.ESLFOverlayState"
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 MigrateBlendListByEnumToCpp(UObject* AnimBlueprintAsset, const FString& OldEnumPath, const FString& NewEnumPath);

	// Fix disconnected boolean variable getters in AnimGraph (bIsBlocking, bIsFalling)
	// These connect to BlendListByBool and NOT Boolean nodes in the main AnimGraph
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixAnimGraphBooleanConnections(UObject* AnimBlueprintAsset);

	// Fix GetAccelerationData graph - reconnects broken SET node inputs
	// K2Node_CallFunction_2.ReturnValue -> K2Node_VariableSet_1.bIsAccelerating
	// K2Node_PropertyAccess_1.Value -> K2Node_VariableSet_5.bIsFalling
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static int32 FixGetAccelerationDataGraph(UObject* AnimBlueprintAsset);

	// Fix ANS_RegisterAttackSequence notify QueuedSection values on a montage
	// Converts invalid enum values to FName section names
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixMontageComboNotifies(UObject* MontageAsset);

	// Fix all montages with ANS_RegisterAttackSequence notifies
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixAllMontageComboNotifies();

	// Fix BT State decorator IntValue for enum-to-int migration
	// Updates decorator's IntValue to match the expected state value
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AI")
	static FString FixBTStateDecoratorToValue(const FString& BehaviorTreePath, int32 ExpectedStateValue);

	// Migrate blackboard enum key from Blueprint enum to C++ enum
	// Changes the enum type from E_AI_States to ESLFAIStates
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AI")
	static FString MigrateBlackboardEnumToCpp(const FString& BlackboardPath, const FString& KeyName);

	// Export Blueprint enum values (UUserDefinedEnum) to log
	// Shows internal name, display name, and integer value for each entry
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AI")
	static FString ExportBlueprintEnumValues(const FString& EnumPath);

	// Export blackboard State key current type and compare with C++ enum
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AI")
	static FString DiagnoseBlackboardStateKey(const FString& BlackboardPath);

	// Export ALL decorator IntValues from a BehaviorTree (including nested nodes)
	// Used to diagnose incorrect State decorator values after enum migration
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AI")
	static FString ExportBTAllDecoratorIntValues(const FString& BehaviorTreePath);

	// Export COMPLETE AnimGraph state - every node, every pin, every connection
	// For comprehensive node-by-node, pin-by-pin comparison between AnimBPs
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|AnimBP")
	static FString ExportAnimGraphComplete(UObject* AnimBlueprintAsset, const FString& OutputFilePath = TEXT(""));

	// EXECUTION ANIMATION DATA
	// Set ExecuteFront/ExecuteBack animation on UPDA_ExecutionAnimData assets
	// AnimationPath: e.g., "/Game/SoulslikeFramework/Demo/_Animations/Combat/Generic/AM_SLF_Generic_Execute"
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool SetExecutionAnimData(const FString& ExecutionDataAssetPath, const FString& FrontAnimPath, const FString& BackAnimPath);

	// Extract execution animation paths from a data asset (for debugging)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static FString GetExecutionAnimData(const FString& ExecutionDataAssetPath);

	// Apply weapon stat data to a weapon item data asset
	// ScalingStats format: "Dexterity:A,Strength:S" (stat name : scaling grade)
	// RequirementStats format: "Dexterity:10,Strength:15" (stat name : required value)
	// StatChanges format: "Physical:34,Magic:10" (attack type : damage value)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyWeaponStatData(
		const FString& ItemAssetPath,
		bool bHasStatScaling,
		const TArray<FString>& ScalingStats,
		bool bHasStatRequirement,
		const TArray<FString>& RequirementStats,
		const TArray<FString>& StatChanges
	);

	// Apply armor stat changes to an armor item data asset
	// StatChanges format: Full tag strings like "SoulslikeFramework.Stat.Defense.Negation.Physical:5"
	// EquipSlots format: "SoulslikeFramework.Equipment.SlotType.Armor"
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|DataAsset")
	static bool ApplyArmorStatChanges(
		const FString& ItemAssetPath,
		const TArray<FString>& StatChanges,
		const TArray<FString>& EquipSlots
	);

	// CONTAINER LOOT EXTRACTION
	// Extract loot configuration from all container instances in the current level
	// Returns JSON string with container name, location, override item, and loot table
	// OutputFilePath: if provided, saves to file; otherwise just returns JSON string
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExtractContainerLootConfig(const FString& OutputFilePath = TEXT(""));

	// Apply loot configuration to container instances from JSON file
	// JsonFilePath: path to container_loot_config.json from extraction
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ApplyContainerLootConfig(const FString& JsonFilePath);

	// Set loot item on a container in the currently loaded level by index
	// ContainerIndex: 0-based index of container (sorted by location)
	// ItemAssetPath: full path to item data asset (e.g., "/Game/.../DA_Sword01")
	// MinAmount, MaxAmount: item count range
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool SetContainerLoot(int32 ContainerIndex, const FString& ItemAssetPath, int32 MinAmount = 1, int32 MaxAmount = 1);

	// Configure all containers in level with default demo items
	// Loads level, sets different items on each container, saves level
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ConfigureDefaultContainerLoot();

	// Diagnose loot configuration for all containers in the current level
	// Shows OverrideItem and LootTable settings for each container
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString DiagnoseContainerLoot();

	// Set a container to use LootTable instead of OverrideItem
	// ContainerIndex: 0-based index of container (sorted by location)
	// LootTablePath: full path to data table (e.g., "/Game/.../DT_ExampleChestTier")
	// bClearOverride: if true, clears OverrideItem to ensure LootTable is used
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static bool SetContainerLootTable(int32 ContainerIndex, const FString& LootTablePath, bool bClearOverride = true);

	// Configure all containers to use the default loot table (DT_ExampleChestTier)
	// Clears OverrideItem on all containers so they use random loot selection
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static int32 ConfigureContainersToUseLootTable();

	// DATATABLE MIGRATION
	// Migrate a DataTable from Blueprint struct (FWeightedLoot) to C++ struct (FSLFWeightedLoot)
	// This reads the Blueprint struct data using property iteration and creates a new DataTable with C++ struct
	// SourceTablePath: path to DataTable using Blueprint struct (e.g., "/Game/.../DT_ExampleChestTier")
	// Returns: JSON string with migration result (row count, success/failure)
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString MigrateWeightedLootDataTable(const FString& SourceTablePath);

	// STAT DEFAULTS EXPORT
	// Export stat default values from Blueprint CDOs to JSON file
	// Extracts CurrentValue, MaxValue, DisplayName, Tag from each stat Blueprint's StatInfo
	// OutputFilePath: path to write JSON (e.g., "C:/scripts/SLFConversion/migration_cache/stat_defaults.json")
	// Returns: JSON string with extracted values
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Export")
	static FString ExportStatDefaults(const FString& OutputFilePath);

	// DESTRUCTIBLE DIAGNOSTICS
	// Diagnose B_Destructible GeometryCollection and physics settings
	// Spawns the actor, checks RestCollection, physics state, collision settings
	// Returns: Detailed diagnostic string with all settings
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Diagnostics")
	static FString DiagnoseDestructible(const FString& BlueprintPath);

	// Compare GeometryCollectionComponent settings with expected values
	// BlueprintPath: path to destructible Blueprint
	// Returns: Comparison report showing differences from expected working values
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Diagnostics")
	static FString CompareDestructibleSettings(const FString& BlueprintPath);

	// Fix B_Destructible by ensuring GeometryCollection CDO property is set and saved
	// BlueprintPath: path to B_Destructible Blueprint
	// GeometryCollectionPath: path to GC asset (e.g., "/Game/.../GC_Barrel")
	// Returns: Result string
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString FixDestructibleGeometryCollection(const FString& BlueprintPath, const FString& GeometryCollectionPath);

	// COMPREHENSIVE VALIDATION
	// Validate a complete migration by checking parent class, variables, functions, event graphs, SCS
	// Returns JSON string with all validation results
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Validation")
	static FString ValidateBlueprintMigration(
		const FString& BlueprintPath,
		const FString& ExpectedParentClassPath,
		bool bExpectClearedEventGraphs,
		bool bExpectClearedFunctions
	);

	// Export detailed AnimNotifyState information for comparison
	// Shows parent class, functions with node counts, variables, and whether logic exists
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Validation")
	static FString ExportAnimNotifyStateDetails(const FString& BlueprintPath);

	// Validate chaos destruction system migration (ANS_ToggleChaosField, B_Chaos_ForceField, B_Destructible)
	// Checks parent classes, event graphs cleared, C++ class exists
	// Returns comprehensive validation report
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Validation")
	static FString ValidateChaosDestructionMigration();

	// Force reparent a Blueprint and verify it worked
	// Returns detailed result including before/after state
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ForceReparentAndValidate(const FString& BlueprintPath, const FString& NewParentClassPath);

	// Clear ALL event graph nodes including Event nodes for interface functions
	// This is needed when C++ implements BlueprintNativeEvent functions that Blueprint was overriding
	// Returns JSON with cleared node counts
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ClearAllEventGraphNodes(const FString& BlueprintPath, bool bIncludeEventNodes = true);

	// Diagnose interface implementation for a Blueprint
	// Returns JSON with parent class, implemented interfaces, and whether C++ _Implementation exists
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Validation")
	static FString DiagnoseInterfaceImplementation(const FString& BlueprintPath);

	// Check if an event graph has any nodes (excluding standard entry points)
	// Returns true if the graph has logic nodes, false if empty
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static bool HasEventGraphLogic(UObject* BlueprintAsset);

	// Get count of logic nodes in all function graphs
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static int32 GetFunctionGraphNodeCount(UObject* BlueprintAsset, const FString& FunctionName);

	// Compare Blueprint to bp_only reference and report differences
	// Uses JSON exports from both projects
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Validation")
	static FString CompareToBpOnly(const FString& BlueprintPath, const FString& BpOnlyExportJsonPath);

	// STATUS EFFECT DATA RESTORATION
	// Apply status effect RankInfo damage data that was lost during migration
	// RankInfo uses FInstancedStruct which requires C++ to properly set
	// StatusEffectAssetPath: e.g., "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison"
	// Returns: Result string with details
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ApplyStatusEffectRankInfo(const FString& StatusEffectAssetPath);

	// Apply status effect data to all known status effects
	// Applies hardcoded damage values from original Blueprint data
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ApplyAllStatusEffectRankInfo();

	// FLASK DATA RESTORATION
	// Apply FSLFFlaskData to DA_Action_DrinkFlask_HP after migration
	// This data was lost during reparenting due to InstancedStruct serialization
	// Returns: Result string with details
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ApplyFlaskData();

	// BOSS CONFIGURATION
	// Configure boss with cinematic, weapons, and status effects
	// BossBlueprintPath: e.g., "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth"
	// CinematicSequencePath: e.g., "/Game/SoulslikeFramework/Cinematics/LS_Boss_Start"
	// WeaponBlueprintPath: e.g., "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace"
	// StatusEffectPath: e.g., "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn"
	// StatusEffectRank: rank for the status effect (default 1)
	// StatusEffectBuildup: buildup amount per hit (default 25.0)
	// Returns: Result string with configuration status
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString ConfigureBoss(
		const FString& BossBlueprintPath,
		const FString& CinematicSequencePath,
		const FString& WeaponBlueprintPath,
		const FString& StatusEffectPath,
		int32 StatusEffectRank = 1,
		double StatusEffectBuildup = 25.0
	);

	// Diagnose boss configuration - check Phases, Weapons, StatusEffects
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Diagnostics")
	static FString DiagnoseBossConfig(const FString& BossBlueprintPath);

	// DIALOG DATATABLE MIGRATION
	// Migrate Dialog DataTables from Blueprint FDialogEntry struct to C++ FSLFDialogEntry struct
	// This is required because FindRow<FSLFDialogEntry>() fails when DataTable uses Blueprint struct
	// DataTablePath: path to DataTable (e.g., "/Game/.../DT_ShowcaseGuideNpc_NoProgress")
	// Returns: JSON string with migration result
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString MigrateDialogDataTable(const FString& DataTablePath);

	// Migrate ALL known dialog DataTables to C++ struct
	// Returns: JSON string with all migration results
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString MigrateAllDialogDataTables();

	// Get dialog entry text for verification
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static FString GetDialogEntryText(const FString& DataTablePath, const FString& RowName);

	// Get dialog entry GameplayEvents count for verification
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Query")
	static int32 GetDialogEntryEventCount(const FString& DataTablePath, const FString& RowName);

	// Verify all dialog DataTables have correct content
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Migration")
	static FString VerifyDialogDataTables();

	// ═══════════════════════════════════════════════════════════════════════════
	// SETTINGS WIDGET MIGRATION
	// Complete migration for W_Settings, W_Settings_Entry, W_Settings_CategoryEntry, PDA_CustomSettings
	// ═══════════════════════════════════════════════════════════════════════════

	/**
	 * Extract ALL settings widget data from bp_only backup to JSON file
	 * Extracts: colors, tags, icons, text, enums, references for all Settings widgets
	 * Run this on bp_only project BEFORE migration
	 * @param OutputFilePath - Path to write JSON (e.g., "C:/scripts/SLFConversion/migration_cache/settings_data.json")
	 * @return JSON string with extracted values
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString ExtractSettingsWidgetData(const FString& OutputFilePath);

	/**
	 * Apply extracted settings widget data after migration
	 * Reads from JSON cache and applies to migrated Blueprints
	 * Run this on SLFConversion project AFTER reparenting
	 * @param JsonFilePath - Path to settings_data.json from extraction
	 * @return Result string with success/failure counts
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString ApplySettingsWidgetData(const FString& JsonFilePath);

	/**
	 * Verify settings widget migration by comparing current values to expected
	 * Checks all colors, tags, icons, text, enums match original
	 * @param JsonFilePath - Path to settings_data.json with expected values
	 * @return Verification report showing matches/mismatches
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString VerifySettingsWidgetData(const FString& JsonFilePath);

	/**
	 * Migrate settings widgets: reparent to C++, clear EventGraphs, apply data
	 * Complete one-step migration for all Settings widgets
	 * @param JsonFilePath - Path to settings_data.json (must be extracted first)
	 * @return Complete migration result
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString MigrateSettingsWidgets(const FString& JsonFilePath);

	/**
	 * Diagnose current state of settings widgets
	 * Shows parent class, C++ properties accessible, EventGraph node counts
	 * @return Diagnostic report for all Settings widgets
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString DiagnoseSettingsWidgets();

	/**
	 * Extract ALL embedded widget instances from W_Settings WidgetBlueprint
	 * This extracts the per-instance properties of:
	 * - W_Settings_Entry widgets (ScreenMode, Resolution, TextureQuality, etc.)
	 * - W_Settings_CategoryEntry widgets (Display, Camera, Gameplay, etc.)
	 * Run on bp_only project to get the original configuration
	 * @param OutputFilePath - Path to write JSON (e.g., "C:/scripts/SLFConversion/migration_cache/settings_embedded.json")
	 * @return JSON string with all embedded widget properties
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString ExtractEmbeddedSettingsWidgets(const FString& OutputFilePath, const FString& WidgetBlueprintPath = TEXT("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"));

	/**
	 * Apply embedded widget data to W_Settings after migration
	 * Reads from JSON cache and applies to each embedded widget instance
	 * @param JsonFilePath - Path to settings_embedded.json from extraction
	 * @return Result string with success/failure counts
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Settings")
	static FString ApplyEmbeddedSettingsWidgets(const FString& JsonFilePath);

	// ═══════════════════════════════════════════════════════════════════════════
	// ANIMATION PIPELINE - Import, Socket, AnimBP, Montage, BlendSpace
	// ═══════════════════════════════════════════════════════════════════════════

	/**
	 * Import a skeletal mesh from FBX file
	 * Creates SkeletalMesh + Skeleton assets in the target directory
	 * @param FBXFilePath - Absolute path to FBX file on disk
	 * @param DestinationPath - UE content path (e.g., "/Game/MyMeshes")
	 * @param AssetName - Name for the mesh asset (skeleton will be AssetName_Skeleton)
	 * @param SkeletonPath - Existing skeleton to use ("_" to create new)
	 * @param ImportScale - Scale multiplier (100 = meter to cm)
	 * @return Result string with created asset paths
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString ImportSkeletalMeshFromFBX(
		const FString& FBXFilePath,
		const FString& DestinationPath,
		const FString& AssetName,
		const FString& SkeletonPath = TEXT("_"),
		float ImportScale = 1.0f
	);

	/**
	 * Batch import animation FBX files into UE5
	 * @param FBXDirectory - Directory containing animation FBX files
	 * @param DestinationPath - UE content path for imported animations
	 * @param SkeletonPath - UE path to target skeleton asset
	 * @param ImportScale - Scale multiplier (100 = meter to cm)
	 * @return Result string with import counts
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString BatchImportAnimationsFromFBX(
		const FString& FBXDirectory,
		const FString& DestinationPath,
		const FString& SkeletonPath,
		float ImportScale = 1.0f
	);

	/**
	 * Import animation from FBX directly using FBX SDK + DataModel bracket API.
	 * Bypasses UE5.7's ControlRig import pipeline which doesn't populate bone tracks
	 * in commandlet mode. Reads FBX bone animation curves and writes directly to
	 * DataModel bone tracks, then DDC compresses and saves.
	 * @param FBXPath - Full filesystem path to the FBX file
	 * @param DestPackagePath - UE content path for the output AnimSequence
	 * @param AnimName - Name for the AnimSequence asset
	 * @param SkeletonPath - UE path to target skeleton asset
	 * @param ImportScale - Scale for translation values (1.0 = use as-is)
	 * @return Result string with bone track counts
	 */
	static FString ImportAnimFBXDirect(
		const FString& FBXPath,
		const FString& DestPackagePath,
		const FString& AnimName,
		const FString& SkeletonPath,
		float ImportScale = 1.0f,
		bool bPoseSpaceDeltas = false
	);

	/**
	 * Batch import animations from FBX directory using direct FBX SDK method.
	 * Calls ImportAnimFBXDirect for each .fbx file in the directory.
	 */
	static FString BatchImportAnimsFBXDirect(
		const FString& FBXDirectory,
		const FString& DestinationPath,
		const FString& SkeletonPath,
		float ImportScale = 1.0f,
		bool bPoseSpaceDeltas = false
	);

	/**
	 * Add a socket to a skeleton asset
	 * @param SkeletonPath - UE path to skeleton asset
	 * @param SocketName - Name for the new socket
	 * @param ParentBoneName - Bone to attach socket to
	 * @param RelativeLocation - Socket offset from bone
	 * @param RelativeRotation - Socket rotation relative to bone
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString AddSocketToSkeleton(
		const FString& SkeletonPath,
		const FString& SocketName,
		const FString& ParentBoneName,
		FVector RelativeLocation = FVector::ZeroVector,
		FRotator RelativeRotation = FRotator::ZeroRotator
	);

	/**
	 * Duplicate an AnimBP and retarget to a different skeleton
	 * @param SourceAnimBPPath - Path to source AnimBP
	 * @param TargetSkeletonPath - Path to new target skeleton
	 * @param NewAnimBPPath - Path for the duplicated AnimBP
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString DuplicateAnimBPForSkeleton(
		const FString& SourceAnimBPPath,
		const FString& TargetSkeletonPath,
		const FString& NewAnimBPPath
	);

	/**
	 * Create an AnimMontage from an AnimSequence
	 * @param SequencePath - UE path to source AnimSequence
	 * @param OutputPath - UE path for new montage
	 * @param SlotName - Montage slot name (default "DefaultSlot")
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString CreateMontageFromSequence(
		const FString& SequencePath,
		const FString& OutputPath,
		const FString& SlotName = TEXT("DefaultSlot")
	);

	/**
	 * Create a 1D BlendSpace with idle/walk/run samples
	 * @param OutputPath - UE path for new BlendSpace
	 * @param SkeletonPath - UE path to skeleton
	 * @param IdleSequencePath - Path to idle animation
	 * @param WalkSequencePath - Path to walk animation
	 * @param RunSequencePath - Path to run animation
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString CreateBlendSpace1DFromSequences(
		const FString& OutputPath,
		const FString& SkeletonPath,
		const FString& IdleSequencePath,
		const FString& WalkSequencePath,
		const FString& RunSequencePath
	);

	/** Create a 2D BlendSpace matching the original ABP_SoulslikeEnemy layout (Direction X, Speed Y) */
	static FString CreateBlendSpace2DFromSequences(
		const FString& OutputPath,
		const FString& SkeletonPath,
		const FString& IdleSequencePath,
		const FString& WalkSequencePath,
		const FString& RunSequencePath
	);

	/**
	 * Add an ANS_WeaponTrace notify state to a montage at specified time range.
	 * Uses TAE (Time Act Event) timings from Elden Ring for accurate hitbox windows.
	 * @param MontagePath - UE path to the montage
	 * @param StartTime - Start time in seconds
	 * @param EndTime - End time in seconds
	 * @param InTraceRadius - Sphere trace radius (default 30)
	 * @param InStartSocket - Start socket name (default weapon_start)
	 * @param InEndSocket - End socket name (default weapon_end)
	 * @return Result string
	 */
	static FString AddWeaponTraceToMontage(
		const FString& MontagePath,
		float StartTime,
		float EndTime,
		float InTraceRadius = 30.0f,
		const FName& InStartSocket = FName("weapon_start"),
		const FName& InEndSocket = FName("weapon_end")
	);

	/**
	 * Configure guard enemy with new mesh, AnimBP, and animation data
	 * @param GuardBlueprintPath - Path to guard Blueprint
	 * @param MeshPath - Path to skeletal mesh
	 * @param AnimBPPath - Path to AnimBP class
	 * @param BlendSpacePath - Path to locomotion blend space
	 * @param AttackMontagePaths - Paths to attack montages
	 * @param HitReactMontagePath - Path to hit reaction montage
	 * @param DeathMontagePaths - Paths to death montages (Fwd, Bwd, Left, Right)
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString ConfigureGuardAnimations(
		const FString& GuardBlueprintPath,
		const FString& MeshPath,
		const FString& AnimBPPath,
		const FString& BlendSpacePath,
		const TArray<FString>& AttackMontagePaths,
		const FString& HitReactMontagePath,
		const TArray<FString>& DeathMontagePaths
	);

	/**
	 * List all bone names in a skeleton asset (for socket setup reference)
	 * @param SkeletonPath - UE path to skeleton asset
	 * @return Newline-separated list of bone names
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString ListSkeletonBones(const FString& SkeletonPath);

	/**
	 * Replace animation references in an AnimBP's AnimGraph nodes
	 * @param AnimBPPath - UE path to AnimBP
	 * @param ReplacementMap - Map of old asset path -> new asset path
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString ReplaceAnimReferencesInAnimBP(
		const FString& AnimBPPath,
		const TMap<FString, FString>& ReplacementMap
	);

	/**
	 * Disable Control Rig nodes in an AnimBP by setting Alpha to 0
	 * Use when AnimBP is duplicated for a skeleton with different bone hierarchy
	 * @param AnimBPPath - UE path to AnimBP
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString DisableControlRigInAnimBP(const FString& AnimBPPath);

	/**
	 * Detailed AnimBP diagnostic: skeleton, all nodes, blend space samples, Control Rig, IK, graph chain
	 * @param AnimBPPath - UE path to AnimBP
	 * @return Detailed diagnostic string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString DiagnoseAnimBPDetailed(const FString& AnimBPPath);

	/**
	 * Exhaustive AnimBP dump: every graph, every node, every pin (defaults+connections),
	 * AnimNode struct properties via reflection, CDO defaults, blend space samples.
	 */
	static FString DumpAnimBPExhaustive(const FString& AnimBPPath);

	/**
	 * Fix BlendSpace1D pin wiring in AnimBP.
	 * BlendSpace1D only uses the X axis. If GroundSpeed is wired to Y instead of X,
	 * this swaps the connections so GroundSpeed feeds X (the only axis that matters).
	 * @param AnimBPPath - UE path to AnimBP
	 * @return Diagnostic/result string
	 */
	static FString FixBlendSpace1DPinWiring(const FString& AnimBPPath);

	/**
	 * Bake an animation with forensically distinct transforms.
	 * Reads source AnimSequence, resamples to target FPS, applies time warp,
	 * smooth noise, and bone rotation offsets, then writes a new AnimSequence.
	 *
	 * @param SourceAnimPath - UE path to source AnimSequence
	 * @param OutputPath - UE content directory for output
	 * @param NewAssetName - Name for the new asset
	 * @param NoiseAmplitudeDegrees - Max noise rotation amplitude in degrees (default 2.0)
	 * @param TimeWarpStrength - Sine-based time warp strength 0-0.95 (default 0.15)
	 * @param BoneRotationOffsetDegrees - Static rotation offset per bone (scaled by region weight)
	 * @param TargetFrameRate - Output frame rate (default 24fps)
	 * @param RandomSeed - Deterministic seed (0 = random)
	 * @return Result string with asset path or error
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString BakeAnimationTransforms(
		const FString& SourceAnimPath,
		const FString& OutputPath,
		const FString& NewAssetName,
		float NoiseAmplitudeDegrees = 2.0f,
		float TimeWarpStrength = 0.15f,
		FVector BoneRotationOffsetDegrees = FVector(0, 10, 0),
		float TargetFrameRate = 24.0f,
		int32 RandomSeed = 0,
		const FString& TargetSkeletonPath = TEXT("")
	);

	/**
	 * Bake animation with explicit bone name mapping for cross-skeleton retarget.
	 * Maps target skeleton bone names to source skeleton bone names when they differ
	 * (e.g., FromSoft "Pelvis" -> UE5 "pelvis").
	 * NOT a UFUNCTION - use for C++ commandlet calls with custom bone mapping.
	 *
	 * @param BoneNameMapping - Maps target bone name -> source bone name
	 */
	static FString BakeAnimWithBoneMapping(
		const FString& SourceAnimPath,
		const FString& OutputPath,
		const FString& NewAssetName,
		const FString& TargetSkeletonPath,
		const TMap<FName, FName>& BoneNameMapping,
		float NoiseAmplitudeDegrees = 2.0f,
		float TimeWarpStrength = 0.15f,
		FVector BoneRotationOffsetDegrees = FVector(0.f, 10.f, 0.f),
		float TargetFrameRate = 24.0f,
		int32 RandomSeed = 0
	);

	/**
	 * Apply forensic transforms (noise, time warp, resample) to an EXISTING animation in-place.
	 * Unlike BakeAnimationTransforms, this does NOT do cross-skeleton retarget.
	 * Reads the animation's own bone data, applies modifications, writes back to same skeleton.
	 *
	 * @param AnimPath - Path to existing UAnimSequence to modify
	 * @param NoiseAmplitudeDegrees - Smooth Catmull-Rom noise (default 2.0)
	 * @param TimeWarpStrength - Sine-based time warp (default 0.15)
	 * @param BoneRotationOffsetDegrees - Static rotation offset (scaled by region weight)
	 * @param TargetFrameRate - Resample to this fps (default 24.0, 0 = keep original)
	 * @param RandomSeed - Deterministic seed (0 = random)
	 * @return Result string
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString ApplyForensicTransforms(
		const FString& AnimPath,
		float NoiseAmplitudeDegrees = 2.0f,
		float TimeWarpStrength = 0.15f,
		FVector BoneRotationOffsetDegrees = FVector(0, 10, 0),
		float TargetFrameRate = 24.0f,
		int32 RandomSeed = 0
	);

	/**
	 * Compare two skeletons' reference poses and an animation's first frame.
	 * Outputs detailed bone-by-bone comparison to a file for analysis.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Animation")
	static FString DiagnoseSkeletonMismatch(
		const FString& SkeletonPathA,
		const FString& SkeletonPathB,
		const FString& AnimPathOnA = TEXT(""),
		const FString& OutputFilePath = TEXT("")
	);

	/** Diagnose animation bone data using DataModel Controller (works in commandlet).
	 * Reports which bones have non-identity transforms at frame 0 and mid-frame. */
	static FString DiagnoseAnimDataModel(const FString& AnimPath);

	/** Deep comparison of two animations - compares skeleton, DataModel, and compressed evaluation */
	static FString CompareAnimations(const FString& AnimPathA, const FString& AnimPathB);

	/**
	 * Place a Blueprint actor in a level (editor-time, no BeginPlay)
	 * @param MapPath - UE content path to the map (e.g., /Game/.../L_Demo_Showcase)
	 * @param BlueprintPath - UE content path to the Blueprint (e.g., /Game/.../B_Soulslike_Enemy_Sentinel)
	 * @param Location - World location to place the actor
	 * @param Rotation - World rotation for the actor
	 * @return Result string
	 */
	static FString PlaceActorInLevel(
		const FString& MapPath,
		const FString& BlueprintPath,
		FVector Location,
		FRotator Rotation = FRotator::ZeroRotator
	);

#endif // WITH_EDITOR

	/**
	 * Dump comprehensive actor state for debugging spawned characters.
	 * Writes class hierarchy, all components (with collision, transform, physics),
	 * capsule, movement, mesh, AnimBP, CDO comparison to a file.
	 * Works at runtime (PIE).
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Debug")
	static FString DumpCharacterState(AActor* Actor);

	/**
	 * Compare two characters side by side. Dumps both to file and highlights differences.
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Debug")
	static FString CompareCharacters(AActor* ActorA, AActor* ActorB, const FString& OutputPath);
};
