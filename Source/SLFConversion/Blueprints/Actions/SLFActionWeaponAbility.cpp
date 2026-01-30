// SLFActionWeaponAbility.cpp
// Logic: Get currently equipped weapon, extract its WeaponAbility data,
// check FP cost, play the ability animation and spawn any associated effects
//
// bp_only Logic:
// 1. ExecuteAction gets WeaponAbility from item's EquipmentDetails
// 2. CheckStatRequirement with AffectedStat and Cost
// 3. If success, call BeginSpecialAttack which:
//    - Plays Montage via PlaySoftMontageReplicated
//    - Spawns AdditionalEffectClass if valid
//    - Calls AdjustStatByRequirement to consume the stat (FP)
#include "SLFActionWeaponAbility.h"
#include "AC_EquipmentManager.h"
#include "AC_StatManager.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/StatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

USLFActionWeaponAbility::USLFActionWeaponAbility()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Initialized"));
}

void USLFActionWeaponAbility::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] ExecuteAction - Weapon skill/art"));

	if (!OwnerActor) return;

	// Get equipment manager to find active weapon
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] No equipment manager"));
		return;
	}

	// Get active weapon slot (right hand by default)
	FGameplayTag WeaponSlot = EquipMgr->GetActiveWeaponSlot(true); // true = right hand
	UPrimaryDataAsset* ItemAsset = nullptr;
	FGuid ItemId;
	EquipMgr->GetItemAtSlotSimple(WeaponSlot, ItemAsset, ItemId);

	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] No weapon equipped in active slot"));
		return;
	}

	// Get WeaponAbility from item's EquipmentDetails
	UPDA_WeaponAbility* WeaponAbility = nullptr;

	if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
	{
		// Access ItemInformation.EquipmentDetails.WeaponAbility directly
		UObject* AbilityObj = ItemData->ItemInformation.EquipmentDetails.WeaponAbility;
		WeaponAbility = Cast<UPDA_WeaponAbility>(AbilityObj);

		if (!WeaponAbility)
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Weapon %s has no WeaponAbility assigned"), *ItemAsset->GetName());
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] ItemAsset is not UPDA_Item"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Found ability: %s, Cost: %.1f, AffectedStat: %s"),
		*WeaponAbility->AbilityName.ToString(),
		WeaponAbility->Cost,
		*WeaponAbility->AffectedStat.ToString());

	// Check if we have enough of the required stat (usually FP) to use the ability
	if (WeaponAbility->AffectedStat.IsValid() && WeaponAbility->Cost > 0.0)
	{
		bool bHasEnough = CheckStatRequirement(WeaponAbility->AffectedStat, WeaponAbility->Cost);
		if (!bHasEnough)
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Insufficient %s for ability (need %.1f)"),
				*WeaponAbility->AffectedStat.ToString(), WeaponAbility->Cost);
			return;
		}
	}

	// Begin special attack - now we execute the ability
	BeginSpecialAttack(WeaponAbility);
}

void USLFActionWeaponAbility::BeginSpecialAttack(UPDA_WeaponAbility* Ability)
{
	if (!Ability || !OwnerActor) return;

	UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] BeginSpecialAttack - %s"), *Ability->AbilityName.ToString());

	// Play ability montage using PlaySoftMontageReplicated (matches bp_only)
	if (!Ability->Montage.IsNull() && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		// Convert to TSoftObjectPtr<UObject> for the interface call
		TSoftObjectPtr<UObject> SoftMontageAsObject;
		SoftMontageAsObject = TSoftObjectPtr<UObject>(Ability->Montage.ToSoftObjectPath());

		IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(OwnerActor, SoftMontageAsObject, 1.0f, 0.0f, NAME_None, false);
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Playing ability montage: %s"), *Ability->Montage.ToString());
	}
	else if (Ability->Montage.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionWeaponAbility] Ability has no montage assigned"));
	}

	// Spawn AdditionalEffectClass if valid (e.g., projectile, buff effect, etc.)
	if (!Ability->AdditionalEffectClass.IsNull())
	{
		UClass* EffectClass = Ability->AdditionalEffectClass.LoadSynchronous();
		if (EffectClass && OwnerActor->GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerActor;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* SpawnedEffect = OwnerActor->GetWorld()->SpawnActor<AActor>(EffectClass, OwnerActor->GetActorLocation(), OwnerActor->GetActorRotation(), SpawnParams);
			if (SpawnedEffect)
			{
				UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Spawned additional effect: %s"), *EffectClass->GetName());
			}
		}
	}

	// Consume the stat (FP) after successful execution
	if (Ability->AffectedStat.IsValid() && Ability->Cost > 0.0)
	{
		AdjustStatByRequirement(Ability->AffectedStat, Ability->Cost);
		UE_LOG(LogTemp, Log, TEXT("[ActionWeaponAbility] Consumed %.1f of %s"),
			Ability->Cost, *Ability->AffectedStat.ToString());
	}
}
