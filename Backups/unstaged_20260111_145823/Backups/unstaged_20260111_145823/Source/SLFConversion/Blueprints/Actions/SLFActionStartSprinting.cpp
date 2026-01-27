// SLFActionStartSprinting.cpp
// Logic: SetMovementMode(Sprinting), get FSprintCost, StartStaminaLoss, SetIsSprinting(true)
#include "SLFActionStartSprinting.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "AC_ActionManager.h"
#include "SLFPrimaryDataAssets.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"

USLFActionStartSprinting::USLFActionStartSprinting()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Initialized"));
}

void USLFActionStartSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] ExecuteAction"));

	if (!OwnerActor) return;

	// Set movement mode to sprinting via interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(OwnerActor, ESLFMovementType::Sprint);
		UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] MovementMode set to Sprint"));
	}

	// Get FSprintCost from Action->RelevantData
	float TickInterval = 0.1f;
	double StaminaChange = -5.0;

	if (Action)
	{
		// Try to get sprint cost from RelevantData
		if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
		{
			// Direct extraction for C++ data assets
			// (FSprintCost would need to be defined, but we use defaults for now)
			UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Using default sprint cost values"));
		}
		else
		{
			// Blueprint data asset - use reflection to get FSprintCost
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

						// Lambda to get double value by property prefix
						auto GetDoubleFromPropPrefix = [&](const TCHAR* Prefix) -> TOptional<double> {
							for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
							{
								FProperty* Prop = *PropIt;
								if (Prop->GetName().StartsWith(Prefix))
								{
									if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
									{
										void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
										return *static_cast<double*>(PropValueAddr);
									}
									if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
									{
										void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
										return static_cast<double>(*static_cast<float*>(PropValueAddr));
									}
								}
							}
							return TOptional<double>();
						};

						TOptional<double> TickOpt = GetDoubleFromPropPrefix(TEXT("TickInterval"));
						TOptional<double> ChangeOpt = GetDoubleFromPropPrefix(TEXT("Change"));

						if (TickOpt.IsSet()) TickInterval = static_cast<float>(TickOpt.GetValue());
						if (ChangeOpt.IsSet()) StaminaChange = ChangeOpt.GetValue();

						UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Sprint cost - Tick: %.2f, Change: %.2f"), TickInterval, StaminaChange);
					}
				}
			}
		}
	}

	// Start stamina loss via ActionManager
	if (UAC_ActionManager* ActionMgr = GetActionManager())
	{
		ActionMgr->EventStartStaminaLoss(TickInterval, StaminaChange);
		ActionMgr->SetIsSprinting(true);
		UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Stamina loss started, IsSprinting = true"));
	}
}
