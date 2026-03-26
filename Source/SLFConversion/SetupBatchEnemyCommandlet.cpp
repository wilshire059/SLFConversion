// SetupBatchEnemyCommandlet.cpp
// Generic commandlet for importing Ashborne enemies from the Meshy AI + Blender pipeline.
// Handles mesh+texture import, animation import, montage creation, AnimBP, data assets.
//
// Usage:
//   -run=SetupBatchEnemy -name=withered_wanderer
//   -run=SetupBatchEnemy -name=withered_wanderer -skipmesh -skipanims
//   -run=SetupBatchEnemy -name=ALL   (batch all enemies found in test_meshes/)

#include "SetupBatchEnemyCommandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "Animation/SLFAnimNotifyStateWeaponTrace.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace1D.h"
#include "Animation/BlendSpace.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "FileHelpers.h"
#include "AssetImportTask.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "Factories/TextureFactory.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionTime.h"
#include "Materials/MaterialExpressionSine.h"
#include "Materials/MaterialExpressionAbs.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionFresnel.h"
#include "Materials/MaterialExpressionNoise.h"
#include "Materials/MaterialExpressionWorldPosition.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Internationalization/Regex.h"

USetupBatchEnemyCommandlet::USetupBatchEnemyCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

FString USetupBatchEnemyCommandlet::ToPascalCase(const FString& SnakeName)
{
	FString Result;
	bool bCapNext = true;
	for (int32 i = 0; i < SnakeName.Len(); i++)
	{
		TCHAR C = SnakeName[i];
		if (C == '_')
		{
			bCapNext = true;
			continue;
		}
		if (bCapNext)
		{
			Result.AppendChar(FChar::ToUpper(C));
			bCapNext = false;
		}
		else
		{
			Result.AppendChar(C);
		}
	}
	return Result;
}

FString USetupBatchEnemyCommandlet::CategorizeAnimID(const FString& AnimID)
{
	// Elden Ring animation ID convention:
	// a000_000xxx = idle/stance
	// a000_001xxx = transitions/movement
	// a000_002xxx = locomotion (walk/run)
	// a000_003xxx = attacks (light)
	// a000_004xxx = heavy attacks
	// a000_005xxx = dodges/evasion
	// a000_006xxx = damage reactions / hit react
	// a000_007xxx = guard/block
	// a000_008xxx = guard hit reactions
	// a000_009xxx = poise break / stagger
	// a000_010xxx = hit reactions (alt)
	// a000_011xxx = death

	// Extract the middle number group (e.g., "003" from "a000_003001")
	FString NumPart;
	if (AnimID.Len() >= 8)
	{
		NumPart = AnimID.Mid(5, 3); // "003" from "a000_003001"
	}

	int32 Category = FCString::Atoi(*NumPart);

	if (Category == 0) return TEXT("Idle");
	if (Category == 1) return TEXT("Locomotion"); // 001xxx = movement/transitions (walk/run for ER enemies)
	if (Category == 2) return TEXT("Locomotion"); // 002xxx = explicit walk/run (rare in ER enemy sets)
	if (Category == 3) return TEXT("Attack");
	if (Category == 4) return TEXT("HeavyAttack");
	if (Category == 5) return TEXT("Dodge");      // 005xxx = dodges/evasion
	if (Category == 6) return TEXT("HitReact");   // 006xxx = damage reactions
	if (Category == 7) return TEXT("Guard");
	if (Category == 8) return TEXT("GuardHit");
	if (Category == 9) return TEXT("PoiseBreak");
	if (Category == 10) return TEXT("HitReact");
	if (Category == 11) return TEXT("Death");

	return TEXT("Other");
}

int32 USetupBatchEnemyCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Warning, TEXT("=== SetupBatchEnemy Params: [%s] ==="), *Params);

	// Parse -name=<enemy_name>
	FString EnemyName;
	if (!FParse::Value(*Params, TEXT("-name="), EnemyName) && !FParse::Value(*Params, TEXT("name="), EnemyName))
	{
		// Try positional
		TArray<FString> Tokens;
		Params.ParseIntoArrayWS(Tokens);
		for (const FString& T : Tokens)
		{
			if (!T.StartsWith(TEXT("-")))
			{
				EnemyName = T;
				break;
			}
		}
	}

	if (EnemyName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Usage: -run=SetupBatchEnemy -name=withered_wanderer"));
		UE_LOG(LogTemp, Error, TEXT("       -run=SetupBatchEnemy -name=ALL"));
		return 1;
	}

	bool bSkipMesh = Params.Contains(TEXT("-skipmesh"));
	bool bSkipAnims = Params.Contains(TEXT("-skipanims"));

	// Handle -name=ALL: process all enemy directories that have fbx/ subdirs
	TArray<FString> EnemiesToProcess;
	if (EnemyName.Equals(TEXT("ALL"), ESearchCase::IgnoreCase))
	{
		IFileManager& FM = IFileManager::Get();
		TArray<FString> FoundDirs;
		FString SearchPath = FString(SourceBaseDir) / TEXT("*");
		FM.FindFiles(FoundDirs, *SearchPath, false, true);
		for (const FString& Dir : FoundDirs)
		{
			FString FBXDir = FString(SourceBaseDir) / Dir / TEXT("fbx");
			if (FM.DirectoryExists(*FBXDir))
			{
				TArray<FString> FBXFiles;
				FM.FindFiles(FBXFiles, *(FBXDir / TEXT("*.fbx")), true, false);
				if (FBXFiles.Num() > 0)
				{
					EnemiesToProcess.Add(Dir);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Found %d enemies to process"), EnemiesToProcess.Num());
	}
	else
	{
		EnemiesToProcess.Add(EnemyName);
	}

	// Initialize Slate (needed for UAssetImportTask)
	if (!FSlateApplication::IsInitialized())
	{
		FSlateApplication::Create();
	}

	int32 SuccessCount = 0;

	for (const FString& Enemy : EnemiesToProcess)
	{
		FString PascalName = ToPascalCase(Enemy);
		FString SourceDir = FString(SourceBaseDir) / Enemy;
		FString FBXDir = SourceDir / TEXT("fbx");
		FString DestDir = FString::Printf(TEXT("/Game/CustomEnemies/%s"), *PascalName);
		FString AnimDir = DestDir / TEXT("Animations");

		UE_LOG(LogTemp, Warning, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("╔══════════════════════════════════════════════════════════════╗"));
		UE_LOG(LogTemp, Warning, TEXT("║  Processing: %s (%s)"), *Enemy, *PascalName);
		UE_LOG(LogTemp, Warning, TEXT("║  Source: %s"), *SourceDir);
		UE_LOG(LogTemp, Warning, TEXT("║  Dest:   %s"), *DestDir);
		UE_LOG(LogTemp, Warning, TEXT("╚══════════════════════════════════════════════════════════════╝"));

		// Verify source directory exists
		if (!IFileManager::Get().DirectoryExists(*SourceDir))
		{
			UE_LOG(LogTemp, Error, TEXT("  Source directory not found: %s"), *SourceDir);
			continue;
		}

		// ═══════════════════════════════════════════════════════════════
		// Step 0: Import Mesh
		// ═══════════════════════════════════════════════════════════════
		if (bSkipMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("--- Step 0: SKIP mesh import (-skipmesh) ---"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("--- Step 0: Import mesh ---"));

			// Find mesh FBX — PREFER fbx/ directory (contains rigged mesh with full skeleton)
			// model.fbx is raw Meshy output with NO skeleton (only 1 bone).
			FString MeshFBXPath;
			{
				TArray<FString> AnimFBXs;
				IFileManager::Get().FindFiles(AnimFBXs, *(FBXDir / TEXT("*.fbx")), true, false);
				if (AnimFBXs.Num() > 0)
				{
					// Use first animation FBX — it contains rigged mesh + c3100 skeleton
					AnimFBXs.Sort();
					MeshFBXPath = FBXDir / AnimFBXs[0];
					UE_LOG(LogTemp, Warning, TEXT("  Using rigged FBX for mesh: %s"), *AnimFBXs[0]);
				}
				else
				{
					// Fallback to model.fbx (raw Meshy, no skeleton)
					MeshFBXPath = SourceDir / TEXT("model.fbx");
				}
			}

			if (FPaths::FileExists(MeshFBXPath))
			{
				FString MeshName = FString::Printf(TEXT("SKM_%s"), *PascalName);
				bool bMeshOk = ImportMesh(MeshFBXPath, DestDir, MeshName);
				if (!bMeshOk)
				{
					UE_LOG(LogTemp, Error, TEXT("  Mesh import FAILED for %s"), *Enemy);
					continue;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  No mesh FBX found for %s"), *Enemy);
				continue;
			}
		}

		// Load skeleton (created during mesh import)
		FString SkeletonAssetPath = FString::Printf(TEXT("%s/SKM_%s_Skeleton"), *DestDir, *PascalName);
		FString SkeletonObjPath = SkeletonAssetPath + FString::Printf(TEXT(".SKM_%s_Skeleton"), *PascalName);
		USkeleton* Skeleton = LoadObject<USkeleton>(nullptr, *SkeletonObjPath);

		if (!Skeleton)
		{
			// Try alternate naming (UE may name it differently)
			FString AltPath = FString::Printf(TEXT("%s/SKM_%s_Skeleton.SKM_%s_Skeleton"), *DestDir, *PascalName, *PascalName);
			Skeleton = LoadObject<USkeleton>(nullptr, *AltPath);
		}

		if (!Skeleton)
		{
			UE_LOG(LogTemp, Error, TEXT("  FATAL: Skeleton not found at %s"), *SkeletonObjPath);
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("  Skeleton: %s (%d bones)"),
			*Skeleton->GetName(), Skeleton->GetReferenceSkeleton().GetNum());

		// Sync skeleton ref pose from mesh + set retargeting
		{
			FString MeshObjPath = FString::Printf(TEXT("%s/SKM_%s.SKM_%s"), *DestDir, *PascalName, *PascalName);
			USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *MeshObjPath);
			if (Mesh)
			{
				Skeleton->UpdateReferencePoseFromMesh(Mesh);

				const FReferenceSkeleton& SkelRef = Skeleton->GetReferenceSkeleton();
				for (int32 i = 0; i < SkelRef.GetRawBoneNum(); i++)
				{
					FName BoneName = SkelRef.GetBoneName(i);
					int32 ParentIdx = SkelRef.GetParentIndex(i);
					if (ParentIdx == INDEX_NONE)
					{
						Skeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::Animation);
					}
					else if (BoneName == FName(TEXT("pelvis")) || BoneName == FName(TEXT("root")) || BoneName == FName(TEXT("root_motion")))
					{
						Skeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::AnimationScaled);
					}
					else
					{
						Skeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::Skeleton);
					}
				}

				Skeleton->SetPreviewMesh(Mesh, true);

				// Save skeleton
				UPackage* SkelPkg = Skeleton->GetOutermost();
				SkelPkg->MarkPackageDirty();
				FString SkelFile = FPackageName::LongPackageNameToFilename(SkelPkg->GetName(), FPackageName::GetAssetPackageExtension());
				FSavePackageArgs SkelSaveArgs;
				SkelSaveArgs.TopLevelFlags = RF_Standalone;
				UPackage::SavePackage(SkelPkg, Skeleton, *SkelFile, SkelSaveArgs);

				UE_LOG(LogTemp, Warning, TEXT("  Skeleton synced, retargeting set, saved"));
			}
		}

		// ═══════════════════════════════════════════════════════════════
		// Step 0b: Texture import (SKIPPED in commandlet — use SLF.SetupTextures post-import)
		// UAssetImportTask for textures crashes in commandlet mode (Slate/ContentBrowser null ptr).
		// Textures can be imported via SLF.SetupSentinelTextures pattern after commandlet completes.
		// ═══════════════════════════════════════════════════════════════
		// Texture import skipped when -skipmesh (crashes on sequential commandlet runs due to Slate/ContentBrowser)
		if (!bSkipMesh)
		{
			ImportTexturesAndCreateMaterial(SourceDir, DestDir, PascalName);
		}

		// ═══════════════════════════════════════════════════════════════
		// Step 1: Import Animations
		// ═══════════════════════════════════════════════════════════════
		TMap<FString, FString> AnimCategoryMap; // AnimName -> Category

		// Check for animation_config.json — if present, use config-driven import
		FString ConfigPath = SourceDir / TEXT("animation_config.json");
		bool bHasAnimConfig = FPaths::FileExists(ConfigPath);

		if (bHasAnimConfig)
		{
			UE_LOG(LogTemp, Warning, TEXT("--- Config-driven import: %s ---"), *ConfigPath);

			FString ConfigJson;
			FFileHelper::LoadFileToString(ConfigJson, *ConfigPath);
			TSharedPtr<FJsonObject> Config;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ConfigJson);
			FJsonSerializer::Deserialize(Reader, Config);

			if (!Config.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("  Failed to parse animation_config.json"));
				continue;
			}

			// Helper: import a specific FBX by source+anim_id, assign a specific montage name
			auto ImportAndCreateMontage = [&](const FString& Source, const FString& AnimId,
				const FString& MontageSuffix, bool bNeedsRootMotion = false) -> bool
			{
				// Find the FBX: {enemy}_{source}_{anim_id}.fbx
				FString FBXName = FString::Printf(TEXT("%s_%s_%s.fbx"), *Enemy, *Source, *AnimId);
				FString FBXPath = FBXDir / FBXName;
				if (!FPaths::FileExists(FBXPath))
				{
					UE_LOG(LogTemp, Warning, TEXT("    FBX not found: %s"), *FBXName);
					return false;
				}

				// Import animation
				FString AnimName = FString::Printf(TEXT("%s_%s"), *PascalName, *MontageSuffix);
				FAssetToolsModule& ATModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

				UAssetImportTask* Task = NewObject<UAssetImportTask>();
				Task->AddToRoot();
				Task->Filename = FBXPath;
				Task->DestinationPath = AnimDir;
				Task->DestinationName = AnimName;
				Task->bAutomated = true;
				Task->bReplaceExisting = true;
				Task->bSave = true;

				UFbxFactory* Factory = NewObject<UFbxFactory>();
				Factory->SetDetectImportTypeOnImport(false);
				UFbxImportUI* UI = Factory->ImportUI;
				UI->bImportMesh = false;
				UI->bImportMaterials = false;
				UI->bImportTextures = false;
				UI->bImportAnimations = true;
				UI->MeshTypeToImport = FBXIT_Animation;
				UI->Skeleton = Skeleton;
				UI->bAutomatedImportShouldDetectType = false;
				UI->bOverrideFullName = true;
				UI->bCreatePhysicsAsset = false;
				UI->AnimSequenceImportData->bSnapToClosestFrameBoundary = true;
				UI->AnimSequenceImportData->AnimationLength = FBXALIT_ExportedTime;

				Task->Factory = Factory;
				Task->Options = UI;
				ATModule.Get().ImportAssetTasks({Task});

				bool bImported = false;
				UAnimSequence* ImportedSeq = nullptr;
				for (UObject* Obj : Task->GetObjects())
				{
					if (UAnimSequence* Seq = Cast<UAnimSequence>(Obj))
					{
						if (bNeedsRootMotion)
						{
							// Dodge/movement anims: enable root motion so character translates
							Seq->bForceRootLock = false;
							Seq->bEnableRootMotion = true;
						}
						else
						{
							// Attack/locomotion anims: lock root to prevent sliding
							Seq->bForceRootLock = true;
							Seq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;
						}
						ImportedSeq = Seq;
						bImported = true;
					}
				}
				Task->RemoveFromRoot();

				if (!bImported || !ImportedSeq)
				{
					UE_LOG(LogTemp, Warning, TEXT("    Failed to import: %s"), *FBXName);
					return false;
				}

				// Force save the imported animation to disk BEFORE creating montage
				// This fixes Bug #21: package not flushed before LoadObject in montage creation
				UPackage* SeqPackage = ImportedSeq->GetOutermost();
				if (SeqPackage)
				{
					FString PackageFilename;
					if (FPackageName::DoesPackageExist(SeqPackage->GetName(), &PackageFilename) || true)
					{
						PackageFilename = FPackageName::LongPackageNameToFilename(SeqPackage->GetName(), FPackageName::GetAssetPackageExtension());
						FSavePackageArgs SaveArgs;
						SaveArgs.TopLevelFlags = RF_Standalone;
						UPackage::SavePackage(SeqPackage, ImportedSeq, *PackageFilename, SaveArgs);
					}
				}

				// Create montage
				FString MontageName = FString::Printf(TEXT("AM_%s_%s"), *PascalName, *MontageSuffix);
				FString MontagePath = DestDir / MontageName;
				FString AnimPath = AnimDir / AnimName;
				FString Result = USLFAutomationLibrary::CreateMontageFromSequence(AnimPath, MontagePath, TEXT("DefaultSlot"));
				UE_LOG(LogTemp, Warning, TEXT("  %s (%s/%s) -> %s"), *MontageName, *Source, *AnimId,
					Result.Contains(TEXT("OK")) || Result.Contains(TEXT("Created")) ? TEXT("OK") : *Result);
				return true;
			};

			// Import attacks
			const TArray<TSharedPtr<FJsonValue>>* Attacks;
			if (Config->TryGetArrayField(TEXT("attacks"), Attacks))
			{
				for (int32 i = 0; i < Attacks->Num(); i++)
				{
					const TSharedPtr<FJsonObject>& A = (*Attacks)[i]->AsObject();
					FString Src = A->GetStringField(TEXT("source"));
					FString Aid = A->GetStringField(TEXT("anim_id"));
					FString Suffix = FString::Printf(TEXT("Attack%02d"), i + 1);
					ImportAndCreateMontage(Src, Aid, Suffix);
				}
			}

			// Import heavy attacks
			const TArray<TSharedPtr<FJsonValue>>* HeavyAttacks;
			if (Config->TryGetArrayField(TEXT("heavy_attacks"), HeavyAttacks))
			{
				for (int32 i = 0; i < HeavyAttacks->Num(); i++)
				{
					const TSharedPtr<FJsonObject>& A = (*HeavyAttacks)[i]->AsObject();
					FString Src = A->GetStringField(TEXT("source"));
					FString Aid = A->GetStringField(TEXT("anim_id"));
					FString Suffix = (HeavyAttacks->Num() == 1)
						? TEXT("HeavyAttack")
						: FString::Printf(TEXT("HeavyAttack%02d"), i + 1);
					ImportAndCreateMontage(Src, Aid, Suffix);
				}
			}

			// Import locomotion
			auto ImportSingle = [&](const FString& Key, const FString& Suffix)
			{
				const TSharedPtr<FJsonObject>* Obj;
				if (Config->TryGetObjectField(Key, Obj))
				{
					FString Src = (*Obj)->GetStringField(TEXT("source"));
					FString Aid = (*Obj)->GetStringField(TEXT("anim_id"));
					ImportAndCreateMontage(Src, Aid, Suffix);
				}
			};
			ImportSingle(TEXT("idle"), TEXT("Idle"));
			ImportSingle(TEXT("walk"), TEXT("Walk"));
			ImportSingle(TEXT("run"), TEXT("Run"));
			ImportSingle(TEXT("guard"), TEXT("Guard"));

			// Import arrays
			auto ImportArray = [&](const FString& Key, const FString& Prefix, int32 Max, bool bRootMotion = false)
			{
				const TArray<TSharedPtr<FJsonValue>>* Arr;
				if (Config->TryGetArrayField(Key, Arr))
				{
					for (int32 i = 0; i < FMath::Min(Arr->Num(), Max); i++)
					{
						const TSharedPtr<FJsonObject>& A = (*Arr)[i]->AsObject();
						FString Src = A->GetStringField(TEXT("source"));
						FString Aid = A->GetStringField(TEXT("anim_id"));
						FString Suffix = FString::Printf(TEXT("%s%02d"), *Prefix, i + 1);
						ImportAndCreateMontage(Src, Aid, Suffix, bRootMotion);
					}
				}
			};
			ImportArray(TEXT("dodges"), TEXT("Dodge"), 4, false);  // No root motion — AI applies controlled impulse
			ImportArray(TEXT("hit_reacts"), TEXT("HitReact"), 3);
			ImportArray(TEXT("deaths"), TEXT("Death"), 3);

			// Populate AnimCategoryMap for downstream steps (blend space, data assets)
			FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			TArray<FAssetData> AnimAssets;
			ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, false);
			for (const FAssetData& AD : AnimAssets)
			{
				if (AD.AssetClassPath.GetAssetName() == FName("AnimSequence"))
				{
					FString AnimName = AD.AssetName.ToString();
					// Categorize by suffix: Attack01 -> Attack, Walk -> Locomotion, etc.
					if (AnimName.Contains(TEXT("Attack"))) AnimCategoryMap.Add(AnimName, TEXT("Attack"));
					else if (AnimName.Contains(TEXT("Heavy"))) AnimCategoryMap.Add(AnimName, TEXT("HeavyAttack"));
					else if (AnimName.Contains(TEXT("Idle"))) AnimCategoryMap.Add(AnimName, TEXT("Idle"));
					else if (AnimName.Contains(TEXT("Walk")) || AnimName.Contains(TEXT("Run"))) AnimCategoryMap.Add(AnimName, TEXT("Locomotion"));
					else if (AnimName.Contains(TEXT("Dodge"))) AnimCategoryMap.Add(AnimName, TEXT("Dodge"));
					else if (AnimName.Contains(TEXT("HitReact"))) AnimCategoryMap.Add(AnimName, TEXT("HitReact"));
					else if (AnimName.Contains(TEXT("Death"))) AnimCategoryMap.Add(AnimName, TEXT("Death"));
					else if (AnimName.Contains(TEXT("Guard"))) AnimCategoryMap.Add(AnimName, TEXT("Guard"));
				}
			}

			// Add sockets
			AddSockets(SkeletonObjPath);

			// Skip legacy import + montage creation (already done above)
			goto SkipLegacyMontages;
		}

		if (bSkipAnims)
		{
			UE_LOG(LogTemp, Warning, TEXT("--- Step 1: SKIP animations (-skipanims) ---"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("--- Step 1: Import animations (legacy scan) ---"));
			int32 AnimCount = ImportAnimations(FBXDir, AnimDir, PascalName, Skeleton);
			UE_LOG(LogTemp, Warning, TEXT("  Imported %d animations"), AnimCount);
		}

		// Discover all imported animations and categorize them
		{
			FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			TArray<FAssetData> AnimAssets;
			ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, false);
			for (const FAssetData& AD : AnimAssets)
			{
				if (AD.AssetClassPath.GetAssetName() == FName("AnimSequence"))
				{
					FString AnimName = AD.AssetName.ToString();
					int32 AIdx = AnimName.Find(TEXT("a000_"));
					FString AnimID = (AIdx != INDEX_NONE) ? AnimName.Mid(AIdx) : TEXT("");
					FString Category = CategorizeAnimID(AnimID);
					AnimCategoryMap.Add(AnimName, Category);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("  Found %d animations in registry"), AnimCategoryMap.Num());
		}

		// ═══════════════════════════════════════════════════════════════
		// Step 2: Add sockets
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Adding sockets ---"));
		AddSockets(SkeletonObjPath);

		// ═══════════════════════════════════════════════════════════════
		// Step 3: Create montages (legacy scan-based)
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 3: Creating montages ---"));
		CreateMontages(DestDir, AnimDir, PascalName, AnimCategoryMap);

		SkipLegacyMontages:

		// ═══════════════════════════════════════════════════════════════
		// Step 4: Create blend space
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 4: Creating blend space ---"));
		CreateBlendSpace(DestDir, AnimDir, PascalName, SkeletonObjPath);

		// ═══════════════════════════════════════════════════════════════
		// Step 5: Create AnimBP
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 5: Creating AnimBP ---"));
		CreateAnimBP(DestDir, AnimDir, PascalName, SkeletonObjPath);

		// ═══════════════════════════════════════════════════════════════
		// Step 6: Create data assets
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 6: Creating data assets ---"));
		CreateDataAssets(DestDir, AnimDir, PascalName, Enemy);

		// ═══════════════════════════════════════════════════════════════
		// Step 7: Add weapon traces
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 7: Adding weapon traces ---"));
		AddWeaponTraces(DestDir, PascalName, Enemy);

		// ═══════════════════════════════════════════════════════════════
		// Step 8: Create Blueprint (parented to ASLFEnemyGeneric, with EnemyTypeName set)
		// ═══════════════════════════════════════════════════════════════
		UE_LOG(LogTemp, Warning, TEXT("--- Step 8: Creating Blueprint ---"));
		{
			FString BPDir = TEXT("/Game/CustomEnemies") / PascalName;
			FString BPName = FString::Printf(TEXT("B_%s"), *PascalName);

			UClass* GenericClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Script/SLFConversion.SLFEnemyGeneric"));
			if (GenericClass)
			{
				// Delete existing
				FString BPDiskPath = FPackageName::LongPackageNameToFilename(BPDir / BPName, FPackageName::GetAssetPackageExtension());
				if (FPaths::FileExists(BPDiskPath))
				{
					IFileManager::Get().Delete(*BPDiskPath, false, true);
				}

				UPackage* BPPkg = CreatePackage(*(BPDir / BPName));
				if (BPPkg)
				{
					UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
						GenericClass, BPPkg, FName(*BPName),
						BPTYPE_Normal, UBlueprint::StaticClass(),
						UBlueprintGeneratedClass::StaticClass());
					if (NewBP)
					{
						// Set EnemyTypeName on the CDO
						AActor* CDO = Cast<AActor>(NewBP->GeneratedClass->GetDefaultObject());
						if (CDO)
						{
							FProperty* Prop = NewBP->GeneratedClass->FindPropertyByName(FName(TEXT("EnemyTypeName")));
							if (Prop)
							{
								FStrProperty* StrProp = CastField<FStrProperty>(Prop);
								if (StrProp)
								{
									StrProp->SetPropertyValue_InContainer(CDO, PascalName);
								}
							}
						}

						FKismetEditorUtilities::CompileBlueprint(NewBP, EBlueprintCompileOptions::SkipGarbageCollection);

						FSavePackageArgs SaveArgs;
						SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
						FString Filename = FPackageName::LongPackageNameToFilename(BPPkg->GetName(), FPackageName::GetAssetPackageExtension());
						UPackage::SavePackage(BPPkg, NewBP, *Filename, SaveArgs);

						UE_LOG(LogTemp, Warning, TEXT("  Created %s (EnemyTypeName=%s)"), *BPName, *PascalName);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  ASLFEnemyGeneric class not found!"));
			}
		}

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

		UE_LOG(LogTemp, Warning, TEXT("=== %s COMPLETE ==="), *PascalName);
		SuccessCount++;
	}

	UE_LOG(LogTemp, Warning, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("=== SetupBatchEnemy: %d/%d enemies processed ==="), SuccessCount, EnemiesToProcess.Num());
	return 0;
}

bool USetupBatchEnemyCommandlet::ImportMesh(const FString& FBXPath, const FString& DestDir, const FString& DestName)
{
	UAssetImportTask* MeshTask = NewObject<UAssetImportTask>();
	MeshTask->AddToRoot();
	MeshTask->Filename = FBXPath;
	MeshTask->DestinationPath = DestDir;
	MeshTask->DestinationName = DestName;
	MeshTask->bAutomated = true;
	MeshTask->bReplaceExisting = true;
	MeshTask->bSave = true;

	UFbxFactory* MeshFactory = NewObject<UFbxFactory>();
	MeshFactory->SetDetectImportTypeOnImport(false);

	UFbxImportUI* MeshUI = MeshFactory->ImportUI;
	MeshUI->bImportMesh = true;
	MeshUI->bImportAnimations = false;
	MeshUI->bImportMaterials = false;
	MeshUI->bImportTextures = false;
	MeshUI->MeshTypeToImport = FBXIT_SkeletalMesh;
	MeshUI->bAutomatedImportShouldDetectType = false;
	MeshUI->bOverrideFullName = true;
	MeshUI->bCreatePhysicsAsset = false;
	MeshUI->SkeletalMeshImportData->bUseT0AsRefPose = true;

	MeshTask->Factory = MeshFactory;
	MeshTask->Options = MeshUI;

	FAssetToolsModule& ATModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	ATModule.Get().ImportAssetTasks({MeshTask});

	TArray<UObject*> Results = MeshTask->GetObjects();
	bool bOk = false;
	for (UObject* Obj : Results)
	{
		if (USkeletalMesh* SM = Cast<USkeletalMesh>(Obj))
		{
			// Enable CPU access on all LODs via GetLODInfo (NOT GetLODInfoArray — different in UE5.5+)
			for (int32 LODIdx = 0; LODIdx < SM->GetLODNum(); LODIdx++)
			{
				if (FSkeletalMeshLODInfo* Info = SM->GetLODInfo(LODIdx))
				{
					Info->bAllowCPUAccess = true;
				}
			}

			// Must release old resources, rebuild, and reinit for CPU buffers to be created
			SM->ReleaseResources();
			SM->ReleaseResourcesFence.Wait();
			// Build() triggers async rebuild - force synchronous completion
			SM->Build();
			FAssetCompilingManager::Get().FinishAllCompilation();
			SM->InitResources();

			// Verify the render data has CPU access
			if (FSkeletalMeshRenderData* RD = SM->GetResourceForRendering())
			{
				for (int32 LODIdx = 0; LODIdx < RD->LODRenderData.Num(); LODIdx++)
				{
					bool bHasCPU = RD->LODRenderData[LODIdx].StaticVertexBuffers.PositionVertexBuffer.GetAllowCPUAccess();
					UE_LOG(LogTemp, Warning, TEXT("  LOD[%d] VertexBuffer CPU access: %s"),
						LODIdx, bHasCPU ? TEXT("YES") : TEXT("NO"));
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("  Mesh imported: %s (%d bones, CPU access on %d LODs)"),
				*SM->GetName(), SM->GetRefSkeleton().GetRawBoneNum(), SM->GetLODInfoArray().Num());
			bOk = true;
			break;
		}
	}
	if (!bOk)
	{
		UE_LOG(LogTemp, Error, TEXT("  Mesh import returned %d objects, none are SkeletalMesh"), Results.Num());
	}
	MeshTask->RemoveFromRoot();
	return bOk;
}

bool USetupBatchEnemyCommandlet::ImportTexturesAndCreateMaterial(
	const FString& EnemyDir, const FString& DestDir, const FString& PascalName)
{
	// Import PBR textures if they exist
	struct FTexImport { const TCHAR* FileName; const TCHAR* Suffix; };
	const FTexImport TextureFiles[] = {
		{ TEXT("texture_base_color.png"), TEXT("_BaseColor") },
		{ TEXT("texture_metallic.png"),   TEXT("_Metallic") },
		{ TEXT("texture_roughness.png"),  TEXT("_Roughness") },
		{ TEXT("texture_normal.png"),     TEXT("_Normal") },
	};

	FAssetToolsModule& ATModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TMap<FString, UTexture2D*> ImportedTextures;

	// Lambda to load existing textures from Content
	auto TryLoadExistingTex = [&](const FString& Suffix) -> UTexture2D*
	{
		FString Path = FString::Printf(TEXT("%s/T_%s%s"), *DestDir, *PascalName, *Suffix);
		return LoadObject<UTexture2D>(nullptr, *Path);
	};

	// Check if textures already exist — skip import loop (crashes in commandlet mode)
	if (TryLoadExistingTex(TEXT("_BaseColor")))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Textures already exist in Content, skipping import"));
		goto CreateMaterial;
	}

	// Use UTextureFactory::FactoryCreateBinary (works in commandlet mode).
	// UAssetImportTask goes through Interchange which crashes on Slate/ContentBrowser null ptr.
	for (const auto& TexFile : TextureFiles)
	{
		FString FilePath = EnemyDir / TexFile.FileName;
		if (!FPaths::FileExists(FilePath))
		{
			UE_LOG(LogTemp, Warning, TEXT("  Texture not found: %s"), TexFile.FileName);
			continue;
		}

		FString TexName = FString::Printf(TEXT("T_%s%s"), *PascalName, TexFile.Suffix);
		FString PackagePath = DestDir / TexName;
		UPackage* TexPackage = CreatePackage(*PackagePath);
		if (!TexPackage) continue;

		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to read: %s"), *FilePath);
			continue;
		}

		UTextureFactory* Factory = NewObject<UTextureFactory>();
		Factory->SuppressImportOverwriteDialog();

		const uint8* DataPtr = FileData.GetData();
		UObject* ImportedObj = Factory->FactoryCreateBinary(
			UTexture2D::StaticClass(),
			TexPackage,
			FName(*TexName),
			RF_Public | RF_Standalone,
			nullptr,
			TEXT("png"),
			DataPtr,
			DataPtr + FileData.Num(),
			GWarn);

		if (UTexture2D* Tex = Cast<UTexture2D>(ImportedObj))
		{
			FAssetCompilingManager::Get().FinishAllCompilation();
			TexPackage->MarkPackageDirty();
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			UPackage::SavePackage(TexPackage, Tex,
				*FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()),
				SaveArgs);
			ImportedTextures.Add(FString(TexFile.Suffix), Tex);
			UE_LOG(LogTemp, Warning, TEXT("  Imported texture: %s (%dx%d)"), *TexName, Tex->GetSizeX(), Tex->GetSizeY());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to import texture: %s"), *FilePath);
		}
	}

CreateMaterial:
	UTexture2D* T_BaseColor = TryLoadExistingTex(TEXT("_BaseColor"));
	UTexture2D* T_Metallic  = TryLoadExistingTex(TEXT("_Metallic"));
	UTexture2D* T_Normal    = TryLoadExistingTex(TEXT("_Normal"));
	UTexture2D* T_Roughness = TryLoadExistingTex(TEXT("_Roughness"));

	if (!T_BaseColor)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No base color texture, skipping material creation"));
		return false;
	}

	// Create full UMaterial (not MaterialInstance) with proper PBR setup
	// Same pattern as SetupSentinelMaterial — works with skeletal meshes
	FString MatName = FString::Printf(TEXT("M_%s"), *PascalName);
	FString MatPkgName = DestDir / MatName;
	UPackage* MatPkg = PreparePackage(MatPkgName, MatName);

	UMaterial* Material = NewObject<UMaterial>(MatPkg, FName(*MatName), RF_Public | RF_Standalone);
	Material->TwoSided = false;
	Material->SetShadingModel(MSM_DefaultLit);

	auto MakeTextureSample = [&](UTexture2D* Tex, EMaterialSamplerType SamplerType, int32 PosX, int32 PosY) -> UMaterialExpressionTextureSample*
	{
		if (!Tex) return nullptr;
		UMaterialExpressionTextureSample* TexExpr = NewObject<UMaterialExpressionTextureSample>(Material);
		TexExpr->Texture = Tex;
		TexExpr->SamplerType = SamplerType;
		TexExpr->MaterialExpressionEditorX = PosX;
		TexExpr->MaterialExpressionEditorY = PosY;
		Material->GetExpressionCollection().AddExpression(TexExpr);
		return TexExpr;
	};

	// Bug #33: All Meshy textures imported as sRGB. Use SAMPLERTYPE_Color for all
	// to match the texture compression. Visual result is identical in-game.
	// Plain PBR textures — original look preserved
	if (auto* Expr = MakeTextureSample(T_BaseColor, SAMPLERTYPE_Color, -400, 0))
		Material->GetEditorOnlyData()->BaseColor.Connect(0, Expr);
	if (auto* Expr = MakeTextureSample(T_Metallic, SAMPLERTYPE_Color, -400, 200))
		Material->GetEditorOnlyData()->Metallic.Connect(0, Expr);
	if (auto* Expr = MakeTextureSample(T_Normal, SAMPLERTYPE_Color, -400, 400))
		Material->GetEditorOnlyData()->Normal.Connect(0, Expr);
	if (auto* Expr = MakeTextureSample(T_Roughness, SAMPLERTYPE_Color, -400, 600))
		Material->GetEditorOnlyData()->Roughness.Connect(0, Expr);

	// WPO: Noise-driven surface morphing (mesh writhes/pulses organically)
	{
		auto AddExpr = [&](UMaterialExpression* E) {
			Material->GetExpressionCollection().AddExpression(E);
		};

		auto* WorldPos = NewObject<UMaterialExpressionWorldPosition>(Material);
		WorldPos->MaterialExpressionEditorX = -600;
		WorldPos->MaterialExpressionEditorY = 800;
		AddExpr(WorldPos);

		auto* TimeNode = NewObject<UMaterialExpressionTime>(Material);
		TimeNode->MaterialExpressionEditorX = -600;
		TimeNode->MaterialExpressionEditorY = 900;
		AddExpr(TimeNode);

		auto* ScrollVec = NewObject<UMaterialExpressionConstant3Vector>(Material);
		ScrollVec->Constant = FLinearColor(5.0f, 5.0f, 30.0f);
		ScrollVec->MaterialExpressionEditorX = -600;
		ScrollVec->MaterialExpressionEditorY = 960;
		AddExpr(ScrollVec);

		auto* TimeScroll = NewObject<UMaterialExpressionMultiply>(Material);
		TimeScroll->A.Connect(0, TimeNode);
		TimeScroll->B.Connect(0, ScrollVec);
		TimeScroll->MaterialExpressionEditorX = -450;
		TimeScroll->MaterialExpressionEditorY = 900;
		AddExpr(TimeScroll);

		auto* AnimatedPos = NewObject<UMaterialExpressionAdd>(Material);
		AnimatedPos->A.Connect(0, WorldPos);
		AnimatedPos->B.Connect(0, TimeScroll);
		AnimatedPos->MaterialExpressionEditorX = -300;
		AnimatedPos->MaterialExpressionEditorY = 850;
		AddExpr(AnimatedPos);

		auto* NoiseNode = NewObject<UMaterialExpressionNoise>(Material);
		NoiseNode->Position.Connect(0, AnimatedPos);
		NoiseNode->Scale = 0.02f;
		NoiseNode->Quality = 2;
		NoiseNode->NoiseFunction = NOISEFUNCTION_GradientALU;
		NoiseNode->bTurbulence = true;
		NoiseNode->Levels = 3;
		NoiseNode->OutputMin = 0.0f;
		NoiseNode->OutputMax = 1.0f;
		NoiseNode->LevelScale = 2.0f;
		NoiseNode->MaterialExpressionEditorX = -150;
		NoiseNode->MaterialExpressionEditorY = 850;
		AddExpr(NoiseNode);

		auto* NormalWS = NewObject<UMaterialExpressionVertexNormalWS>(Material);
		NormalWS->MaterialExpressionEditorX = -150;
		NormalWS->MaterialExpressionEditorY = 1000;
		AddExpr(NormalWS);

		auto* DisplaceAmount = NewObject<UMaterialExpressionConstant>(Material);
		DisplaceAmount->R = 5.0f;
		DisplaceAmount->MaterialExpressionEditorX = -150;
		DisplaceAmount->MaterialExpressionEditorY = 1060;
		AddExpr(DisplaceAmount);

		auto* NoiseMul = NewObject<UMaterialExpressionMultiply>(Material);
		NoiseMul->A.Connect(0, NoiseNode);
		NoiseMul->B.Connect(0, DisplaceAmount);
		NoiseMul->MaterialExpressionEditorX = 0;
		NoiseMul->MaterialExpressionEditorY = 900;
		AddExpr(NoiseMul);

		auto* FinalWPO = NewObject<UMaterialExpressionMultiply>(Material);
		FinalWPO->A.Connect(0, NoiseMul);
		FinalWPO->B.Connect(0, NormalWS);
		FinalWPO->MaterialExpressionEditorX = 100;
		FinalWPO->MaterialExpressionEditorY = 900;
		AddExpr(FinalWPO);

		Material->GetEditorOnlyData()->WorldPositionOffset.Connect(0, FinalWPO);
	}

	// CRITICAL: Must set bUsedWithSkeletalMesh or material fails at runtime
	bool bNeedsRecompile = false;
	Material->SetMaterialUsage(bNeedsRecompile, MATUSAGE_SkeletalMesh);

	Material->PreEditChange(nullptr);
	Material->PostEditChange();
	SaveAsset(Material, MatPkg);
	UE_LOG(LogTemp, Warning, TEXT("  Created PBR material: %s"), *MatName);

	// Assign material to mesh
	FString MeshPath = FString::Printf(TEXT("%s/SKM_%s.SKM_%s"), *DestDir, *PascalName, *PascalName);
	USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
	if (Mesh)
	{
		TArray<FSkeletalMaterial>& MeshMaterials = Mesh->GetMaterials();
		for (int32 i = 0; i < MeshMaterials.Num(); i++)
			MeshMaterials[i].MaterialInterface = Material;

		// Enable CPU access via GetLODInfo (correct API for UE5.5+)
		for (int32 LODIdx = 0; LODIdx < Mesh->GetLODNum(); LODIdx++)
		{
			if (FSkeletalMeshLODInfo* Info = Mesh->GetLODInfo(LODIdx))
			{
				Info->bAllowCPUAccess = true;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("  Set bAllowCPUAccess=true on %d LODs"), Mesh->GetLODNum());

		Mesh->MarkPackageDirty();
		UPackage* MeshPkg = Mesh->GetOutermost();
		FSavePackageArgs SA;
		SA.TopLevelFlags = RF_Standalone;
		UPackage::SavePackage(MeshPkg, Mesh,
			*FPackageName::LongPackageNameToFilename(MeshPkg->GetName(), FPackageName::GetAssetPackageExtension()), SA);
		UE_LOG(LogTemp, Warning, TEXT("  Assigned M_%s to SKM_%s"), *PascalName, *PascalName);
	}

	return true;
}

int32 USetupBatchEnemyCommandlet::ImportAnimations(
	const FString& FBXDir, const FString& DestAnimDir, const FString& PascalName, USkeleton* Skeleton)
{
	// Find all FBX files in the directory
	TArray<FString> FBXFiles;
	IFileManager::Get().FindFiles(FBXFiles, *(FBXDir / TEXT("*.fbx")), true, false);

	if (FBXFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No FBX files found in %s"), *FBXDir);
		return 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("  Found %d FBX files to import"), FBXFiles.Num());

	FAssetToolsModule& ATModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	int32 SuccessCount = 0;

	for (const FString& FBXFile : FBXFiles)
	{
		FString FullPath = FBXDir / FBXFile;
		FString BaseName = FPaths::GetBaseFilename(FBXFile);

		// Rename to PascalCase: "withered_wanderer_a000_003001" -> "WitheredWanderer_a000_003001"
		// Extract the anim ID part (everything after the enemy name)
		FString AnimID;
		int32 AIdx = BaseName.Find(TEXT("a000_"));
		if (AIdx != INDEX_NONE)
		{
			AnimID = BaseName.Mid(AIdx);
		}
		else
		{
			AnimID = BaseName;
		}
		FString AnimName = FString::Printf(TEXT("%s_%s"), *PascalName, *AnimID);

		UAssetImportTask* ImportTask = NewObject<UAssetImportTask>();
		ImportTask->AddToRoot();
		ImportTask->Filename = FullPath;
		ImportTask->DestinationPath = DestAnimDir;
		ImportTask->DestinationName = AnimName;
		ImportTask->bAutomated = true;
		ImportTask->bReplaceExisting = true;
		ImportTask->bSave = true;

		UFbxFactory* FbxFactory = NewObject<UFbxFactory>();
		FbxFactory->SetDetectImportTypeOnImport(false);

		UFbxImportUI* FbxUI = FbxFactory->ImportUI;
		FbxUI->bImportMesh = false;
		FbxUI->bImportMaterials = false;
		FbxUI->bImportTextures = false;
		FbxUI->bImportAnimations = true;
		FbxUI->MeshTypeToImport = FBXIT_Animation;
		FbxUI->Skeleton = Skeleton;
		FbxUI->bAutomatedImportShouldDetectType = false;
		FbxUI->bOverrideFullName = true;
		FbxUI->bCreatePhysicsAsset = false;

		FbxUI->AnimSequenceImportData->bImportMeshesInBoneHierarchy = false;
		FbxUI->AnimSequenceImportData->AnimationLength = FBXALIT_ExportedTime;
		FbxUI->AnimSequenceImportData->bUseDefaultSampleRate = true;
		FbxUI->AnimSequenceImportData->bRemoveRedundantKeys = false;
		FbxUI->AnimSequenceImportData->bSnapToClosestFrameBoundary = true;

		ImportTask->Factory = FbxFactory;
		ImportTask->Options = FbxUI;

		ATModule.Get().ImportAssetTasks({ImportTask});

		TArray<UObject*> ImportedObjects = ImportTask->GetObjects();
		bool bFound = false;
		for (UObject* Obj : ImportedObjects)
		{
			if (UAnimSequence* AnimSeq = Cast<UAnimSequence>(Obj))
			{
				AnimSeq->bForceRootLock = true;
				AnimSeq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;

				int32 NumBoneTracks = AnimSeq->GetDataModel() ? AnimSeq->GetDataModel()->GetNumBoneTracks() : 0;
				UE_LOG(LogTemp, Log, TEXT("  %s: %d tracks, %.2fs"), *AnimName, NumBoneTracks, AnimSeq->GetPlayLength());
				SuccessCount++;
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("  %s: FAILED to import as AnimSequence"), *AnimName);
		}

		ImportTask->RemoveFromRoot();
	}

	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	return SuccessCount;
}

void USetupBatchEnemyCommandlet::CreateMontages(
	const FString& DestDir, const FString& AnimDir, const FString& PascalName,
	const TMap<FString, FString>& AnimCategoryMap)
{
	// Group animations by category
	TMap<FString, TArray<FString>> CategoryToAnims;
	for (const auto& Pair : AnimCategoryMap)
	{
		CategoryToAnims.FindOrAdd(Pair.Value).Add(Pair.Key);
	}

	// Sort each category's anims for deterministic ordering
	for (auto& Pair : CategoryToAnims)
	{
		Pair.Value.Sort();
	}

	FString Result;
	int32 MontageCount = 0;

	// Attack montages (up to 8)
	auto CreateCategoryMontages = [&](const FString& Category, const FString& MontagePrefix, int32 MaxCount)
	{
		TArray<FString>* Anims = CategoryToAnims.Find(Category);
		if (!Anims) return;

		int32 Count = FMath::Min(Anims->Num(), MaxCount);
		for (int32 i = 0; i < Count; i++)
		{
			FString AnimPath = AnimDir / (*Anims)[i];
			FString MontageName = (Count == 1)
				? FString::Printf(TEXT("AM_%s_%s"), *PascalName, *MontagePrefix)
				: FString::Printf(TEXT("AM_%s_%s%02d"), *PascalName, *MontagePrefix, i + 1);
			FString MontagePath = DestDir / MontageName;

			Result = USLFAutomationLibrary::CreateMontageFromSequence(AnimPath, MontagePath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("  %s → %s: %s"), *(*Anims)[i], *MontageName,
				Result.Contains(TEXT("OK")) || Result.Contains(TEXT("Created")) ? TEXT("OK") : *Result);
			MontageCount++;
		}
	};

	CreateCategoryMontages(TEXT("Attack"), TEXT("Attack"), 8);
	CreateCategoryMontages(TEXT("HeavyAttack"), TEXT("HeavyAttack"), 2);
	CreateCategoryMontages(TEXT("HitReact"), TEXT("HitReact"), 3);
	CreateCategoryMontages(TEXT("Death"), TEXT("Death"), 3);
	CreateCategoryMontages(TEXT("Dodge"), TEXT("Dodge"), 4);
	CreateCategoryMontages(TEXT("Guard"), TEXT("Guard"), 1);
	CreateCategoryMontages(TEXT("GuardHit"), TEXT("GuardHit"), 1);
	CreateCategoryMontages(TEXT("PoiseBreak"), TEXT("PoiseBreak"), 2);
	CreateCategoryMontages(TEXT("Idle"), TEXT("Idle"), 1);

	// Locomotion: pick Walk and Run by Elden Ring anim ID convention
	// 002000=walk fwd, 002100=run fwd (c3100), 002030=run fwd (infantry)
	// 002001=walk back, 002002/002003=sidestep L/R, 001800=transition
	{
		TArray<FString>* LocoAnims = CategoryToAnims.Find(TEXT("Locomotion"));
		if (LocoAnims && LocoAnims->Num() > 0)
		{
			FString WalkAnim, RunAnim;
			for (const FString& AnimName : *LocoAnims)
			{
				if (AnimName.Contains(TEXT("002000"))) WalkAnim = AnimName;
				else if (AnimName.Contains(TEXT("002100"))) RunAnim = AnimName;   // c3100 run
				else if (AnimName.Contains(TEXT("002030")) && RunAnim.IsEmpty()) RunAnim = AnimName; // infantry run
			}
			// Fallback: first loco anim = walk, second = run
			if (WalkAnim.IsEmpty() && LocoAnims->Num() >= 1) WalkAnim = (*LocoAnims)[0];
			if (RunAnim.IsEmpty() && LocoAnims->Num() >= 2) RunAnim = (*LocoAnims)[1];

			if (!WalkAnim.IsEmpty())
			{
				FString WalkPath = AnimDir / WalkAnim;
				FString WalkMontagePath = DestDir / FString::Printf(TEXT("AM_%s_Walk"), *PascalName);
				Result = USLFAutomationLibrary::CreateMontageFromSequence(WalkPath, WalkMontagePath, TEXT("DefaultSlot"));
				UE_LOG(LogTemp, Warning, TEXT("  %s → AM_%s_Walk: %s"), *WalkAnim, *PascalName,
					Result.Contains(TEXT("OK")) || Result.Contains(TEXT("Created")) ? TEXT("OK") : *Result);
				MontageCount++;
			}
			if (!RunAnim.IsEmpty())
			{
				FString RunPath = AnimDir / RunAnim;
				FString RunMontagePath = DestDir / FString::Printf(TEXT("AM_%s_Run"), *PascalName);
				Result = USLFAutomationLibrary::CreateMontageFromSequence(RunPath, RunMontagePath, TEXT("DefaultSlot"));
				UE_LOG(LogTemp, Warning, TEXT("  %s → AM_%s_Run: %s"), *RunAnim, *PascalName,
					Result.Contains(TEXT("OK")) || Result.Contains(TEXT("Created")) ? TEXT("OK") : *Result);
				MontageCount++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  Created %d montages total"), MontageCount);
}

void USetupBatchEnemyCommandlet::CreateBlendSpace(
	const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& SkeletonPath)
{
	// Find idle, walk, run animations
	FString IdleName, WalkName, RunName;

	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AnimAssets;
	ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, false);

	for (const FAssetData& AD : AnimAssets)
	{
		FString Name = AD.AssetName.ToString();
		FString AnimID;
		int32 AIdx = Name.Find(TEXT("a000_"));
		if (AIdx != INDEX_NONE) AnimID = Name.Mid(AIdx);

		FString Cat = CategorizeAnimID(AnimID);
		if (Cat == TEXT("Idle") && IdleName.IsEmpty()) IdleName = Name;
		if (Cat == TEXT("Locomotion"))
		{
			if (WalkName.IsEmpty()) WalkName = Name;
			else if (RunName.IsEmpty()) RunName = Name;
		}
	}

	if (IdleName.IsEmpty() || WalkName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot create blend space: missing Idle or Walk anim"));
		return;
	}
	if (RunName.IsEmpty()) RunName = WalkName; // Fallback

	FString BSName = FString::Printf(TEXT("BS_%s_Locomotion"), *PascalName);
	FString Result = USLFAutomationLibrary::CreateBlendSpace2DFromSequences(
		DestDir / BSName,
		SkeletonPath,
		AnimDir / IdleName,
		AnimDir / WalkName,
		AnimDir / RunName
	);
	UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
}

void USetupBatchEnemyCommandlet::CreateAnimBP(
	const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& SkeletonPath)
{
	FString ABPName = FString::Printf(TEXT("ABP_%s"), *PascalName);
	FString ABPPath = DestDir / ABPName;
	FString Result;

	// Duplicate from base SoulslikeEnemy ABP
	Result = USLFAutomationLibrary::DuplicateAnimBPForSkeleton(
		TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"),
		SkeletonPath,
		ABPPath
	);
	UE_LOG(LogTemp, Warning, TEXT("  Duplicate: %s"), *Result);

	// Replace animation references
	{
		FString BSPath = DestDir / FString::Printf(TEXT("BS_%s_Locomotion"), *PascalName);

		// Find idle animation
		FString IdleAnimPath;
		FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AnimAssets;
		ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, false);
		for (const FAssetData& AD : AnimAssets)
		{
			FString Name = AD.AssetName.ToString();
			FString AnimID;
			int32 AIdx = Name.Find(TEXT("a000_"));
			if (AIdx != INDEX_NONE) AnimID = Name.Mid(AIdx);
			if (CategorizeAnimID(AnimID) == TEXT("Idle"))
			{
				IdleAnimPath = AnimDir / Name;
				break;
			}
		}

		TMap<FString, FString> AnimReplacementMap;
		AnimReplacementMap.Add(
			TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Blendspaces/ABS_SoulslikeEnemy"),
			BSPath);
		if (!IdleAnimPath.IsEmpty())
		{
			AnimReplacementMap.Add(
				TEXT("/Game/SoulslikeFramework/Demo/RetargetedParagonAnimations/Kwang/AS_RT_Idle_Kwang"),
				IdleAnimPath);
		}

		Result = USLFAutomationLibrary::ReplaceAnimReferencesInAnimBP(ABPPath, AnimReplacementMap);
		UE_LOG(LogTemp, Warning, TEXT("  Replace refs: %s"), *Result);
	}

	// Remove Control Rig + IK Rig nodes
	Result = USLFAutomationLibrary::DisableControlRigInAnimBP(ABPPath);
	UE_LOG(LogTemp, Warning, TEXT("  Disable ControlRig: %s"), *Result);
}

void USetupBatchEnemyCommandlet::CreateDataAssets(
	const FString& DestDir, const FString& AnimDir, const FString& PascalName, const FString& EnemySnakeName)
{
	FString Result;

	// Check for animation_config.json
	FString ConfigPath = FString(SourceBaseDir) / EnemySnakeName / TEXT("animation_config.json");
	bool bHasConfig = FPaths::FileExists(ConfigPath);

	// Discover animations by category
	TMap<FString, TArray<FString>> CategoryAnims;
	{
		FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AnimAssets;
		ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, false);
		for (const FAssetData& AD : AnimAssets)
		{
			FString Name = AD.AssetName.ToString();
			FString AnimID;
			int32 AIdx = Name.Find(TEXT("a000_"));
			if (AIdx != INDEX_NONE) AnimID = Name.Mid(AIdx);
			FString Cat = CategorizeAnimID(AnimID);
			CategoryAnims.FindOrAdd(Cat).Add(Name);
		}
		for (auto& Pair : CategoryAnims) Pair.Value.Sort();
	}

	// 6a: PDA_AnimData
	{
		FString PkgName = DestDir / FString::Printf(TEXT("PDA_%s_AnimData"), *PascalName);
		FString ObjName = FString::Printf(TEXT("PDA_%s_AnimData"), *PascalName);
		UPackage* Pkg = PreparePackage(PkgName, ObjName);
		UPDA_AnimData* AnimData = NewObject<UPDA_AnimData>(Pkg, FName(*ObjName), RF_Public | RF_Standalone);

		FString BSPath = DestDir / FString::Printf(TEXT("BS_%s_Locomotion"), *PascalName);
		AnimData->WalkRunLocomotion = LoadObject<UBlendSpace>(nullptr, *BSPath);

		TArray<FString>* IdleAnims = CategoryAnims.Find(TEXT("Idle"));
		if (IdleAnims && IdleAnims->Num() > 0)
		{
			AnimData->Idle = LoadObject<UAnimSequenceBase>(nullptr, *(AnimDir / (*IdleAnims)[0]));
		}

		SaveAsset(AnimData, Pkg);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_%s_AnimData: SAVED"), *PascalName);
	}

	// 6b: PDA_WeaponAnimset
	{
		FString PkgName = DestDir / FString::Printf(TEXT("PDA_%s_WeaponAnimset"), *PascalName);
		FString ObjName = FString::Printf(TEXT("PDA_%s_WeaponAnimset"), *PascalName);
		UPackage* Pkg = PreparePackage(PkgName, ObjName);
		UPDA_WeaponAnimset* WA = NewObject<UPDA_WeaponAnimset>(Pkg, FName(*ObjName), RF_Public | RF_Standalone);

		// Find first attack montage
		FString Attack01Path = DestDir / FString::Printf(TEXT("AM_%s_Attack01"), *PascalName);
		FString HeavyPath = DestDir / FString::Printf(TEXT("AM_%s_HeavyAttack01"), *PascalName);
		if (!LoadObject<UAnimMontage>(nullptr, *HeavyPath))
		{
			HeavyPath = DestDir / FString::Printf(TEXT("AM_%s_HeavyAttack"), *PascalName);
		}
		FString GuardHitPath = DestDir / FString::Printf(TEXT("AM_%s_GuardHit"), *PascalName);

		WA->OneH_LightComboMontage_R = LoadObject<UAnimMontage>(nullptr, *Attack01Path);
		WA->OneH_HeavyComboMontage_R = LoadObject<UAnimMontage>(nullptr, *HeavyPath);

		// Find guard sequence
		TArray<FString>* GuardAnims = CategoryAnims.Find(TEXT("Guard"));
		if (GuardAnims && GuardAnims->Num() > 0)
		{
			WA->Guard_R = LoadObject<UAnimSequenceBase>(nullptr, *(AnimDir / (*GuardAnims)[0]));
		}
		WA->Guard_R_Hit = LoadObject<UAnimMontage>(nullptr, *GuardHitPath);

		SaveAsset(WA, Pkg);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_%s_WeaponAnimset: SAVED"), *PascalName);
	}

	// 6c: PDA_CombatReaction
	{
		FString PkgName = DestDir / FString::Printf(TEXT("PDA_%s_CombatReaction"), *PascalName);
		FString ObjName = FString::Printf(TEXT("PDA_%s_CombatReaction"), *PascalName);
		UPackage* Pkg = PreparePackage(PkgName, ObjName);
		UPDA_CombatReactionAnimData* Rx = NewObject<UPDA_CombatReactionAnimData>(Pkg, FName(*ObjName), RF_Public | RF_Standalone);

		FString HitReactPath = DestDir / FString::Printf(TEXT("AM_%s_HitReact01"), *PascalName);
		if (!LoadObject<UAnimMontage>(nullptr, *HitReactPath))
		{
			HitReactPath = DestDir / FString::Printf(TEXT("AM_%s_HitReact"), *PascalName);
		}
		Rx->ReactionMontage = LoadObject<UAnimMontage>(nullptr, *HitReactPath);

		// Death montages
		for (int32 i = 1; i <= 3; i++)
		{
			FString DeathPath = DestDir / FString::Printf(TEXT("AM_%s_Death%02d"), *PascalName, i);
			UAnimMontage* DeathMontage = LoadObject<UAnimMontage>(nullptr, *DeathPath);
			if (!DeathMontage)
			{
				DeathPath = DestDir / FString::Printf(TEXT("AM_%s_Death"), *PascalName);
				DeathMontage = LoadObject<UAnimMontage>(nullptr, *DeathPath);
			}
			if (DeathMontage)
			{
				ESLFDirection Dir = (i == 1) ? ESLFDirection::Fwd : (i == 2) ? ESLFDirection::Bwd : ESLFDirection::Left;
				Rx->Death.Add(Dir, DeathMontage);
			}
		}

		SaveAsset(Rx, Pkg);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_%s_CombatReaction: SAVED"), *PascalName);
	}

	// 6d: PDA_PoiseBreak
	{
		FString PkgName = DestDir / FString::Printf(TEXT("PDA_%s_PoiseBreak"), *PascalName);
		FString ObjName = FString::Printf(TEXT("PDA_%s_PoiseBreak"), *PascalName);
		UPackage* Pkg = PreparePackage(PkgName, ObjName);
		UPDA_PoiseBreakAnimData* PB = NewObject<UPDA_PoiseBreakAnimData>(Pkg, FName(*ObjName), RF_Public | RF_Standalone);

		TArray<FString>* PBAnims = CategoryAnims.Find(TEXT("PoiseBreak"));
		if (PBAnims && PBAnims->Num() >= 1)
			PB->PoiseBreak_Start = LoadObject<UAnimSequence>(nullptr, *(AnimDir / (*PBAnims)[0]));
		if (PBAnims && PBAnims->Num() >= 2)
			PB->PoiseBreak_Loop = LoadObject<UAnimSequence>(nullptr, *(AnimDir / (*PBAnims)[1]));

		SaveAsset(PB, Pkg);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_%s_PoiseBreak: SAVED"), *PascalName);
	}

	// 6e: AI Ability data assets (one per attack montage)
	if (bHasConfig)
	{
		// Config-driven: create exactly the number of DAs matching the config
		FString ConfigJson;
		FFileHelper::LoadFileToString(ConfigJson, *ConfigPath);
		TSharedPtr<FJsonObject> Config;
		TSharedRef<TJsonReader<>> CfgReader = TJsonReaderFactory<>::Create(ConfigJson);
		FJsonSerializer::Deserialize(CfgReader, Config);

		if (Config.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* Attacks;
			if (Config->TryGetArrayField(TEXT("attacks"), Attacks))
			{
				for (int32 i = 0; i < Attacks->Num(); i++)
				{
					FString DAName = FString::Printf(TEXT("DA_%s_Attack%02d"), *PascalName, i + 1);
					FString MontageName = FString::Printf(TEXT("AM_%s_Attack%02d"), *PascalName, i + 1);

					FString PkgName = DestDir / DAName;
					UPackage* Pkg = PreparePackage(PkgName, DAName);
					UPDA_AI_Ability* Ability = NewObject<UPDA_AI_Ability>(Pkg, FName(*DAName), RF_Public | RF_Standalone);
					Ability->Montage = LoadObject<UAnimMontage>(nullptr, *(DestDir / MontageName));
					Ability->Score = 1.0;
					Ability->Cooldown = 2.0;
					SaveAsset(Ability, Pkg);
					UE_LOG(LogTemp, Warning, TEXT("  %s: SAVED"), *DAName);
				}
			}

			const TArray<TSharedPtr<FJsonValue>>* Heavies;
			if (Config->TryGetArrayField(TEXT("heavy_attacks"), Heavies))
			{
				for (int32 i = 0; i < Heavies->Num(); i++)
				{
					// SLFEnemyGeneric discovers DA_*_HeavyAttack01
					FString DAName = FString::Printf(TEXT("DA_%s_HeavyAttack%02d"), *PascalName, i + 1);
					FString MontageName = (Heavies->Num() == 1)
						? FString::Printf(TEXT("AM_%s_HeavyAttack"), *PascalName)
						: FString::Printf(TEXT("AM_%s_HeavyAttack%02d"), *PascalName, i + 1);

					FString PkgName = DestDir / DAName;
					UPackage* Pkg = PreparePackage(PkgName, DAName);
					UPDA_AI_Ability* Ability = NewObject<UPDA_AI_Ability>(Pkg, FName(*DAName), RF_Public | RF_Standalone);
					Ability->Montage = LoadObject<UAnimMontage>(nullptr, *(DestDir / MontageName));
					Ability->Score = 0.5;
					Ability->Cooldown = 5.0;
					SaveAsset(Ability, Pkg);
					UE_LOG(LogTemp, Warning, TEXT("  %s: SAVED"), *DAName);
				}
			}
		}
	}
	else
	{
		// Legacy: scan all imported animations
		TArray<FString>* AttackAnims = CategoryAnims.Find(TEXT("Attack"));
		int32 AttackCount = AttackAnims ? FMath::Min(AttackAnims->Num(), 8) : 0;
		TArray<FString>* HeavyAnims = CategoryAnims.Find(TEXT("HeavyAttack"));

		for (int32 i = 0; i < AttackCount; i++)
		{
			FString DAName = FString::Printf(TEXT("DA_%s_Attack%02d"), *PascalName, i + 1);
			FString MontageName = FString::Printf(TEXT("AM_%s_Attack%02d"), *PascalName, i + 1);

			FString PkgName = DestDir / DAName;
			UPackage* Pkg = PreparePackage(PkgName, DAName);
			UPDA_AI_Ability* Ability = NewObject<UPDA_AI_Ability>(Pkg, FName(*DAName), RF_Public | RF_Standalone);
			Ability->Montage = LoadObject<UAnimMontage>(nullptr, *(DestDir / MontageName));
			Ability->Score = 1.0;
			Ability->Cooldown = 2.0;
			SaveAsset(Ability, Pkg);
			UE_LOG(LogTemp, Warning, TEXT("  %s: SAVED"), *DAName);
		}

		if (HeavyAnims)
		{
			for (int32 i = 0; i < FMath::Min(HeavyAnims->Num(), 2); i++)
			{
				FString DAName = FString::Printf(TEXT("DA_%s_HeavyAttack%02d"), *PascalName, i + 1);
				FString MontageName = (HeavyAnims->Num() == 1)
					? FString::Printf(TEXT("AM_%s_HeavyAttack"), *PascalName)
					: FString::Printf(TEXT("AM_%s_HeavyAttack%02d"), *PascalName, i + 1);

				FString PkgName = DestDir / DAName;
				UPackage* Pkg = PreparePackage(PkgName, DAName);
				UPDA_AI_Ability* Ability = NewObject<UPDA_AI_Ability>(Pkg, FName(*DAName), RF_Public | RF_Standalone);
				Ability->Montage = LoadObject<UAnimMontage>(nullptr, *(DestDir / MontageName));
				Ability->Score = 0.5;
				Ability->Cooldown = 5.0;
				SaveAsset(Ability, Pkg);
				UE_LOG(LogTemp, Warning, TEXT("  %s: SAVED"), *DAName);
			}
		}
	}
}

void USetupBatchEnemyCommandlet::AddSockets(const FString& SkeletonPath)
{
	struct FSocketDef { const TCHAR* Name; const TCHAR* Bone; FVector Offset; };
	const FSocketDef Sockets[] = {
		{ TEXT("weapon_start"), TEXT("hand_r"),   FVector::ZeroVector },
		{ TEXT("weapon_end"),   TEXT("weapon_r"), FVector::ZeroVector },
		{ TEXT("hand_r"),       TEXT("hand_r"),   FVector::ZeroVector },
		{ TEXT("hand_l"),       TEXT("hand_l"),   FVector::ZeroVector },
		{ TEXT("foot_l"),       TEXT("foot_l"),   FVector::ZeroVector },
		{ TEXT("foot_r"),       TEXT("foot_r"),   FVector::ZeroVector },
	};

	for (const auto& S : Sockets)
	{
		FString Result = USLFAutomationLibrary::AddSocketToSkeleton(SkeletonPath, S.Name, S.Bone, S.Offset);
		UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
	}

	// Save skeleton
	USkeleton* Skel = LoadObject<USkeleton>(nullptr, *SkeletonPath);
	if (Skel)
	{
		UPackage* Pkg = Skel->GetOutermost();
		Pkg->MarkPackageDirty();
		FString Fn = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SA;
		SA.TopLevelFlags = RF_Standalone;
		UPackage::SavePackage(Pkg, Skel, *Fn, SA);
	}
}

void USetupBatchEnemyCommandlet::AddWeaponTraces(const FString& DestDir, const FString& PascalName, const FString& EnemySnakeName)
{
	// Try TAE-driven multi-window placement first
	TSet<FString> TAEProcessedMontages;
	AddWeaponTracesFromTAE(DestDir, PascalName, EnemySnakeName, &TAEProcessedMontages);

	if (TAEProcessedMontages.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  TAE-driven weapon traces: %d montages processed"), TAEProcessedMontages.Num());
	}

	// Heuristic fallback for montages that TAE didn't cover
	UE_LOG(LogTemp, Warning, TEXT("  Applying heuristic weapon traces to remaining montages..."));
	FString Result;

	// Auto-detect attack montages — skip any already processed by TAE
	for (int32 i = 1; i <= 8; i++)
	{
		FString MontageName = FString::Printf(TEXT("AM_%s_Attack%02d"), *PascalName, i);
		if (TAEProcessedMontages.Contains(MontageName)) continue;
		FString MontagePath = DestDir / MontageName;
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
		if (!Montage) continue;

		float Duration = Montage->GetPlayLength();
		// Tighter hitbox windows: active during the swing only (40-65% of animation)
		float StartTime = Duration * 0.35f;
		float EndTime = Duration * 0.65f;

		Result = USLFAutomationLibrary::AddWeaponTraceToMontage(
			MontagePath,
			StartTime,
			EndTime,
			80.0f,              // Smaller radius (was 120)
			FName("weapon_start"),
			FName("weapon_end"),
			180.0f,             // Shorter reach (was 300 — too far)
			EAxis::X,
			false,
			40.0f,              // Base damage
			20.0f,              // Poise damage
			true,
			FName("lowerarm_r")
		);

		UE_LOG(LogTemp, Warning, TEXT("  %s (%.2f-%.2fs): %s"),
			*MontageName, StartTime, EndTime, *Result);
	}

	// Heavy attack montages — skip any already processed by TAE
	for (int32 i = 1; i <= 2; i++)
	{
		FString MontageName = (i == 1)
			? FString::Printf(TEXT("AM_%s_HeavyAttack"), *PascalName)
			: FString::Printf(TEXT("AM_%s_HeavyAttack%02d"), *PascalName, i);
		if (TAEProcessedMontages.Contains(MontageName)) continue;
		FString MontagePath = DestDir / MontageName;
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
		if (!Montage) continue;

		float Duration = Montage->GetPlayLength();
		// Heavy attacks: slam comes late (55-75% of animation)
		float StartTime = Duration * 0.50f;
		float EndTime = Duration * 0.70f;

		Result = USLFAutomationLibrary::AddWeaponTraceToMontage(
			MontagePath,
			StartTime,
			EndTime,
			100.0f,             // Moderate radius (was 150)
			FName("weapon_start"),
			FName("weapon_end"),
			220.0f,             // Shorter reach (was 350)
			EAxis::X,
			false,
			80.0f,              // High damage
			50.0f,              // High poise
			true,
			FName("lowerarm_r")
		);
		UE_LOG(LogTemp, Warning, TEXT("  %s (%.2f-%.2fs): %s"), *MontageName, StartTime, EndTime, *Result);
	}
}

bool USetupBatchEnemyCommandlet::AddWeaponTracesFromTAE(const FString& DestDir, const FString& PascalName, const FString& EnemySnakeName, TSet<FString>* OutProcessedMontages)
{
	// Look for tae_hitboxes.json in source directory
	FString JsonPath = FString(SourceBaseDir) / EnemySnakeName / TEXT("tae_hitboxes.json");

	if (!FPaths::FileExists(JsonPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("  TAE JSON not found: %s"), *JsonPath);
		return false;
	}

	// Read and parse JSON
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonPath))
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to read TAE JSON: %s"), *JsonPath);
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to parse TAE JSON: %s"), *JsonPath);
		return false;
	}

	const TSharedPtr<FJsonObject>* AnimationsObj;
	if (!RootObject->TryGetObjectField(TEXT("animations"), AnimationsObj))
	{
		UE_LOG(LogTemp, Error, TEXT("  TAE JSON missing 'animations' field"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("  Loaded TAE JSON: %d animations"), (*AnimationsObj)->Values.Num());

	// Load animation_config.json for montage-slot-to-anim_id mapping
	TMap<int32, FString> AttackSlotToAnimId;   // Attack index (1-8) -> anim_id
	TMap<int32, FString> HeavySlotToAnimId;    // Heavy index (1-2) -> anim_id
	{
		FString ConfigPath = FString(SourceBaseDir) / EnemySnakeName / TEXT("animation_config.json");
		FString ConfigStr;
		if (FFileHelper::LoadFileToString(ConfigStr, *ConfigPath))
		{
			TSharedPtr<FJsonObject> Config;
			TSharedRef<TJsonReader<>> CfgReader = TJsonReaderFactory<>::Create(ConfigStr);
			if (FJsonSerializer::Deserialize(CfgReader, Config) && Config.IsValid())
			{
				const TArray<TSharedPtr<FJsonValue>>* Attacks;
				if (Config->TryGetArrayField(TEXT("attacks"), Attacks))
				{
					for (int32 idx = 0; idx < Attacks->Num(); idx++)
					{
						FString Aid = (*Attacks)[idx]->AsObject()->GetStringField(TEXT("anim_id"));
						AttackSlotToAnimId.Add(idx + 1, Aid);
					}
				}
				const TArray<TSharedPtr<FJsonValue>>* Heavies;
				if (Config->TryGetArrayField(TEXT("heavy_attacks"), Heavies))
				{
					for (int32 idx = 0; idx < Heavies->Num(); idx++)
					{
						FString Aid = (*Heavies)[idx]->AsObject()->GetStringField(TEXT("anim_id"));
						HeavySlotToAnimId.Add(idx + 1, Aid);
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Config mapping: %d attacks, %d heavies"),
					AttackSlotToAnimId.Num(), HeavySlotToAnimId.Num());
			}
		}
	}

	int32 MontagesProcessed = 0;

	// Process attack montages (AM_{PascalName}_Attack01 through Attack08)
	for (int32 i = 1; i <= 8; i++)
	{
		FString MontageName = FString::Printf(TEXT("AM_%s_Attack%02d"), *PascalName, i);
		FString MontagePath = DestDir / MontageName;
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
		if (!Montage) continue;

		// Get anim_id: first try config mapping, then regex from AnimSequence name
		FString AnimId;

		// Config-driven: direct slot -> anim_id mapping
		if (FString* ConfigAnimId = AttackSlotToAnimId.Find(i))
		{
			AnimId = *ConfigAnimId;
		}
		else
		{
			// Legacy: extract from AnimSequence asset name
			if (Montage->SlotAnimTracks.Num() > 0 && Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() > 0)
			{
				UAnimSequenceBase* AnimRef = Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].GetAnimReference();
				if (AnimRef)
				{
					FString SeqName = AnimRef->GetName();
					FRegexPattern Pattern(TEXT("(a\\d+_\\d+)"));
					FRegexMatcher Matcher(Pattern, SeqName);
					if (Matcher.FindNext())
					{
						AnimId = Matcher.GetCaptureGroup(1);
					}
				}
			}
		}

		if (AnimId.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s: Could not determine anim_id, skipping TAE"), *MontageName);
			continue;
		}

		// Look up hitbox windows in JSON
		const TSharedPtr<FJsonObject>* AnimObj;
		if (!(*AnimationsObj)->TryGetObjectField(AnimId, AnimObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s: No TAE data for anim_id %s"), *MontageName, *AnimId);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* WindowsArray;
		if (!(*AnimObj)->TryGetArrayField(TEXT("hitbox_windows"), WindowsArray))
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s: No hitbox_windows in TAE data"), *MontageName);
			continue;
		}

		// Build weapon trace windows
		TArray<USLFAutomationLibrary::FWeaponTraceWindow> Windows;
		for (const TSharedPtr<FJsonValue>& WindowVal : *WindowsArray)
		{
			const TSharedPtr<FJsonObject>& WindowObj = WindowVal->AsObject();
			if (!WindowObj.IsValid()) continue;

			USLFAutomationLibrary::FWeaponTraceWindow W;
			W.StartTime = WindowObj->GetNumberField(TEXT("start"));
			W.EndTime = WindowObj->GetNumberField(TEXT("end"));
			W.DamageMultiplier = 1.0f;
			Windows.Add(W);
		}

		if (Windows.Num() == 0) continue;

		// Scale damage inversely with window count (maintain total DPS)
		float DamagePerHit = (Windows.Num() > 1) ? 40.0f : 60.0f;
		float PoiseDamagePerHit = (Windows.Num() > 1) ? 20.0f : 30.0f;

		// Right arm trace
		FString Result = USLFAutomationLibrary::AddMultipleWeaponTracesToMontage(
			MontagePath,
			Windows,
			120.0f,                 // TraceRadius
			FName("weapon_start"),
			FName("weapon_end"),
			300.0f,                 // WeaponReach
			DamagePerHit,
			PoiseDamagePerHit,
			true,                   // Debug draw
			FName("lowerarm_r")
		);

		// Left arm trace — offset by 0.12s to match natural swing delay
		TArray<USLFAutomationLibrary::FWeaponTraceWindow> LWindows;
		for (const auto& W : Windows)
		{
			USLFAutomationLibrary::FWeaponTraceWindow LW;
			LW.StartTime = W.StartTime + 0.12f;
			LW.EndTime = W.EndTime + 0.15f;
			LW.DamageMultiplier = 0.5f; // Half damage to prevent double-dipping
			LWindows.Add(LW);
		}
		USLFAutomationLibrary::AddMultipleWeaponTracesToMontage(
			MontagePath,
			LWindows,
			100.0f,                 // Slightly smaller radius
			FName("weapon_start"),
			FName("weapon_end"),
			250.0f,                 // Slightly shorter reach
			DamagePerHit * 0.5f,
			PoiseDamagePerHit * 0.5f,
			true,
			FName("lowerarm_l")
		);

		UE_LOG(LogTemp, Warning, TEXT("    %s [%s]: %d windows (dual arm, L offset 0.12s) -> %s"),
			*MontageName, *AnimId, Windows.Num(), *Result);
		MontagesProcessed++;
		if (OutProcessedMontages) OutProcessedMontages->Add(MontageName);
	}

	// Process heavy attack montages
	for (int32 i = 1; i <= 2; i++)
	{
		FString MontageName = (i == 1)
			? FString::Printf(TEXT("AM_%s_HeavyAttack"), *PascalName)
			: FString::Printf(TEXT("AM_%s_HeavyAttack%02d"), *PascalName, i);
		FString MontagePath = DestDir / MontageName;
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
		if (!Montage) continue;

		FString AnimId;

		// Config-driven mapping
		if (FString* ConfigAnimId = HeavySlotToAnimId.Find(i))
		{
			AnimId = *ConfigAnimId;
		}
		else
		{
			// Legacy: extract from AnimSequence name
			if (Montage->SlotAnimTracks.Num() > 0 && Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() > 0)
			{
				UAnimSequenceBase* AnimRef = Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].GetAnimReference();
				if (AnimRef)
				{
					FString SeqName = AnimRef->GetName();
					FRegexPattern Pattern(TEXT("(a\\d+_\\d+)"));
					FRegexMatcher Matcher(Pattern, SeqName);
					if (Matcher.FindNext())
					{
						AnimId = Matcher.GetCaptureGroup(1);
					}
				}
			}
		}

		if (AnimId.IsEmpty()) continue;

		const TSharedPtr<FJsonObject>* AnimObj;
		if (!(*AnimationsObj)->TryGetObjectField(AnimId, AnimObj)) continue;

		const TArray<TSharedPtr<FJsonValue>>* WindowsArray;
		if (!(*AnimObj)->TryGetArrayField(TEXT("hitbox_windows"), WindowsArray)) continue;

		TArray<USLFAutomationLibrary::FWeaponTraceWindow> Windows;
		for (const TSharedPtr<FJsonValue>& WindowVal : *WindowsArray)
		{
			const TSharedPtr<FJsonObject>& WindowObj = WindowVal->AsObject();
			if (!WindowObj.IsValid()) continue;

			USLFAutomationLibrary::FWeaponTraceWindow W;
			W.StartTime = WindowObj->GetNumberField(TEXT("start"));
			W.EndTime = WindowObj->GetNumberField(TEXT("end"));
			W.DamageMultiplier = 1.0f;
			Windows.Add(W);
		}

		if (Windows.Num() == 0) continue;

		FString Result = USLFAutomationLibrary::AddMultipleWeaponTracesToMontage(
			MontagePath,
			Windows,
			300.0f,                 // Heavy radius (wide for slam AoE)
			FName("weapon_start"),
			FName("weapon_end"),
			150.0f,                 // Short reach (slam hits near body, not extended forward)
			100.0f,                 // Heavy damage
			60.0f,                  // Heavy poise
			true,
			FName("R_Hand")         // Direction from hand (not lowerarm — keeps trace near body)
		);

		UE_LOG(LogTemp, Warning, TEXT("    %s [%s]: %d windows -> %s"),
			*MontageName, *AnimId, Windows.Num(), *Result);
		MontagesProcessed++;
		if (OutProcessedMontages) OutProcessedMontages->Add(MontageName);
	}

	UE_LOG(LogTemp, Warning, TEXT("  TAE weapon traces: %d montages processed"), MontagesProcessed);
	return MontagesProcessed > 0;
}

bool USetupBatchEnemyCommandlet::SaveAsset(UObject* Asset, UPackage* Pkg)
{
	FAssetRegistryModule::AssetCreated(Asset);
	Pkg->MarkPackageDirty();
	FString FileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	return UPackage::SavePackage(Pkg, Asset, *FileName, SaveArgs);
}

UPackage* USetupBatchEnemyCommandlet::PreparePackage(const FString& PkgName, const FString& ObjName)
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
}

#else // !WITH_EDITOR

USetupBatchEnemyCommandlet::USetupBatchEnemyCommandlet() {}
int32 USetupBatchEnemyCommandlet::Main(const FString& Params) { return 0; }

#endif // WITH_EDITOR
