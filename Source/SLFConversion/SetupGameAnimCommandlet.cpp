// SetupGameAnimCommandlet.cpp
// Copies animations from GameAnimationSample preserving original folder structure
// so skeleton references resolve. Then creates montages.

#include "SetupGameAnimCommandlet.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

int32 USetupGameAnimCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Log, TEXT("=== SetupGameAnimCommandlet START ==="));

	SourceContentDir = TEXT("C:/scripts/GameAnimationSample/Content/");
	DestContentDir = FPaths::ProjectContentDir();

	UE_LOG(LogTemp, Log, TEXT("Source: %s"), *SourceContentDir);
	UE_LOG(LogTemp, Log, TEXT("Dest:   %s"), *DestContentDir);

	// Create montages output folder
	IFileManager& FM = IFileManager::Get();
	FM.MakeDirectory(*(DestContentDir / TEXT("GameAnimations/Montages")), true);

	// Phase 1: Copy skeleton + mesh (preserving original path so anim refs resolve)
	UE_LOG(LogTemp, Log, TEXT("--- Phase 1: Copy skeleton ---"));
	CopyAnimAsset(TEXT("Characters/UEFN_Mannequin/Meshes/SK_UEFN_Mannequin.uasset"),
		TEXT("Characters/UEFN_Mannequin/Meshes"));

	// Phase 2: Copy all needed animations (preserving original paths)
	SetupRestingPointAnims();
	SetupSlideAnims();
	SetupTraversalAnims();
	SetupLandingAnims();
	SetupAvoidanceAnims();

	// Phase 3: Create montages (done after all sequences are copied)
	// This must be done in a second pass after the editor restarts with the copied assets
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("=== FILES COPIED SUCCESSFULLY ==="));
	UE_LOG(LogTemp, Log, TEXT("Next step: Run -run=SetupGameAnim -montages to create montages"));
	UE_LOG(LogTemp, Log, TEXT("(Assets must be loaded by the engine first)"));

	if (Params.Contains(TEXT("-montages")))
	{
		UE_LOG(LogTemp, Log, TEXT("--- Phase 3: Creating montages ---"));
		CreateMontageBatch();
	}

	UE_LOG(LogTemp, Log, TEXT("=== SetupGameAnimCommandlet COMPLETE ==="));
	return 0;
}

bool USetupGameAnimCommandlet::CopyAnimAsset(const FString& RelativePath, const FString& DestFolder)
{
	FString SourceFile = SourceContentDir / RelativePath;
	FString FileName = FPaths::GetCleanFilename(RelativePath);
	FString DestDir = DestContentDir / DestFolder;
	FString DestFile = DestDir / FileName;

	// Ensure destination directory exists
	IFileManager::Get().MakeDirectory(*DestDir, true);

	if (!IFileManager::Get().FileExists(*SourceFile))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Source not found: %s"), *SourceFile);
		return false;
	}

	if (IFileManager::Get().FileExists(*DestFile))
	{
		UE_LOG(LogTemp, Log, TEXT("  Already exists: %s"), *FileName);
		return true;
	}

	uint32 Result = IFileManager::Get().Copy(*DestFile, *SourceFile);
	if (Result == COPY_OK)
	{
		UE_LOG(LogTemp, Log, TEXT("  Copied: %s"), *FileName);
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("  FAILED to copy: %s"), *RelativePath);
	return false;
}

UAnimMontage* USetupGameAnimCommandlet::CreateMontageFromSequence(const FString& SequencePath, const FString& MontageName, const FString& SaveFolder)
{
	UAnimSequence* Sequence = LoadObject<UAnimSequence>(nullptr, *SequencePath);
	if (!Sequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not load sequence: %s"), *SequencePath);
		return nullptr;
	}

	// Delete existing montage file first (Bug #21)
	FString MontagePackagePath = FString::Printf(TEXT("/Game/GameAnimations/Montages/%s"), *MontageName);
	FString MontageDiskPath = FPackageName::LongPackageNameToFilename(MontagePackagePath, FPackageName::GetAssetPackageExtension());
	if (IFileManager::Get().FileExists(*MontageDiskPath))
	{
		IFileManager::Get().Delete(*MontageDiskPath);
		UE_LOG(LogTemp, Log, TEXT("  Deleted existing: %s"), *MontageDiskPath);
	}

	UPackage* Package = CreatePackage(*MontagePackagePath);
	if (!Package) return nullptr;

	UAnimMontage* Montage = NewObject<UAnimMontage>(Package, *MontageName, RF_Public | RF_Standalone);
	if (!Montage) return nullptr;

	USkeleton* Skeleton = Sequence->GetSkeleton();
	if (Skeleton)
	{
		Montage->SetSkeleton(Skeleton);
	}

	// Set up slot track with animation segment
	FSlotAnimationTrack& SlotTrack = Montage->SlotAnimTracks[0];
	SlotTrack.SlotName = FName("DefaultSlot");

	FAnimSegment Segment;
	Segment.SetAnimReference(Sequence);
	Segment.AnimStartTime = 0.0f;
	Segment.AnimEndTime = Sequence->GetPlayLength();
	Segment.AnimPlayRate = 1.0f;
	Segment.StartPos = 0.0f;
	Segment.LoopingCount = 1;
	SlotTrack.AnimTrack.AnimSegments.Add(Segment);

	float Length = Sequence->GetPlayLength();
	Montage->SetCompositeLength(Length);

	if (Montage->CompositeSections.Num() == 0)
	{
		FCompositeSection DefaultSection;
		DefaultSection.SectionName = FName("Default");
		DefaultSection.SetTime(0.0f);
		Montage->CompositeSections.Add(DefaultSection);
	}

	Montage->BlendIn.SetBlendTime(0.15f);
	Montage->BlendOut.SetBlendTime(0.25f);

	Package->MarkPackageDirty();
	FString PackageFilename = FPackageName::LongPackageNameToFilename(MontagePackagePath, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	UPackage::SavePackage(Package, Montage, *PackageFilename, SaveArgs);

	UE_LOG(LogTemp, Log, TEXT("  Created montage: %s (%.2fs) from %s"), *MontageName, Length, *SequencePath);
	return Montage;
}

void USetupGameAnimCommandlet::SetupRestingPointAnims()
{
	UE_LOG(LogTemp, Log, TEXT("--- Copying Resting Point (bench) animations ---"));

	const FString BenchSrc = TEXT("Characters/UEFN_Mannequin/Animations/Interactions/Bench/");
	const FString BenchDst = TEXT("Characters/UEFN_Mannequin/Animations/Interactions/Bench");

	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_into_F_Lfoot.uasset"), BenchDst);
	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_idle_loop.uasset"), BenchDst);
	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_out_to_stand.uasset"), BenchDst);
	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_out_to_walk.uasset"), BenchDst);
	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_into_L_090_Lfoot.uasset"), BenchDst);
	CopyAnimAsset(BenchSrc + TEXT("M_interaction_bench_into_R_090_Rfoot.uasset"), BenchDst);
}

void USetupGameAnimCommandlet::SetupSlideAnims()
{
	UE_LOG(LogTemp, Log, TEXT("--- Copying Slide animations ---"));

	const FString SlideSrc = TEXT("Characters/UEFN_Mannequin/Animations/Slide/");
	const FString SlideDst = TEXT("Characters/UEFN_Mannequin/Animations/Slide");

	TArray<FString> SlideFiles;
	IFileManager::Get().FindFiles(SlideFiles, *(SourceContentDir / SlideSrc), TEXT("*.uasset"));
	for (const FString& File : SlideFiles)
	{
		CopyAnimAsset(SlideSrc + File, SlideDst);
	}
	UE_LOG(LogTemp, Log, TEXT("  Total slide files: %d"), SlideFiles.Num());
}

void USetupGameAnimCommandlet::SetupTraversalAnims()
{
	UE_LOG(LogTemp, Log, TEXT("--- Copying Traversal animations ---"));

	const FString Base = TEXT("Characters/UEFN_Mannequin/Animations/Traversal/");

	auto CopyFolder = [&](const FString& Subfolder)
	{
		FString SrcDir = Base + Subfolder + TEXT("/");
		FString DstDir = TEXT("Characters/UEFN_Mannequin/Animations/Traversal/") + Subfolder;
		TArray<FString> Files;
		IFileManager::Get().FindFiles(Files, *(SourceContentDir / SrcDir), TEXT("*.uasset"));
		for (const FString& File : Files)
		{
			CopyAnimAsset(SrcDir + File, DstDir);
		}
		UE_LOG(LogTemp, Log, TEXT("  %s: %d files"), *Subfolder, Files.Num());
	};

	CopyFolder(TEXT("Vault"));
	CopyFolder(TEXT("Mantle"));
	CopyFolder(TEXT("Hurdle"));
	CopyFolder(TEXT("Climb"));
}

void USetupGameAnimCommandlet::SetupLandingAnims()
{
	UE_LOG(LogTemp, Log, TEXT("--- Copying Landing animations ---"));

	const FString JumpSrc = TEXT("Characters/UEFN_Mannequin/Animations/Jump/");
	const FString JumpDst = TEXT("Characters/UEFN_Mannequin/Animations/Jump");

	// Copy all jump animations (includes landings, takeoffs, loops)
	TArray<FString> JumpFiles;
	IFileManager::Get().FindFiles(JumpFiles, *(SourceContentDir / JumpSrc), TEXT("*.uasset"));
	for (const FString& File : JumpFiles)
	{
		CopyAnimAsset(JumpSrc + File, JumpDst);
	}
	UE_LOG(LogTemp, Log, TEXT("  Total jump/landing files: %d"), JumpFiles.Num());
}

void USetupGameAnimCommandlet::SetupAvoidanceAnims()
{
	UE_LOG(LogTemp, Log, TEXT("--- Copying Avoidance animations ---"));

	const FString AvoidSrc = TEXT("Characters/UEFN_Mannequin/Animations/Avoidance/");
	const FString AvoidDst = TEXT("Characters/UEFN_Mannequin/Animations/Avoidance");

	TArray<FString> AvoidFiles;
	IFileManager::Get().FindFiles(AvoidFiles, *(SourceContentDir / AvoidSrc), TEXT("*.uasset"));
	for (const FString& File : AvoidFiles)
	{
		CopyAnimAsset(AvoidSrc + File, AvoidDst);
	}
	UE_LOG(LogTemp, Log, TEXT("  Total avoidance files: %d"), AvoidFiles.Num());
}

void USetupGameAnimCommandlet::CreateMontageBatch()
{
	// === BENCH (sit/stand) ===
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Interactions/Bench/M_interaction_bench_into_F_Lfoot"),
		TEXT("AM_Rest_SitDown"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Interactions/Bench/M_interaction_bench_out_to_stand"),
		TEXT("AM_Rest_StandUp"), TEXT("Montages"));

	// === SLIDE ===
	// Actual names: M_Neutral_Slide_FootOut_Into_Lfoot, _Loop, _Out_Idle_Stand
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Slide/M_Neutral_Slide_FootOut_Into_Lfoot"),
		TEXT("AM_Slide"), TEXT("Montages"));

	// === TRAVERSAL ===
	// Pre-made montages already exist (AM_ prefix files). Use sequences for our custom montages.
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Traversal/Vault/M_Neutral_Traversal_Vault_1_0_run_F_Lfoot"),
		TEXT("AM_Vault"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Traversal/Hurdle/M_Neutral_Traversal_Hurdle_1_0_run_F_Lfoot"),
		TEXT("AM_Hurdle"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Traversal/Mantle/M_Neutral_Traversal_Mantle_1_0_run_F_Lfoot"),
		TEXT("AM_Mantle"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Traversal/Climb/M_Neutral_Traversal_Climb_Start_2_5_run_F_Lfoot"),
		TEXT("AM_Climb"), TEXT("Montages"));

	// === SWIMMING (FreeAnimationLibrary) ===
	CreateMontageFromSequence(
		TEXT("/Game/FreeAnimationLibrary/Animations/Swim/anim_SwimIdle"),
		TEXT("AM_SwimIdle"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/FreeAnimationLibrary/Animations/Swim/anim_Swim_Surface_Fwd"),
		TEXT("AM_SwimForward"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/FreeAnimationLibrary/Animations/Swim/anim_Swim_Surface_Left"),
		TEXT("AM_SwimLeft"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/FreeAnimationLibrary/Animations/Swim/anim_Swim_Surface_Right"),
		TEXT("AM_SwimRight"), TEXT("Montages"));

	// === DOUBLE JUMP ===
	// Standing forward jump start — looks like a second upward push when played mid-air
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Jump/M_Neutral_Jump_F_Start_Stand_Lfoot"),
		TEXT("AM_DoubleJump"), TEXT("Montages"));

	// === LANDING ===
	// Actual names have foot suffix: _Land_Run_Light_Lfoot etc.
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Jump/M_Neutral_Jump_F_Land_Run_Light_Lfoot"),
		TEXT("AM_Landing_Light"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Jump/M_Neutral_Jump_F_Land_Run_Heavy_Lfoot"),
		TEXT("AM_Landing_Heavy"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Jump/M_Neutral_Jump_F_Land_Roll_Lfoot"),
		TEXT("AM_Landing_Roll"), TEXT("Montages"));
	CreateMontageFromSequence(
		TEXT("/Game/Characters/UEFN_Mannequin/Animations/Jump/M_Neutral_Jump_F_Land_Sprint_Heavy_Lfoot"),
		TEXT("AM_Landing_Stumble"), TEXT("Montages"));
}
