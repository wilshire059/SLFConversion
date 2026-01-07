// B_Action_ComboHeavy.cpp
// C++ implementation for Blueprint B_Action_ComboHeavy
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ComboHeavy.json

#include "Blueprints/B_Action_ComboHeavy.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"

UB_Action_ComboHeavy::UB_Action_ComboHeavy()
{
}

void UB_Action_ComboHeavy::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No OwnerActor"));
		return;
	}

	// Get equipment manager to check stance
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No EquipmentManager found"));
		return;
	}

	// Check if two-hand stance is active
	bool bOutLeftHand = false;
	bool bOutRightHand = false;
	bool bOutLeftHand_1 = false;
	bool bOutRightHand_1 = false;
	bool bOutLeftHand_2 = false;
	bool bOutRightHand_2 = false;
	EquipMgr->IsTwoHandStanceActive(bOutLeftHand, bOutRightHand, bOutLeftHand_1, bOutRightHand_1, bOutLeftHand_2, bOutRightHand_2);
	bool bIsTwoHanded = bOutLeftHand || bOutRightHand;

	// Get weapon animset for right hand (heavy attack uses right hand)
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Right, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No weapon animset found"));
		return;
	}

	// Determine which montage property to extract
	// 2h_HeavyComboMontage for two-handed, 1h_HeavyComboMontage_R for one-handed
	const TCHAR* MontagePrefix = bIsTwoHanded ? TEXT("2h_HeavyComboMontage") : TEXT("1h_HeavyComboMontage_R");
	UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Looking for montage with prefix: %s"), MontagePrefix);

	// Extract montage using reflection
	UAnimMontage* Montage = nullptr;
	for (TFieldIterator<FProperty> PropIt(Animset->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		FString PropName = Prop->GetName();
		if (PropName.StartsWith(MontagePrefix))
		{
			if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
				if (SoftPtr)
				{
					Montage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
					UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Found montage from property: %s"), *PropName);
				}
			}
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
				Montage = Cast<UAnimMontage>(Obj);
				UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Found direct montage from property: %s"), *PropName);
			}
			break;
		}
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboHeavy] No montage found with prefix: %s"), MontagePrefix);
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboHeavy] Playing montage: %s"), *Montage->GetName());
	}
}

