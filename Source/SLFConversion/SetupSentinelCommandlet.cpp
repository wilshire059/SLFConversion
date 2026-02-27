#include "SetupSentinelCommandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace1D.h"
#include "Animation/BlendSpace.h"
#include "UObject/SavePackage.h"
#include "SLFPrimaryDataAssets.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Misc/FileHelper.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "FileHelpers.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Animation/AnimationAsset.h"
#include "AssetImportTask.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

USetupSentinelCommandlet::USetupSentinelCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 USetupSentinelCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Warning, TEXT("=== SetupSentinel Params: [%s] ==="), *Params);

	// -diagnose mode: headless frame-by-frame bone transform comparison
	// Compares FLVER mesh (works) vs Sentinel mesh (A-pose) to find root cause
	if (Params.Contains(TEXT("-diagnose")) || Params.Contains(TEXT("diagnose")))
	{
		UE_LOG(LogTemp, Warning, TEXT("=== SENTINEL ANIMATION DIAGNOSIS v2 ==="));

		const FString SentinelMeshPath = TEXT("/Game/CustomEnemies/Sentinel/SKM_Sentinel.SKM_Sentinel");
		const FString SentinelSkelPath = TEXT("/Game/CustomEnemies/Sentinel/SKM_Sentinel_Skeleton.SKM_Sentinel_Skeleton");

		USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *SentinelMeshPath);
		USkeleton* Skel = LoadObject<USkeleton>(nullptr, *SentinelSkelPath);

		if (!Mesh || !Skel)
		{
			UE_LOG(LogTemp, Error, TEXT("Missing Sentinel mesh or skeleton!"));
			return 1;
		}

		const FReferenceSkeleton& MeshRef = Mesh->GetRefSkeleton();
		const FReferenceSkeleton& SkelRef = Skel->GetReferenceSkeleton();
		const TArray<FMatrix44f>& InvRefMatrices = Mesh->GetRefBasesInvMatrix();
		int32 MeshBoneCount = MeshRef.GetRawBoneNum();
		int32 SkelBoneCount = SkelRef.GetRawBoneNum();

		TArray<FString> R;
		R.Add(TEXT("=== SENTINEL ANIMATION DIAGNOSIS v2 ==="));
		R.Add(FString::Printf(TEXT("Mesh: %s (%d bones)"), *Mesh->GetName(), MeshBoneCount));
		R.Add(FString::Printf(TEXT("Skeleton: %s (%d bones)"), *Skel->GetName(), SkelBoneCount));
		R.Add(FString::Printf(TEXT("InvRefMatrices: %d"), InvRefMatrices.Num()));

		// ─── Section 1: Mesh bind pose (RefSkeleton) ───
		R.Add(TEXT(""));
		R.Add(TEXT("═══ SECTION 1: MESH BIND POSE (RefSkeleton) ═══"));
		// Build component-space bind poses
		TArray<FTransform> BindCS;
		BindCS.SetNum(MeshBoneCount);
		for (int32 i = 0; i < MeshBoneCount; i++)
		{
			const FTransform& Local = MeshRef.GetRefBonePose()[i];
			int32 ParentIdx = MeshRef.GetParentIndex(i);
			BindCS[i] = (ParentIdx == INDEX_NONE) ? Local : Local * BindCS[ParentIdx];

			R.Add(FString::Printf(TEXT("  [%d] %s parent=%d Local: Pos=(%.4f,%.4f,%.4f) Rot=(%.6f,%.6f,%.6f,%.6f) Scale=(%.3f,%.3f,%.3f)"),
				i, *MeshRef.GetBoneName(i).ToString(), ParentIdx,
				Local.GetTranslation().X, Local.GetTranslation().Y, Local.GetTranslation().Z,
				Local.GetRotation().X, Local.GetRotation().Y, Local.GetRotation().Z, Local.GetRotation().W,
				Local.GetScale3D().X, Local.GetScale3D().Y, Local.GetScale3D().Z));
			R.Add(FString::Printf(TEXT("         CS: Pos=(%.4f,%.4f,%.4f)"),
				BindCS[i].GetTranslation().X, BindCS[i].GetTranslation().Y, BindCS[i].GetTranslation().Z));
		}

		// ─── Section 2: Skeleton RefPose vs Mesh RefPose ───
		R.Add(TEXT(""));
		R.Add(TEXT("═══ SECTION 2: SKELETON vs MESH BIND POSE COMPARISON ═══"));
		int32 PoseMismatchCount = 0;
		for (int32 i = 0; i < MeshBoneCount; i++)
		{
			FName BoneName = MeshRef.GetBoneName(i);
			int32 SkelIdx = SkelRef.FindBoneIndex(BoneName);
			if (SkelIdx == INDEX_NONE)
			{
				R.Add(FString::Printf(TEXT("  [%d] %s: NOT IN SKELETON"), i, *BoneName.ToString()));
				continue;
			}

			const FTransform& MeshPose = MeshRef.GetRefBonePose()[i];
			const FTransform& SkelPose = SkelRef.GetRefBonePose()[SkelIdx];

			float PosDiff = (MeshPose.GetTranslation() - SkelPose.GetTranslation()).Size();
			float RotDiff = FMath::RadiansToDegrees(MeshPose.GetRotation().AngularDistance(SkelPose.GetRotation()));

			if (PosDiff > 0.01f || RotDiff > 0.5f)
			{
				PoseMismatchCount++;
				R.Add(FString::Printf(TEXT("  [%d] %s MISMATCH (posDiff=%.4f rotDiff=%.2fdeg) skelIdx=%d"),
					i, *BoneName.ToString(), PosDiff, RotDiff, SkelIdx));
				R.Add(FString::Printf(TEXT("    Mesh:  Pos=(%.4f,%.4f,%.4f) Rot=(%.6f,%.6f,%.6f,%.6f)"),
					MeshPose.GetTranslation().X, MeshPose.GetTranslation().Y, MeshPose.GetTranslation().Z,
					MeshPose.GetRotation().X, MeshPose.GetRotation().Y, MeshPose.GetRotation().Z, MeshPose.GetRotation().W));
				R.Add(FString::Printf(TEXT("    Skel:  Pos=(%.4f,%.4f,%.4f) Rot=(%.6f,%.6f,%.6f,%.6f)"),
					SkelPose.GetTranslation().X, SkelPose.GetTranslation().Y, SkelPose.GetTranslation().Z,
					SkelPose.GetRotation().X, SkelPose.GetRotation().Y, SkelPose.GetRotation().Z, SkelPose.GetRotation().W));
			}
		}
		R.Add(FString::Printf(TEXT("  Pose mismatches: %d / %d bones"), PoseMismatchCount, MeshBoneCount));

		// ─── Section 3: Animation data at frame 0 vs bind pose ───
		R.Add(TEXT(""));
		R.Add(TEXT("═══ SECTION 3: ANIM FRAME 0 vs BIND POSE (skinning delta) ═══"));

		const TCHAR* AnimNames[] = { TEXT("Sentinel_Idle"), TEXT("Sentinel_Attack01") };
		for (const TCHAR* AnimName : AnimNames)
		{
			FString AnimPath = FString::Printf(TEXT("/Game/CustomEnemies/Sentinel/Animations/%s.%s"), AnimName, AnimName);
			UAnimSequence* Anim = LoadObject<UAnimSequence>(nullptr, *AnimPath);
			if (!Anim)
			{
				R.Add(FString::Printf(TEXT("--- %s: NOT FOUND ---"), AnimName));
				continue;
			}
			R.Add(FString::Printf(TEXT("--- %s (%.3fs, %d keys) ---"), AnimName,
				Anim->GetPlayLength(), Anim->GetNumberOfSampledKeys()));

			USkeleton* AnimSkel = Anim->GetSkeleton();
			if (!AnimSkel) continue;
			const FReferenceSkeleton& AnimRef = AnimSkel->GetReferenceSkeleton();
			int32 NumAnimBones = AnimRef.GetRawBoneNum();

			// Build anim CS at frame 0
			TArray<FTransform> AnimCS;
			AnimCS.SetNum(NumAnimBones);
			for (int32 b = 0; b < NumAnimBones; b++)
			{
				FTransform AnimLocal;
				FAnimExtractContext Ctx(0.0f);
				Anim->GetBoneTransform(AnimLocal, FSkeletonPoseBoneIndex(b), Ctx, false);
				int32 PIdx = AnimRef.GetParentIndex(b);
				AnimCS[b] = (PIdx == INDEX_NONE) ? AnimLocal : AnimLocal * AnimCS[PIdx];
			}

			// For each bone: compute skinning delta = AnimCS * InvRefMatrix
			// At frame 0 of idle, this should be near-identity
			R.Add(TEXT("  Bone | AnimLocal vs MeshBindLocal | SkinDelta (AnimCS*InvRef)"));
			int32 LargeDeltaCount = 0;
			for (int32 b = 0; b < NumAnimBones; b++)
			{
				FName BoneName = AnimRef.GetBoneName(b);
				int32 MeshIdx = MeshRef.FindBoneIndex(BoneName);
				if (MeshIdx == INDEX_NONE) continue;

				// Animation bone-local at frame 0
				FTransform AnimLocal;
				FAnimExtractContext Ctx0(0.0f);
				Anim->GetBoneTransform(AnimLocal, FSkeletonPoseBoneIndex(b), Ctx0, false);

				// Mesh bind bone-local
				const FTransform& MeshLocal = MeshRef.GetRefBonePose()[MeshIdx];

				// Delta between anim local and mesh bind local
				float LocalPosDiff = (AnimLocal.GetTranslation() - MeshLocal.GetTranslation()).Size();
				float LocalRotDiff = FMath::RadiansToDegrees(AnimLocal.GetRotation().AngularDistance(MeshLocal.GetRotation()));

				// Skinning matrix = AnimCS * InvRef (should be near-identity at bind pose)
				FTransform SkinDelta = FTransform::Identity;
				if (MeshIdx < InvRefMatrices.Num())
				{
					FMatrix44d InvD(InvRefMatrices[MeshIdx]);
					FMatrix44d SkinMat = AnimCS[b].ToMatrixWithScale() * InvD;
					SkinDelta.SetFromMatrix(SkinMat);
				}
				float SkinPosMag = SkinDelta.GetTranslation().Size();
				float SkinRotMag = FMath::RadiansToDegrees(SkinDelta.GetRotation().AngularDistance(FQuat::Identity));
				FVector SkinScale = SkinDelta.GetScale3D();
				bool bScaleOk = SkinScale.Equals(FVector::OneVector, 0.05f);

				bool bLargeDelta = (SkinPosMag > 0.5f || SkinRotMag > 5.0f || !bScaleOk);
				if (bLargeDelta) LargeDeltaCount++;

				// Always print bones with large deltas, first 10, and key bones
				bool bKeyBone = BoneName.ToString().Contains(TEXT("Shield")) ||
					BoneName.ToString().Contains(TEXT("Sword")) ||
					BoneName.ToString().Contains(TEXT("Hand")) ||
					BoneName.ToString().Contains(TEXT("Hips")) ||
					BoneName.ToString().Contains(TEXT("Spine")) ||
					BoneName.ToString().Contains(TEXT("Head"));

				if (bLargeDelta || b < 10 || bKeyBone)
				{
					R.Add(FString::Printf(TEXT("  [%d→%d] %s%s"),
						b, MeshIdx, *BoneName.ToString(),
						bLargeDelta ? TEXT(" *** LARGE DELTA ***") : TEXT("")));
					R.Add(FString::Printf(TEXT("    AnimLocal:  Pos=(%.4f,%.4f,%.4f) Rot=(%.6f,%.6f,%.6f,%.6f)"),
						AnimLocal.GetTranslation().X, AnimLocal.GetTranslation().Y, AnimLocal.GetTranslation().Z,
						AnimLocal.GetRotation().X, AnimLocal.GetRotation().Y, AnimLocal.GetRotation().Z, AnimLocal.GetRotation().W));
					R.Add(FString::Printf(TEXT("    MeshBind:   Pos=(%.4f,%.4f,%.4f) Rot=(%.6f,%.6f,%.6f,%.6f)"),
						MeshLocal.GetTranslation().X, MeshLocal.GetTranslation().Y, MeshLocal.GetTranslation().Z,
						MeshLocal.GetRotation().X, MeshLocal.GetRotation().Y, MeshLocal.GetRotation().Z, MeshLocal.GetRotation().W));
					R.Add(FString::Printf(TEXT("    LocalDiff:  pos=%.4f rot=%.2fdeg"),
						LocalPosDiff, LocalRotDiff));
					R.Add(FString::Printf(TEXT("    SkinDelta:  Pos=(%.4f,%.4f,%.4f) mag=%.4f rot=%.2fdeg Scale=(%.4f,%.4f,%.4f)"),
						SkinDelta.GetTranslation().X, SkinDelta.GetTranslation().Y, SkinDelta.GetTranslation().Z,
						SkinPosMag, SkinRotMag,
						SkinScale.X, SkinScale.Y, SkinScale.Z));
				}
			}
			R.Add(FString::Printf(TEXT("  SUMMARY: %d bones with large skinning delta (>0.5 pos or >5deg rot or bad scale)"), LargeDeltaCount));

			// ─── Section 3b: Check for extreme bone transforms across animation ───
			R.Add(TEXT(""));
			R.Add(FString::Printf(TEXT("  --- %s: EXTREME TRANSFORM SCAN (all frames) ---"), AnimName));
			float Duration = Anim->GetPlayLength();
			int32 NumSampleFrames = 10;
			for (int32 b = 0; b < NumAnimBones; b++)
			{
				FName BoneName = AnimRef.GetBoneName(b);
				float MaxPosMag = 0.0f;
				float MaxRotDeg = 0.0f;
				float MinScale = 999.0f, MaxScale = 0.0f;

				for (int32 f = 0; f < NumSampleFrames; f++)
				{
					float T = Duration * f / FMath::Max(1, NumSampleFrames - 1);
					FTransform BoneT;
					FAnimExtractContext Ctx(T);
					Anim->GetBoneTransform(BoneT, FSkeletonPoseBoneIndex(b), Ctx, false);

					float PosMag = BoneT.GetTranslation().Size();
					float RotDeg = FMath::RadiansToDegrees(BoneT.GetRotation().AngularDistance(FQuat::Identity));
					FVector S = BoneT.GetScale3D();
					float SMin = FMath::Min3(S.X, S.Y, S.Z);
					float SMax = FMath::Max3(S.X, S.Y, S.Z);

					MaxPosMag = FMath::Max(MaxPosMag, PosMag);
					MaxRotDeg = FMath::Max(MaxRotDeg, RotDeg);
					MinScale = FMath::Min(MinScale, SMin);
					MaxScale = FMath::Max(MaxScale, SMax);
				}

				bool bExtreme = (MaxPosMag > 100.0f || MinScale < 0.5f || MaxScale > 2.0f);
				if (bExtreme)
				{
					R.Add(FString::Printf(TEXT("    [%d] %s EXTREME: maxPos=%.2f maxRot=%.1fdeg scaleRange=[%.4f,%.4f]"),
						b, *BoneName.ToString(), MaxPosMag, MaxRotDeg, MinScale, MaxScale));
				}
			}
		}

		// ─── Section 4: Bone hierarchy match ───
		R.Add(TEXT(""));
		R.Add(TEXT("═══ SECTION 4: BONE HIERARCHY MESH vs SKELETON ═══"));
		int32 HierarchyMismatches = 0;
		for (int32 i = 0; i < MeshBoneCount; i++)
		{
			FName MBone = MeshRef.GetBoneName(i);
			int32 MParent = MeshRef.GetParentIndex(i);
			FName MParentName = (MParent != INDEX_NONE) ? MeshRef.GetBoneName(MParent) : FName(TEXT("ROOT"));

			int32 SIdx = SkelRef.FindBoneIndex(MBone);
			if (SIdx == INDEX_NONE)
			{
				R.Add(FString::Printf(TEXT("  [%d] %s: NOT IN SKELETON"), i, *MBone.ToString()));
				HierarchyMismatches++;
				continue;
			}
			int32 SParent = SkelRef.GetParentIndex(SIdx);
			FName SParentName = (SParent != INDEX_NONE) ? SkelRef.GetBoneName(SParent) : FName(TEXT("ROOT"));

			if (MParentName != SParentName)
			{
				HierarchyMismatches++;
				R.Add(FString::Printf(TEXT("  [%d] %s: PARENT MISMATCH mesh=%s skel=%s"),
					i, *MBone.ToString(), *MParentName.ToString(), *SParentName.ToString()));
			}
		}
		R.Add(FString::Printf(TEXT("  Hierarchy mismatches: %d / %d bones"), HierarchyMismatches, MeshBoneCount));

		// ─── Section 5: Vertex weight distribution ───
		R.Add(TEXT(""));
		R.Add(TEXT("═══ SECTION 5: MESH VERTEX/LOD INFO ═══"));
		{
			FSkeletalMeshRenderData* RD = Mesh->GetResourceForRendering();
			if (RD && RD->LODRenderData.Num() > 0)
			{
				const FSkeletalMeshLODRenderData& LOD0 = RD->LODRenderData[0];
				R.Add(FString::Printf(TEXT("  LOD0 Vertices: %d"), LOD0.GetNumVertices()));
				R.Add(FString::Printf(TEXT("  LOD0 ActiveBoneIndices: %d"), LOD0.ActiveBoneIndices.Num()));
				R.Add(FString::Printf(TEXT("  LOD0 RequiredBones: %d"), LOD0.RequiredBones.Num()));

				FString BoneList;
				for (int32 i = 0; i < LOD0.ActiveBoneIndices.Num(); i++)
				{
					int32 BIdx = LOD0.ActiveBoneIndices[i];
					FName BName = (BIdx < MeshRef.GetRawBoneNum()) ? MeshRef.GetBoneName(BIdx) : FName(TEXT("?"));
					if (i > 0) BoneList += TEXT(", ");
					BoneList += FString::Printf(TEXT("%d:%s"), BIdx, *BName.ToString());
				}
				R.Add(FString::Printf(TEXT("  ActiveBones: %s"), *BoneList));
			}
			else
			{
				R.Add(TEXT("  RenderData: NULL (run without -NullRHI!)"));
			}
		}

		// Write report
		FString ReportPath = TEXT("C:/scripts/SLFConversion/sentinel_diagnosis_report.txt");
		FString FullReport = FString::Join(R, TEXT("\n"));
		FFileHelper::SaveStringToFile(FullReport, *ReportPath);
		UE_LOG(LogTemp, Warning, TEXT("=== DIAGNOSIS COMPLETE — saved to %s ==="), *ReportPath);
		for (const FString& Line : R)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Line);
		}
		return 0;
	}

	// -dumpjson mode: export retargeted animation bone transforms to JSON for Blender validation
	if (Params.Contains(TEXT("-dumpjson")))
	{
		UE_LOG(LogTemp, Warning, TEXT("=== DUMP ANIMATION JSON ==="));

		const FString AnimDir = TEXT("/Game/CustomEnemies/Sentinel/Animations");
		const FString SentinelMeshPath = TEXT("/Game/CustomEnemies/Sentinel/SKM_Sentinel.SKM_Sentinel");
		const FString OutputPath = TEXT("C:/scripts/elden_ring_tools/output/sentinel/ue5_retarget_dump.json");

		USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *SentinelMeshPath);
		if (!SentinelMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load Sentinel mesh"));
			return 1;
		}

		const FReferenceSkeleton& RefSkel = SentinelMesh->GetRefSkeleton();
		const int32 NumBones = RefSkel.GetRawBoneNum();

		// Build JSON string manually
		FString Json = TEXT("{\n");

		// Dump ref pose
		Json += TEXT("  \"ref_pose\": {\n");
		Json += FString::Printf(TEXT("    \"num_bones\": %d,\n"), NumBones);
		Json += TEXT("    \"bones\": [\n");
		for (int32 i = 0; i < NumBones; i++)
		{
			const FTransform& T = RefSkel.GetRefBonePose()[i];
			FVector Pos = T.GetTranslation();
			FQuat Rot = T.GetRotation();
			FVector Scl = T.GetScale3D();
			Json += FString::Printf(TEXT("      {\"name\": \"%s\", \"parent\": %d, \"pos\": [%.6f, %.6f, %.6f], \"rot\": [%.6f, %.6f, %.6f, %.6f], \"scale\": [%.6f, %.6f, %.6f]}%s\n"),
				*RefSkel.GetBoneName(i).ToString(), RefSkel.GetParentIndex(i),
				Pos.X, Pos.Y, Pos.Z, Rot.X, Rot.Y, Rot.Z, Rot.W,
				Scl.X, Scl.Y, Scl.Z, i < NumBones - 1 ? TEXT(",") : TEXT(""));
		}
		Json += TEXT("    ]\n  },\n");

		// Dump animations (selected set)
		const FString AnimNames[] = {
			TEXT("Sentinel_Idle"), TEXT("Sentinel_Attack01"), TEXT("Sentinel_Walk"),
			TEXT("Sentinel_Run"), TEXT("Sentinel_HitReact")
		};

		Json += TEXT("  \"animations\": {\n");
		int32 AnimIdx = 0;
		for (const FString& AnimName : AnimNames)
		{
			FString AnimPath = AnimDir / AnimName + TEXT(".") + AnimName;
			UAnimSequence* Anim = LoadObject<UAnimSequence>(nullptr, *AnimPath);
			if (!Anim)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Skipping missing: %s"), *AnimPath);
				continue;
			}

			int32 NumKeys = Anim->GetNumberOfSampledKeys();
			double PlayLen = Anim->GetPlayLength();
			UE_LOG(LogTemp, Warning, TEXT("  Dumping %s: %d keys, %.3fs"), *AnimName, NumKeys, PlayLen);

			Json += FString::Printf(TEXT("    \"%s\": {\n"), *AnimName);
			Json += FString::Printf(TEXT("      \"num_keys\": %d, \"length\": %.6f,\n"), NumKeys, PlayLen);
			Json += TEXT("      \"frames\": [\n");

			// Sample up to 8 frames: 0, 25%, 50%, 75%, 100% + extras
			TArray<int32> SampleFrames;
			SampleFrames.Add(0);
			if (NumKeys > 4)
			{
				SampleFrames.Add(NumKeys / 4);
				SampleFrames.Add(NumKeys / 2);
				SampleFrames.Add(3 * NumKeys / 4);
			}
			SampleFrames.Add(NumKeys - 1);

			for (int32 fi = 0; fi < SampleFrames.Num(); fi++)
			{
				int32 KeyIdx = SampleFrames[fi];
				double Time = NumKeys > 1 ? PlayLen * KeyIdx / (NumKeys - 1) : 0.0;

				Json += TEXT("        {\n");
				Json += FString::Printf(TEXT("          \"frame\": %d, \"time\": %.6f,\n"), KeyIdx, Time);
				Json += TEXT("          \"local\": [\n");

				for (int32 b = 0; b < NumBones; b++)
				{
					FTransform BoneT;
					Anim->GetBoneTransform(BoneT, FSkeletonPoseBoneIndex(b), Time, false);
					FVector P = BoneT.GetTranslation();
					FQuat R = BoneT.GetRotation();
					Json += FString::Printf(TEXT("            [%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f]%s\n"),
						P.X, P.Y, P.Z, R.X, R.Y, R.Z, R.W,
						b < NumBones - 1 ? TEXT(",") : TEXT(""));
				}
				Json += TEXT("          ]\n");
				Json += FString::Printf(TEXT("        }%s\n"), fi < SampleFrames.Num() - 1 ? TEXT(",") : TEXT(""));
			}

			Json += TEXT("      ]\n");
			AnimIdx++;
			Json += FString::Printf(TEXT("    }%s\n"), AnimIdx < UE_ARRAY_COUNT(AnimNames) ? TEXT(",") : TEXT(""));
		}

		Json += TEXT("  }\n}\n");

		FFileHelper::SaveStringToFile(Json, *OutputPath);
		UE_LOG(LogTemp, Warning, TEXT("=== JSON dump saved to %s ==="), *OutputPath);
		return 0;
	}

	// -test mode: isolation test to find Sentinel A-pose root cause
	if (Params.Contains(TEXT("-test")))
	{
		UE_LOG(LogTemp, Warning, TEXT("=== ANIMATION ISOLATION TEST v2 ==="));

		const FString TestDir = TEXT("/Game/Temp/AnimTest");
		const FString C3100FBX = TEXT("C:/scripts/elden_ring_tools/output/c3100/c3100/fbx/a000_002002.fbx");
		const FString SentinelFBX = TEXT("C:/scripts/elden_ring_tools/output/sentinel/final/Sentinel_Attack01.fbx");
		const FString C3100SkelPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh_Skeleton.c3100_mesh_Skeleton");
		const FString SentinelSkelPath = TEXT("/Game/CustomEnemies/Sentinel/SKM_Sentinel_Skeleton.SKM_Sentinel_Skeleton");
		const FString C3100MeshPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh.c3100_mesh");
		const FString SentinelMeshPath = TEXT("/Game/CustomEnemies/Sentinel/SKM_Sentinel.SKM_Sentinel");

		USkeleton* C3100Skel = LoadObject<USkeleton>(nullptr, *C3100SkelPath);
		USkeleton* SentinelSkel = LoadObject<USkeleton>(nullptr, *SentinelSkelPath);
		USkeletalMesh* C3100Mesh = LoadObject<USkeletalMesh>(nullptr, *C3100MeshPath);
		USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *SentinelMeshPath);

		// Log preview mesh state
		UE_LOG(LogTemp, Warning, TEXT("=== PREVIEW MESH CHECK ==="));
		if (C3100Skel)
		{
			USkeletalMesh* PM = C3100Skel->GetPreviewMesh(false);
			UE_LOG(LogTemp, Warning, TEXT("  c3100 skeleton PreviewMesh: %s"), PM ? *PM->GetName() : TEXT("NULL"));
		}
		if (SentinelSkel)
		{
			USkeletalMesh* PM = SentinelSkel->GetPreviewMesh(false);
			UE_LOG(LogTemp, Warning, TEXT("  Sentinel skeleton PreviewMesh: %s"), PM ? *PM->GetName() : TEXT("NULL"));

			// FIX: Set the Sentinel mesh as preview mesh on the Sentinel skeleton
			if (SentinelMesh)
			{
				SentinelSkel->SetPreviewMesh(SentinelMesh, true);
				UE_LOG(LogTemp, Warning, TEXT("  -> Set Sentinel skeleton PreviewMesh to SKM_Sentinel"));
				// Save skeleton with updated preview mesh
				UPackage* SkelPkg = SentinelSkel->GetOutermost();
				SkelPkg->MarkPackageDirty();
				FString SkelFile = FPackageName::LongPackageNameToFilename(SkelPkg->GetName(), FPackageName::GetAssetPackageExtension());
				FSavePackageArgs SaveArgs;
				SaveArgs.TopLevelFlags = RF_Standalone;
				UPackage::SavePackage(SkelPkg, SentinelSkel, *SkelFile, SaveArgs);
			}
		}

		// Step 0: Baseline — c3100 FBX + c3100 skeleton
		UE_LOG(LogTemp, Warning, TEXT("--- Step 0: c3100 FBX + c3100 skeleton (BASELINE) ---"));
		FString R0 = USLFAutomationLibrary::ImportAnimFBXDirect(C3100FBX, TestDir, TEXT("Step0_c3100fbx_c3100skel"), C3100SkelPath, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *R0);

		// Step 1: Same FBX + Sentinel skeleton (now with preview mesh set)
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1: c3100 FBX + Sentinel skeleton (preview mesh fixed) ---"));
		FString R1 = USLFAutomationLibrary::ImportAnimFBXDirect(C3100FBX, TestDir, TEXT("Step1_c3100fbx_sentinelskel"), SentinelSkelPath, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *R1);

		// Step 2: Sentinel ARP FBX + Sentinel skeleton
		UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Sentinel ARP FBX + Sentinel skeleton ---"));
		FString R2 = USLFAutomationLibrary::ImportAnimFBXDirect(SentinelFBX, TestDir, TEXT("Step2_sentinelfbx_sentinelskel"), SentinelSkelPath, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *R2);

		// Step 3: c3100 FBX + c3100 skeleton, but set per-anim preview mesh to Sentinel mesh
		UE_LOG(LogTemp, Warning, TEXT("--- Step 3: c3100 FBX + c3100 skeleton + Sentinel preview mesh ---"));
		FString R3 = USLFAutomationLibrary::ImportAnimFBXDirect(C3100FBX, TestDir, TEXT("Step3_c3100fbx_c3100skel_sentinelpreview"), C3100SkelPath, 1.0f);
		{
			// Set per-animation preview mesh to Sentinel mesh
			UAnimSequence* Step3Anim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Temp/AnimTest/Step3_c3100fbx_c3100skel_sentinelpreview.Step3_c3100fbx_c3100skel_sentinelpreview"));
			if (Step3Anim && SentinelMesh)
			{
				Step3Anim->SetPreviewMesh(SentinelMesh, true);
				UPackage* Pkg = Step3Anim->GetOutermost();
				Pkg->MarkPackageDirty();
				FString Fn = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
				FSavePackageArgs SA;
				SA.TopLevelFlags = RF_Standalone;
				UPackage::SavePackage(Pkg, Step3Anim, *Fn, SA);
				UE_LOG(LogTemp, Warning, TEXT("  Set Step3 PreviewMesh to SKM_Sentinel"));
			}
		}

		// Step 4: Reassign Sentinel mesh to use c3100 skeleton, import anim against c3100 skel
		UE_LOG(LogTemp, Warning, TEXT("--- Step 4: Sentinel mesh reassigned to c3100 skeleton ---"));
		if (SentinelMesh && C3100Skel)
		{
			USkeleton* OldSkel = SentinelMesh->GetSkeleton();
			UE_LOG(LogTemp, Warning, TEXT("  Sentinel mesh old skeleton: %s"),
				OldSkel ? *OldSkel->GetName() : TEXT("NULL"));

			SentinelMesh->SetSkeleton(C3100Skel);
			UE_LOG(LogTemp, Warning, TEXT("  -> Reassigned to c3100_mesh_Skeleton"));

			// Save mesh with new skeleton
			UPackage* MeshPkg = SentinelMesh->GetOutermost();
			MeshPkg->MarkPackageDirty();
			FString MeshFile = FPackageName::LongPackageNameToFilename(MeshPkg->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs MeshSaveArgs;
			MeshSaveArgs.TopLevelFlags = RF_Standalone;
			UPackage::SavePackage(MeshPkg, SentinelMesh, *MeshFile, MeshSaveArgs);

			// Also set as preview mesh on c3100 skeleton for these anims
			// Import animation against c3100 skeleton
			FString R4 = USLFAutomationLibrary::ImportAnimFBXDirect(SentinelFBX, TestDir, TEXT("Step4_sentinelfbx_c3100skel"), C3100SkelPath, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *R4);

			// Restore Sentinel mesh back to its original skeleton
			if (OldSkel)
			{
				SentinelMesh->SetSkeleton(OldSkel);
				MeshPkg->MarkPackageDirty();
				UPackage::SavePackage(MeshPkg, SentinelMesh, *MeshFile, MeshSaveArgs);
				UE_LOG(LogTemp, Warning, TEXT("  -> Restored Sentinel mesh to original skeleton"));
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("=== TEST COMPLETE — check /Game/Temp/AnimTest/ in editor ==="));
		UE_LOG(LogTemp, Warning, TEXT("  Step 0: Baseline (c3100+c3100) — should animate"));
		UE_LOG(LogTemp, Warning, TEXT("  Step 1: Sentinel skeleton with preview mesh set — tests ref pose"));
		UE_LOG(LogTemp, Warning, TEXT("  Step 2: ARP FBX on Sentinel skel — tests FBX data"));
		UE_LOG(LogTemp, Warning, TEXT("  Step 3: c3100 anim with Sentinel as preview mesh — tests mesh compatibility"));
		UE_LOG(LogTemp, Warning, TEXT("  Step 4: ARP FBX on c3100 skel — tests if c3100 skel makes ARP data work"));
		return 0;
	}

	// Parse -forensic flag: use Phase 3 forensically distinct FBXes (Mannequin skeleton)
	bool bForensic = Params.Contains(TEXT("-forensic")) || Params.Contains(TEXT("forensic"));

	// Source c3100 paths (used for forensic comparison in validation step)
	const FString SourceAnimDir = TEXT("/Game/EldenRingAnimations/c3100_guard/Animations");

	// Sentinel output paths
	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel");
	const FString SentinelAnimDir = SentinelDir / TEXT("Animations");
	FString Result;

	UE_LOG(LogTemp, Warning, TEXT("=== SetupSentinel Commandlet: Starting %s==="),
		bForensic ? TEXT("(FORENSIC MODE) ") : TEXT(""));

	// ═══════════════════════════════════════════════════════════════════════
	// Step 0: Import Sentinel Mesh from ARP-baked FBX
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 0: Import Sentinel Mesh ---"));
	{
		bool bSkipMesh = Params.Contains(TEXT("-skipmesh")) || Params.Contains(TEXT("skipmesh"));
		if (bSkipMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("  SKIPPING mesh import (-skipmesh flag). Using existing mesh+skeleton."));
		}
		else if (bForensic)
		{
			// Forensic: import mesh via UAssetImportTask (same pipeline as animations)
			// This ensures mesh bind pose and animation bind pose use identical FBX import code paths
			FString FBXPath = TEXT("C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/final/SKM_Sentinel.fbx");
			if (!FPaths::FileExists(FBXPath))
			{
				UE_LOG(LogTemp, Error, TEXT("  No Sentinel FBX found at %s"), *FBXPath);
				return 1;
			}

			// Initialize Slate early (needed for UAssetImportTask)
			if (!FSlateApplication::IsInitialized())
			{
				FSlateApplication::Create();
			}

			UAssetImportTask* MeshTask = NewObject<UAssetImportTask>();
			MeshTask->AddToRoot();
			MeshTask->Filename = FBXPath;
			MeshTask->DestinationPath = SentinelDir;
			MeshTask->DestinationName = TEXT("SKM_Sentinel");
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

			TArray<UObject*> MeshResults = MeshTask->GetObjects();
			bool bMeshOk = false;
			for (UObject* Obj : MeshResults)
			{
				if (USkeletalMesh* SM = Cast<USkeletalMesh>(Obj))
				{
					UE_LOG(LogTemp, Warning, TEXT("  Mesh imported: %s (%d bones)"),
						*SM->GetName(), SM->GetRefSkeleton().GetRawBoneNum());
					bMeshOk = true;
					break;
				}
			}
			if (!bMeshOk)
			{
				UE_LOG(LogTemp, Error, TEXT("  Mesh import FAILED (returned %d objects)"), MeshResults.Num());
				for (UObject* Obj : MeshResults)
				{
					UE_LOG(LogTemp, Error, TEXT("    Got: %s (%s)"), *Obj->GetName(), *Obj->GetClass()->GetName());
				}
			}
			MeshTask->RemoveFromRoot();
		}
		else
		{
			FString FBXPath = TEXT("C:/scripts/elden_ring_tools/output/sentinel/final/SKM_Sentinel.fbx");
			if (FPaths::FileExists(FBXPath))
			{
				Result = USLFAutomationLibrary::ImportSkeletalMeshFromFBX(
					FBXPath, SentinelDir, TEXT("SKM_Sentinel"), TEXT("_"), 1.0f);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  No Sentinel FBX found."));
				return 1;
			}
		}
	}

	// Diagnostic: dump imported mesh bounding box to verify geometry
	{
		USkeletalMesh* DiagMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir / TEXT("SKM_Sentinel")));
		if (DiagMesh)
		{
			FBoxSphereBounds Bounds = DiagMesh->GetBounds();
			UE_LOG(LogTemp, Warning, TEXT("  MESH BOUNDS: Origin=(%.1f,%.1f,%.1f) Extent=(%.1f,%.1f,%.1f) Radius=%.1f"),
				Bounds.Origin.X, Bounds.Origin.Y, Bounds.Origin.Z,
				Bounds.BoxExtent.X, Bounds.BoxExtent.Y, Bounds.BoxExtent.Z,
				Bounds.SphereRadius);

			const FReferenceSkeleton& MRef = DiagMesh->GetRefSkeleton();
			UE_LOG(LogTemp, Warning, TEXT("  MESH BONES: %d, Skeleton: %s"),
				MRef.GetRawBoneNum(),
				DiagMesh->GetSkeleton() ? *DiagMesh->GetSkeleton()->GetName() : TEXT("NULL"));

			// Dump first 5 bone ref poses to check scale
			for (int32 i = 0; i < FMath::Min(5, MRef.GetRawBoneNum()); i++)
			{
				const FTransform& T = MRef.GetRefBonePose()[i];
				UE_LOG(LogTemp, Warning, TEXT("    Bone[%d] %s: Pos=(%.4f,%.4f,%.4f) Rot=(%.4f,%.4f,%.4f,%.4f)"),
					i, *MRef.GetBoneName(i).ToString(),
					T.GetTranslation().X, T.GetTranslation().Y, T.GetTranslation().Z,
					T.GetRotation().X, T.GetRotation().Y, T.GetRotation().Z, T.GetRotation().W);
			}
		}
	}

	// NOTE: const_cast ref pose x100 fix was here but didn't persist through
	// UE5 serialization. Skeleton stays meter-scale. Animations compensate
	// with Scale=(100,100,100) at runtime. This causes stretching but works.

	const FString SentinelSkeletonAssetPath = SentinelDir / TEXT("SKM_Sentinel_Skeleton");
	FString SentinelSkeletonPath = SentinelSkeletonAssetPath + TEXT(".SKM_Sentinel_Skeleton");
	USkeleton* SentinelSkeleton = LoadObject<USkeleton>(nullptr, *SentinelSkeletonPath);

	if (!SentinelSkeleton)
	{
		UE_LOG(LogTemp, Error, TEXT("FATAL: Sentinel skeleton not found at %s"), *SentinelSkeletonPath);
		return 1;
	}
	UE_LOG(LogTemp, Warning, TEXT("  Using Sentinel skeleton: %s (%d bones)"),
		*SentinelSkeletonPath, SentinelSkeleton->GetReferenceSkeleton().GetNum());

	// Sync skeleton ref pose from mesh bind pose + set preview mesh + translation retargeting
	{
		USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir / TEXT("SKM_Sentinel")));
		if (SentinelMesh)
		{
			// CRITICAL: Sync skeleton's ReferenceSkeleton from the mesh's RefSkeleton.
			// The mesh's bind pose (RefSkeleton) and skeleton's ref pose can diverge when
			// mesh and animations are imported through different code paths. This ensures
			// the animation evaluator uses the same bone orientations as the mesh skinning.
			UE_LOG(LogTemp, Warning, TEXT("  Syncing skeleton ref pose from mesh bind pose..."));
			{
				// Compare BEFORE sync
				const FReferenceSkeleton& MeshRef = SentinelMesh->GetRefSkeleton();
				const FReferenceSkeleton& SkelRef = SentinelSkeleton->GetReferenceSkeleton();
				int32 MismatchCount = 0;
				for (int32 i = 0; i < FMath::Min(MeshRef.GetRawBoneNum(), SkelRef.GetRawBoneNum()); i++)
				{
					FName BoneName = MeshRef.GetBoneName(i);
					int32 SkelIdx = SkelRef.FindRawBoneIndex(BoneName);
					if (SkelIdx != INDEX_NONE)
					{
						const FTransform& MP = MeshRef.GetRefBonePose()[i];
						const FTransform& SP = SkelRef.GetRefBonePose()[SkelIdx];
						float PosDiff = (MP.GetTranslation() - SP.GetTranslation()).Size();
						float RotDiff = FMath::RadiansToDegrees(MP.GetRotation().AngularDistance(SP.GetRotation()));
						if (PosDiff > 0.001f || RotDiff > 0.1f)
						{
							MismatchCount++;
							if (MismatchCount <= 5)
							{
								UE_LOG(LogTemp, Warning, TEXT("    MISMATCH [%d] %s: posDiff=%.4f rotDiff=%.2fdeg"),
									i, *BoneName.ToString(), PosDiff, RotDiff);
							}
						}
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Bind pose mismatches: %d/%d bones"),
					MismatchCount, MeshRef.GetRawBoneNum());
			}

			// Sync skeleton ref pose from mesh
			SentinelSkeleton->UpdateReferencePoseFromMesh(SentinelMesh);
			UE_LOG(LogTemp, Warning, TEXT("  UpdateReferencePoseFromMesh completed"));

			// Set translation retargeting: Skeleton for all non-root bones
			// This forces UE5 to use the mesh's bone lengths instead of animation bone lengths
			{
				const FReferenceSkeleton& SkelRef = SentinelSkeleton->GetReferenceSkeleton();
				int32 NumBones = SkelRef.GetRawBoneNum();
				for (int32 i = 0; i < NumBones; i++)
				{
					FName BoneName = SkelRef.GetBoneName(i);
					int32 ParentIdx = SkelRef.GetParentIndex(i);
					if (ParentIdx == INDEX_NONE)
					{
						// Root bone: Animation (keep animation root motion)
						SentinelSkeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::Animation);
					}
					else if (BoneName == FName(TEXT("pelvis")) || BoneName == FName(TEXT("root")) || BoneName == FName(TEXT("root_motion")))
					{
						// Pelvis/root: AnimationScaled (preserves height/proportions)
						SentinelSkeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::AnimationScaled);
					}
					else
					{
						// All other bones: Skeleton (use mesh bone lengths, animation rotations only)
						SentinelSkeleton->SetBoneTranslationRetargetingMode(i, EBoneTranslationRetargetingMode::Skeleton);
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("  Set translation retargeting: root=Animation, pelvis=AnimScaled, others=Skeleton"));
			}

			// Set preview mesh
			SentinelSkeleton->SetPreviewMesh(SentinelMesh, true);

			// Save skeleton
			UPackage* SkelPkg = SentinelSkeleton->GetOutermost();
			SkelPkg->MarkPackageDirty();
			FString SkelFile = FPackageName::LongPackageNameToFilename(SkelPkg->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SkelSaveArgs;
			SkelSaveArgs.TopLevelFlags = RF_Standalone;
			UPackage::SavePackage(SkelPkg, SentinelSkeleton, *SkelFile, SkelSaveArgs);
			UE_LOG(LogTemp, Warning, TEXT("  Skeleton saved (ref pose synced, retargeting set, preview mesh set)"));
		}
	}

	// -meshonly flag: stop after mesh import + skeleton setup
	bool bMeshOnly = Params.Contains(TEXT("-meshonly")) || Params.Contains(TEXT("meshonly"));
	if (bMeshOnly)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== -meshonly flag: Stopping after mesh import. ==="));
		return 0;
	}

	// -skipanims flag: skip animation import/retarget, jump to montage/ability/trace/bonescale steps
	bool bSkipAnims = Params.Contains(TEXT("-skipanims")) || Params.Contains(TEXT("skipanims"));

	// ═══════════════════════════════════════════════════════════════════════
	// Step 1: Import/Retarget animations
	// ═══════════════════════════════════════════════════════════════════════

	// Expected animation names (shared by both paths)
	const TCHAR* ExpectedAnims[] = {
		TEXT("Sentinel_Idle"), TEXT("Sentinel_Walk"), TEXT("Sentinel_Run"),
		TEXT("Sentinel_Attack01"), TEXT("Sentinel_Attack02"), TEXT("Sentinel_Attack03_Fast"),
		TEXT("Sentinel_HeavyAttack"),
		TEXT("Sentinel_HitReact"), TEXT("Sentinel_HitReact_Light"), TEXT("Sentinel_GuardHit"),
		TEXT("Sentinel_Death_Front"), TEXT("Sentinel_Death_Back"), TEXT("Sentinel_Death_Left"),
		TEXT("Sentinel_Dodge_Fwd"), TEXT("Sentinel_Dodge_Bwd"),
		TEXT("Sentinel_Dodge_Left"), TEXT("Sentinel_Dodge_Right"),
		TEXT("Sentinel_Guard"),
		TEXT("Sentinel_PoiseBreak_Start"), TEXT("Sentinel_PoiseBreak_Loop"),
	};

	if (bSkipAnims)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1: SKIPPED (-skipanims flag) ---"));
	}
	else if (bForensic)
	{
		// ─── Forensic mode: UAssetImportTask (native UE5 FBX pipeline) ───
		// Uses UE5's native FBX import which handles PreRotation/bone orientation correctly.
		// Uses original Phase 3 FBXes (mesh+animation) — these are the SAME files that produced
		// the mesh import, guaranteeing matching bind poses. bImportMesh=false skips mesh re-import.
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1: UAssetImportTask (native UE5 FBX pipeline) ---"));

		// Use original Phase 3 FBXes (with mesh) to guarantee matching bind pose
		const FString ForensicDir = TEXT("C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/final");
		const FString DestPath = TEXT("/Game/CustomEnemies/Sentinel/Animations");

		// Load the skeleton
		USkeleton* SentinelSkel = LoadObject<USkeleton>(nullptr, *SentinelSkeletonPath);
		if (!SentinelSkel)
		{
			UE_LOG(LogTemp, Error, TEXT("  Cannot load skeleton: %s"), *SentinelSkeletonPath);
			return 1;
		}
		UE_LOG(LogTemp, Warning, TEXT("  Skeleton: %s (%d bones)"),
			*SentinelSkel->GetName(), SentinelSkel->GetReferenceSkeleton().GetNum());

		// Initialize Slate if not already (commandlet mode)
		if (!FSlateApplication::IsInitialized())
		{
			UE_LOG(LogTemp, Warning, TEXT("  Initializing Slate for commandlet mode..."));
			FSlateApplication::Create();
		}

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

		int32 AnimSuccess = 0;
		int32 AnimFail = 0;

		for (const TCHAR* AnimName : ExpectedAnims)
		{
			FString FBXFile = ForensicDir / FString(AnimName) + TEXT(".fbx");
			if (!FPaths::FileExists(FBXFile))
			{
				UE_LOG(LogTemp, Warning, TEXT("  SKIP %s: not found"), AnimName);
				AnimFail++;
				continue;
			}

			// Create UAssetImportTask
			UAssetImportTask* ImportTask = NewObject<UAssetImportTask>();
			ImportTask->AddToRoot();
			ImportTask->Filename = FBXFile;
			ImportTask->DestinationPath = DestPath;
			ImportTask->DestinationName = FString(AnimName);
			ImportTask->bAutomated = true;
			ImportTask->bReplaceExisting = true;
			ImportTask->bSave = true;

			// Configure FBX factory explicitly
			UFbxFactory* FbxFactory = NewObject<UFbxFactory>();
			FbxFactory->SetDetectImportTypeOnImport(false);

			UFbxImportUI* FbxUI = FbxFactory->ImportUI;
			FbxUI->bImportMesh = false;
			FbxUI->bImportMaterials = false;
			FbxUI->bImportTextures = false;
			FbxUI->bImportAnimations = true;
			FbxUI->MeshTypeToImport = FBXIT_Animation;
			FbxUI->Skeleton = SentinelSkel;
			FbxUI->bAutomatedImportShouldDetectType = false;
			FbxUI->bOverrideFullName = true;
			FbxUI->bCreatePhysicsAsset = false;

			// Animation settings
			FbxUI->AnimSequenceImportData->bImportMeshesInBoneHierarchy = false;
			FbxUI->AnimSequenceImportData->AnimationLength = FBXALIT_ExportedTime;
			FbxUI->AnimSequenceImportData->bUseDefaultSampleRate = true;
			FbxUI->AnimSequenceImportData->bRemoveRedundantKeys = false;
			FbxUI->AnimSequenceImportData->bSnapToClosestFrameBoundary = true;

			ImportTask->Factory = FbxFactory;
			ImportTask->Options = FbxUI;

			// Execute
			AssetToolsModule.Get().ImportAssetTasks({ImportTask});

			// Check result
			TArray<UObject*> ImportedObjects = ImportTask->GetObjects();
			bool bFound = false;
			for (UObject* Obj : ImportedObjects)
			{
				if (UAnimSequence* AnimSeq = Cast<UAnimSequence>(Obj))
				{
					AnimSeq->bForceRootLock = true;
					AnimSeq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;

					int32 NumBoneTracks = AnimSeq->GetDataModel() ? AnimSeq->GetDataModel()->GetNumBoneTracks() : 0;
					float Duration = AnimSeq->GetPlayLength();
					UE_LOG(LogTemp, Warning, TEXT("  %s: OK — %d boneTracks, %.2fs"),
						AnimName, NumBoneTracks, Duration);
					AnimSuccess++;
					bFound = true;
					break;
				}
				else if (Obj)
				{
					UE_LOG(LogTemp, Warning, TEXT("  %s: imported as %s (not AnimSequence)"),
						AnimName, *Obj->GetClass()->GetName());
				}
			}

			if (!bFound)
			{
				if (ImportedObjects.Num() == 0)
					UE_LOG(LogTemp, Error, TEXT("  %s: returned 0 objects"), AnimName);
				AnimFail++;
			}

			ImportTask->RemoveFromRoot();
		}

		UE_LOG(LogTemp, Warning, TEXT("  Animation import: %d succeeded, %d failed"),
			AnimSuccess, AnimFail);
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	else
	{
		// ─── Original mode: IK Retarget from c3100 guard animations ───
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1: IK Retarget c3100 → Sentinel ---"));

		struct FAnimMapping { const TCHAR* SourceAnim; const TCHAR* SentinelName; };
		const FAnimMapping AnimMappings[] = {
			{ TEXT("a000_000020"), TEXT("Sentinel_Idle") },
			{ TEXT("a000_002000"), TEXT("Sentinel_Walk") },
			{ TEXT("a000_002100"), TEXT("Sentinel_Run") },
			{ TEXT("a000_003000"), TEXT("Sentinel_Attack01") },
			{ TEXT("a000_003001"), TEXT("Sentinel_Attack02") },
			{ TEXT("a000_003017"), TEXT("Sentinel_Attack03_Fast") },
			{ TEXT("a000_004100"), TEXT("Sentinel_HeavyAttack") },
			{ TEXT("a000_010000"), TEXT("Sentinel_HitReact") },
			{ TEXT("a000_010001"), TEXT("Sentinel_HitReact_Light") },
			{ TEXT("a000_008030"), TEXT("Sentinel_GuardHit") },
			{ TEXT("a000_011010"), TEXT("Sentinel_Death_Front") },
			{ TEXT("a000_011060"), TEXT("Sentinel_Death_Back") },
			{ TEXT("a000_011070"), TEXT("Sentinel_Death_Left") },
			{ TEXT("a000_005000"), TEXT("Sentinel_Dodge_Fwd") },
			{ TEXT("a000_005001"), TEXT("Sentinel_Dodge_Bwd") },
			{ TEXT("a000_005002"), TEXT("Sentinel_Dodge_Left") },
			{ TEXT("a000_005003"), TEXT("Sentinel_Dodge_Right") },
			{ TEXT("a000_007000"), TEXT("Sentinel_Guard") },
			{ TEXT("a000_009210"), TEXT("Sentinel_PoiseBreak_Start") },
			{ TEXT("a000_009200"), TEXT("Sentinel_PoiseBreak_Loop") },
		};

		TArray<FString> SourceAnimPaths;
		for (const auto& M : AnimMappings)
		{
			SourceAnimPaths.Add(SourceAnimDir / M.SourceAnim);
		}

		const FString C3100MeshPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh");
		const FString SentinelMeshPath = SentinelDir / TEXT("SKM_Sentinel");

		Result = USLFAutomationLibrary::RetargetAnimationsViaIKRig(
			C3100MeshPath,
			SentinelMeshPath,
			SourceAnimPaths,
			SentinelAnimDir,
			FName(TEXT("Pelvis")),
			FName(TEXT("Hips")),
			TEXT("/Game/Temp/IKRetarget_Sentinel")
		);
		{
			TArray<FString> ResultLines;
			Result.ParseIntoArrayLines(ResultLines);
			for (const FString& Line : ResultLines)
			{
				UE_LOG(LogTemp, Warning, TEXT("[IKRetarget] %s"), *Line);
			}
		}

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

		// Step 1b: Rename retargeted animations from c3100 names to Sentinel names
		UE_LOG(LogTemp, Warning, TEXT("--- Step 1b: Renaming retargeted animations ---"));
		for (const auto& M : AnimMappings)
		{
			FString SrcName = FString(M.SourceAnim);
			FString DstName = FString(M.SentinelName);
			FString SrcPath = SentinelAnimDir / SrcName;
			FString DstPath = SentinelAnimDir / DstName;

			UAnimSequence* ExistingDst = LoadObject<UAnimSequence>(nullptr, *DstPath);
			if (ExistingDst)
			{
				UE_LOG(LogTemp, Warning, TEXT("  %s already exists"), *DstName);
				continue;
			}

			UAnimSequence* SrcAnim = LoadObject<UAnimSequence>(nullptr, *SrcPath);
			if (!SrcAnim)
			{
				UE_LOG(LogTemp, Warning, TEXT("  %s not found (retarget may have used different name)"), *SrcName);
				continue;
			}

			UPackage* NewPkg = CreatePackage(*DstPath);
			NewPkg->FullyLoad();
			bool bRenamed = SrcAnim->Rename(*DstName, NewPkg, REN_DontCreateRedirectors | REN_NonTransactional);
			if (bRenamed)
			{
				FAssetRegistryModule::AssetCreated(SrcAnim);
				NewPkg->MarkPackageDirty();
				FString Fn = FPackageName::LongPackageNameToFilename(NewPkg->GetName(), FPackageName::GetAssetPackageExtension());
				FSavePackageArgs SA;
				SA.TopLevelFlags = RF_Standalone;
				UPackage::SavePackage(NewPkg, SrcAnim, *Fn, SA);
				UE_LOG(LogTemp, Warning, TEXT("  Renamed: %s → %s"), *SrcName, *DstName);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  FAILED to rename: %s → %s"), *SrcName, *DstName);
			}
		}
	}

	// Step 1c: Verify expected animations exist
	int32 AnimFoundCount = 0;
	for (const TCHAR* AnimName : ExpectedAnims)
	{
		UAnimSequence* TestAnim = LoadObject<UAnimSequence>(nullptr, *(SentinelAnimDir / AnimName));
		if (TestAnim)
		{
			AnimFoundCount++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Missing animation: %s"), AnimName);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  Found %d/%d expected animations"), AnimFoundCount, (int32)UE_ARRAY_COUNT(ExpectedAnims));
	UE_LOG(LogTemp, Warning, TEXT("--- Step 1c: Forensic transforms SKIPPED (GetBoneTransform returns identity in commandlet) ---"));

	// ═══════════════════════════════════════════════════════════════════════
	// Step 2: Add sockets to skeleton
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 2: Adding sockets ---"));

	// Socket bone names differ between ARP skeleton (PascalCase) and Mannequin (snake_case).
	// For mesh-baked weapons: weapon_start on hand bone, weapon_end on weapon bone.
	// The weapon_r bone tracks the actual weapon position in the mesh, so the trace
	// naturally follows the weapon during animations — no axis guessing needed.
	struct FSocketInfo { const TCHAR* Name; const TCHAR* Bone; FVector Offset; };
	const FSocketInfo ForensicSockets[] = {
		{ TEXT("weapon_start"), TEXT("hand_r"),   FVector::ZeroVector },
		{ TEXT("weapon_end"),   TEXT("weapon_r"), FVector::ZeroVector },
		{ TEXT("hand_r"),       TEXT("hand_r"),   FVector::ZeroVector },
		{ TEXT("hand_l"),       TEXT("hand_l"),   FVector::ZeroVector },
		{ TEXT("foot_l"),       TEXT("foot_l"),   FVector::ZeroVector },
		{ TEXT("foot_r"),       TEXT("foot_r"),   FVector::ZeroVector },
	};
	const FSocketInfo ARPSockets[] = {
		{ TEXT("weapon_start"), TEXT("Hand_R"), FVector(0, 0, 50) },
		{ TEXT("weapon_end"),   TEXT("Hand_R"), FVector(0, 0, -50) },
		{ TEXT("hand_r"),       TEXT("Hand_R"),  FVector::ZeroVector },
		{ TEXT("hand_l"),       TEXT("Hand_L"),  FVector::ZeroVector },
		{ TEXT("foot_l"),       TEXT("Foot_L"),  FVector::ZeroVector },
		{ TEXT("foot_r"),       TEXT("Foot_R"),  FVector::ZeroVector },
	};
	const FSocketInfo* Sockets = bForensic ? ForensicSockets : ARPSockets;
	const int32 NumSockets = UE_ARRAY_COUNT(ForensicSockets); // Both arrays are same size
	for (int32 i = 0; i < NumSockets; i++)
	{
		Result = USLFAutomationLibrary::AddSocketToSkeleton(SentinelSkeletonPath, Sockets[i].Name, Sockets[i].Bone, Sockets[i].Offset);
		UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
	}

	// Save skeleton
	{
		if (SentinelSkeleton)
		{
			UPackage* Pkg = SentinelSkeleton->GetOutermost();
			Pkg->MarkPackageDirty();
			FString PackageFileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Standalone;
			bool bSaved = UPackage::SavePackage(Pkg, SentinelSkeleton, *PackageFileName, SaveArgs);
			UE_LOG(LogTemp, Warning, TEXT("  Skeleton save: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// Step 3: Create montages from baked animations
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 3: Creating montages ---"));

	struct FMontageInfo { const TCHAR* AnimName; const TCHAR* MontageName; };

	const FMontageInfo AttackMontages[] = {
		{ TEXT("Sentinel_Attack01"),      TEXT("AM_Sentinel_Attack01") },
		{ TEXT("Sentinel_Attack02"),      TEXT("AM_Sentinel_Attack02") },
		{ TEXT("Sentinel_Attack03_Fast"), TEXT("AM_Sentinel_Attack03_Fast") },
		{ TEXT("Sentinel_HeavyAttack"),   TEXT("AM_Sentinel_HeavyAttack") },
	};

	const FMontageInfo ReactionMontages[] = {
		{ TEXT("Sentinel_HitReact"),       TEXT("AM_Sentinel_HitReact") },
		{ TEXT("Sentinel_HitReact_Light"), TEXT("AM_Sentinel_HitReact_Light") },
		{ TEXT("Sentinel_GuardHit"),       TEXT("AM_Sentinel_GuardHit") },
	};

	const FMontageInfo DeathMontages[] = {
		{ TEXT("Sentinel_Death_Front"), TEXT("AM_Sentinel_Death_Front") },
		{ TEXT("Sentinel_Death_Back"),  TEXT("AM_Sentinel_Death_Back") },
		{ TEXT("Sentinel_Death_Left"),  TEXT("AM_Sentinel_Death_Left") },
	};

	const FMontageInfo DodgeMontages[] = {
		{ TEXT("Sentinel_Dodge_Fwd"),   TEXT("AM_Sentinel_Dodge_Fwd") },
		{ TEXT("Sentinel_Dodge_Bwd"),   TEXT("AM_Sentinel_Dodge_Bwd") },
		{ TEXT("Sentinel_Dodge_Left"),  TEXT("AM_Sentinel_Dodge_Left") },
		{ TEXT("Sentinel_Dodge_Right"), TEXT("AM_Sentinel_Dodge_Right") },
	};

	const FMontageInfo LocomotionMontages[] = {
		{ TEXT("Sentinel_Idle"), TEXT("AM_Sentinel_Idle") },
		{ TEXT("Sentinel_Walk"), TEXT("AM_Sentinel_Walk") },
		{ TEXT("Sentinel_Run"),  TEXT("AM_Sentinel_Run") },
	};

	auto CreateMontages = [&](const TCHAR* Category, const FMontageInfo* Montages, int32 Count)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Creating %s montages (%d)..."), Category, Count);
		for (int32 i = 0; i < Count; i++)
		{
			FString SeqPath = SentinelAnimDir / Montages[i].AnimName;
			FString OutPath = SentinelDir / Montages[i].MontageName;
			Result = USLFAutomationLibrary::CreateMontageFromSequence(SeqPath, OutPath, TEXT("DefaultSlot"));
			UE_LOG(LogTemp, Warning, TEXT("    %s"), *Result);
		}
	};

	CreateMontages(TEXT("Attack"), AttackMontages, UE_ARRAY_COUNT(AttackMontages));
	CreateMontages(TEXT("Reaction"), ReactionMontages, UE_ARRAY_COUNT(ReactionMontages));
	CreateMontages(TEXT("Death"), DeathMontages, UE_ARRAY_COUNT(DeathMontages));
	CreateMontages(TEXT("Dodge"), DodgeMontages, UE_ARRAY_COUNT(DodgeMontages));
	CreateMontages(TEXT("Locomotion"), LocomotionMontages, UE_ARRAY_COUNT(LocomotionMontages));

	// ═══════════════════════════════════════════════════════════════════════
	// Step 4: Create 2D blend space
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 4: Creating 2D blend space ---"));
	Result = USLFAutomationLibrary::CreateBlendSpace2DFromSequences(
		SentinelDir / TEXT("BS_Sentinel_Locomotion"),
		SentinelSkeletonPath,
		SentinelAnimDir / TEXT("Sentinel_Idle"),
		SentinelAnimDir / TEXT("Sentinel_Walk"),
		SentinelAnimDir / TEXT("Sentinel_Run")
	);
	UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);

	// ═══════════════════════════════════════════════════════════════════════
	// Step 5: Duplicate AnimBP and retarget
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 5: Duplicating AnimBP ---"));
	Result = USLFAutomationLibrary::DuplicateAnimBPForSkeleton(
		TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"),
		SentinelSkeletonPath,
		SentinelDir / TEXT("ABP_Sentinel")
	);
	UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);

	// Step 5b: Replace animation references in AnimBP
	UE_LOG(LogTemp, Warning, TEXT("--- Step 5b: Replacing AnimGraph references ---"));
	{
		TMap<FString, FString> AnimReplacementMap;
		AnimReplacementMap.Add(
			TEXT("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Blendspaces/ABS_SoulslikeEnemy"),
			SentinelDir / TEXT("BS_Sentinel_Locomotion"));
		AnimReplacementMap.Add(
			TEXT("/Game/SoulslikeFramework/Demo/RetargetedParagonAnimations/Kwang/AS_RT_Idle_Kwang"),
			SentinelAnimDir / TEXT("Sentinel_Idle"));

		Result = USLFAutomationLibrary::ReplaceAnimReferencesInAnimBP(
			SentinelDir / TEXT("ABP_Sentinel"),
			AnimReplacementMap);
		UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
	}

	// Step 5c: Remove Control Rig + IK Rig nodes
	UE_LOG(LogTemp, Warning, TEXT("--- Step 5c: Removing Control Rig + IK Rig ---"));
	Result = USLFAutomationLibrary::DisableControlRigInAnimBP(SentinelDir / TEXT("ABP_Sentinel"));
	UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);

	// ═══════════════════════════════════════════════════════════════════════
	// Step 6: Create data assets
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6: Creating data assets ---"));

	auto CreateAndSaveDataAsset = [](UObject* Asset, UPackage* Pkg, const FString& AssetPath) -> bool
	{
		FAssetRegistryModule::AssetCreated(Asset);
		Pkg->MarkPackageDirty();
		FString FileName = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		return UPackage::SavePackage(Pkg, Asset, *FileName, SaveArgs);
	};

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

	// 6a: PDA_Sentinel_AnimData
	{
		FString PkgName = SentinelDir / TEXT("PDA_Sentinel_AnimData");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_Sentinel_AnimData"));
		UPDA_AnimData* AnimData = NewObject<UPDA_AnimData>(Pkg, TEXT("PDA_Sentinel_AnimData"), RF_Public | RF_Standalone);

		AnimData->WalkRunLocomotion = LoadObject<UBlendSpace>(nullptr, *(SentinelDir / TEXT("BS_Sentinel_Locomotion")));
		AnimData->Idle = LoadObject<UAnimSequenceBase>(nullptr, *(SentinelAnimDir / TEXT("Sentinel_Idle")));

		bool bSaved = CreateAndSaveDataAsset(AnimData, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_Sentinel_AnimData: %s (BS=%s, Idle=%s)"),
			bSaved ? TEXT("SAVED") : TEXT("FAILED"),
			AnimData->WalkRunLocomotion ? TEXT("yes") : TEXT("no"),
			AnimData->Idle ? TEXT("yes") : TEXT("no"));
	}

	// 6b: PDA_Sentinel_WeaponAnimset
	{
		FString PkgName = SentinelDir / TEXT("PDA_Sentinel_WeaponAnimset");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_Sentinel_WeaponAnimset"));
		UPDA_WeaponAnimset* WeaponAnimset = NewObject<UPDA_WeaponAnimset>(Pkg, TEXT("PDA_Sentinel_WeaponAnimset"), RF_Public | RF_Standalone);

		WeaponAnimset->OneH_LightComboMontage_R = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_Attack01")));
		WeaponAnimset->OneH_HeavyComboMontage_R = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_HeavyAttack")));
		WeaponAnimset->Guard_R = LoadObject<UAnimSequenceBase>(nullptr, *(SentinelAnimDir / TEXT("Sentinel_Guard")));
		WeaponAnimset->Guard_R_Hit = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_GuardHit")));

		bool bSaved = CreateAndSaveDataAsset(WeaponAnimset, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_Sentinel_WeaponAnimset: %s"), bSaved ? TEXT("SAVED") : TEXT("FAILED"));
	}

	// 6c: PDA_Sentinel_CombatReaction
	{
		FString PkgName = SentinelDir / TEXT("PDA_Sentinel_CombatReaction");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_Sentinel_CombatReaction"));
		UPDA_CombatReactionAnimData* Reaction = NewObject<UPDA_CombatReactionAnimData>(Pkg, TEXT("PDA_Sentinel_CombatReaction"), RF_Public | RF_Standalone);

		Reaction->ReactionMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_HitReact")));
		Reaction->Death.Add(ESLFDirection::Fwd, LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_Death_Front"))));
		Reaction->Death.Add(ESLFDirection::Bwd, LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_Death_Back"))));
		Reaction->Death.Add(ESLFDirection::Left, LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_Death_Left"))));
		Reaction->Death.Add(ESLFDirection::Right, LoadObject<UAnimMontage>(nullptr, *(SentinelDir / TEXT("AM_Sentinel_Death_Front"))));

		bool bSaved = CreateAndSaveDataAsset(Reaction, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_Sentinel_CombatReaction: %s"), bSaved ? TEXT("SAVED") : TEXT("FAILED"));
	}

	// 6d: PDA_Sentinel_PoiseBreak
	{
		FString PkgName = SentinelDir / TEXT("PDA_Sentinel_PoiseBreak");
		UPackage* Pkg = PreparePackage(PkgName, TEXT("PDA_Sentinel_PoiseBreak"));
		UPDA_PoiseBreakAnimData* PoiseBreak = NewObject<UPDA_PoiseBreakAnimData>(Pkg, TEXT("PDA_Sentinel_PoiseBreak"), RF_Public | RF_Standalone);

		PoiseBreak->PoiseBreak_Start = LoadObject<UAnimSequence>(nullptr, *(SentinelAnimDir / TEXT("Sentinel_PoiseBreak_Start")));
		PoiseBreak->PoiseBreak_Loop = LoadObject<UAnimSequence>(nullptr, *(SentinelAnimDir / TEXT("Sentinel_PoiseBreak_Loop")));

		bool bSaved = CreateAndSaveDataAsset(PoiseBreak, Pkg, PkgName);
		UE_LOG(LogTemp, Warning, TEXT("  PDA_Sentinel_PoiseBreak: %s (Start=%s, Loop=%s)"),
			bSaved ? TEXT("SAVED") : TEXT("FAILED"),
			PoiseBreak->PoiseBreak_Start ? TEXT("yes") : TEXT("no"),
			PoiseBreak->PoiseBreak_Loop ? TEXT("yes") : TEXT("no"));
	}

	// ═══════════════════════════════════════════════════════════════════════
	// Step 6b: Create AI Ability data assets
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6b: Creating AI Ability data assets ---"));

	struct FAIAbilityDef { const TCHAR* Name; const TCHAR* MontageName; double Score; double Cooldown; };
	const FAIAbilityDef AbilityDefs[] = {
		{ TEXT("DA_Sentinel_Attack01"),      TEXT("AM_Sentinel_Attack01"),      1.0, 2.0 },
		{ TEXT("DA_Sentinel_Attack02"),      TEXT("AM_Sentinel_Attack02"),      1.0, 2.0 },
		{ TEXT("DA_Sentinel_Attack03_Fast"), TEXT("AM_Sentinel_Attack03_Fast"), 1.5, 1.0 },
		{ TEXT("DA_Sentinel_HeavyAttack"),   TEXT("AM_Sentinel_HeavyAttack"),  0.5, 5.0 },
	};

	for (const auto& Def : AbilityDefs)
	{
		FString PkgName = SentinelDir / Def.Name;
		UPackage* Pkg = PreparePackage(PkgName, Def.Name);
		UPDA_AI_Ability* AIAbility = NewObject<UPDA_AI_Ability>(Pkg, Def.Name, RF_Public | RF_Standalone);

		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / Def.MontageName));
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

	// ═══════════════════════════════════════════════════════════════════════
	// Step 6c: Add weapon trace notifies to attack montages
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6c: Adding weapon trace notifies ---"));
	{
		// Directional reach mode: trace from weapon_start along socket axis for
		// WeaponReach cm. This extends the hitbox from the grip to the blade tip.
		// weapon_r bone is at the hilt, NOT the blade tip, so two-socket mode only
		// covered ~20-30cm near the hand. Debug draw ON for visual tuning.
		struct FWeaponTraceConfig
		{
			const TCHAR* MontageName;
			float StartTime;
			float EndTime;
			float TraceRadius;
			float WeaponReach;   // Reach in cm from socket along axis (0 = two-socket mode)
			float Damage;        // Per-attack damage (-1 = default 50)
			float PoiseDamage;   // Per-attack poise damage (-1 = default 25)
		};

		const FWeaponTraceConfig TraceConfigs[] = {
			//                                     Start   End    Radius  Reach   Dmg    Poise
			{ TEXT("AM_Sentinel_Attack01"),         1.167f, 1.517f, 60.0f, 200.0f, 60.0f, 30.0f },
			{ TEXT("AM_Sentinel_Attack02"),         0.700f, 1.083f, 60.0f, 200.0f, 60.0f, 30.0f },
			{ TEXT("AM_Sentinel_Attack03_Fast"),    0.700f, 1.083f, 50.0f, 180.0f, 45.0f, 20.0f },
			{ TEXT("AM_Sentinel_HeavyAttack"),      2.200f, 2.750f, 90.0f, 250.0f, 100.0f, 60.0f },
		};

		for (const auto& Cfg : TraceConfigs)
		{
			Result = USLFAutomationLibrary::AddWeaponTraceToMontage(
				SentinelDir / Cfg.MontageName,
				Cfg.StartTime,
				Cfg.EndTime,
				Cfg.TraceRadius,
				FName("weapon_start"),	// Socket on hand_r bone (grip origin)
				FName("weapon_end"),	// Ignored in directional reach mode
				Cfg.WeaponReach,		// Reach from grip along weapon direction
				EAxis::X,				// Fallback axis (unused when DirectionBone set)
				false,					// Don't negate (direction is elbow->hand->blade)
				Cfg.Damage,				// Per-attack damage
				Cfg.PoiseDamage,		// Per-attack poise damage
				true,					// Debug draw ON for tuning
				FName("lowerarm_r")		// Direction: elbow -> hand -> blade tip
			);
			UE_LOG(LogTemp, Warning, TEXT("  %s"), *Result);
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// Step 6d: Bone scale overrides (DISABLED — global-space anim import should fix stretching)
	// ═══════════════════════════════════════════════════════════════════════
	// Previously needed because ImportAnimFBXDirect converted LOCAL transforms individually
	// then chained them (Y-axis reflection doesn't distribute over quaternion multiplication).
	// Now uses GLOBAL transform import which matches UE5's mesh import exactly.
	// Re-enable with AddBoneScaleOverrides() if arm proportions still need tuning.
	UE_LOG(LogTemp, Warning, TEXT("--- Step 6d: Bone scale overrides SKIPPED (global-space import should fix stretching) ---"));

	// ═══════════════════════════════════════════════════════════════════════
	// Step 7: Validation
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 7: Validation ---"));
	{
		// AnimBP diagnostic
		FString DiagResult = USLFAutomationLibrary::DiagnoseAnimBPDetailed(SentinelDir / TEXT("ABP_Sentinel"));
		TArray<FString> DiagLines;
		DiagResult.ParseIntoArrayLines(DiagLines);
		for (const FString& Line : DiagLines)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DiagAnimBP] %s"), *Line);
		}

		// Verify montage lengths
		UE_LOG(LogTemp, Warning, TEXT("--- Verifying montage lengths ---"));
		const TCHAR* MontagesToVerify[] = {
			TEXT("AM_Sentinel_Attack01"), TEXT("AM_Sentinel_Attack02"), TEXT("AM_Sentinel_HeavyAttack"),
			TEXT("AM_Sentinel_HitReact"), TEXT("AM_Sentinel_Idle"), TEXT("AM_Sentinel_Walk"), TEXT("AM_Sentinel_Run"),
		};
		for (const TCHAR* MName : MontagesToVerify)
		{
			UAnimMontage* TestMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir / MName));
			if (TestMontage)
			{
				float Len = TestMontage->GetPlayLength();
				UE_LOG(LogTemp, Warning, TEXT("  %s: %.3f sec %s"),
					MName, Len, Len > 0.01f ? TEXT("OK") : TEXT("*** ZERO LENGTH ***"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  %s: FAILED TO LOAD"), MName);
			}
		}

		// Verify baked animations have distinct transforms from source
		UE_LOG(LogTemp, Warning, TEXT("--- Forensic comparison: Sentinel vs Source ---"));
		{
			UAnimSequence* SourceIdle = LoadObject<UAnimSequence>(nullptr, *(SourceAnimDir / TEXT("a000_000020")));
			UAnimSequence* SentinelIdle = LoadObject<UAnimSequence>(nullptr, *(SentinelAnimDir / TEXT("Sentinel_Idle")));

			if (SourceIdle && SentinelIdle)
			{
				// Compare bone transforms at frame 0
				USkeleton* Skel = SourceIdle->GetSkeleton();
				if (Skel)
				{
					int32 NumBones = Skel->GetReferenceSkeleton().GetNum();
					int32 DifferentBones = 0;
					float MaxRotDiff = 0.0f;

					for (int32 i = 0; i < NumBones; i++)
					{
						FTransform SourceT, SentinelT;
						FAnimExtractContext CtxComp(0.0);
						SourceIdle->GetBoneTransform(SourceT, FSkeletonPoseBoneIndex(i), CtxComp, false);
						SentinelIdle->GetBoneTransform(SentinelT, FSkeletonPoseBoneIndex(i), CtxComp, false);

						float RotDiff = SourceT.GetRotation().AngularDistance(SentinelT.GetRotation());
						RotDiff = FMath::RadiansToDegrees(RotDiff);
						MaxRotDiff = FMath::Max(MaxRotDiff, RotDiff);

						if (RotDiff > 0.1f)
							DifferentBones++;
					}

					UE_LOG(LogTemp, Warning, TEXT("  Idle comparison: %d/%d bones differ, max rotation diff: %.2f degrees"),
						DifferentBones, NumBones, MaxRotDiff);
					UE_LOG(LogTemp, Warning, TEXT("  Source FPS: %d keys, Sentinel FPS: %d keys"),
						SourceIdle->GetNumberOfSampledKeys(), SentinelIdle->GetNumberOfSampledKeys());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Could not compare - source=%s sentinel=%s"),
					SourceIdle ? TEXT("found") : TEXT("missing"),
					SentinelIdle ? TEXT("found") : TEXT("missing"));
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// Step 8: Create B_Soulslike_Enemy_Sentinel Blueprint
	// ═══════════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Warning, TEXT("--- Step 8: Create Sentinel Blueprint ---"));
	{
		FString BPPath = TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel");
		UBlueprint* ExistingBP = LoadObject<UBlueprint>(nullptr, *BPPath);
		if (ExistingBP)
		{
			UE_LOG(LogTemp, Warning, TEXT("  B_Soulslike_Enemy_Sentinel already exists, reparenting to ASLFEnemySentinel"));
			// Ensure correct parent
			UClass* SentinelClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.SLFEnemySentinel"));
			if (SentinelClass && ExistingBP->ParentClass != SentinelClass)
			{
				ExistingBP->ParentClass = SentinelClass;
				FKismetEditorUtilities::CompileBlueprint(ExistingBP, EBlueprintCompileOptions::SkipGarbageCollection);
				UE_LOG(LogTemp, Warning, TEXT("  Reparented to ASLFEnemySentinel"));
			}
		}
		else
		{
			// Create new Blueprint from ASLFEnemySentinel
			UClass* SentinelClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.SLFEnemySentinel"));
			if (!SentinelClass)
			{
				SentinelClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Script/SLFConversion.SLFEnemySentinel"));
			}
			if (SentinelClass)
			{
				FString PackagePath = TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies");
				FString AssetName = TEXT("B_Soulslike_Enemy_Sentinel");
				UPackage* Package = CreatePackage(*(PackagePath / AssetName));
				if (Package)
				{
					UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
						SentinelClass,
						Package,
						FName(*AssetName),
						BPTYPE_Normal,
						UBlueprint::StaticClass(),
						UBlueprintGeneratedClass::StaticClass()
					);
					if (NewBP)
					{
						FKismetEditorUtilities::CompileBlueprint(NewBP, EBlueprintCompileOptions::SkipGarbageCollection);

						// Save
						FSavePackageArgs SaveArgs;
						SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
						FString PackageFilename = FPackageName::LongPackageNameToFilename(
							Package->GetName(), FPackageName::GetAssetPackageExtension());
						UPackage::SavePackage(Package, NewBP, *PackageFilename, SaveArgs);

						UE_LOG(LogTemp, Warning, TEXT("  Created B_Soulslike_Enemy_Sentinel (parent: ASLFEnemySentinel)"));
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("  FAILED to create Blueprint!"));
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("  Could not find ASLFEnemySentinel class!"));
			}
		}
	}

	// Step 9: Level placement.
	// Programmatic actor placement from commandlets corrupts existing level actors.
	// The Sentinel auto-spawns itself in the demo level at game start.
	// It can also be dragged from Content Browser into the viewport:
	//   /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel

	UE_LOG(LogTemp, Warning, TEXT("=== SetupSentinel Commandlet: COMPLETE ==="));
	return 0;
}

#else // !WITH_EDITOR

USetupSentinelCommandlet::USetupSentinelCommandlet() {}
int32 USetupSentinelCommandlet::Main(const FString& Params) { return 0; }

#endif // WITH_EDITOR
