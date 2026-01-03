// SLFBTTaskTryExecuteAbility.cpp
// C++ AI implementation for SLFBTTaskTryExecuteAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated - tries to execute AI ability by tag

#include "SLFBTTaskTryExecuteAbility.h"
#include "AIController.h"
#include "Components/AC_AI_BehaviorManager.h"
#include "Components/AICombatManagerComponent.h"

USLFBTTaskTryExecuteAbility::USLFBTTaskTryExecuteAbility()
{
	NodeName = "Try Execute Ability";
}

EBTNodeResult::Type USLFBTTaskTryExecuteAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UE_LOG(LogTemp, Log, TEXT("[BTT_TryExecuteAbility] Attempting ability: %s"), *AbilityTag.ToString());

	// Get AI_CombatManager component and try to get ability
	UAICombatManagerComponent* AICombatManager = ControlledPawn->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		UDataAsset* OutAbility = nullptr;
		bool bSuccess = AICombatManager->TryGetAbility(OutAbility);
		return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
