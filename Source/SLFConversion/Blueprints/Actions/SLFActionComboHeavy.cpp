// SLFActionComboHeavy.cpp
// Logic: Get weapon animset, use CombatManager::EventBeginSoftCombo for combo system
#include "SLFActionComboHeavy.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "AC_EquipmentManager.h"
#include "AC_CombatManager.h"
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
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	bool bIsTwoHanded = EquipMgr ? EquipMgr->IsTwoHandStanceActiveSimple() : false;

	// Get weapon animset
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No WeaponAnimset found"));
		return;
	}

	// Get the appropriate montage reference
	TSoftObjectPtr<UAnimMontage> MontageRef;
	if (bIsTwoHanded)
	{
		MontageRef = WeaponAnimset->TwoH_HeavyComboMontage;
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 2h_HeavyComboMontage"));
	}
	else
	{
		MontageRef = WeaponAnimset->OneH_HeavyComboMontage_R;
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Using 1h_HeavyComboMontage"));
	}

	if (MontageRef.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] Montage reference is null"));
		return;
	}

	// Get the character's skeletal mesh
	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] OwnerActor is not a Character"));
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No skeletal mesh found"));
		return;
	}

	// Use CombatManager's combo system
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (CombatMgr)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Calling EventBeginSoftCombo"));
		CombatMgr->EventBeginSoftCombo(Mesh, MontageRef, 1.0);
	}
	else
	{
		// Fallback
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No CombatManager, playing directly"));
		if (UAnimMontage* Montage = MontageRef.LoadSynchronous())
		{
			if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
			}
		}
	}
}
