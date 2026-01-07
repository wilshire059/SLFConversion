// SLFAIController.cpp
// C++ implementation for AIC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation

#include "Framework/SLFAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

ASLFAIController::ASLFAIController()
{
	// Note: AIPerception component is defined in Blueprint SCS
	// Use inherited GetAIPerceptionComponent() from AAIController
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

	// Note: Behavior tree initialization happens via BPI_AIC::InitializeBehavior
	// Called from AC_AI_BehaviorManager component
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_AIC INTERFACE IMPLEMENTATIONS [1/1]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFAIController::InitializeBehavior_Implementation(UBehaviorTree* BehaviorTree)
{
	// JSON Logic: Event InitializeBehavior -> RunBehaviorTree(BehaviorTree)
	// Comment from Blueprint: "When initialized, run behavior tree. Called from AC_AI_BehaviorManager."

	UE_LOG(LogTemp, Log, TEXT("[SLFAIController] InitializeBehavior - Tree: %s"),
		BehaviorTree ? *BehaviorTree->GetName() : TEXT("null"));

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogTemp, Log, TEXT("[SLFAIController] Running behavior tree: %s"), *BehaviorTree->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFAIController] InitializeBehavior called with null BehaviorTree"));
	}
}
