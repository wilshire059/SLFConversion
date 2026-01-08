// SLFActionComboHeavy.cpp
// Logic: Get weapon animset, extract 1h_HeavyComboMontage_R or 2h_HeavyComboMontage, play montage
#include "SLFActionComboHeavy.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/EquipmentManagerComponent.h"
#include "SLFPrimaryDataAssets.h"

USLFActionComboHeavy::USLFActionComboHeavy()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Initialized"));
}

void USLFActionComboHeavy::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] ExecuteAction"));

	if (!OwnerActor) return;

	// Get equipment manager to check stance
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	bool bIsTwoHanded = EquipMgr ? EquipMgr->IsTwoHandStanceActive() : false;

	// Get weapon animset and cast to C++ type for direct property access
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No weapon animset found or not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed (heavy attack uses right hand for 1h)
	UAnimMontage* Montage = nullptr;
	if (bIsTwoHanded)
	{
		Montage = WeaponAnimset->TwoH_HeavyComboMontage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 2h_HeavyComboMontage"));
	}
	else
	{
		Montage = WeaponAnimset->OneH_HeavyComboMontage_R.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 1h_HeavyComboMontage_R"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] Montage is null (TwoHanded=%s)"), bIsTwoHanded ? TEXT("true") : TEXT("false"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Playing montage: %s"), *Montage->GetName());
	}
}
