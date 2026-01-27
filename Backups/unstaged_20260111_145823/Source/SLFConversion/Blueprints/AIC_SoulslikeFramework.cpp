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
#include "BehaviorTree/BTNode.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAIC_SoulslikeFramework::AAIC_SoulslikeFramework()
{
	// Create AI Perception Component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerceptionComponent);

	// Configure default sight sense
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.0f;
	SightConfig->LoseSightRadius = 2500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
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

	BTDebugTimer += DeltaTime;
	if (BTDebugTimer >= BTDebugInterval)
	{
		BTDebugTimer = 0.0f;

		// Get BT component
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(GetBrainComponent());
		if (!BTComp)
		{
			return;
		}

		// Only log if we have a pawn in combat state
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			uint8 State = BB->GetValueAsEnum(FName("State"));
			if (State != 4) // Only log in Combat state
			{
				return;
			}
		}

		FString PawnName = GetPawn() ? GetPawn()->GetName() : TEXT("Unknown");

		UE_LOG(LogTemp, Warning, TEXT("[AIC_BT_DEBUG] %s - BT State:"), *PawnName);
		UE_LOG(LogTemp, Warning, TEXT("  IsRunning: %s, IsPaused: %s"),
			BTComp->IsRunning() ? TEXT("YES") : TEXT("NO"),
			BTComp->IsPaused() ? TEXT("YES") : TEXT("NO"));

		// Get the tree asset
		if (UBehaviorTree* TreeAsset = BTComp->GetCurrentTree())
		{
			UE_LOG(LogTemp, Warning, TEXT("  CurrentTree: %s"), *TreeAsset->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  CurrentTree: NULL"));
		}

		// Get the currently active node
		const UBTNode* ActiveNode = BTComp->GetActiveNode();
		if (ActiveNode)
		{
			UE_LOG(LogTemp, Warning, TEXT("  ActiveNode: %s - %s"),
				*ActiveNode->GetClass()->GetName(),
				*ActiveNode->GetNodeName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  ActiveNode: NULL"));
		}
	}
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
