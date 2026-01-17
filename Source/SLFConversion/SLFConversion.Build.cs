// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SLFConversion : ModuleRules
{
	public SLFConversion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Add include paths so subdirectories can find root headers
		PublicIncludePaths.AddRange(new string[] {
			ModuleDirectory,
			System.IO.Path.Combine(ModuleDirectory, "AI"),
			System.IO.Path.Combine(ModuleDirectory, "Animation"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Actions"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Actors"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Buffs"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Stats"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "StatusEffects"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Items"),
			System.IO.Path.Combine(ModuleDirectory, "Blueprints", "Projectiles"),
			System.IO.Path.Combine(ModuleDirectory, "Components"),
			System.IO.Path.Combine(ModuleDirectory, "Framework"),
			System.IO.Path.Combine(ModuleDirectory, "Interfaces"),
			System.IO.Path.Combine(ModuleDirectory, "Testing"),
			System.IO.Path.Combine(ModuleDirectory, "Tests")
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",      // For UInputMappingContext
			"GameplayTags",
			"Niagara",
			"StructUtils",
			"AIModule",           // For AI types
			"LevelSequence",      // For ULevelSequence
			"MovieScene",         // For FMovieSceneSequencePlaybackSettings
			"NavigationSystem",   // For UNavigationSystemV1
			"UMG",                // For UI types
			"SlateCore",          // For EVerticalAlignment
			"Slate",              // For Slate types
			"AnimGraphRuntime",   // For UKismetAnimationLibrary (CalculateDirection)
			"SkeletalMerging",    // For FSkeletalMeshMergeParams
			"FieldSystemEngine",  // For UOperatorField
			"GeometryCollectionEngine",  // For UGeometryCollection
			"MediaAssets",        // For UMediaPlayer
			"PhysicsCore",        // For EPhysicalSurface
			"EngineCameras"       // For ULegacyCameraShake
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Editor-only modules for Blueprint automation
		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[] {
				"UnrealEd",
				"Kismet",
				"KismetCompiler",
				"BlueprintGraph",
				"AnimGraph",        // For AnimGraphNode_Base and property bindings
				"AssetRegistry",    // For dependency analysis
				"UMGEditor",        // For WidgetBlueprint access
				"Json",             // For JSON serialization
				"JsonUtilities",    // For JSON helpers
				"EditorScriptingUtilities"  // For UEditorAssetLibrary
			});
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
