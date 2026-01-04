// B_Action_Dodge.cpp
// C++ implementation for Blueprint B_Action_Dodge
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_Dodge.json

#include "Blueprints/B_Action_Dodge.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "StructUtils/InstancedStruct.h"

UB_Action_Dodge::UB_Action_Dodge()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Initialized"));
}

void UB_Action_Dodge::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] ExecuteAction"));

	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	// Extract DodgeMontages from Action data asset's RelevantData (FInstancedStruct)
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

						// The data is stored as Blueprint struct FDodgeMontages, not C++ FSLFDodgeMontages
						// Extract properties via reflection from the stored struct
						if (StoredStructType)
						{
							const void* StructData = InstancedStruct->GetMemory();

							// Look for montage properties: Fwd, Bwd, Left, Right
							auto GetMontageFromProp = [&](const TCHAR* PropName) -> UAnimMontage* {
								FProperty* MontageProp = StoredStructType->FindPropertyByName(FName(PropName));
								if (MontageProp)
								{
									if (FObjectProperty* ObjProp = CastField<FObjectProperty>(MontageProp))
									{
										void* PropValueAddr = MontageProp->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
										UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
										return Cast<UAnimMontage>(Obj);
									}
									else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(MontageProp))
									{
										void* PropValueAddr = MontageProp->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
										TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
										if (SoftPtr)
										{
											return Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
										}
									}
								}
								return nullptr;
							};

							DodgeMontages.Fwd = GetMontageFromProp(TEXT("Fwd"));
							DodgeMontages.Bwd = GetMontageFromProp(TEXT("Bwd"));
							DodgeMontages.Left = GetMontageFromProp(TEXT("Left"));
							DodgeMontages.Right = GetMontageFromProp(TEXT("Right"));

							UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Extracted montages: Fwd=%s, Bwd=%s, Left=%s, Right=%s"),
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

UAnimMontage* UB_Action_Dodge::GetDirectionalDodgeMontage()
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

