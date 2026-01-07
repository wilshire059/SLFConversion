// SLFAnimNotifyStateTrail.cpp
#include "SLFAnimNotifyStateTrail.h"
#include "Components/AC_EquipmentManager.h"
#include "Blueprints/B_Item.h"
#include "Blueprints/SLFWeaponBase.h"
#include "NiagaraComponent.h"

FString USLFAnimNotifyStateTrail::GetNotifyName_Implementation() const
{
	return TEXT("Trail Effect");
}

void USLFAnimNotifyStateTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_Trail] Begin - Activating weapon trail"));

	// Get EquipmentManager to find the active weapon
	if (UAC_EquipmentManager* EquipmentManager = Owner->FindComponentByClass<UAC_EquipmentManager>())
	{
		// Get active weapon slot (right hand)
		FGameplayTag ActiveSlot = EquipmentManager->GetActiveWeaponSlot(true);
		
		AB_Item* Item = nullptr;
		AB_Item* Unused = nullptr;
		EquipmentManager->GetSpawnedItemAtSlot(ActiveSlot, Item, Unused);

		if (ASLFWeaponBase* Weapon = Cast<ASLFWeaponBase>(Item))
		{
			if (Weapon->TrailComponent)
			{
				Weapon->TrailComponent->Activate(true);
			}
		}
	}
}

void USLFAnimNotifyStateTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_Trail] End - Deactivating weapon trail"));

	// Get EquipmentManager to find the active weapon
	if (UAC_EquipmentManager* EquipmentManager = Owner->FindComponentByClass<UAC_EquipmentManager>())
	{
		// Get active weapon slot (right hand)
		FGameplayTag ActiveSlot = EquipmentManager->GetActiveWeaponSlot(true);
		
		AB_Item* Item = nullptr;
		AB_Item* Unused = nullptr;
		EquipmentManager->GetSpawnedItemAtSlot(ActiveSlot, Item, Unused);

		if (ASLFWeaponBase* Weapon = Cast<ASLFWeaponBase>(Item))
		{
			if (Weapon->TrailComponent)
			{
				Weapon->TrailComponent->Deactivate();
			}
		}
	}
}
