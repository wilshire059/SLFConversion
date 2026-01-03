// SLFBTTaskToggleFocus.cpp
#include "SLFBTTaskToggleFocus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USLFBTTaskToggleFocus::USLFBTTaskToggleFocus()
{
	NodeName = "Toggle Focus";
}

EBTNodeResult::Type USLFBTTaskToggleFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	if (bEnableFocus)
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			AActor* FocusActor = Cast<AActor>(Blackboard->GetValueAsObject(FocusActorKey.SelectedKeyName));
			if (FocusActor)
			{
				AIController->SetFocus(FocusActor);
				UE_LOG(LogTemp, Log, TEXT("[BTT_ToggleFocus] Focus ON: %s"), *FocusActor->GetName());
			}
		}
	}
	else
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		UE_LOG(LogTemp, Log, TEXT("[BTT_ToggleFocus] Focus OFF"));
	}

	return EBTNodeResult::Succeeded;
}
