#include "SetupC3100Commandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace1D.h"
#include "UObject/SavePackage.h"
#include "SLFPrimaryDataAssets.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Misc/FileHelper.h"

USetupC3100Commandlet::USetupC3100Commandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 USetupC3100Commandlet::Main(const FString& Params)
{
	const FString SkeletonPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh_Skeleton");
	const FString AnimDir = TEXT("/Game/EldenRingAnimations/c3100_guard/Animations");
	const FString OutputDir = TEXT("/Game/EldenRingAnimations/c3100_guard");
	FString Result;

	UE_LOG(LogTemp, Warning, TEXT("=== SetupC3100 Commandlet: Starting ==="));

	// Step 0: Force DDC re-compression on all c3100 animations
	// Bug #11: DataModel has correct cm-scale bone data but compressed eval can have scale mismatch.
	// Re-running CacheDerivedDataForCurrentPlatform + FinishAllCompilation fixes this.
	UE_LOG(LogTemp, Warning, TEXT("--- Step 0: Re-compressing all animations (DDC fix) ---"));
	{
		FString AnimSearchDir = AnimDir + TEXT("/");
		TArray<FAssetData> AnimAssets;
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		AssetRegistryModule.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, true);

		int32 Recompressed = 0;
		for (const FAssetData& AssetData : AnimAssets)
		{
			if (AssetData.AssetClassPath.GetAssetName() != TEXT("AnimSequence"))
				continue;

			UAnimSequence* AnimSeq = Cast<UAnimSequence>(AssetData.GetAsset());
			if (!AnimSeq) continue;

			// Lock root bone to reference pose - prevents snap-back on loop.
			// Elden Ring animations have root/Master bone translation baked in.
			// Without this, the mesh visually drifts then teleports back on loop.
			// CharacterMovement drives actual capsule translation for locomotion.
			// Lock root bone to FIRST FRAME (not ref pose!).
			// Ref pose is meter-scale (1x), but animations encode cm-scale (100x) in root bone.
			// AnimFirstFrame preserves the correct scale while preventing drift/snap-back.
			AnimSeq->bForceRootLock = true;
			AnimSeq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;

			AnimSeq->CacheDerivedDataForCurrentPlatform();
			Recompressed++;
		}

		// Wait for ALL compression to finish
		FAssetCompilingManager::Get().FinishAllCompilation();

		// Save all recompressed animations
		int32 Saved = 0;
		for (const FAssetData& AssetData : AnimAssets)
		{
			if (AssetData.AssetClassPath.GetAssetName() != TEXT("AnimSequence"))
				continue;

			UAnimSequence* AnimSeq = Cast<UAnimSequence>(AssetData.GetAsset());
			if (!AnimSeq) continue;

			UPackage* Pkg = AnimSeq->GetOutermost();
			Pkg->MarkPackageDirty();
			FString FileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Standalone;
			if (UPackage::SavePackage(Pkg, AnimSeq, *FileName, SaveArgs))
				Saved++;
		}

		UE_LOG(LogTemp, Warning, TEXT("  Re-compressed %d animations, saved %d"), Recompressed, Saved);
	}

	// Step 1: Add sockets
	UE_LOG(LogTemp, Warning, TEXT("--- Step 1: Adding sockets ---"));

	struct FSocketInfo { const TCHAR* Name; const TCHAR* Bone; FVector Offset; };
	const FSocketInfo Sockets[] = {
		{ TEXT("weapon_start"), TEXT("R_Sword"), FVector(0, 0, 50) },
		{ TEXT("weapon_end"),   TEXT("R_Sword"), FVector(0, 0, -50) },
		{ TEXT("hand_r"),       TEXT("R_Hand"),  FVector::ZeroVector },
		{ TEXT("hand_l"),       TEXT("L_Hand"),  FVector::ZeroVector },
		{ TEXT("foot_l"),       TEXT("L_Foot"),  FVector::ZeroVector },
		{ TEXT("foot_r"),       TEXT("R_Foot"),  FVector::ZeroVector },
	};
	for (const auto& S : Sockets)
	{
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, S.Name, S.Bone, S.Offset);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	}

	// Save skeleton after adding sockets
	{
		USkeleton* Skeleton = LoadObject<USkeleton>(nullptr, *SkeletonPath);
		if (Skeleton)
		{
			UPackage* Pkg = Skeleton->GetOutermost();
			Pkg->MarkPackageDirty();
			FString PackageFileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Standalone;
			bool bSaved = UPackage::SavePackage(Pkg, Skeleton, *PackageFileName, SaveArgs);
			UE_LOG(LogTemp, Warning, TEXT("Skeleton save: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skeleton save: FAILED - could not load skeleton"));
		}
	}

	// Step 2: Create montages
	UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Creating montages ---"));

	struct FMontageInfo { const TCHAR* Seq; const TCHAR* Name; };

	const FMontageInfo AttackMontages[] = {
		{ TEXT("a000_003000"), TEXT("AM_c3100_Attack01") },
		{ TEXT("a000_003001"), TEXT("AM_c3100_Attack02") },
		{ TEXT("a000_003017"), TEXT("AM_c3100_Attack03_Fast") },
		{ TEXT("a000_004100"), TEXT("AM_c3100_HeavyAttack") },
	};

	const FMontageInfo ReactionMontages[] = {
		{ TEXT("a000_010000"), TEXT("AM_c3100_HitReact") },
		{ TEXT("a000_010001"), TEXT("AM_c3100_HitReact_Light") },
		{ TEXT("a000_008030"), TEXT("AM_c3100_GuardHit") },
	};

	const FMontageInfo DeathMontages[] = {
		{ TEXT("a000_011010"), TEXT("AM_c3100_Death_Front") },
		{ TEXT("a000_011060"), TEXT("AM_c3100_Death_Back") },
		{ TEXT("a000_011070"), TEXT("AM_c3100_Death_Left") },
	};

	const FMontageInfo DodgeMontages[] = {
		{ TEXT("a000_005000"), TEXT("AM_c3100_Dodge_Fwd") },
		{ TEXT("a000_005001"), TEXT("AM_c3100_Dodge_Bwd") },
		{ TEXT("a000_005002"), TEXT("AM_c3100_Dodge_Left") },
		{ TEXT("a000_005003"), TEXT("AM_c3100_Dodge_Right") },
	};

	const FMontageInfo LocomotionMontages[] = {
		{ TEXT("a000_000020"), TEXT("AM_c3100_Idle") },
		{ TEXT("a000_002000"), TEXT("AM_c3100_Walk") },
		{ TEXT("a000_002100"), TEXT("AM_c3100_Run") },
	};

	auto CreateMontages = [&](const TCHAR* Category, const FMontageInfo* Montages, int32 Count)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Creating %s montages (%d)..."), Category, Count);
		for (int32 i = 0; i < Count; i++)
		{
			FString SeqPath = FString::Printf(TEXT("%s/%s"), *AnimDir, Montages[i].Seq);
			FString OutPath = FString::Printf(TEXT("%s/%s"), *OutputDir, Montages[i].Name);
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
		}
	};

	CreateMontages(TEXT("Attack"), AttackMontages, UE_ARRAY_COUNT(AttackMontages));
	CreateMontages(TEXT("Reaction"), ReactionMontages, UE_ARRAY_COUNT(ReactionMontages));
	CreateMontages(TEXT("Death"), DeathMontages, UE_ARRAY_COUNT(DeathMontages));
	CreateMontages(TEXT("Dodge"), DodgeMontages, UE_ARRAY_COUNT(DodgeMontages));
	CreateMontages(TEXT("Locomotion"), LocomotionMontages, UE_ARRAY_COUNT(LocomotionMontages));

	// Step 3: Create locomotion blend space (2D, matching original ABS_SoulslikeEnemy layout)
	// Using 2D BlendSpace because BlendSpace1D evaluation path has scale issues at runtime.
	// Original wiring: Direction→X, GroundSpeed→Y, which is preserved from the duplicated AnimBP.
	UE_LOG(LogTemp, Warning, TEXT("--- Step 3: Creating 2D blend space ---"));
	Result = USLFAutomationLibrary::CreateBlendSpace2DFromSequences(
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

	// Step 4b: Replace animation references in AnimBP
	UE_LOG(LogTemp, Warning, TEXT("--- Step 4b: Replacing AnimGraph references ---"));
	{
		TMap<FString, FString> AnimReplacementMap;
		// Locomotion blend space
		AnimReplacementMap.Add(
			TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Blendspaces/ABS_SoulslikeEnemy"),
			OutputDir / TEXT("BS_c3100_Locomotion"));
		// Idle animation (Kwang -> c3100 idle)
		AnimReplacementMap.Add(
			TEXT("/Game/SoulslikeFramework/Demo/RetargetedParagonAnimations/Kwang/AS_RT_Idle_Kwang"),
			AnimDir / TEXT("a000_000020"));

		Result = USLFAutomationLibrary::ReplaceAnimReferencesInAnimBP(
			OutputDir / TEXT("ABP_c3100_Guard"),
			AnimReplacementMap);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	}

	// Step 4c: Remove Control Rig + IK Rig nodes (c3100 skeleton has different bone hierarchy than Mannequin)
	UE_LOG(LogTemp, Warning, TEXT("--- Step 4c: Removing Control Rig + IK Rig ---"));
	Result = USLFAutomationLibrary::DisableControlRigInAnimBP(OutputDir / TEXT("ABP_c3100_Guard"));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

	// Step 4d: REMOVED - no longer needed. Using 2D BlendSpace with original Direction→X, GroundSpeed→Y wiring.

	// Step 5: Create data assets
	UE_LOG(LogTemp, Warning, TEXT("--- Step 5: Creating data assets ---"));

	auto CreateAndSaveDataAsset = [](UObject* Asset, UPackage* Pkg, const FString& AssetPath) -> bool
	{
		FAssetRegistryModule::AssetCreated(Asset);
		Pkg->MarkPackageDirty();
		FString FileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		return UPackage::SavePackage(Pkg, Asset, *FileName, SaveArgs);
	};

	// Helper: prepare package (fully load existing, remove old object)
	auto PreparePackage = [](const FString& PkgName, const FString& ObjName) -> UPackage*
	{
		UPackage* ExistingPkg = FindPackage(nullptr, *PkgName);
		if (ExistingPkg)
		{
			ExistingPkg->FullyLoad();
			UObject* ExistingObj = StaticFindObjectFast(nullptr, ExistingPkg, FName(*ObjName));
			if (ExistingObj)
			{
				ExistingObj->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
			}
		}
		UPackage* Pkg = CreatePackage(*PkgName);
		Pkg->FullyLoad();
		return Pkg;
	};

	// 5a: PDA_c3100_AnimData
	{
		FString PkgName = OutputDir / TEXT("PDA_c3100_AnimData");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_c3100_AnimData"));
		UPDA_AnimData* AnimData = NewObject<UPDA_AnimData>(Pkg, TEXT("PDA_c3100_AnimData"), RF_Public | RF_Standalone);

		AnimData->WalkRunLocomotion = LoadObject<UBlendSpace>(nullptr, *(OutputDir / TEXT("BS_c3100_Locomotion")));
		AnimData->Idle = LoadObject<UAnimSequenceBase>(nullptr, *(AnimDir / TEXT("a000_000020")));

		bool bSaved = CreateAndSaveDataAsset(AnimData, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_c3100_AnimData: %s (BS=%s, Idle=%s)"),
			bSaved ? TEXT("SAVED") : TEXT("FAILED"),
			AnimData->WalkRunLocomotion ? TEXT("yes") : TEXT("no"),
			AnimData->Idle ? TEXT("yes") : TEXT("no"));
	}

	// 5b: PDA_c3100_WeaponAnimset
	{
		FString PkgName = OutputDir / TEXT("PDA_c3100_WeaponAnimset");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_c3100_WeaponAnimset"));
		UPDA_WeaponAnimset* WeaponAnimset = NewObject<UPDA_WeaponAnimset>(Pkg, TEXT("PDA_c3100_WeaponAnimset"), RF_Public | RF_Standalone);

		WeaponAnimset->OneH_LightComboMontage_R = LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_Attack01")));
		WeaponAnimset->OneH_HeavyComboMontage_R = LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_HeavyAttack")));
		WeaponAnimset->Guard_R = LoadObject<UAnimSequenceBase>(nullptr, *(AnimDir / TEXT("a000_007000")));
		WeaponAnimset->Guard_R_Hit = LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_GuardHit")));

		bool bSaved = CreateAndSaveDataAsset(WeaponAnimset, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_c3100_WeaponAnimset: %s"), bSaved ? TEXT("SAVED") : TEXT("FAILED"));
	}

	// 5c: PDA_c3100_CombatReaction
	{
		FString PkgName = OutputDir / TEXT("PDA_c3100_CombatReaction");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_c3100_CombatReaction"));
		UPDA_CombatReactionAnimData* Reaction = NewObject<UPDA_CombatReactionAnimData>(Pkg, TEXT("PDA_c3100_CombatReaction"), RF_Public | RF_Standalone);

		Reaction->ReactionMontage = LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_HitReact")));
		Reaction->Death.Add(ESLFDirection::Fwd, LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_Death_Front"))));
		Reaction->Death.Add(ESLFDirection::Bwd, LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_Death_Back"))));
		Reaction->Death.Add(ESLFDirection::Left, LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_Death_Left"))));
		Reaction->Death.Add(ESLFDirection::Right, LoadObject<UAnimMontage>(nullptr, *(OutputDir / TEXT("AM_c3100_Death_Front"))));

		bool bSaved = CreateAndSaveDataAsset(Reaction, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_c3100_CombatReaction: %s"), bSaved ? TEXT("SAVED") : TEXT("FAILED"));
	}

	// 5d: PDA_c3100_PoiseBreak
	{
		FString PkgName = OutputDir / TEXT("PDA_c3100_PoiseBreak");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_c3100_PoiseBreak"));
		UPDA_PoiseBreakAnimData* PoiseBreak = NewObject<UPDA_PoiseBreakAnimData>(Pkg, TEXT("PDA_c3100_PoiseBreak"), RF_Public | RF_Standalone);

		PoiseBreak->PoiseBreak_Start = LoadObject<UAnimSequence>(nullptr, *(AnimDir / TEXT("a000_009210")));
		PoiseBreak->PoiseBreak_Loop = LoadObject<UAnimSequence>(nullptr, *(AnimDir / TEXT("a000_009200")));

		bool bSaved = CreateAndSaveDataAsset(PoiseBreak, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_c3100_PoiseBreak: %s (Start=%s, Loop=%s)"),
			bSaved ? TEXT("SAVED") : TEXT("FAILED"),
			PoiseBreak->PoiseBreak_Start ? TEXT("yes") : TEXT("no"),
			PoiseBreak->PoiseBreak_Loop ? TEXT("yes") : TEXT("no"));
	}

	// Step 6: Create AI Ability data assets
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6: Creating AI Ability data assets ---"));

	struct FAIAbilityDef { const TCHAR* Name; const TCHAR* MontageName; double Score; double Cooldown; };
	const FAIAbilityDef AbilityDefs[] = {
		{ TEXT("DA_c3100_Attack01"),      TEXT("AM_c3100_Attack01"),      1.0, 2.0 },
		{ TEXT("DA_c3100_Attack02"),      TEXT("AM_c3100_Attack02"),      1.0, 2.0 },
		{ TEXT("DA_c3100_Attack03_Fast"), TEXT("AM_c3100_Attack03_Fast"), 1.5, 1.0 },
		{ TEXT("DA_c3100_HeavyAttack"),   TEXT("AM_c3100_HeavyAttack"),  0.5, 5.0 },
	};

	for (const auto& Def : AbilityDefs)
	{
		FString PkgName = OutputDir / Def.Name;
		UPackage* Pkg = PreparePackage(PkgName, Def.Name);
		UPDA_AI_Ability* AIAbility = NewObject<UPDA_AI_Ability>(Pkg, Def.Name, RF_Public | RF_Standalone);

		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(OutputDir / Def.MontageName));
		if (Montage)
		{
			AIAbility->Montage = Montage;
		}
		AIAbility->Score = Def.Score;
		AIAbility->Cooldown = Def.Cooldown;

		bool bSaved = CreateAndSaveDataAsset(AIAbility, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  %s: %s (Montage=%s, Score=%.1f, CD=%.1f)"),
			Def.Name,
			bSaved ? TEXT("SAVED") : TEXT("FAILED"),
			Montage ? TEXT("yes") : TEXT("no"),
			Def.Score, Def.Cooldown);
	}

	// Step 6b: Verify montage lengths persist after save
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6b: Verifying montage lengths after save ---"));
	{
		const TCHAR* MontagesToVerify[] = {
			TEXT("AM_c3100_Attack01"), TEXT("AM_c3100_Attack02"), TEXT("AM_c3100_Attack03_Fast"),
			TEXT("AM_c3100_HeavyAttack"), TEXT("AM_c3100_HitReact"), TEXT("AM_c3100_Idle")
		};
		for (const TCHAR* MontageName : MontagesToVerify)
		{
			FString FullPath = OutputDir / MontageName;
			// Force reload from disk
			UAnimMontage* TestMontage = LoadObject<UAnimMontage>(nullptr, *FullPath);
			if (TestMontage)
			{
				float Len = TestMontage->GetPlayLength();
				UE_LOG(LogTemp, Warning, TEXT("  %s: %.3f sec %s"),
					MontageName, Len, Len > 0.01f ? TEXT("OK") : TEXT("*** ZERO LENGTH ***"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  %s: FAILED TO LOAD"), MontageName);
			}
		}
	}

	// Step 6c: Add weapon trace notifies to attack montages using TAE timings
	// TAE (Time Act Events) from Elden Ring define precise hitbox activation windows.
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6c: Adding weapon trace notifies (TAE timings) ---"));
	{
		struct FWeaponTraceConfig
		{
			const TCHAR* MontageName;
			float StartTime;
			float EndTime;
			float TraceRadius;
		};

		// Hitbox timings from c3100_tae.json, widened for UE5 variable framerate:
		// Original TAE windows are 3-4 frames at 30fps (100-133ms) - too tight for UE5.
		// Widened: start 0.1s earlier (wind-up), end 0.15s later (follow-through).
		// a000_003000 (Attack01):     TAE 1.267-1.367s → 1.167-1.517s
		// a000_003001 (Attack02):     TAE 0.800-0.933s → 0.700-1.083s
		// a000_003017 (Attack03):     TAE 0.800-0.933s → 0.700-1.083s
		// a000_004100 (HeavyAttack):  Impact ~2.3-2.6s → 2.200-2.750s
		const FWeaponTraceConfig TraceConfigs[] = {
			{ TEXT("AM_c3100_Attack01"),      1.167f, 1.517f, 40.0f },
			{ TEXT("AM_c3100_Attack02"),      0.700f, 1.083f, 40.0f },
			{ TEXT("AM_c3100_Attack03_Fast"), 0.700f, 1.083f, 40.0f },
			{ TEXT("AM_c3100_HeavyAttack"),   2.200f, 2.750f, 80.0f },  // AoE slam - larger radius
		};

		for (const auto& Cfg : TraceConfigs)
		{
			Result = USLFAutomationLibrary::AddWeaponTraceToMontage(
				OutputDir / Cfg.MontageName,
				Cfg.StartTime,
				Cfg.EndTime,
				Cfg.TraceRadius
			);
			UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
		}
	}

	// Step 7: Run AnimBP diagnostic
	UE_LOG(LogTemp, Warning, TEXT("--- Step 7: AnimBP Diagnostic ---"));
	{
		FString DiagResult = USLFAutomationLibrary::DiagnoseAnimBPDetailed(OutputDir / TEXT("ABP_c3100_Guard"));
		// Log each line separately so UE doesn't truncate
		TArray<FString> DiagLines;
		DiagResult.ParseIntoArrayLines(DiagLines);
		for (const FString& Line : DiagLines)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DiagAnimBP] %s"), *Line);
		}
	}

	// Step 8: Verify all animations use the FLVER skeleton
	UE_LOG(LogTemp, Warning, TEXT("--- Step 8: Animation skeleton verification ---"));
	{
		USkeleton* ExpectedSkel = LoadObject<USkeleton>(nullptr, *SkeletonPath);
		FString ExpectedName = ExpectedSkel ? ExpectedSkel->GetName() : TEXT("NULL");
		UE_LOG(LogTemp, Warning, TEXT("  Expected skeleton: %s (%d bones)"),
			*ExpectedName, ExpectedSkel ? ExpectedSkel->GetReferenceSkeleton().GetNum() : 0);

		const TCHAR* AnimsToCheck[] = {
			TEXT("a000_000020"), TEXT("a000_002000"), TEXT("a000_002100"),
			TEXT("a000_003000"), TEXT("a000_003001"), TEXT("a000_003017"),
			TEXT("a000_004100"), TEXT("a000_010000"), TEXT("a000_005000"),
		};
		int32 Matched = 0, Mismatched = 0;
		for (const TCHAR* AnimName : AnimsToCheck)
		{
			FString FullPath = AnimDir / AnimName;
			UAnimSequence* Seq = LoadObject<UAnimSequence>(nullptr, *FullPath);
			if (!Seq)
			{
				UE_LOG(LogTemp, Warning, TEXT("  %s: NOT FOUND"), AnimName);
				continue;
			}
			USkeleton* AnimSkel = Seq->GetSkeleton();
			FString AnimSkelName = AnimSkel ? AnimSkel->GetPathName() : TEXT("NULL");
			bool bMatch = (AnimSkel == ExpectedSkel);
			if (bMatch)
			{
				Matched++;
			}
			else
			{
				Mismatched++;
				UE_LOG(LogTemp, Warning, TEXT("  *** MISMATCH: %s uses skeleton: %s (%d bones)"),
					AnimName, *AnimSkelName, AnimSkel ? AnimSkel->GetReferenceSkeleton().GetNum() : 0);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("  Result: %d matched, %d MISMATCHED out of %d checked"),
			Matched, Mismatched, Matched + Mismatched);
	}

	// Step 9: Exhaustive AnimBP comparison dumps
	UE_LOG(LogTemp, Warning, TEXT("--- Step 9: AnimBP comparison dumps ---"));
	{
		FString DumpA = USLFAutomationLibrary::DumpAnimBPExhaustive(OutputDir / TEXT("ABP_c3100_Guard"));
		FString FileA = FPaths::ProjectDir() / TEXT("migration_cache/animBP_dump_c3100.txt");
		FFileHelper::SaveStringToFile(DumpA, *FileA);
		UE_LOG(LogTemp, Warning, TEXT("  Saved c3100 dump (%d chars) to %s"), DumpA.Len(), *FileA);

		// Also dump the original/backup ABP_SoulslikeEnemy for comparison
		FString DumpB = USLFAutomationLibrary::DumpAnimBPExhaustive(
			TEXT("/Game/Temp/AnimBP_Compare/ABP_SoulslikeEnemy"));
		if (DumpB.Contains(TEXT("ERROR")))
		{
			// Try the main project location
			DumpB = USLFAutomationLibrary::DumpAnimBPExhaustive(
				TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"));
		}
		FString FileB = FPaths::ProjectDir() / TEXT("migration_cache/animBP_dump_original.txt");
		FFileHelper::SaveStringToFile(DumpB, *FileB);
		UE_LOG(LogTemp, Warning, TEXT("  Saved original dump (%d chars) to %s"), DumpB.Len(), *FileB);
	}

	// Step 9b: Evaluate bone transforms from key animations to check scale
	UE_LOG(LogTemp, Warning, TEXT("--- Step 9b: Animation bone transform scale check ---"));
	{
		struct FAnimCheck { const TCHAR* Name; const TCHAR* Path; };
		const FAnimCheck Anims[] = {
			{ TEXT("Idle (a000_000020)"),  TEXT("/Game/EldenRingAnimations/c3100_guard/Animations/a000_000020") },
			{ TEXT("Walk (a000_002000)"),  TEXT("/Game/EldenRingAnimations/c3100_guard/Animations/a000_002000") },
			{ TEXT("Run (a000_002100)"),   TEXT("/Game/EldenRingAnimations/c3100_guard/Animations/a000_002100") },
			{ TEXT("Attack (a000_003000)"),TEXT("/Game/EldenRingAnimations/c3100_guard/Animations/a000_003000") },
		};

		USkeleton* Skel = LoadObject<USkeleton>(nullptr, *SkeletonPath);
		if (Skel)
		{
			const FReferenceSkeleton& RefSkel = Skel->GetReferenceSkeleton();
			int32 NumBones = RefSkel.GetNum();

			// First log the reference pose bone spread
			FVector RefMin(BIG_NUMBER), RefMax(-BIG_NUMBER);
			TArray<FTransform> RefComponentSpace;
			RefComponentSpace.SetNum(NumBones);
			for (int32 i = 0; i < NumBones; i++)
			{
				RefComponentSpace[i] = RefSkel.GetRefBonePose()[i];
				int32 ParentIdx = RefSkel.GetParentIndex(i);
				if (ParentIdx >= 0)
				{
					RefComponentSpace[i] = RefComponentSpace[i] * RefComponentSpace[ParentIdx];
				}
				FVector Loc = RefComponentSpace[i].GetLocation();
				RefMin = RefMin.ComponentMin(Loc);
				RefMax = RefMax.ComponentMax(Loc);
			}
			FVector RefSize = RefMax - RefMin;
			UE_LOG(LogTemp, Warning, TEXT("  Reference Pose: AABB Size=(%.2f, %.2f, %.2f) NumBones=%d"),
				RefSize.X, RefSize.Y, RefSize.Z, NumBones);

			// Find Pelvis and Head indices
			int32 PelvisIdx = RefSkel.FindBoneIndex(TEXT("Pelvis"));
			int32 HeadIdx = RefSkel.FindBoneIndex(TEXT("Head"));
			int32 RootIdx = 0;

			if (PelvisIdx >= 0)
				UE_LOG(LogTemp, Warning, TEXT("  RefPose Pelvis[%d]: (%.4f, %.4f, %.4f)"),
					PelvisIdx, RefComponentSpace[PelvisIdx].GetLocation().X,
					RefComponentSpace[PelvisIdx].GetLocation().Y,
					RefComponentSpace[PelvisIdx].GetLocation().Z);
			if (HeadIdx >= 0)
				UE_LOG(LogTemp, Warning, TEXT("  RefPose Head[%d]: (%.4f, %.4f, %.4f)"),
					HeadIdx, RefComponentSpace[HeadIdx].GetLocation().X,
					RefComponentSpace[HeadIdx].GetLocation().Y,
					RefComponentSpace[HeadIdx].GetLocation().Z);

			// Now evaluate each animation at frame 0
			for (const auto& AC : Anims)
			{
				UAnimSequence* Anim = LoadObject<UAnimSequence>(nullptr, AC.Path);
				if (!Anim)
				{
					UE_LOG(LogTemp, Warning, TEXT("  %s: FAILED TO LOAD"), AC.Name);
					continue;
				}

				// Get bone transforms at frame 0
				TArray<FTransform> BoneLocal;
				BoneLocal.SetNum(NumBones);

				// Use GetBoneTransform for each bone at time 0
				FMemMark Mark(FMemStack::Get());
				for (int32 i = 0; i < NumBones; i++)
				{
					FName BoneName = RefSkel.GetBoneName(i);
					FTransform BoneT;
					Anim->GetBoneTransform(BoneT, FSkeletonPoseBoneIndex(i), 0.0, false);
					BoneLocal[i] = BoneT;
				}

				// Build component-space transforms
				TArray<FTransform> AnimComponentSpace;
				AnimComponentSpace.SetNum(NumBones);
				for (int32 i = 0; i < NumBones; i++)
				{
					AnimComponentSpace[i] = BoneLocal[i];
					int32 ParentIdx = RefSkel.GetParentIndex(i);
					if (ParentIdx >= 0)
					{
						AnimComponentSpace[i] = AnimComponentSpace[i] * AnimComponentSpace[ParentIdx];
					}
				}

				// Compute bone spread
				FVector AnimMin(BIG_NUMBER), AnimMax(-BIG_NUMBER);
				for (int32 i = 0; i < NumBones; i++)
				{
					FVector Loc = AnimComponentSpace[i].GetLocation();
					AnimMin = AnimMin.ComponentMin(Loc);
					AnimMax = AnimMax.ComponentMax(Loc);
				}
				FVector AnimSize = AnimMax - AnimMin;

				UE_LOG(LogTemp, Warning, TEXT("  %s: AABB Size=(%.2f, %.2f, %.2f) Duration=%.3fs NumFrames=%d"),
					AC.Name, AnimSize.X, AnimSize.Y, AnimSize.Z,
					Anim->GetPlayLength(), Anim->GetNumberOfSampledKeys());

				if (PelvisIdx >= 0)
					UE_LOG(LogTemp, Warning, TEXT("    Pelvis: (%.4f, %.4f, %.4f)"),
						AnimComponentSpace[PelvisIdx].GetLocation().X,
						AnimComponentSpace[PelvisIdx].GetLocation().Y,
						AnimComponentSpace[PelvisIdx].GetLocation().Z);
				if (HeadIdx >= 0)
					UE_LOG(LogTemp, Warning, TEXT("    Head: (%.4f, %.4f, %.4f)"),
						AnimComponentSpace[HeadIdx].GetLocation().X,
						AnimComponentSpace[HeadIdx].GetLocation().Y,
						AnimComponentSpace[HeadIdx].GetLocation().Z);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("=== SetupC3100 Commandlet: COMPLETE ==="));
	return 0;
}

#else // !WITH_EDITOR

USetupC3100Commandlet::USetupC3100Commandlet() {}
int32 USetupC3100Commandlet::Main(const FString& Params) { return 0; }

#endif // WITH_EDITOR
