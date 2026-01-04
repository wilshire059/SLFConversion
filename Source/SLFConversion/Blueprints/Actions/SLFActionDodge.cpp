// SLFActionDodge.cpp
// C++ implementation for B_Action_Dodge
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action_Dodge
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (DodgeMontages)
// Functions:         1/1 (ExecuteAction)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFActionDodge.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"

USLFActionDodge::USLFActionDodge()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Initialized"));
}

void USLFActionDodge::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] ExecuteAction"));

	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	// Extract DodgeMontages from Action data asset's RelevantData (FInstancedStruct)
	// Use reflection since the data asset may be a Blueprint-derived class
	if (Action)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Action class: %s"), *Action->GetClass()->GetName());

		// Try direct cast first (C++ UPDA_Action)
		if (UPDA_Action* ActionData = Cast<UPDA_Action>(Action))
		{
			if (const FSLFDodgeMontages* MontageData = ActionData->RelevantData.GetPtr<FSLFDodgeMontages>())
			{
				DodgeMontages = *MontageData;
				UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Loaded DodgeMontages from C++ UPDA_Action"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] RelevantData does not contain FSLFDodgeMontages"));
			}
		}
		else
		{
			// Blueprint-derived data asset - use reflection to access RelevantData
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
						UE_LOG(LogTemp, Log, TEXT("[ActionDodge] InstancedStruct contains: %s"),
							StoredStructType ? *StoredStructType->GetName() : TEXT("NULL"));

						// Try direct extraction first (C++ struct)
						if (const FSLFDodgeMontages* MontageData = InstancedStruct->GetPtr<FSLFDodgeMontages>())
						{
							DodgeMontages = *MontageData;
							UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Loaded DodgeMontages via direct GetPtr"));
						}
						else if (StoredStructType)
						{
							// Blueprint struct - extract properties via reflection
							// Properties have GUID suffixes like "Forward_31_GUID" so we search by prefix
							const void* StructData = InstancedStruct->GetMemory();

							auto GetMontageFromPropPrefix = [&](const TCHAR* Prefix) -> UAnimMontage* {
								for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
								{
									FProperty* Prop = *PropIt;
									FString PropName = Prop->GetName();
									if (PropName.StartsWith(Prefix))
									{
										UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Found property %s for prefix %s"), *PropName, Prefix);
										if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
										{
											void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
											UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
											return Cast<UAnimMontage>(Obj);
										}
										else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
										{
											void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
											TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
											if (SoftPtr)
											{
												return Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
											}
										}
									}
								}
								return nullptr;
							};

							DodgeMontages.Fwd = GetMontageFromPropPrefix(TEXT("Forward"));
							DodgeMontages.Bwd = GetMontageFromPropPrefix(TEXT("Backstep"));
							DodgeMontages.Left = GetMontageFromPropPrefix(TEXT("Left"));
							DodgeMontages.Right = GetMontageFromPropPrefix(TEXT("Right"));

							UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Extracted montages via reflection: Fwd=%s, Bwd=%s, Left=%s, Right=%s"),
								DodgeMontages.Fwd ? *DodgeMontages.Fwd->GetName() : TEXT("NULL"),
								DodgeMontages.Bwd ? *DodgeMontages.Bwd->GetName() : TEXT("NULL"),
								DodgeMontages.Left ? *DodgeMontages.Left->GetName() : TEXT("NULL"),
								DodgeMontages.Right ? *DodgeMontages.Right->GetName() : TEXT("NULL"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] InstancedStruct is empty or invalid"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] RelevantData property found but not FInstancedStruct"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] No RelevantData property found on %s"), *Action->GetClass()->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] Action is null"));
	}

	UAnimMontage* MontageToPlay = GetDirectionalDodgeMontage();
	if (MontageToPlay)
	{
		Character->PlayAnimMontage(MontageToPlay);
		UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Playing montage: %s"), *MontageToPlay->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] No montage found for direction"));
	}
}

UAnimMontage* USLFActionDodge::GetDirectionalDodgeMontage()
{
	if (!OwnerActor) return DodgeMontages.Fwd;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return DodgeMontages.Fwd;

	// Get movement input direction
	FVector InputVector = Character->GetCharacterMovement()->GetLastInputVector();

	if (InputVector.IsNearlyZero())
	{
		return DodgeMontages.Bwd;  // Default to backstep
	}

	// Transform to local space
	FVector LocalInput = Character->GetActorRotation().UnrotateVector(InputVector);
	LocalInput.Normalize();

	// Determine direction
	if (FMath::Abs(LocalInput.X) > FMath::Abs(LocalInput.Y))
	{
		return LocalInput.X > 0 ? DodgeMontages.Fwd : DodgeMontages.Bwd;
	}
	else
	{
		return LocalInput.Y > 0 ? DodgeMontages.Right : DodgeMontages.Left;
	}
}
