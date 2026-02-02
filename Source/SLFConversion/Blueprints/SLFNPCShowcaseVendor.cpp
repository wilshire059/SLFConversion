// SLFNPCShowcaseVendor.cpp
// C++ implementation for B_Soulslike_NPC_ShowcaseVendor
//
// MESH MERGING: This class implements runtime mesh merging exactly like bp_only:
// 1. Collect meshes from Head, Body, Arms, Legs SCS components
// 2. Use SkeletalMergingLibrary::MergeMeshes() to create single merged mesh
// 3. Apply merged mesh to main character mesh
// 4. Hide the modular components

#include "SLFNPCShowcaseVendor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AIInteractionManagerComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "SkeletalMergingLibrary.h"

ASLFNPCShowcaseVendor::ASLFNPCShowcaseVendor()
{
	// Default display name for Vendor NPC
	NPCDisplayName = FText::FromString(TEXT("Vendor"));

	// ═══════════════════════════════════════════════════════════════════
	// DEFAULT MANNY MODULAR MESH PARTS (exactly like bp_only)
	// These will be merged at runtime using SkeletalMergingLibrary
	// ═══════════════════════════════════════════════════════════════════
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HeadMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyHead"));
	if (HeadMeshFinder.Succeeded())
	{
		HeadMesh = HeadMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> UpperBodyMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyUpperBody"));
	if (UpperBodyMeshFinder.Succeeded())
	{
		UpperBodyMesh = UpperBodyMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmsMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyArms"));
	if (ArmsMeshFinder.Succeeded())
	{
		ArmsMesh = ArmsMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> LowerBodyMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyLowerBody"));
	if (LowerBodyMeshFinder.Succeeded())
	{
		LowerBodyMesh = LowerBodyMeshFinder.Object;
	}

	// Fallback mesh if merging fails
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMeshFinder(
		TEXT("/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny"));
	if (DefaultMeshFinder.Succeeded())
	{
		DefaultVendorMesh = DefaultMeshFinder.Object;
	}

	// Initialize component pointers
	HeadComponent = nullptr;
	BodyComponent = nullptr;
	ArmsComponent = nullptr;
	LegsComponent = nullptr;

	// Default dialog asset - DA_ExampleDialog_Vendor (vendor-specific: Buy/Sell/Trade/Leave)
	static ConstructorHelpers::FObjectFinder<UPrimaryDataAsset> DefaultDialogFinder(
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor"));
	if (DefaultDialogFinder.Succeeded())
	{
		DefaultDialogAsset = DefaultDialogFinder.Object;
	}

	// Default vendor asset - DA_ExampleVendor (items for sale)
	static ConstructorHelpers::FObjectFinder<UPrimaryDataAsset> DefaultVendorFinder(
		TEXT("/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor"));
	if (DefaultVendorFinder.Succeeded())
	{
		DefaultVendorAsset = DefaultVendorFinder.Object;
	}
}

void ASLFNPCShowcaseVendor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] BeginPlay: %s - C++ class running"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] DefaultVendorAsset IsNull: %s"),
		DefaultVendorAsset.IsNull() ? TEXT("YES") : TEXT("NO"));

	// ═══════════════════════════════════════════════════════════════════
	// STEP 1: Configure AIInteractionManager with vendor-specific data
	// ═══════════════════════════════════════════════════════════════════
	if (AIInteractionManager)
	{
		// Set NPC name
		AIInteractionManager->Name = NPCDisplayName;
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Set Name: %s"), *NPCDisplayName.ToString());

		// Set dialog asset (vendor dialog with Buy/Sell/Trade/Leave)
		if (!DefaultDialogAsset.IsNull())
		{
			UPrimaryDataAsset* DialogData = DefaultDialogAsset.LoadSynchronous();
			if (DialogData)
			{
				AIInteractionManager->DialogAsset = DialogData;
				UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Set DialogAsset: %s"), *DialogData->GetName());
			}
		}

		// Set vendor asset (items for sale)
		if (!DefaultVendorAsset.IsNull())
		{
			UPrimaryDataAsset* VendorData = DefaultVendorAsset.LoadSynchronous();
			if (VendorData)
			{
				AIInteractionManager->VendorAsset = VendorData;
				UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Set VendorAsset: %s"), *VendorData->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] DefaultVendorAsset.LoadSynchronous() returned NULL!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] DefaultVendorAsset IsNull!"));
		}

		// VERIFY: Log final state of AIInteractionManager
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] VERIFY - AIInteractionManager: %p, VendorAsset: %p (%s)"),
			(void*)AIInteractionManager,
			(void*)AIInteractionManager->VendorAsset,
			AIInteractionManager->VendorAsset ? *AIInteractionManager->VendorAsset->GetName() : TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] AIInteractionManager is nullptr!"));
	}

	// ═══════════════════════════════════════════════════════════════════
	// STEP 2: Cache references to Blueprint SCS modular mesh components
	// ═══════════════════════════════════════════════════════════════════
	TArray<USkeletalMeshComponent*> AllSkeletalMeshes;
	GetComponents<USkeletalMeshComponent>(AllSkeletalMeshes);

	for (USkeletalMeshComponent* SMC : AllSkeletalMeshes)
	{
		if (!SMC) continue;

		FString CompName = SMC->GetName();
		if (CompName.Equals(TEXT("Head")))
		{
			HeadComponent = SMC;
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Found Head component"));
		}
		else if (CompName.Equals(TEXT("Body")))
		{
			BodyComponent = SMC;
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Found Body component"));
		}
		else if (CompName.Equals(TEXT("Arms")))
		{
			ArmsComponent = SMC;
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Found Arms component"));
		}
		else if (CompName.Equals(TEXT("Legs")))
		{
			LegsComponent = SMC;
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Found Legs component"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════
	// STEP 3: Set up mesh merge data and perform runtime mesh merging
	// This matches bp_only BeginPlay logic exactly
	// ═══════════════════════════════════════════════════════════════════
	USkeletalMeshComponent* CharMesh = GetMesh();
	if (!CharMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] No character mesh component found!"));
		return;
	}

	// Collect meshes to merge - first try from SCS components, then fallback to defaults
	TArray<USkeletalMesh*> MeshesToMerge;

	// Head mesh
	if (HeadComponent && HeadComponent->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(HeadComponent->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Head from SCS: %s"), *HeadComponent->GetSkeletalMeshAsset()->GetName());
	}
	else if (!HeadMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = HeadMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshesToMerge.Add(LoadedMesh);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Head from default: %s"), *LoadedMesh->GetName());
		}
	}

	// Body (UpperBody) mesh
	if (BodyComponent && BodyComponent->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(BodyComponent->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Body from SCS: %s"), *BodyComponent->GetSkeletalMeshAsset()->GetName());
	}
	else if (!UpperBodyMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = UpperBodyMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshesToMerge.Add(LoadedMesh);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using UpperBody from default: %s"), *LoadedMesh->GetName());
		}
	}

	// Arms mesh
	if (ArmsComponent && ArmsComponent->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(ArmsComponent->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Arms from SCS: %s"), *ArmsComponent->GetSkeletalMeshAsset()->GetName());
	}
	else if (!ArmsMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = ArmsMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshesToMerge.Add(LoadedMesh);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Arms from default: %s"), *LoadedMesh->GetName());
		}
	}

	// Legs (LowerBody) mesh
	if (LegsComponent && LegsComponent->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(LegsComponent->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using Legs from SCS: %s"), *LegsComponent->GetSkeletalMeshAsset()->GetName());
	}
	else if (!LowerBodyMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = LowerBodyMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshesToMerge.Add(LoadedMesh);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Using LowerBody from default: %s"), *LoadedMesh->GetName());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Mesh merge: %d meshes collected"), MeshesToMerge.Num());

	// Perform mesh merging if we have enough parts
	bool bMeshMergeSuccessful = false;
	if (MeshesToMerge.Num() >= 4)
	{
		// Set up merge parameters
		MeshMergeData.MeshesToMerge = MeshesToMerge;

		// Get skeleton from character mesh or first mesh part
		if (CharMesh->GetSkeletalMeshAsset())
		{
			MeshMergeData.Skeleton = CharMesh->GetSkeletalMeshAsset()->GetSkeleton();
		}
		else if (MeshesToMerge.Num() > 0 && MeshesToMerge[0])
		{
			MeshMergeData.Skeleton = MeshesToMerge[0]->GetSkeleton();
		}

		// Perform the merge
		USkeletalMesh* MergedMesh = USkeletalMergingLibrary::MergeMeshes(MeshMergeData);

		if (IsValid(MergedMesh))
		{
			CharMesh->SetSkeletalMeshAsset(MergedMesh);
			bMeshMergeSuccessful = true;
			UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Mesh merge SUCCESSFUL - applied to character mesh"));

			// Hide the modular components after successful merge (like bp_only does)
			if (HeadComponent)
			{
				HeadComponent->SetVisibility(false);
				HeadComponent->SetHiddenInGame(true);
			}
			if (BodyComponent)
			{
				BodyComponent->SetVisibility(false);
				BodyComponent->SetHiddenInGame(true);
			}
			if (ArmsComponent)
			{
				ArmsComponent->SetVisibility(false);
				ArmsComponent->SetHiddenInGame(true);
			}
			if (LegsComponent)
			{
				LegsComponent->SetVisibility(false);
				LegsComponent->SetHiddenInGame(true);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Mesh merge FAILED - MergedMesh is invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Not enough meshes to merge (%d), need 4"), MeshesToMerge.Num());
	}

	// Fallback: if mesh merge failed, use default vendor mesh
	if (!bMeshMergeSuccessful)
	{
		if (!DefaultVendorMesh.IsNull())
		{
			USkeletalMesh* VendorMesh = DefaultVendorMesh.LoadSynchronous();
			if (VendorMesh)
			{
				CharMesh->SetSkeletalMeshAsset(VendorMesh);
				UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] FALLBACK: Applied default vendor mesh: %s"), *VendorMesh->GetName());
			}
		}

		// Still hide modular components
		if (HeadComponent) { HeadComponent->SetVisibility(false); HeadComponent->SetHiddenInGame(true); }
		if (BodyComponent) { BodyComponent->SetVisibility(false); BodyComponent->SetHiddenInGame(true); }
		if (ArmsComponent) { ArmsComponent->SetVisibility(false); ArmsComponent->SetHiddenInGame(true); }
		if (LegsComponent) { LegsComponent->SetVisibility(false); LegsComponent->SetHiddenInGame(true); }
	}

	// ═══════════════════════════════════════════════════════════════════
	// STEP 4: Apply AnimClass - Always ensure correct AnimClass is set
	// This fixes the A-pose issue when Blueprint has wrong/no AnimClass
	// ═══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] NPCAnimClass: %s, Current AnimClass: %s, AnimInstance: %s"),
		NPCAnimClass ? *NPCAnimClass->GetName() : TEXT("NULL"),
		CharMesh->GetAnimClass() ? *CharMesh->GetAnimClass()->GetName() : TEXT("NULL"),
		CharMesh->GetAnimInstance() ? *CharMesh->GetAnimInstance()->GetName() : TEXT("NULL"));

	if (NPCAnimClass)
	{
		UClass* CurrentAnimClass = CharMesh->GetAnimClass();

		// ALWAYS apply after mesh change to ensure animation works
		if (!CharMesh->GetAnimInstance() || CurrentAnimClass != NPCAnimClass)
		{
			CharMesh->SetAnimInstanceClass(NPCAnimClass);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Applied NPCAnimClass: %s (was: %s)"),
				*NPCAnimClass->GetName(),
				CurrentAnimClass ? *CurrentAnimClass->GetName() : TEXT("NULL"));

			// Force initialize the anim instance
			CharMesh->InitAnim(true);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Initialized AnimInstance: %s"),
				CharMesh->GetAnimInstance() ? *CharMesh->GetAnimInstance()->GetName() : TEXT("NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] AnimClass already correct: %s"),
				*NPCAnimClass->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] No NPCAnimClass set!"));
	}
}
