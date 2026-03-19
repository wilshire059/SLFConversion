#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "IKRetargetMultiSourceCommandlet.generated.h"

/**
 * Import multi-source enemy animations and retarget to c3100 via UE5 IK Retargeter.
 *
 * Usage:
 *   UnrealEditor-Cmd.exe SLFConversion.uproject -run=IKRetargetMultiSource -unattended -nosplash
 *
 * Reads raw FBX from C:/scripts/elden_ring_tools/output/multisource_raw/<chr_id>/
 * Outputs retargeted AnimSequences to /Game/Temp/IKRetarget/<chr_id>/
 * Dumps JSON to C:/scripts/elden_ring_tools/output/multisource_raw/ik_retarget_dump.json
 */
UCLASS()
class UIKRetargetMultiSourceCommandlet : public UCommandlet
{
	GENERATED_BODY()
public:
	UIKRetargetMultiSourceCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	struct FSourceEnemy
	{
		FString ChrId;
		TArray<FString> AnimFbxPaths;  // Filesystem paths to animation FBX files
		FString MeshFbxPath;           // Filesystem path to mesh FBX
	};

	struct FAnimMapping
	{
		FString SentinelName;  // e.g. "Sentinel_Attack01"
		FString ChrId;         // e.g. "c3560"
	};

	USkeletalMesh* ImportMeshFBX(const FString& FbxPath, const FString& PackageDir, const FString& AssetName);
	UAnimSequence* ImportAnimFBX(const FString& FbxPath, const FString& PackageDir, const FString& AssetName, USkeleton* TargetSkeleton);

	class UIKRigDefinition* CreateIKRigForERSkeleton(USkeletalMesh* Mesh, const FString& PackageDir, const FString& Name);
	class UIKRetargeter* CreateRetargeter(
		UIKRigDefinition* SourceRig, UIKRigDefinition* TargetRig,
		USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh,
		const FString& PackageDir, const FString& Name);

	void DumpRetargetedAnimsToJSON(
		const TArray<UAnimSequence*>& Anims,
		const TArray<FString>& AnimNames,
		const FString& JsonPath);

	bool SaveAsset(UObject* Asset, const FString& PackagePath);
	void DeleteExistingAsset(const FString& PackagePath);

	static constexpr const TCHAR* RawFbxDir = TEXT("C:/scripts/elden_ring_tools/output/multisource_raw");
	static constexpr const TCHAR* TempContentDir = TEXT("/Game/Temp/IKRetarget");
	static constexpr const TCHAR* C3100MeshPath = TEXT("/Game/EldenRingAnimations/c3100_guard/c3100_mesh");
};
