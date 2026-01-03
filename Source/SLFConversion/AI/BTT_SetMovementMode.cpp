// BTT_SetMovementMode.cpp
// C++ AI implementation for BTT_SetMovementMode
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - sets AI movement mode via interface

#include "AI/BTT_SetMovementMode.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

UBTT_SetMovementMode::UBTT_SetMovementMode()
{
	NodeName = TEXT("Set Movement Mode");
}

EBTNodeResult::Type UBTT_SetMovementMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// From Blueprint: Call SetMovementMode via BPI_GenericCharacter interface
	if (ControlledPawn->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(ControlledPawn, MovementMode);
		UE_LOG(LogTemp, Log, TEXT("UBTT_SetMovementMode::ExecuteTask - Set movement mode %d on %s"),
			(int32)MovementMode, *ControlledPawn->GetName());
	}

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_SetMovementMode::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
