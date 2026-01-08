// AIBehaviorManagerComponent.cpp
// C++ implementation for AC_AI_BehaviorManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_AI_BehaviorManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         11/11 (initialized in constructor)
// Functions:         7/7 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "AIBehaviorManagerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interfaces/BPI_AIC.h"

#include "BehaviorTree/BehaviorTree.h"

UAIBehaviorManagerComponent::UAIBehaviorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize behavior tree
	Behavior = nullptr;

	// Initialize state
	CurrentState = ESLFAIStates::Idle;
	PreviousState = ESLFAIStates::Idle;
	CurrentTarget = nullptr;

	// Initialize patrol
	PatrolPath = nullptr;

	// Initialize distance thresholds
	MaxChaseDistanceThreshold = 2000.0;
	AttackDistanceThreshold = 200.0;
	StrafeDistanceThreshold = 500.0;
	SpeedAdjustDistanceThreshold = 300.0;

	// Initialize strafe config
	MinimumStrafePointDistanceThreshold = 100.0;
}

void UAIBehaviorManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] BeginPlay on %s - Initial state: %d"),
		*GetOwner()->GetName(), static_cast<int32>(CurrentState));

	// Initialize behavior tree on the AI controller
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController()))
		{
			// Check if the controller implements BPI_AIC interface
			if (Behavior && AIC->GetClass()->ImplementsInterface(UBPI_AIC::StaticClass()))
			{
				UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] Initializing behavior tree: %s"), *Behavior->GetName());
				IBPI_AIC::Execute_InitializeBehavior(AIC, Behavior);
			}
			else if (Behavior)
			{
				// Fallback: directly run behavior tree if interface not implemented
				UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] Running behavior tree directly: %s"), *Behavior->GetName());
				AIC->RunBehaviorTree(Behavior);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] No behavior tree assigned!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] Owner has no AI controller!"));
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE MANAGEMENT [1-2/7]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBehaviorManagerComponent::SetState_Implementation(ESLFAIStates NewState)
{
	if (CurrentState != NewState)
	{
		PreviousState = CurrentState;
		CurrentState = NewState;

		UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] SetState: %d -> %d"),
			static_cast<int32>(PreviousState), static_cast<int32>(CurrentState));

		// Update blackboard state key
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			BB->SetValueAsEnum(FName("AIState"), static_cast<uint8>(CurrentState));
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// TARGET MANAGEMENT [3-4/7]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBehaviorManagerComponent::SetTarget_Implementation(AActor* NewTarget)
{
	CurrentTarget = NewTarget;

	UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] SetTarget: %s"),
		NewTarget ? *NewTarget->GetName() : TEXT("null"));

	// Update blackboard target key
	if (UBlackboardComponent* BB = GetBlackboard())
	{
		BB->SetValueAsObject(FName("TargetActor"), NewTarget);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PATROL [5/7]
// ═══════════════════════════════════════════════════════════════════════════════

void UAIBehaviorManagerComponent::SetPatrolPath_Implementation(AActor* NewPath)
{
	PatrolPath = NewPath;

	UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] SetPatrolPath: %s"),
		NewPath ? *NewPath->GetName() : TEXT("null"));

	// Update blackboard patrol path key
	if (UBlackboardComponent* BB = GetBlackboard())
	{
		BB->SetValueAsObject(FName("PatrolPath"), NewPath);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLACKBOARD [6-7/7]
// ═══════════════════════════════════════════════════════════════════════════════

UBlackboardComponent* UAIBehaviorManagerComponent::GetBlackboard_Implementation()
{
	if (AActor* Owner = GetOwner())
	{
		// Try to get AI controller from pawn
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
			{
				return AIC->GetBlackboardComponent();
			}
		}
	}
	return nullptr;
}

void UAIBehaviorManagerComponent::SetKeyValue_Implementation(FName KeyName, UObject* Value)
{
	if (UBlackboardComponent* BB = GetBlackboard())
	{
		BB->SetValueAsObject(KeyName, Value);
		UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] SetKeyValue: %s = %s"),
			*KeyName.ToString(), Value ? *Value->GetName() : TEXT("null"));
	}
}
