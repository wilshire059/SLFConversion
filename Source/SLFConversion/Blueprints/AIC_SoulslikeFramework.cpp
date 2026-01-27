// AIC_SoulslikeFramework.cpp
// C++ implementation for Blueprint AIC_SoulslikeFramework
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AIC_SoulslikeFramework
// ═══════════════════════════════════════════════════════════════════════════════
// Functions:         1/1 implemented (InitializeBehavior)
// Components:        AIPerceptionComponent created in constructor
// ═══════════════════════════════════════════════════════════════════════════════
//
// 20-PASS VALIDATION: 2026-01-11 - Fixed inheritance from UObject -> AAIController

#include "Blueprints/AIC_SoulslikeFramework.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAIC_SoulslikeFramework::AAIC_SoulslikeFramework()
{
	// Create AI Perception Component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerceptionComponent);

	// Configure default sight sense
	// SOULS-LIKE: Enemies have 180 degree field of view (can see anything in front hemisphere)
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.0f;
	SightConfig->LoseSightRadius = 2500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 180.0f;  // Full hemisphere (was 90)
	SightConfig->SetMaxAge(5.0f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	UE_LOG(LogTemp, Log, TEXT("[AIC_SoulslikeFramework] Constructor - AI Perception configured"));
}

void AAIC_SoulslikeFramework::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[AIC_SoulslikeFramework] BeginPlay"));
}

void AAIC_SoulslikeFramework::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Log, TEXT("[AIC_SoulslikeFramework] OnPossess - Pawn: %s"),
		InPawn ? *InPawn->GetName() : TEXT("null"));
}

void AAIC_SoulslikeFramework::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// State machine now handles all AI logic - see USLFAIStateMachineComponent
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_AIC INTERFACE IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════════

void AAIC_SoulslikeFramework::InitializeBehavior_Implementation(UBehaviorTree* BehaviorTree)
{
	if (!BehaviorTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIC_SoulslikeFramework] InitializeBehavior - No behavior tree provided!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[AIC_SoulslikeFramework] InitializeBehavior - Starting tree: %s"),
		*BehaviorTree->GetName());

	// Run the behavior tree
	bool bSuccess = RunBehaviorTree(BehaviorTree);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("[AIC_SoulslikeFramework] Behavior tree started successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AIC_SoulslikeFramework] Failed to start behavior tree!"));
	}
}
