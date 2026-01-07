// B_Action_PickupItemMontage.cpp
// C++ implementation for Blueprint B_Action_PickupItemMontage
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_PickupItemMontage.json

#include "Blueprints/B_Action_PickupItemMontage.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "StructUtils/InstancedStruct.h"

UB_Action_PickupItemMontage::UB_Action_PickupItemMontage()
{
}

void UB_Action_PickupItemMontage::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] No OwnerActor"));
		return;
	}

	if (!Action)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] No Action data"));
		return;
	}

	// Get RelevantData from Action (FInstancedStruct containing FMontage)
	TSoftObjectPtr<UObject> MontageRef;

	// Try to extract montage from Action->RelevantData
	FProperty* RelevantDataProp = Action->GetClass()->FindPropertyByName(FName("RelevantData"));
	if (RelevantDataProp)
	{
		FStructProperty* StructProp = CastField<FStructProperty>(RelevantDataProp);
		if (StructProp && StructProp->Struct->GetFName() == FName("InstancedStruct"))
		{
			void* PropAddr = RelevantDataProp->ContainerPtrToValuePtr<void>(Action);
			FInstancedStruct* InstancedStruct = static_cast<FInstancedStruct*>(PropAddr);
			if (InstancedStruct && InstancedStruct->IsValid())
			{
				const UScriptStruct* StoredStructType = InstancedStruct->GetScriptStruct();
				const void* StructData = InstancedStruct->GetMemory();

				// Find the AnimMontage soft reference in the FMontage struct
				for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
				{
					FProperty* Prop = *PropIt;
					if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
					{
						// Found soft object property - extract it
						void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
						FSoftObjectPtr* SoftPtr = static_cast<FSoftObjectPtr*>(PropValueAddr);
						if (SoftPtr && !SoftPtr->IsNull())
						{
							MontageRef = TSoftObjectPtr<UObject>(SoftPtr->ToSoftObjectPath());
							UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Found montage: %s"), *MontageRef.ToString());
							break;
						}
					}
				}
			}
		}
	}

	// If we found a valid montage reference, play it
	if (!MontageRef.IsNull())
	{
		if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(
				OwnerActor,
				MontageRef,
				1.0,    // PlayRate
				0.0,    // StartPosition
				NAME_None, // StartSection
				false   // bPrio
			);
			UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Called PlaySoftMontageReplicated"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] OwnerActor does not implement BPI_GenericCharacter"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] No valid montage found in RelevantData"));
	}
}
