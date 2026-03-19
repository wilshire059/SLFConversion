#include "IKRetargetMultiSourceCommandlet.h"

#if WITH_EDITOR

#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Misc/FileHelper.h"
#include "AssetImportTask.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Framework/Application/SlateApplication.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

// IKRig headers
#include "Rig/IKRigDefinition.h"
#include "RigEditor/IKRigController.h"
#include "Retargeter/IKRetargeter.h"
#include "RetargetEditor/IKRetargeterController.h"
#include "Retargeter/IKRetargetProcessor.h"
#include "Retargeter/IKRetargetProfile.h"

UIKRetargetMultiSourceCommandlet::UIKRetargetMultiSourceCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UIKRetargetMultiSourceCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Warning, TEXT("=== IKRetargetMultiSource Commandlet ==="));

	// Initialize Slate (needed for asset import tasks)
	if (!FSlateApplication::IsInitialized())
	{
		FSlateApplication::Create();
		UE_LOG(LogTemp, Warning, TEXT("  Slate initialized"));
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

	// ─── Animation mapping: SentinelName -> (ChrId, source FBX name) ───
	struct FAnimDef { const TCHAR* SentinelName; const TCHAR* ChrId; };
	const FAnimDef AnimDefs[] = {
		{ TEXT("Sentinel_Attack01"),       TEXT("c3560") },
		{ TEXT("Sentinel_Attack02"),       TEXT("c4090") },
		{ TEXT("Sentinel_Attack03_Fast"),  TEXT("c3180") },
		{ TEXT("Sentinel_HeavyAttack"),    TEXT("c4060") },
		{ TEXT("Sentinel_HitReact"),       TEXT("c4080") },
		{ TEXT("Sentinel_HitReact_Light"), TEXT("c4080") },
		{ TEXT("Sentinel_GuardHit"),       TEXT("c3180") },
		{ TEXT("Sentinel_Death_Front"),    TEXT("c4060") },
		{ TEXT("Sentinel_Death_Back"),     TEXT("c4060") },
		{ TEXT("Sentinel_Death_Left"),     TEXT("c4060") },
		{ TEXT("Sentinel_Dodge_Fwd"),      TEXT("c3180") },
		{ TEXT("Sentinel_Dodge_Bwd"),      TEXT("c3180") },
		{ TEXT("Sentinel_Dodge_Left"),     TEXT("c3180") },
		{ TEXT("Sentinel_Dodge_Right"),    TEXT("c3180") },
	};

	// Unique source enemies (excluding c3100 which is the target)
	const TCHAR* SourceEnemies[] = { TEXT("c3180"), TEXT("c3560"), TEXT("c4060"), TEXT("c4080"), TEXT("c4090") };

	// ─── Step 0: Load c3100 (target) skeleton and mesh ───
	UE_LOG(LogTemp, Warning, TEXT("--- Step 0: Load c3100 target ---"));
	USkeletalMesh* C3100Mesh = LoadObject<USkeletalMesh>(nullptr, C3100MeshPath);
	if (!C3100Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("  c3100 mesh not found at %s"), C3100MeshPath);
		return 1;
	}
	USkeleton* C3100Skel = C3100Mesh->GetSkeleton();
	UE_LOG(LogTemp, Warning, TEXT("  c3100: %s (%d bones)"),
		*C3100Mesh->GetName(), C3100Mesh->GetRefSkeleton().GetRawBoneNum());

	// ─── Step 1: Import source enemy meshes (creates skeleton per enemy) ───
	UE_LOG(LogTemp, Warning, TEXT("--- Step 1: Import source enemy meshes ---"));
	TMap<FString, USkeletalMesh*> SourceMeshes;
	TMap<FString, USkeleton*> SourceSkeletons;

	for (const TCHAR* ChrId : SourceEnemies)
	{
		FString MeshFbx = FString::Printf(TEXT("%s/%s/%s_mesh.fbx"), RawFbxDir, ChrId, ChrId);
		if (!FPaths::FileExists(MeshFbx))
		{
			UE_LOG(LogTemp, Error, TEXT("  MISSING: %s"), *MeshFbx);
			continue;
		}

		FString PkgDir = FString::Printf(TEXT("%s/%s"), TempContentDir, ChrId);
		FString AssetName = FString::Printf(TEXT("%s_mesh"), ChrId);

		USkeletalMesh* Mesh = ImportMeshFBX(MeshFbx, PkgDir, AssetName);
		if (Mesh)
		{
			SourceMeshes.Add(ChrId, Mesh);
			SourceSkeletons.Add(ChrId, Mesh->GetSkeleton());
			UE_LOG(LogTemp, Warning, TEXT("  %s: %d bones, skeleton=%s"),
				ChrId, Mesh->GetRefSkeleton().GetRawBoneNum(),
				*Mesh->GetSkeleton()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  %s: mesh import FAILED"), ChrId);
		}
	}

	// ─── Step 2: Import source animations (onto source skeletons) ───
	UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Import source animations ---"));
	TMap<FString, UAnimSequence*> SourceAnims;  // Key: SentinelName

	for (const auto& AD : AnimDefs)
	{
		FString ChrId = AD.ChrId;
		FString SentinelName = AD.SentinelName;

		USkeleton** SkelPtr = SourceSkeletons.Find(ChrId);
		if (!SkelPtr || !*SkelPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("  SKIP %s: no skeleton for %s"), *SentinelName, *ChrId);
			continue;
		}

		FString AnimFbx = FString::Printf(TEXT("%s/%s/%s.fbx"), RawFbxDir, *ChrId, *SentinelName);
		if (!FPaths::FileExists(AnimFbx))
		{
			UE_LOG(LogTemp, Warning, TEXT("  SKIP %s: FBX not found at %s"), *SentinelName, *AnimFbx);
			continue;
		}

		FString PkgDir = FString::Printf(TEXT("%s/%s/Anims"), TempContentDir, *ChrId);
		UAnimSequence* Anim = ImportAnimFBX(AnimFbx, PkgDir, SentinelName, *SkelPtr);
		if (Anim)
		{
			SourceAnims.Add(SentinelName, Anim);
			UE_LOG(LogTemp, Warning, TEXT("  %s: %.2fs, %d keys"),
				*SentinelName, Anim->GetPlayLength(), Anim->GetNumberOfSampledKeys());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  %s: anim import FAILED"), *SentinelName);
		}
	}

	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

	// ─── Step 3: Create IKRig for target (c3100) ───
	UE_LOG(LogTemp, Warning, TEXT("--- Step 3: Create IKRig for c3100 (target) ---"));
	FString TargetRigDir = FString::Printf(TEXT("%s/IKRigs"), TempContentDir);
	UIKRigDefinition* TargetRig = CreateIKRigForERSkeleton(C3100Mesh, TargetRigDir, TEXT("IKRig_c3100"));
	if (!TargetRig)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to create target IKRig"));
		return 1;
	}

	// ─── Step 4: For each source enemy, create IKRig + IKRetargeter + retarget via FIKRetargetProcessor ───
	TArray<UAnimSequence*> AllRetargetedAnims;
	TArray<FString> AllRetargetedNames;

	const FReferenceSkeleton& C3100RefSkel = C3100Mesh->GetRefSkeleton();
	const int32 NumTgtBones = C3100RefSkel.GetRawBoneNum();

	for (const TCHAR* ChrId : SourceEnemies)
	{
		USkeletalMesh** MeshPtr = SourceMeshes.Find(ChrId);
		if (!MeshPtr || !*MeshPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("  SKIP %s: no mesh imported"), ChrId);
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("--- Step 4: IK Retarget %s -> c3100 ---"), ChrId);

		// Create source IKRig
		FString SourceRigName = FString::Printf(TEXT("IKRig_%s"), ChrId);
		UIKRigDefinition* SourceRig = CreateIKRigForERSkeleton(*MeshPtr, TargetRigDir, SourceRigName);
		if (!SourceRig)
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to create source IKRig for %s"), ChrId);
			continue;
		}

		// Create IKRetargeter
		FString RetargeterName = FString::Printf(TEXT("RTG_%s_to_c3100"), ChrId);
		UIKRetargeter* Retargeter = CreateRetargeter(
			SourceRig, TargetRig, *MeshPtr, C3100Mesh, TargetRigDir, RetargeterName);
		if (!Retargeter)
		{
			UE_LOG(LogTemp, Error, TEXT("  Failed to create retargeter for %s"), ChrId);
			continue;
		}

		// Initialize the processor (no UI dependency!)
		FIKRetargetProcessor Processor;
		FRetargetProfile Profile;
		Processor.Initialize(*MeshPtr, C3100Mesh, Retargeter, Profile, true);

		if (!Processor.IsInitialized())
		{
			UE_LOG(LogTemp, Error, TEXT("  FIKRetargetProcessor failed to initialize for %s"), ChrId);
			continue;
		}
		UE_LOG(LogTemp, Warning, TEXT("  Processor initialized for %s"), ChrId);

		// Source skeleton info
		const FReferenceSkeleton& SrcRefSkel = (*MeshPtr)->GetRefSkeleton();
		const int32 NumSrcBones = SrcRefSkel.GetRawBoneNum();

		// Build source ref CS poses for extracting global poses from local anim data
		TArray<FTransform> SrcRefLocal, SrcRefCS;
		SrcRefLocal.SetNum(NumSrcBones);
		SrcRefCS.SetNum(NumSrcBones);
		for (int32 i = 0; i < NumSrcBones; i++)
		{
			SrcRefLocal[i] = SrcRefSkel.GetRefBonePose()[i];
			int32 ParIdx = SrcRefSkel.GetParentIndex(i);
			if (ParIdx == INDEX_NONE)
			{
				SrcRefCS[i] = SrcRefLocal[i];
			}
			else
			{
				FTransform Chained;
				FTransform::Multiply(&Chained, &SrcRefLocal[i], &SrcRefCS[ParIdx]);
				SrcRefCS[i] = Chained;
			}
		}

		// Retarget each animation for this enemy
		for (const auto& AD : AnimDefs)
		{
			if (FString(AD.ChrId) != FString(ChrId)) continue;
			FString SentinelName = AD.SentinelName;
			UAnimSequence** AnimPtr = SourceAnims.Find(SentinelName);
			if (!AnimPtr || !*AnimPtr) continue;

			UAnimSequence* SrcAnim = *AnimPtr;
			const double PlayLen = SrcAnim->GetPlayLength();
			const int32 NumKeys = SrcAnim->GetNumberOfSampledKeys();
			const FFrameRate SrcFrameRate = SrcAnim->GetSamplingFrameRate();
			const double FrameDuration = NumKeys > 1 ? PlayLen / (NumKeys - 1) : PlayLen;

			UE_LOG(LogTemp, Warning, TEXT("    Retargeting %s (%.3fs, %d keys)"), *SentinelName, PlayLen, NumKeys);

			// Create output AnimSequence on c3100 skeleton
			FString OutPkgName = FString::Printf(TEXT("%s/Retargeted/%s"), TempContentDir, *SentinelName);
			DeleteExistingAsset(OutPkgName);
			UPackage* OutPkg = CreatePackage(*OutPkgName);
			OutPkg->FullyLoad();

			UAnimSequence* OutAnim = NewObject<UAnimSequence>(OutPkg, *SentinelName, RF_Public | RF_Standalone);
			OutAnim->SetSkeleton(C3100Skel);

			IAnimationDataController& Controller = OutAnim->GetController();
			Controller.OpenBracket(NSLOCTEXT("IKRt", "IKRt", "IK Retarget"));
			Controller.InitializeModel();
			Controller.SetFrameRate(SrcFrameRate);
			Controller.SetNumberOfFrames(FFrameNumber(NumKeys > 0 ? NumKeys - 1 : 0));

			for (int32 b = 0; b < NumTgtBones; b++)
				Controller.AddBoneCurve(C3100RefSkel.GetBoneName(b));

			// Per-bone key storage
			TArray<TArray<FVector>> AllPosKeys;
			TArray<TArray<FQuat>> AllRotKeys;
			TArray<TArray<FVector>> AllScaleKeys;
			AllPosKeys.SetNum(NumTgtBones);
			AllRotKeys.SetNum(NumTgtBones);
			AllScaleKeys.SetNum(NumTgtBones);

			// Build target ref CS for local-from-global conversion
			TArray<FTransform> TgtRefLocal, TgtRefCS;
			TgtRefLocal.SetNum(NumTgtBones);
			TgtRefCS.SetNum(NumTgtBones);
			for (int32 i = 0; i < NumTgtBones; i++)
			{
				TgtRefLocal[i] = C3100RefSkel.GetRefBonePose()[i];
				int32 ParIdx = C3100RefSkel.GetParentIndex(i);
				if (ParIdx == INDEX_NONE)
				{
					TgtRefCS[i] = TgtRefLocal[i];
				}
				else
				{
					FTransform Chained;
					FTransform::Multiply(&Chained, &TgtRefLocal[i], &TgtRefCS[ParIdx]);
					TgtRefCS[i] = Chained;
				}
			}

			for (int32 KeyIdx = 0; KeyIdx < NumKeys; KeyIdx++)
			{
				double Time = FMath::Min(KeyIdx * FrameDuration, PlayLen);

				// Extract source local pose and build global
				TArray<FTransform> SrcGlobal;
				SrcGlobal.SetNum(NumSrcBones);
				for (int32 i = 0; i < NumSrcBones; i++)
				{
					FTransform Local;
					SrcAnim->GetBoneTransform(Local, FSkeletonPoseBoneIndex(i), Time, false);
					int32 ParIdx = SrcRefSkel.GetParentIndex(i);
					if (ParIdx == INDEX_NONE)
					{
						SrcGlobal[i] = Local;
					}
					else
					{
						FTransform Chained;
						FTransform::Multiply(&Chained, &Local, &SrcGlobal[ParIdx]);
						SrcGlobal[i] = Chained;
					}
				}

				// Run the IK Retargeter processor
				TArray<FTransform>& TgtGlobal = Processor.RunRetargeter(SrcGlobal, Profile, 0.033f);

				// Convert global to local for each target bone
				for (int32 b = 0; b < NumTgtBones; b++)
				{
					int32 ParentIdx = C3100RefSkel.GetParentIndex(b);
					FTransform Local;
					if (ParentIdx == INDEX_NONE)
					{
						Local = TgtGlobal[b];
					}
					else
					{
						FTransform ParentInv = TgtGlobal[ParentIdx].Inverse();
						Local = TgtGlobal[b];
						Local *= ParentInv;
					}
					AllPosKeys[b].Add(Local.GetTranslation());
					AllRotKeys[b].Add(Local.GetRotation());
					AllScaleKeys[b].Add(Local.GetScale3D());
				}
			}

			// Write keys to DataModel
			for (int32 b = 0; b < NumTgtBones; b++)
				Controller.SetBoneTrackKeys(C3100RefSkel.GetBoneName(b), AllPosKeys[b], AllRotKeys[b], AllScaleKeys[b]);

			Controller.NotifyPopulated();
			Controller.CloseBracket();

			// Compress and save
			OutAnim->CacheDerivedDataForCurrentPlatform();
			OutAnim->WaitOnExistingCompression(true);
			FAssetCompilingManager::Get().FinishAllCompilation();

			SaveAsset(OutAnim, OutPkgName);
			AllRetargetedAnims.Add(OutAnim);
			AllRetargetedNames.Add(SentinelName);

			UE_LOG(LogTemp, Warning, TEXT("      -> %s: OK (%d bones, %d keys)"),
				*SentinelName, NumTgtBones, NumKeys);
		}
	}

	// ─── Step 5: Dump retargeted animations to JSON for Blender visualization ───
	UE_LOG(LogTemp, Warning, TEXT("--- Step 5: Dump retargeted anims to JSON ---"));
	FString JsonPath = FString::Printf(TEXT("%s/ik_retarget_dump.json"), RawFbxDir);
	DumpRetargetedAnimsToJSON(AllRetargetedAnims, AllRetargetedNames, JsonPath);

	UE_LOG(LogTemp, Warning, TEXT("=== IKRetargetMultiSource COMPLETE ==="));
	UE_LOG(LogTemp, Warning, TEXT("  Retargeted: %d animations"), AllRetargetedAnims.Num());
	UE_LOG(LogTemp, Warning, TEXT("  JSON: %s"), *JsonPath);

	return 0;
}

USkeletalMesh* UIKRetargetMultiSourceCommandlet::ImportMeshFBX(
	const FString& FbxPath, const FString& PackageDir, const FString& AssetName)
{
	UAssetImportTask* Task = NewObject<UAssetImportTask>();
	Task->AddToRoot();
	Task->Filename = FbxPath;
	Task->DestinationPath = PackageDir;
	Task->DestinationName = AssetName;
	Task->bAutomated = true;
	Task->bReplaceExisting = true;
	Task->bSave = true;

	UFbxFactory* Factory = NewObject<UFbxFactory>();
	Factory->SetDetectImportTypeOnImport(false);

	UFbxImportUI* UI = Factory->ImportUI;
	UI->bImportMesh = true;
	UI->bImportAnimations = false;
	UI->bImportMaterials = false;
	UI->bImportTextures = false;
	UI->MeshTypeToImport = FBXIT_SkeletalMesh;
	UI->bAutomatedImportShouldDetectType = false;
	UI->bOverrideFullName = true;
	UI->bCreatePhysicsAsset = false;
	UI->SkeletalMeshImportData->bUseT0AsRefPose = true;
	UI->SkeletalMeshImportData->bConvertScene = false;
	UI->SkeletalMeshImportData->ImportUniformScale = 100.0f;

	Task->Factory = Factory;
	Task->Options = UI;

	FAssetToolsModule& ATM = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	ATM.Get().ImportAssetTasks({Task});

	USkeletalMesh* Result = nullptr;
	for (UObject* Obj : Task->GetObjects())
	{
		if (USkeletalMesh* SM = Cast<USkeletalMesh>(Obj))
		{
			Result = SM;
			break;
		}
	}
	Task->RemoveFromRoot();
	return Result;
}

UAnimSequence* UIKRetargetMultiSourceCommandlet::ImportAnimFBX(
	const FString& FbxPath, const FString& PackageDir, const FString& AssetName, USkeleton* TargetSkeleton)
{
	UAssetImportTask* Task = NewObject<UAssetImportTask>();
	Task->AddToRoot();
	Task->Filename = FbxPath;
	Task->DestinationPath = PackageDir;
	Task->DestinationName = AssetName;
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
	UI->Skeleton = TargetSkeleton;
	UI->bAutomatedImportShouldDetectType = false;
	UI->bOverrideFullName = true;
	UI->bCreatePhysicsAsset = false;
	UI->AnimSequenceImportData->bImportMeshesInBoneHierarchy = false;
	UI->AnimSequenceImportData->AnimationLength = FBXALIT_ExportedTime;
	UI->AnimSequenceImportData->bUseDefaultSampleRate = true;
	UI->AnimSequenceImportData->bRemoveRedundantKeys = false;
	UI->AnimSequenceImportData->bSnapToClosestFrameBoundary = true;

	Task->Factory = Factory;
	Task->Options = UI;

	FAssetToolsModule& ATM = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	ATM.Get().ImportAssetTasks({Task});

	UAnimSequence* Result = nullptr;
	for (UObject* Obj : Task->GetObjects())
	{
		if (UAnimSequence* Anim = Cast<UAnimSequence>(Obj))
		{
			Result = Anim;
			break;
		}
	}
	Task->RemoveFromRoot();
	return Result;
}

UIKRigDefinition* UIKRetargetMultiSourceCommandlet::CreateIKRigForERSkeleton(
	USkeletalMesh* Mesh, const FString& PackageDir, const FString& Name)
{
	// Create IKRig package
	FString PkgName = PackageDir / Name;

	// Delete existing
	DeleteExistingAsset(PkgName);

	UPackage* Pkg = CreatePackage(*PkgName);
	Pkg->FullyLoad();

	UIKRigDefinition* IKRig = NewObject<UIKRigDefinition>(Pkg, *Name, RF_Public | RF_Standalone);
	UIKRigController* Ctrl = UIKRigController::GetController(IKRig);
	if (!Ctrl)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to get IKRigController for %s"), *Name);
		return nullptr;
	}

	Ctrl->SetSkeletalMesh(Mesh);

	// Set retarget root to Pelvis (all ER humanoids have this)
	const FReferenceSkeleton& RefSkel = Mesh->GetRefSkeleton();
	FName RootBone = NAME_None;
	for (const TCHAR* CandidateName : { TEXT("Pelvis"), TEXT("Root"), TEXT("Master") })
	{
		if (RefSkel.FindBoneIndex(FName(CandidateName)) != INDEX_NONE)
		{
			RootBone = FName(CandidateName);
			break;
		}
	}
	if (RootBone != NAME_None)
	{
		Ctrl->SetRetargetRoot(RootBone);
		UE_LOG(LogTemp, Warning, TEXT("  IKRig %s: retarget root = %s"), *Name, *RootBone.ToString());
	}

	// Define retarget chains for Elden Ring humanoid skeleton
	// All ER enemies share these bone names (where present)
	struct FChainDef { const TCHAR* ChainName; const TCHAR* StartBone; const TCHAR* EndBone; };
	const FChainDef Chains[] = {
		{ TEXT("Spine"),         TEXT("Spine"),       TEXT("Spine2") },
		{ TEXT("Head"),          TEXT("Neck"),        TEXT("Head") },
		{ TEXT("LeftClavicle"),  TEXT("L_Clavicle"),  TEXT("L_Clavicle") },
		{ TEXT("RightClavicle"), TEXT("R_Clavicle"),  TEXT("R_Clavicle") },
		{ TEXT("LeftArm"),       TEXT("L_UpperArm"),  TEXT("L_Hand") },
		{ TEXT("RightArm"),      TEXT("R_UpperArm"),  TEXT("R_Hand") },
		{ TEXT("LeftLeg"),       TEXT("L_Thigh"),     TEXT("L_Foot") },
		{ TEXT("RightLeg"),      TEXT("R_Thigh"),     TEXT("R_Foot") },
		{ TEXT("LeftFoot"),      TEXT("L_Foot"),      TEXT("L_Toe0") },
		{ TEXT("RightFoot"),     TEXT("R_Foot"),      TEXT("R_Toe0") },
	};

	int32 ChainCount = 0;
	for (const auto& CD : Chains)
	{
		int32 StartIdx = RefSkel.FindBoneIndex(FName(CD.StartBone));
		int32 EndIdx = RefSkel.FindBoneIndex(FName(CD.EndBone));
		if (StartIdx == INDEX_NONE || EndIdx == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("    Chain %s: SKIP (missing bones %s=%d, %s=%d)"),
				CD.ChainName, CD.StartBone, StartIdx, CD.EndBone, EndIdx);
			continue;
		}

		FName Result = Ctrl->AddRetargetChain(FName(CD.ChainName), FName(CD.StartBone), FName(CD.EndBone), NAME_None);
		if (Result != NAME_None)
		{
			ChainCount++;
			UE_LOG(LogTemp, Warning, TEXT("    Chain %s: %s -> %s"), CD.ChainName, CD.StartBone, CD.EndBone);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("  IKRig %s: %d chains defined"), *Name, ChainCount);

	// Save
	SaveAsset(IKRig, PkgName);
	return IKRig;
}

UIKRetargeter* UIKRetargetMultiSourceCommandlet::CreateRetargeter(
	UIKRigDefinition* SourceRig, UIKRigDefinition* TargetRig,
	USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh,
	const FString& PackageDir, const FString& Name)
{
	FString PkgName = PackageDir / Name;
	DeleteExistingAsset(PkgName);

	UPackage* Pkg = CreatePackage(*PkgName);
	Pkg->FullyLoad();

	UIKRetargeter* Retargeter = NewObject<UIKRetargeter>(Pkg, *Name, RF_Public | RF_Standalone);
	UIKRetargeterController* Ctrl = UIKRetargeterController::GetController(Retargeter);
	if (!Ctrl)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to get IKRetargeterController"));
		return nullptr;
	}

	// Set source and target IK Rigs
	Ctrl->SetIKRig(ERetargetSourceOrTarget::Source, SourceRig);
	Ctrl->SetIKRig(ERetargetSourceOrTarget::Target, TargetRig);

	// Set preview meshes
	Ctrl->SetPreviewMesh(ERetargetSourceOrTarget::Source, SourceMesh);
	Ctrl->SetPreviewMesh(ERetargetSourceOrTarget::Target, TargetMesh);

	// Add default retarget operations (Pelvis Motion, FK Chains, IK, Root Motion)
	Ctrl->AddDefaultOps();

	// Auto-map chains — since all ER enemies use same bone names, Exact mapping works
	Ctrl->AutoMapChains(EAutoMapChainType::Exact, true);

	// Log chain mappings
	UE_LOG(LogTemp, Warning, TEXT("  Retargeter %s: chains mapped (Exact mode)"), *Name);

	SaveAsset(Retargeter, PkgName);
	return Retargeter;
}

void UIKRetargetMultiSourceCommandlet::DumpRetargetedAnimsToJSON(
	const TArray<UAnimSequence*>& Anims,
	const TArray<FString>& AnimNames,
	const FString& JsonPath)
{
	if (Anims.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No retargeted anims to dump"));
		return;
	}

	// Get skeleton ref data from first anim
	USkeleton* Skel = Anims[0]->GetSkeleton();
	if (!Skel)
	{
		UE_LOG(LogTemp, Error, TEXT("  First retargeted anim has no skeleton"));
		return;
	}

	const FReferenceSkeleton& RefSkel = Skel->GetReferenceSkeleton();
	const int32 NumBones = RefSkel.GetNum();

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

	// Write skeleton info
	TArray<TSharedPtr<FJsonValue>> BonesArray;
	for (int32 i = 0; i < NumBones; i++)
	{
		TSharedPtr<FJsonObject> BoneObj = MakeShared<FJsonObject>();
		BoneObj->SetStringField(TEXT("name"), RefSkel.GetBoneName(i).ToString());
		BoneObj->SetNumberField(TEXT("parent"), RefSkel.GetParentIndex(i));

		const FTransform& RefLocal = RefSkel.GetRefBonePose()[i];
		FVector P = RefLocal.GetTranslation();
		FQuat R = RefLocal.GetRotation();
		FVector S = RefLocal.GetScale3D();

		TArray<TSharedPtr<FJsonValue>> PV, RV, SV;
		PV.Add(MakeShared<FJsonValueNumber>(P.X));
		PV.Add(MakeShared<FJsonValueNumber>(P.Y));
		PV.Add(MakeShared<FJsonValueNumber>(P.Z));
		RV.Add(MakeShared<FJsonValueNumber>(R.X));
		RV.Add(MakeShared<FJsonValueNumber>(R.Y));
		RV.Add(MakeShared<FJsonValueNumber>(R.Z));
		RV.Add(MakeShared<FJsonValueNumber>(R.W));
		SV.Add(MakeShared<FJsonValueNumber>(S.X));
		SV.Add(MakeShared<FJsonValueNumber>(S.Y));
		SV.Add(MakeShared<FJsonValueNumber>(S.Z));
		BoneObj->SetArrayField(TEXT("pos"), PV);
		BoneObj->SetArrayField(TEXT("rot"), RV);
		BoneObj->SetArrayField(TEXT("scale"), SV);

		BonesArray.Add(MakeShared<FJsonValueObject>(BoneObj));
	}
	Root->SetArrayField(TEXT("skeleton"), BonesArray);

	// Write animations
	TSharedPtr<FJsonObject> AnimsObj = MakeShared<FJsonObject>();
	for (int32 ai = 0; ai < Anims.Num(); ai++)
	{
		UAnimSequence* Anim = Anims[ai];
		FString Name = (ai < AnimNames.Num()) ? AnimNames[ai] : Anim->GetName();
		const double PlayLen = Anim->GetPlayLength();
		const int32 NumKeys = Anim->GetNumberOfSampledKeys();
		const double FrameDuration = NumKeys > 1 ? PlayLen / (NumKeys - 1) : PlayLen;

		TSharedPtr<FJsonObject> AnimObj = MakeShared<FJsonObject>();
		AnimObj->SetNumberField(TEXT("duration"), PlayLen);
		AnimObj->SetNumberField(TEXT("num_keys"), NumKeys);

		// Sample ALL frames
		TArray<TSharedPtr<FJsonValue>> FramesArray;
		for (int32 ki = 0; ki < NumKeys; ki++)
		{
			double Time = FMath::Min(ki * FrameDuration, PlayLen);

			TSharedPtr<FJsonObject> FrameObj = MakeShared<FJsonObject>();
			FrameObj->SetNumberField(TEXT("frame"), ki);

			TArray<TSharedPtr<FJsonValue>> BoneLocals;
			for (int32 bi = 0; bi < NumBones; bi++)
			{
				FTransform BoneLocal;
				Anim->GetBoneTransform(BoneLocal, FSkeletonPoseBoneIndex(bi), Time, false);

				FVector P = BoneLocal.GetTranslation();
				FQuat R = BoneLocal.GetRotation();
				// Compact: [px, py, pz, rx, ry, rz, rw]
				TArray<TSharedPtr<FJsonValue>> BV;
				BV.Add(MakeShared<FJsonValueNumber>(P.X));
				BV.Add(MakeShared<FJsonValueNumber>(P.Y));
				BV.Add(MakeShared<FJsonValueNumber>(P.Z));
				BV.Add(MakeShared<FJsonValueNumber>(R.X));
				BV.Add(MakeShared<FJsonValueNumber>(R.Y));
				BV.Add(MakeShared<FJsonValueNumber>(R.Z));
				BV.Add(MakeShared<FJsonValueNumber>(R.W));
				BoneLocals.Add(MakeShared<FJsonValueArray>(BV));
			}
			FrameObj->SetArrayField(TEXT("local"), BoneLocals);
			FramesArray.Add(MakeShared<FJsonValueObject>(FrameObj));
		}
		AnimObj->SetArrayField(TEXT("frames"), FramesArray);
		AnimsObj->SetObjectField(Name, AnimObj);
	}
	Root->SetObjectField(TEXT("animations"), AnimsObj);

	// Write to file
	FString JsonStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(JsonStr, *JsonPath);
	UE_LOG(LogTemp, Warning, TEXT("  JSON dumped: %s (%d anims, %d bones)"),
		*JsonPath, Anims.Num(), NumBones);
}

bool UIKRetargetMultiSourceCommandlet::SaveAsset(UObject* Asset, const FString& PackagePath)
{
	UPackage* Pkg = Asset->GetOutermost();
	FAssetRegistryModule::AssetCreated(Asset);
	Pkg->MarkPackageDirty();
	FString Filename = FPackageName::LongPackageNameToFilename(
		Pkg->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	return UPackage::SavePackage(Pkg, Asset, *Filename, SaveArgs);
}

void UIKRetargetMultiSourceCommandlet::DeleteExistingAsset(const FString& PackagePath)
{
	FString Filename = FPackageName::LongPackageNameToFilename(
		PackagePath, FPackageName::GetAssetPackageExtension());
	if (FPaths::FileExists(Filename))
	{
		IFileManager::Get().Delete(*Filename);
	}
}

#endif // WITH_EDITOR
