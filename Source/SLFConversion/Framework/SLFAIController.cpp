// SLFAIController.cpp
#include "SLFAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ASLFAIController::ASLFAIController()
{
}

void ASLFAIController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SLFAIController] BeginPlay"));
}

void ASLFAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("[SLFAIController] OnPossess: %s"), *InPawn->GetName());

	// TODO: Run behavior tree from AI_BehaviorManager component
}
