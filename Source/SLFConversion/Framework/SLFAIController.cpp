// SLFAIController.cpp
// C++ implementation for AIC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation

#include "Framework/SLFAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ASLFAIController::ASLFAIController()
{
	// Create AI Perception Component in C++ (was previously in Blueprint SCS)
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

	UE_LOG(LogTemp, Log, TEXT("[SLFAIController] Constructor - AI Perception configured"));
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

void ASLFAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Log blackboard state every 2 seconds
	static float DebugTimer = 0.0f;
	DebugTimer += DeltaTime;
	if (DebugTimer < 2.0f)
	{
		return;
	}
	DebugTimer = 0.0f;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	// Get State as Int
	uint8 StateEnum = BB->GetValueAsEnum(FName("State"));

	// Get TargetActor
	UObject* Target = BB->GetValueAsObject(FName("Target"));

	UE_LOG(LogTemp, Warning, TEXT("[SLFAIController_BB] %s: State=%d, Target=%s"),
		*MyPawn->GetName(),
		static_cast<int32>(StateEnum),
		Target ? *Target->GetName() : TEXT("null"));
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
