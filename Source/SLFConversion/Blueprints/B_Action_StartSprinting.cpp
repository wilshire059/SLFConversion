// B_Action_StartSprinting.cpp
// C++ implementation for Blueprint B_Action_StartSprinting
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Action_StartSprinting.json

#include "Blueprints/B_Action_StartSprinting.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/AC_ActionManager.h"
#include "StructUtils/InstancedStruct.h"

UB_Action_StartSprinting::UB_Action_StartSprinting()
{
}

void UB_Action_StartSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] ExecuteAction"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionStartSprinting] No OwnerActor"));
		return;
	}

	// Set movement mode to Sprinting via BPI_GenericCharacter interface
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(OwnerActor, ESLFMovementType::Sprint);
		UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] MovementMode set to Sprint"));
	}

	// Get FSprintCost from Action->RelevantData (TickInterval and Change)
	float TickInterval = 0.1f;  // Default values
	double StaminaChange = -5.0;

	if (Action)
	{
		// Try to get RelevantData from Action using reflection
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

					// Extract TickInterval and Change from FSprintCost struct
					for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
					{
						FProperty* Prop = *PropIt;
						if (Prop->GetName().StartsWith(TEXT("TickInterval")))
						{
							if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
							{
								void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
								TickInterval = static_cast<float>(*static_cast<double*>(PropValueAddr));
							}
							else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
							{
								void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
								TickInterval = *static_cast<float*>(PropValueAddr);
							}
						}
						else if (Prop->GetName().StartsWith(TEXT("Change")))
						{
							if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
							{
								void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
								StaminaChange = *static_cast<double*>(PropValueAddr);
							}
							else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
							{
								void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
								StaminaChange = static_cast<double>(*static_cast<float*>(PropValueAddr));
							}
						}
					}
					UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] SprintCost - Tick: %.2f, Change: %.2f"), TickInterval, StaminaChange);
				}
			}
		}
	}

	// Start stamina loss and set sprinting flag via ActionManager
	UAC_ActionManager* ActionMgr = GetActionManager();
	if (ActionMgr)
	{
		ActionMgr->EventStartStaminaLoss(TickInterval, StaminaChange);
		ActionMgr->SetIsSprinting(true);
		UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Stamina loss started, IsSprinting = true"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionStartSprinting] No ActionManager found"));
	}
}
