// Fill out your copyright notice in the Description page of Project Settings.

#include "SLFConversion.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "HAL/IConsoleManager.h"
#include "EditorAssetLibrary.h"

// Console command to reparent a Blueprint to a C++ class
// Usage: SLF.Reparent /Game/Path/To/Blueprint /Script/Module.CppClassName
static FAutoConsoleCommand ReparentBlueprintCmd(
	TEXT("SLF.Reparent"),
	TEXT("Reparent a Blueprint to a C++ class.\nUsage: SLF.Reparent <BlueprintPath> <CppClassPath>\nExample: SLF.Reparent /Game/BP/B_Test /Script/SLFConversion.CppClass"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 2)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.Reparent requires 2 arguments: <BlueprintPath> <CppClassPath>"));
			return;
		}

		const FString& BlueprintPath = Args[0];
		const FString& CppClassPath = Args[1];

		UE_LOG(LogTemp, Warning, TEXT("[SLF.Reparent] Loading Blueprint: %s"), *BlueprintPath);

		UObject* BP = LoadObject<UObject>(nullptr, *BlueprintPath);
		if (!BP)
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.Reparent] Failed to load Blueprint: %s"), *BlueprintPath);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SLF.Reparent] Reparenting to: %s"), *CppClassPath);

		bool bSuccess = USLFAutomationLibrary::ReparentBlueprint(BP, CppClassPath);
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLF.Reparent] SUCCESS - Reparented %s to %s"), *BlueprintPath, *CppClassPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.Reparent] FAILED - Could not reparent %s to %s"), *BlueprintPath, *CppClassPath);
		}
	})
);

// Console command to clear Blueprint logic
// Usage: SLF.ClearLogic /Game/Path/To/Blueprint
static FAutoConsoleCommand ClearBlueprintLogicCmd(
	TEXT("SLF.ClearLogic"),
	TEXT("Clear all logic from a Blueprint.\nUsage: SLF.ClearLogic <BlueprintPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.ClearLogic requires 1 argument: <BlueprintPath>"));
			return;
		}

		const FString& BlueprintPath = Args[0];

		UE_LOG(LogTemp, Warning, TEXT("[SLF.ClearLogic] Loading Blueprint: %s"), *BlueprintPath);

		UObject* BP = LoadObject<UObject>(nullptr, *BlueprintPath);
		if (!BP)
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.ClearLogic] Failed to load Blueprint: %s"), *BlueprintPath);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SLF.ClearLogic] Clearing logic..."));

		bool bSuccess = USLFAutomationLibrary::ClearAllBlueprintLogic(BP);
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLF.ClearLogic] SUCCESS - Cleared logic from %s"), *BlueprintPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.ClearLogic] FAILED - Could not clear logic from %s"), *BlueprintPath);
		}
	})
);

// Console command to save an asset
// Usage: SLF.Save /Game/Path/To/Asset
static FAutoConsoleCommand SaveAssetCmd(
	TEXT("SLF.Save"),
	TEXT("Save an asset to disk.\nUsage: SLF.Save <AssetPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.Save requires 1 argument: <AssetPath>"));
			return;
		}

		const FString& AssetPath = Args[0];

		UE_LOG(LogTemp, Warning, TEXT("[SLF.Save] Saving asset: %s"), *AssetPath);

		bool bSaved = UEditorAssetLibrary::SaveAsset(AssetPath, false);

		if (bSaved)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLF.Save] SUCCESS - Saved %s"), *AssetPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.Save] FAILED - Could not save %s"), *AssetPath);
		}
	})
);

// Console command to diagnose interface implementation
// Usage: SLF.DiagnoseInterface /Game/Path/To/Blueprint
static FAutoConsoleCommand DiagnoseInterfaceCmd(
	TEXT("SLF.DiagnoseInterface"),
	TEXT("Diagnose interface implementation for a Blueprint.\nUsage: SLF.DiagnoseInterface <BlueprintPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.DiagnoseInterface requires 1 argument: <BlueprintPath>"));
			return;
		}

		const FString& BlueprintPath = Args[0];
		FString Result = USLFAutomationLibrary::DiagnoseInterfaceImplementation(BlueprintPath);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.DiagnoseInterface] Result:\n%s"), *Result);
	})
);

// Console command to remove Blueprint interfaces
// Usage: SLF.RemoveInterfaces /Game/Path/To/Blueprint
static FAutoConsoleCommand RemoveInterfacesCmd(
	TEXT("SLF.RemoveInterfaces"),
	TEXT("Remove all Blueprint-implemented interfaces from a Blueprint.\nUsage: SLF.RemoveInterfaces <BlueprintPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.RemoveInterfaces requires 1 argument: <BlueprintPath>"));
			return;
		}

		const FString& BlueprintPath = Args[0];

		UObject* BP = LoadObject<UObject>(nullptr, *BlueprintPath);
		if (!BP)
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.RemoveInterfaces] Failed to load: %s"), *BlueprintPath);
			return;
		}

		int32 Removed = USLFAutomationLibrary::RemoveImplementedInterfaces(BP);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.RemoveInterfaces] Removed %d interfaces from %s"), Removed, *BlueprintPath);

		if (Removed > 0)
		{
			USLFAutomationLibrary::CompileAndSave(BP);
			UE_LOG(LogTemp, Warning, TEXT("[SLF.RemoveInterfaces] Compiled and saved"));
		}
	})
);

// Console command to fix B_Chaos_ForceField interface issue
// Usage: SLF.FixChaosInterface
static FAutoConsoleCommand FixChaosInterfaceCmd(
	TEXT("SLF.FixChaosInterface"),
	TEXT("Fix B_Chaos_ForceField by removing Blueprint interface and clearing stale functions.\nUsage: SLF.FixChaosInterface"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		const FString BlueprintPath = TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField");

		UE_LOG(LogTemp, Warning, TEXT("=== FIXING B_CHAOS_FORCEFIELD INTERFACE ==="));

		// Load Blueprint
		UObject* BP = LoadObject<UObject>(nullptr, *BlueprintPath);
		if (!BP)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load Blueprint"));
			return;
		}

		// Step 1: Remove Blueprint interfaces
		UE_LOG(LogTemp, Warning, TEXT("Step 1: Removing Blueprint interfaces..."));
		int32 RemovedInterfaces = USLFAutomationLibrary::RemoveImplementedInterfaces(BP);
		UE_LOG(LogTemp, Warning, TEXT("Removed %d Blueprint interfaces"), RemovedInterfaces);

		// Step 2: Clear all function graphs (removes stale interface function overrides)
		UE_LOG(LogTemp, Warning, TEXT("Step 2: Clearing function graphs..."));
		int32 ClearedFunctions = USLFAutomationLibrary::ClearFunctionGraphs(BP);
		UE_LOG(LogTemp, Warning, TEXT("Cleared %d function graphs"), ClearedFunctions);

		// Step 3: Clear event graphs
		UE_LOG(LogTemp, Warning, TEXT("Step 3: Clearing event graphs..."));
		int32 ClearedEvents = USLFAutomationLibrary::ClearEventGraphs(BP);
		UE_LOG(LogTemp, Warning, TEXT("Cleared %d event graphs"), ClearedEvents);

		// Step 4: Force compile and save
		UE_LOG(LogTemp, Warning, TEXT("Step 4: Compiling and saving..."));
		USLFAutomationLibrary::CompileAndSave(BP);

		// Diagnose after
		UE_LOG(LogTemp, Warning, TEXT("--- VERIFYING ---"));
		FString After = USLFAutomationLibrary::DiagnoseInterfaceImplementation(BlueprintPath);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *After);

		UE_LOG(LogTemp, Warning, TEXT("=== DONE ==="));
	})
);

// Console command to apply status effect RankInfo damage data
// Usage: SLF.ApplyStatusEffects
static FAutoConsoleCommand ApplyStatusEffectsCmd(
	TEXT("SLF.ApplyStatusEffects"),
	TEXT("Apply status effect RankInfo damage data (poison, bleed, burn, etc.).\nUsage: SLF.ApplyStatusEffects"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== APPLYING STATUS EFFECT RANK INFO ==="));

		FString Result = USLFAutomationLibrary::ApplyAllStatusEffectRankInfo();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

		UE_LOG(LogTemp, Warning, TEXT("=== DONE ==="));
	})
);

#endif // WITH_EDITOR

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SLFConversion, "SLFConversion" );
