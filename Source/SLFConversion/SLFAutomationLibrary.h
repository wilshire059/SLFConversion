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

#endif // WITH_EDITOR
};
