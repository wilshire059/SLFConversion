// SLFNPCVara.cpp

#include "Blueprints/SLFNPCVara.h"
#include "Components/SLFZoneManagerComponent.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Kismet/GameplayStatics.h"

ASLFNPCVara::ASLFNPCVara()
{
	// Default NPC config
	NPCConfig.NPCName = FText::FromString(TEXT("Vara"));
	NPCConfig.Faction = ESLFFaction::Neutral;
	NPCConfig.bIsMerchant = true;
	NPCConfig.bIsWanderer = true;

	bIsTraceable = true;
}

void ASLFNPCVara::BeginPlay()
{
	Super::BeginPlay();

	// Set mesh if configured
	if (!VaraMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = VaraMesh.LoadSynchronous();
		if (LoadedMesh && GetMesh())
		{
			GetMesh()->SetSkeletalMeshAsset(LoadedMesh);
		}
	}

	// Configure AIInteractionManager with Vara's dialog
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

		if (!VendorDataAsset.IsNull())
		{
			UPrimaryDataAsset* VendorAsset = VendorDataAsset.LoadSynchronous();
			if (VendorAsset)
			{
				AIInteractionManager->VendorAsset = VendorAsset;
			}
		}
	}
}

void ASLFNPCVara::UpdateLocationFromProgress()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	USLFZoneManagerComponent* ZoneMgr = PC->FindComponentByClass<USLFZoneManagerComponent>();
	if (!ZoneMgr) return;

	// Vara moves based on which bosses have been defeated
	static const FGameplayTag HubTag = FGameplayTag::RequestGameplayTag(FName("Zone.Hub"), false);
	static const FGameplayTag AshfieldsTag = FGameplayTag::RequestGameplayTag(FName("Zone.Ashfields"), false);
	static const FGameplayTag IronworksTag = FGameplayTag::RequestGameplayTag(FName("Zone.Ironworks"), false);
	static const FGameplayTag RiftTag = FGameplayTag::RequestGameplayTag(FName("Zone.Rift"), false);
	static const FGameplayTag SanctumTag = FGameplayTag::RequestGameplayTag(FName("Zone.Sanctum"), false);

	static const FGameplayTag BroodmotherTag = FGameplayTag::RequestGameplayTag(FName("Boss.Broodmother"), false);
	static const FGameplayTag ForgemasterTag = FGameplayTag::RequestGameplayTag(FName("Boss.Forgemaster"), false);
	static const FGameplayTag ConvergentTag = FGameplayTag::RequestGameplayTag(FName("Boss.Convergent"), false);
	static const FGameplayTag HierophantTag = FGameplayTag::RequestGameplayTag(FName("Boss.Hierophant"), false);

	// Progress through zones as bosses are defeated
	if (ZoneMgr->IsBossDefeated(HierophantTag))
	{
		CurrentLocationZone = SanctumTag; // Near The Core entrance
	}
	else if (ZoneMgr->IsBossDefeated(ConvergentTag))
	{
		CurrentLocationZone = RiftTag;
	}
	else if (ZoneMgr->IsBossDefeated(ForgemasterTag))
	{
		CurrentLocationZone = IronworksTag;
	}
	else if (ZoneMgr->IsBossDefeated(BroodmotherTag))
	{
		CurrentLocationZone = AshfieldsTag;
	}
	else
	{
		CurrentLocationZone = HubTag; // Start in hub
	}
}

void ASLFNPCVara::GetContextualDialog_Implementation(TArray<FSLFDialogEntry>& OutEntries)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	USLFZoneManagerComponent* ZoneMgr = PC->FindComponentByClass<USLFZoneManagerComponent>();
	if (!ZoneMgr) return;

	static const FGameplayTag BroodmotherTag = FGameplayTag::RequestGameplayTag(FName("Boss.Broodmother"), false);

	// Build dialog based on progress
	FSLFDialogEntry Entry;

	if (!ZoneMgr->IsBossDefeated(BroodmotherTag))
	{
		// Pre-first boss dialog
		Entry.Entry = FText::FromString(TEXT("The Ashfields are a cruel welcome, but they're the gentlest thing this world has left. Seek the Broodmother in the collapsed bunker — she guards the way forward."));
		OutEntries.Add(Entry);

		FSLFDialogEntry Entry2;
		Entry2.Entry = FText::FromString(TEXT("Take what you need from my wares. You'll find the world beyond far less generous."));
		OutEntries.Add(Entry2);
	}
	else if (ZoneMgr->IsCoreAccessible())
	{
		// All bosses down — point toward The Core
		Entry.Entry = FText::FromString(TEXT("You've broken through every barrier. The Core awaits. Whatever happened during the Convergence... you'll find the truth there. Or it will find you."));
		OutEntries.Add(Entry);
	}
	else
	{
		// Mid-game
		Entry.Entry = FText::FromString(TEXT("You grow stronger. The Convergence's corruption runs deep — four guardians stand between you and the Core. Seek them out."));
		OutEntries.Add(Entry);
	}
}

void ASLFNPCVara::OnInteract_Implementation(AActor* InteractingActor)
{
	// Call parent (handles dialog UI opening)
	Super::OnInteract_Implementation(InteractingActor);
}

void ASLFNPCVara::OnTraced_Implementation(AActor* TracedBy)
{
	Super::OnTraced_Implementation(TracedBy);
}
