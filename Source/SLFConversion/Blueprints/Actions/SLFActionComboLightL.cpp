// SLFActionComboLightL.cpp
// Logic: Get weapon animset, extract 1h_LightComboMontage_L or 2h_LightComboMontage, play montage
#include "SLFActionComboLightL.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/EquipmentManagerComponent.h"
#include "UObject/UnrealType.h"

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

	// Get weapon animset
	UDataAsset* Animset = GetWeaponAnimset();
	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No weapon animset found"));
		return;
	}

	// Determine which montage property to extract
	const TCHAR* MontagePrefix = bIsTwoHanded ? TEXT("2h_LightComboMontage") : TEXT("1h_LightComboMontage_L");
	UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Looking for montage with prefix: %s"), MontagePrefix);

	// Extract montage using reflection (Blueprint animset has GUID suffixes on property names)
	UAnimMontage* Montage = nullptr;
	for (TFieldIterator<FProperty> PropIt(Animset->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		FString PropName = Prop->GetName();
		if (PropName.StartsWith(MontagePrefix))
		{
			// Handle TSoftObjectPtr<UAnimMontage>
			if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
				if (SoftPtr)
				{
					Montage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
					UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Found montage from property: %s"), *PropName);
				}
			}
			// Handle direct UAnimMontage* reference
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
				Montage = Cast<UAnimMontage>(Obj);
				UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Found direct montage from property: %s"), *PropName);
			}
			break;
		}
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionComboLightL] No montage found with prefix: %s"), MontagePrefix);
		return;
	}

	// Play montage via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[ActionComboLightL] Playing montage: %s"), *Montage->GetName());
	}
}
