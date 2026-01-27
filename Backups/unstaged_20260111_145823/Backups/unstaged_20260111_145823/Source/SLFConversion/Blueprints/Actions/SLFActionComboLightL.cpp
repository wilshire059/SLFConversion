// SLFActionComboLightL.cpp
// Logic: Get weapon animset, use CombatManager::EventBeginSoftCombo for combo system
#include "SLFActionComboLightL.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "AC_EquipmentManager.h"
#include "AC_CombatManager.h"
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
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	bool bIsTwoHanded = EquipMgr ? EquipMgr->IsTwoHandStanceActiveSimple() : false;

	// Get weapon animset - for left hand, we might want left weapon
	// But typically light attack uses right hand animset
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No WeaponAnimset found"));
		return;
	}

	// Get the appropriate montage reference (soft ptr for async loading)
	// Left uses OneH_LightComboMontage_L
	TSoftObjectPtr<UAnimMontage> MontageRef;
	if (bIsTwoHanded)
	{
		MontageRef = WeaponAnimset->TwoH_LightComboMontage;
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Using 2h_LightComboMontage"));
	}
	else
	{
		MontageRef = WeaponAnimset->OneH_LightComboMontage_L;
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Using 1h_LightComboMontage_L"));
	}

	if (MontageRef.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] Montage reference is null"));
		return;
	}

	// Get the character's skeletal mesh
	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] OwnerActor is not a Character"));
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No skeletal mesh found"));
		return;
	}

	// Use CombatManager's combo system for proper combo handling
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (CombatMgr)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Calling EventBeginSoftCombo"));
		CombatMgr->EventBeginSoftCombo(Mesh, MontageRef, 1.0);
	}
	else
	{
		// Fallback: play directly if no combat manager
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No CombatManager, playing directly"));
		if (UAnimMontage* Montage = MontageRef.LoadSynchronous())
		{
			if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
			{
				IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
			}
		}
	}
}
