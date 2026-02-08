// SLFNPCShowcaseGuide.cpp
// C++ implementation for B_Soulslike_NPC_ShowcaseGuide

#include "SLFNPCShowcaseGuide.h"
#include "Components/AIInteractionManagerComponent.h"
#include "UObject/ConstructorHelpers.h"

ASLFNPCShowcaseGuide::ASLFNPCShowcaseGuide()
{
	// Default display name for Guide NPC
	NPCDisplayName = FText::FromString(TEXT("Guide"));

	// Default dialog asset - DA_ExampleDialog (Talk-based, NOT vendor)
	// Located at: /Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog
	static ConstructorHelpers::FObjectFinder<UPrimaryDataAsset> DefaultDialogFinder(
		TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"));
	if (DefaultDialogFinder.Succeeded())
	{
		DefaultDialogAsset = DefaultDialogFinder.Object;
	}
}

void ASLFNPCShowcaseGuide::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseGuide] BeginPlay: %s - Configuring AIInteractionManager"), *GetName());

	// Configure AIInteractionManager with Guide-specific data
	// Guide NPC has dialog but NO vendor
	if (AIInteractionManager)
	{
		// Set NPC name
		AIInteractionManager->Name = NPCDisplayName;
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseGuide] Set Name: %s"), *NPCDisplayName.ToString());

		// Set dialog asset
		if (!DefaultDialogAsset.IsNull())
		{
			UPrimaryDataAsset* DialogData = DefaultDialogAsset.LoadSynchronous();
			if (DialogData)
			{
				AIInteractionManager->DialogAsset = DialogData;
				UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseGuide] Set DialogAsset: %s"), *DialogData->GetName());
			}
		}

		// Guide NPC has NO vendor - explicitly set to nullptr
		AIInteractionManager->VendorAsset = nullptr;
		UE_LOG(LogTemp, Log, TEXT("[NPCShowcaseGuide] VendorAsset set to nullptr (Guide has no vendor)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCShowcaseGuide] AIInteractionManager is nullptr!"));
	}
}
