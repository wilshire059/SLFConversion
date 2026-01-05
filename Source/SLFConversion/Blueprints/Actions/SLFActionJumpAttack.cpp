// SLFActionJumpAttack.cpp
// Logic: Get weapon animset, extract JumpAttackMontage, play montage
#include "SLFActionJumpAttack.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "UObject/UnrealType.h"

USLFActionJumpAttack::USLFActionJumpAttack()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Initialized"));
}

void USLFActionJumpAttack::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] ExecuteAction - Plunging attack"));

	if (!OwnerActor) return;

	// Get weapon animset
	UDataAsset* Animset = GetWeaponAnimset();
	if (!Animset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionJumpAttack] No weapon animset found"));
		return;
	}

	// Extract JumpAttackMontage using reflection
	UAnimMontage* Montage = nullptr;
	for (TFieldIterator<FProperty> PropIt(Animset->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;
		FString PropName = Prop->GetName();
		if (PropName.StartsWith(TEXT("JumpAttackMontage")))
		{
			// Handle TSoftObjectPtr<UAnimMontage>
			if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
				if (SoftPtr)
				{
					Montage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
					UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Found montage from property: %s"), *PropName);
				}
			}
			// Handle direct UAnimMontage* reference
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
			{
				void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(Animset);
				UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
				Montage = Cast<UAnimMontage>(Obj);
				UE_LOG(LogTemp, Log, TEXT("[ActionJumpAttack] Found direct montage from property: %s"), *PropName);
			}
			break;
		}
	}

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
