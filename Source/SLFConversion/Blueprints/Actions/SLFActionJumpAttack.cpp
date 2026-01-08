// SLFActionJumpAttack.cpp
// Logic: Get weapon animset, extract JumpAttackMontage, play montage
#include "SLFActionJumpAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFPrimaryDataAssets.h"

USLFActionJumpAttack::USLFActionJumpAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Initialized"));
}

void USLFActionJumpAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] ExecuteAction - Plunging attack"));

	if (!OwnerActor) return;

	// Get weapon animset and cast to C++ type for direct property access
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] No weapon animset found or not UPDA_WeaponAnimset"));
		return;
	}

	// Direct C++ property access - no reflection needed
	UAnimMontage* Montage = WeaponAnimset->JumpAttackMontage.LoadSynchronous();

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] No JumpAttackMontage found"));
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Playing montage: %s"), *Montage->GetName());
	}
}
