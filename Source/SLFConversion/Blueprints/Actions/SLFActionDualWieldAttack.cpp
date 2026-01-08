// SLFActionDualWieldAttack.cpp
// Logic: Get weapon animset, extract LightDualWieldMontage, play montage
#include "SLFActionDualWieldAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"

USLFActionDualWieldAttack::USLFActionDualWieldAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] Initialized"));
}

void USLFActionDualWieldAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] ExecuteAction - Both hands attack"));

	if (!OwnerActor) return;

	// Get weapon animset and cast to C++ type for direct property access
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDualWieldAttack] No weapon animset found or not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->LightDualWieldMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDualWieldAttack] No LightDualWieldMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionDualWieldAttack] Playing montage: %s"), *Montage->GetName());
	}
}
