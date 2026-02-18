// Fill out your copyright notice in the Description page of Project Settings.

#include "SLFConversion.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "HAL/IConsoleManager.h"
#include "EditorAssetLibrary.h"
#include "Misc/FileHelper.h"

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

// ============================================================================
// ANIMATION PIPELINE COMMANDS
// ============================================================================

// Console command to import a skeletal mesh from FBX
// Usage: SLF.ImportMesh <FBXPath> <DestPath> <AssetName> [SkeletonPath] [Scale]
static FAutoConsoleCommand ImportMeshCmd(
	TEXT("SLF.ImportMesh"),
	TEXT("Import skeletal mesh from FBX.\nUsage: SLF.ImportMesh <FBXPath> <DestPath> <AssetName> [SkeletonPath] [Scale]"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.ImportMesh requires: <FBXPath> <DestPath> <AssetName> [SkeletonPath] [Scale]"));
			return;
		}

		FString SkeletonPath = Args.Num() > 3 ? Args[3] : TEXT("_");
		float Scale = Args.Num() > 4 ? FCString::Atof(*Args[4]) : 1.0f;

		FString Result = USLFAutomationLibrary::ImportSkeletalMeshFromFBX(
			Args[0], Args[1], Args[2], SkeletonPath, Scale);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.ImportMesh]\n%s"), *Result);
	})
);

// Console command to batch import animations from FBX directory
// Usage: SLF.ImportAnims <FBXDir> <DestPath> <SkeletonPath> [Scale]
static FAutoConsoleCommand ImportAnimsCmd(
	TEXT("SLF.ImportAnims"),
	TEXT("Batch import animations from FBX directory.\nUsage: SLF.ImportAnims <FBXDir> <DestPath> <SkeletonPath> [Scale]"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.ImportAnims requires: <FBXDir> <DestPath> <SkeletonPath> [Scale]"));
			return;
		}

		float Scale = Args.Num() > 3 ? FCString::Atof(*Args[3]) : 1.0f;

		FString Result = USLFAutomationLibrary::BatchImportAnimationsFromFBX(
			Args[0], Args[1], Args[2], Scale);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.ImportAnims]\n%s"), *Result);
	})
);

// Console command to add a socket to a skeleton
// Usage: SLF.AddSocket <SkeletonPath> <SocketName> <BoneName> [X] [Y] [Z]
// Note: Location components are separate args (NOT comma-separated) to work with -ExecCmds
static FAutoConsoleCommand AddSocketCmd(
	TEXT("SLF.AddSocket"),
	TEXT("Add socket to skeleton.\nUsage: SLF.AddSocket <SkeletonPath> <SocketName> <BoneName> [X] [Y] [Z]"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.AddSocket requires: <SkeletonPath> <SocketName> <BoneName> [X] [Y] [Z]"));
			return;
		}

		FVector Location = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;

		// Space-separated X Y Z (avoids comma issues with -ExecCmds)
		if (Args.Num() > 3) Location.X = FCString::Atof(*Args[3]);
		if (Args.Num() > 4) Location.Y = FCString::Atof(*Args[4]);
		if (Args.Num() > 5) Location.Z = FCString::Atof(*Args[5]);

		FString Result = USLFAutomationLibrary::AddSocketToSkeleton(
			Args[0], Args[1], Args[2], Location, Rotation);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.AddSocket]\n%s"), *Result);
	})
);

// Console command to duplicate AnimBP for a different skeleton
// Usage: SLF.DuplicateAnimBP <SourcePath> <SkeletonPath> <OutputPath>
static FAutoConsoleCommand DuplicateAnimBPCmd(
	TEXT("SLF.DuplicateAnimBP"),
	TEXT("Duplicate AnimBP for different skeleton.\nUsage: SLF.DuplicateAnimBP <SourcePath> <SkeletonPath> <OutputPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.DuplicateAnimBP requires: <SourcePath> <SkeletonPath> <OutputPath>"));
			return;
		}

		FString Result = USLFAutomationLibrary::DuplicateAnimBPForSkeleton(Args[0], Args[1], Args[2]);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.DuplicateAnimBP]\n%s"), *Result);
	})
);

// Console command to create AnimMontage from AnimSequence
// Usage: SLF.CreateMontage <SequencePath> <OutputPath> [SlotName]
static FAutoConsoleCommand CreateMontageCmd(
	TEXT("SLF.CreateMontage"),
	TEXT("Create AnimMontage from AnimSequence.\nUsage: SLF.CreateMontage <SequencePath> <OutputPath> [SlotName]"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 2)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.CreateMontage requires: <SequencePath> <OutputPath> [SlotName]"));
			return;
		}

		FString SlotName = Args.Num() > 2 ? Args[2] : TEXT("DefaultSlot");

		FString Result = USLFAutomationLibrary::CreateMontageFromSequence(Args[0], Args[1], SlotName);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.CreateMontage]\n%s"), *Result);
	})
);

// Console command to create 1D BlendSpace
// Usage: SLF.CreateBlendSpace1D <OutputPath> <SkeletonPath> <IdlePath> <WalkPath> <RunPath>
static FAutoConsoleCommand CreateBlendSpace1DCmd(
	TEXT("SLF.CreateBlendSpace1D"),
	TEXT("Create 1D BlendSpace with idle/walk/run.\nUsage: SLF.CreateBlendSpace1D <OutputPath> <SkeletonPath> <IdlePath> <WalkPath> <RunPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 5)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.CreateBlendSpace1D requires: <OutputPath> <SkeletonPath> <IdlePath> <WalkPath> <RunPath>"));
			return;
		}

		FString Result = USLFAutomationLibrary::CreateBlendSpace1DFromSequences(
			Args[0], Args[1], Args[2], Args[3], Args[4]);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.CreateBlendSpace1D]\n%s"), *Result);
	})
);

// Console command to do complete c3100 guard setup (sockets + montages + blendspace + AnimBP)
// Usage: SLF.SetupC3100
static FAutoConsoleCommand SetupC3100Cmd(
	TEXT("SLF.SetupC3100"),
	TEXT("Complete c3100 guard setup: sockets, montages, blend space, AnimBP.\nUsage: SLF.SetupC3100"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		const FString SkeletonPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh_Skeleton");
		const FString AnimDir = TEXT("/Game/EldenRingAnimations/c3100_guard/Animations");
		const FString OutputDir = TEXT("/Game/EldenRingAnimations/c3100_guard");
		FString Result;

		UE_LOG(LogTemp, Warning, TEXT("=== SLF.SetupC3100: Starting complete setup ==="));

		// Step 1: Add sockets
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1: Adding sockets ---"));
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("weapon_start"), TEXT("R_Sword"), FVector(0, 0, 50));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("weapon_end"), TEXT("R_Sword"), FVector(0, 0, -50));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("hand_r"), TEXT("R_Hand"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("hand_l"), TEXT("L_Hand"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("foot_l"), TEXT("L_Foot"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, TEXT("foot_r"), TEXT("R_Foot"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

		// Step 2: Create attack montages
		UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Creating montages ---"));
		struct FMontageInfo { const TCHAR* Seq; const TCHAR* Name; };
		const FMontageInfo AttackMontages[] = {
			{ TEXT("a000_003000"), TEXT("AM_c3100_Attack01") },
			{ TEXT("a000_003001"), TEXT("AM_c3100_Attack02") },
			{ TEXT("a000_003017"), TEXT("AM_c3100_Attack03_Fast") },
			{ TEXT("a000_004100"), TEXT("AM_c3100_HeavyAttack") },
		};
		for (const auto& M : AttackMontages)
		{
			FString SeqPath = FString::Printf(TEXT("%s/%s"), *AnimDir, M.Seq);
			FString OutPath = FString::Printf(TEXT("%s/%s"), *OutputDir, M.Name);
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}

		// Reaction montages
		const FMontageInfo ReactionMontages[] = {
			{ TEXT("a000_010000"), TEXT("AM_c3100_HitReact") },
			{ TEXT("a000_010001"), TEXT("AM_c3100_HitReact_Light") },
			{ TEXT("a000_008030"), TEXT("AM_c3100_GuardHit") },
		};
		for (const auto& M : ReactionMontages)
		{
			FString SeqPath = FString::Printf(TEXT("%s/%s"), *AnimDir, M.Seq);
			FString OutPath = FString::Printf(TEXT("%s/%s"), *OutputDir, M.Name);
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}

		// Death montages
		const FMontageInfo DeathMontages[] = {
			{ TEXT("a000_011010"), TEXT("AM_c3100_Death_Front") },
			{ TEXT("a000_011060"), TEXT("AM_c3100_Death_Back") },
			{ TEXT("a000_011070"), TEXT("AM_c3100_Death_Left") },
		};
		for (const auto& M : DeathMontages)
		{
			FString SeqPath = FString::Printf(TEXT("%s/%s"), *AnimDir, M.Seq);
			FString OutPath = FString::Printf(TEXT("%s/%s"), *OutputDir, M.Name);
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}

		// Dodge montages
		const FMontageInfo DodgeMontages[] = {
			{ TEXT("a000_005000"), TEXT("AM_c3100_Dodge_Fwd") },
			{ TEXT("a000_005001"), TEXT("AM_c3100_Dodge_Bwd") },
			{ TEXT("a000_005002"), TEXT("AM_c3100_Dodge_Left") },
			{ TEXT("a000_005003"), TEXT("AM_c3100_Dodge_Right") },
		};
		for (const auto& M : DodgeMontages)
		{
			FString SeqPath = FString::Printf(TEXT("%s/%s"), *AnimDir, M.Seq);
			FString OutPath = FString::Printf(TEXT("%s/%s"), *OutputDir, M.Name);
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}

		// Step 3: Create locomotion blend space
		UE_LOG(LogTemp, Warning, TEXT("--- Step 3: Creating blend space ---"));
		Result = USLFAutomationLibrary::CreateBlendSpace1DFromSequences(
			OutputDir / TEXT("BS_c3100_Locomotion"),
			SkeletonPath,
			AnimDir / TEXT("a000_000020"),  // Idle
			AnimDir / TEXT("a000_002000"),  // Walk
			AnimDir / TEXT("a000_002100")   // Run
		);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

		// Step 4: Duplicate AnimBP
		UE_LOG(LogTemp, Warning, TEXT("--- Step 4: Duplicating AnimBP ---"));
		Result = USLFAutomationLibrary::DuplicateAnimBPForSkeleton(
			TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"),
			SkeletonPath,
			OutputDir / TEXT("ABP_c3100_Guard")
		);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

		// Step 4b: Replace animation references (blend space + idle)
		UE_LOG(LogTemp, Warning, TEXT("--- Step 4b: Replacing AnimGraph references ---"));
		{
			TMap<FString, FString> AnimReplacementMap;
			AnimReplacementMap.Add(
				TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Blendspaces/ABS_SoulslikeEnemy"),
				OutputDir / TEXT("BS_c3100_Locomotion"));
			AnimReplacementMap.Add(
				TEXT("/Game/SoulslikeFramework/Demo/RetargetedParagonAnimations/Kwang/AS_RT_Idle_Kwang"),
				AnimDir / TEXT("a000_000020"));

			Result = USLFAutomationLibrary::ReplaceAnimReferencesInAnimBP(
				OutputDir / TEXT("ABP_c3100_Guard"),
				AnimReplacementMap);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}

		// Step 4c: Remove Control Rig + IK Rig nodes
		UE_LOG(LogTemp, Warning, TEXT("--- Step 4c: Removing Control Rig + IK Rig ---"));
		Result = USLFAutomationLibrary::DisableControlRigInAnimBP(OutputDir / TEXT("ABP_c3100_Guard"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

		UE_LOG(LogTemp, Warning, TEXT("=== SLF.SetupC3100: COMPLETE ==="));
	})
);

// Console command to diagnose an AnimBP in detail
// Usage: SLF.DiagAnimBP <AnimBPPath>
static FAutoConsoleCommand DiagAnimBPCmd(
	TEXT("SLF.DiagAnimBP"),
	TEXT("Detailed AnimBP diagnostic: skeleton, nodes, blend spaces, Control Rig.\nUsage: SLF.DiagAnimBP <AnimBPPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.DiagAnimBP requires: <AnimBPPath>"));
			return;
		}

		FString Result = USLFAutomationLibrary::DiagnoseAnimBPDetailed(Args[0]);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.DiagAnimBP]\n%s"), *Result);
	})
);

// Console command to dump two AnimBPs to files for comparison
// Usage: SLF.CompareAnimBP <AnimBPPath1> <AnimBPPath2> <OutputDir>
static FAutoConsoleCommand CompareAnimBPCmd(
	TEXT("SLF.CompareAnimBP"),
	TEXT("Dump two AnimBPs exhaustively to files for comparison.\nUsage: SLF.CompareAnimBP <Path1> <Path2> <OutputDir>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.CompareAnimBP requires: <Path1> <Path2> <OutputDir>"));
			return;
		}

		FString OutputDir = Args[2];

		FString Dump1 = USLFAutomationLibrary::DumpAnimBPExhaustive(Args[0]);
		FString File1 = OutputDir / TEXT("animBP_dump_A.txt");
		FFileHelper::SaveStringToFile(Dump1, *File1);
		UE_LOG(LogTemp, Warning, TEXT("[CompareAnimBP] Saved %d chars to %s"), Dump1.Len(), *File1);

		FString Dump2 = USLFAutomationLibrary::DumpAnimBPExhaustive(Args[1]);
		FString File2 = OutputDir / TEXT("animBP_dump_B.txt");
		FFileHelper::SaveStringToFile(Dump2, *File2);
		UE_LOG(LogTemp, Warning, TEXT("[CompareAnimBP] Saved %d chars to %s"), Dump2.Len(), *File2);
	})
);

// Console command to list bone names in a skeleton
// Usage: SLF.ListBones <SkeletonPath>
static FAutoConsoleCommand ListBonesCmd(
	TEXT("SLF.ListBones"),
	TEXT("List all bone names in a skeleton.\nUsage: SLF.ListBones <SkeletonPath>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SLF.ListBones requires: <SkeletonPath>"));
			return;
		}

		FString Result = USLFAutomationLibrary::ListSkeletonBones(Args[0]);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.ListBones]\n%s"), *Result);
	})
);

#endif // WITH_EDITOR

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SLFConversion, "SLFConversion" );
