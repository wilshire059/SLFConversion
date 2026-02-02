// SLFNPCShowcaseVendor.cpp
// C++ implementation for B_Soulslike_NPC_ShowcaseVendor

#include "SLFNPCShowcaseVendor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AIInteractionManagerComponent.h"
#include "UObject/ConstructorHelpers.h"

ASLFNPCShowcaseVendor::ASLFNPCShowcaseVendor()
{
	// Default display name for Vendor NPC
	NPCDisplayName = FText::FromString(TEXT("Vendor"));

	// Default vendor mesh - SKM_Manny (Manny character - different from Guide's Quinn)
	// NOTE: bp_only used dynamic mesh merging with SkeletalMergingLibrary.MergeMeshes on:
	// - SKM_MannyHead, SKM_MannyUpperBody, SKM_MannyArms, SKM_MannyLowerBody
	// This is a simplified approach using the pre-merged Manny mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMeshFinder(
		TEXT("/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny"));
	if (DefaultMeshFinder.Succeeded())
	{
		DefaultVendorMesh = DefaultMeshFinder.Object;
	}

	// Default dialog asset - DA_ExampleDialog_Vendor (vendor-specific: Buy/Sell/Trade/Leave)
	// Located at: /Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor
	static ConstructorHelpers::FObjectFinder<UPrimaryDataAsset> DefaultDialogFinder(
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor"));
	if (DefaultDialogFinder.Succeeded())
	{
		DefaultDialogAsset = DefaultDialogFinder.Object;
	}

	// Default vendor asset - DA_ExampleVendor (items for sale)
	// Located at: /Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor
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
	// STEP 2: Set up vendor mesh
	// ═══════════════════════════════════════════════════════════════════
	USkeletalMeshComponent* CharMesh = GetMesh();
	if (!CharMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] No character mesh component found!"));
		return;
	}

	bool bMeshWasChanged = false;

	// Check if mesh is already set
	if (CharMesh->GetSkeletalMeshAsset())
	{
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Character mesh already set: %s"),
			*CharMesh->GetSkeletalMeshAsset()->GetName());
	}
	else
	{
		// Apply the default vendor mesh
		if (!DefaultVendorMesh.IsNull())
		{
			USkeletalMesh* VendorMesh = DefaultVendorMesh.LoadSynchronous();
			if (VendorMesh)
			{
				CharMesh->SetSkeletalMesh(VendorMesh);
				bMeshWasChanged = true;
				UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Applied default vendor mesh: %s"),
					*VendorMesh->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] Failed to load default vendor mesh!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseVendor] No default vendor mesh configured!"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════
	// STEP 3: Apply AnimClass - Always ensure correct AnimClass is set
	// This fixes the A-pose issue when Blueprint has wrong/no AnimClass
	// ═══════════════════════════════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] NPCAnimClass: %s, Current AnimClass: %s, AnimInstance: %s"),
		NPCAnimClass ? *NPCAnimClass->GetName() : TEXT("NULL"),
		CharMesh->GetAnimClass() ? *CharMesh->GetAnimClass()->GetName() : TEXT("NULL"),
		CharMesh->GetAnimInstance() ? *CharMesh->GetAnimInstance()->GetName() : TEXT("NULL"));

	if (NPCAnimClass)
	{
		UClass* CurrentAnimClass = CharMesh->GetAnimClass();

		// ALWAYS apply if mesh changed, no anim instance, or wrong AnimClass
		// This ensures NPCs animate correctly even if Blueprint had wrong settings
		if (bMeshWasChanged || !CharMesh->GetAnimInstance() || CurrentAnimClass != NPCAnimClass)
		{
			CharMesh->SetAnimInstanceClass(NPCAnimClass);
			UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseVendor] Applied NPCAnimClass: %s (mesh changed: %s, was: %s)"),
				*NPCAnimClass->GetName(),
				bMeshWasChanged ? TEXT("yes") : TEXT("no"),
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
