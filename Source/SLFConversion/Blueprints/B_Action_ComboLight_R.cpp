// B_Action_ComboLight_R.cpp
// C++ implementation for Blueprint B_Action_ComboLight_R
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_ComboLight_R.json

#include "Blueprints/B_Action_ComboLight_R.h"
#include "Components/AC_EquipmentManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Animation/AnimMontage.h"

UB_Action_ComboLight_R::UB_Action_ComboLight_R()
{
}

void UB_Action_ComboLight_R::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No OwnerActor"));
		return;
	}

	// Get equipment manager to check stance
	UAC_EquipmentManager* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No EquipmentManager found"));
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

	// Get weapon animset for right hand
	UPrimaryDataAsset* Animset = nullptr;
	UPrimaryDataAsset* Animset1 = nullptr;
	GetWeaponAnimset(ESLFActionWeaponSlot::Right, Animset, Animset1);

	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No weapon animset found"));
		return;
	}

	// Determine which montage property to extract
	// 2h_LightComboMontage for two-handed, 1h_LightComboMontage_R for one-handed right
	const TCHAR* MontagePrefix = bIsTwoHanded ? TEXT("2h_LightComboMontage") : TEXT("1h_LightComboMontage_R");
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Looking for montage with prefix: %s"), MontagePrefix);

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
					UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Found montage from property: %s"), *PropName);
				}
			}
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
				Montage = Cast<UAnimMontage>(Obj);
				UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Found direct montage from property: %s"), *PropName);
			}
			break;
		}
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLight_R] No montage found with prefix: %s"), MontagePrefix);
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLight_R] Playing montage: %s"), *Montage->GetName());
	}
}

