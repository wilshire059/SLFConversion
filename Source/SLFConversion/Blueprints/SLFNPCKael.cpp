// SLFNPCKael.cpp

#include "Blueprints/SLFNPCKael.h"
#include "Components/AIInteractionManagerComponent.h"

ASLFNPCKael::ASLFNPCKael()
{
	NPCConfig.NPCName = FText::FromString(TEXT("Kael"));
	NPCConfig.Faction = ESLFFaction::Neutral;
	NPCConfig.bIsMerchant = false; // Upgrader, not merchant
	NPCConfig.bIsWanderer = false; // Stays in hub

	bIsTraceable = true;
}

void ASLFNPCKael::BeginPlay()
{
	Super::BeginPlay();

	if (!KaelMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = KaelMesh.LoadSynchronous();
		if (LoadedMesh && GetMesh())
		{
			GetMesh()->SetSkeletalMeshAsset(LoadedMesh);
		}
	}

	// Configure AIInteractionManager
	if (AIInteractionManager)
	{
		AIInteractionManager->Name = NPCConfig.NPCName;

		if (!DefaultDialogAsset.IsNull())
		{
			UPrimaryDataAsset* DialogAsset = DefaultDialogAsset.LoadSynchronous();
			if (DialogAsset)
			{
				AIInteractionManager->DialogAsset = DialogAsset;
			}
		}
	}
}

int32 ASLFNPCKael::GetUpgradeCost(int32 CurrentLevel) const
{
	if (CurrentLevel >= MaxUpgradeLevel) return -1; // Max level

	// Scaling: cost increases quadratically
	// Level 0->1: 500, 1->2: 1000, 2->3: 1500, ..., 9->10: 5000
	return BaseUpgradeCost * (CurrentLevel + 1);
}

bool ASLFNPCKael::CanUpgradeWeapon(AActor* Player, UDataAsset* Weapon, int32 CurrentLevel) const
{
	if (!Player || !Weapon) return false;
	if (CurrentLevel >= MaxUpgradeLevel) return false;

	// Currency check would go through BPI_Controller::Execute_GetCurrency
	// Material check would go through InventoryManager
	// For now, return true — full implementation when inventory system is wired
	return true;
}

void ASLFNPCKael::OnInteract_Implementation(AActor* InteractingActor)
{
	Super::OnInteract_Implementation(InteractingActor);
}

void ASLFNPCKael::OnTraced_Implementation(AActor* TracedBy)
{
	Super::OnTraced_Implementation(TracedBy);
}
