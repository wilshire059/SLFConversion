// SLFActionSprintAttack.cpp
// Logic: Get weapon animset, extract SprintAttackMontage, play montage
#include "SLFActionSprintAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"

USLFActionSprintAttack::USLFActionSprintAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] Initialized"));
}

void USLFActionSprintAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] ExecuteAction - Running attack"));

	if (!OwnerActor) return;

	// Get weapon animset and cast to C++ type for direct property access
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionSprintAttack] No weapon animset found or not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->SprintAttackMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionSprintAttack] No SprintAttackMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionSprintAttack] Playing montage: %s"), *Montage->GetName());
	}
}
