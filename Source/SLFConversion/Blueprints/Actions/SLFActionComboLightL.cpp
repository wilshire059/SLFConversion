// SLFActionComboLightL.cpp
// Logic: Get weapon animset, extract 1h_LightComboMontage_L or 2h_LightComboMontage, play montage
#include "SLFActionComboLightL.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/EquipmentManagerComponent.h"
#include "SLFPrimaryDataAssets.h"

USLFActionComboLightL::USLFActionComboLightL()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Initialized"));
}

void USLFActionComboLightL::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] ExecuteAction"));

	if (!OwnerActor) return;

	// Get equipment manager to check stance
	UEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	bool bIsTwoHanded = EquipMgr ? EquipMgr->IsTwoHandStanceActive() : false;

	// Get weapon animset and cast to C++ type for direct property access
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No weapon animset found or not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = nullptr;
	if (bIsTwoHanded)
	{
		Montage = WeaponAnimset->TwoH_LightComboMontage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Using 2h_LightComboMontage"));
	}
	else
	{
		Montage = WeaponAnimset->OneH_LightComboMontage_L.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Using 1h_LightComboMontage_L"));
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] Montage is null (TwoHanded=%s)"), bIsTwoHanded ? TEXT("true") : TEXT("false"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Playing montage: %s"), *Montage->GetName());
	}
}
