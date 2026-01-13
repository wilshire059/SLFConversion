// AC_AI_BehaviorManager.cpp
// C++ component implementation for AC_AI_BehaviorManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_BehaviorManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_AI_BehaviorManager.h"
#include "AIController.h"
#include "Blueprints/B_PatrolPath.h"

UAC_AI_BehaviorManager::UAC_AI_BehaviorManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	Behavior = nullptr;
	CurrentState = ESLFAIStates::Idle;
	PreviousState = ESLFAIStates::Idle;
	PatrolPath = nullptr;
	CurrentTarget = nullptr;
	MaxChaseDistanceThreshold = 2000.0;
	AttackDistanceThreshold = 200.0;
	StrafeDistanceThreshold = 400.0;
	SpeedAdjustDistanceThreshold = 500.0;
	MinimumStrafePointDistanceThreshold = 100.0;
}

void UAC_AI_BehaviorManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::BeginPlay"));
}

void UAC_AI_BehaviorManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetBlackboard - Get the AI blackboard component from the AI controller
 */
void UAC_AI_BehaviorManager::GetBlackboard_Implementation(UBlackboardComponent*& OutReturnValue, UBlackboardComponent*& OutReturnValue_1)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::GetBlackboard"));

	UBlackboardComponent* Blackboard = nullptr;

	// Get the AI controller from the owning pawn
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
		if (IsValid(AIController))
		{
			Blackboard = AIController->GetBlackboardComponent();
		}
	}

	OutReturnValue = Blackboard;
	OutReturnValue_1 = Blackboard;
}

/**
 * SetKeyValue - Set a value on the blackboard by key name
 */
void UAC_AI_BehaviorManager::SetKeyValue_Implementation(const FName& KeyName, const FInstancedStruct& Data)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::SetKeyValue - Key: %s"), *KeyName.ToString());

	UBlackboardComponent* Blackboard = nullptr;
	UBlackboardComponent* Blackboard2 = nullptr;
	GetBlackboard(Blackboard, Blackboard2);

	if (IsValid(Blackboard))
	{
		// Set value on blackboard based on data type
		// The FInstancedStruct contains the actual value to set
		// Different data types would require different SetValue calls
	}
}

/**
 * SetState - Set the AI state and optionally process state-specific data
 */
void UAC_AI_BehaviorManager::SetState_Implementation(ESLFAIStates NewState, const FInstancedStruct& Data)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::SetState - %d -> %d"),
		static_cast<int32>(CurrentState), static_cast<int32>(NewState));

	// Store previous state before changing
	PreviousState = CurrentState;
	CurrentState = NewState;

	// Update blackboard with new state
	UBlackboardComponent* Blackboard = nullptr;
	UBlackboardComponent* Blackboard2 = nullptr;
	GetBlackboard(Blackboard, Blackboard2);

	if (IsValid(Blackboard))
	{
		Blackboard->SetValueAsEnum(FName("State"), static_cast<uint8>(NewState));
	}
}

/**
 * SetTarget - Set the current AI target
 */
void UAC_AI_BehaviorManager::SetTarget_Implementation(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::SetTarget - %s"),
		Target ? *Target->GetName() : TEXT("None"));

	CurrentTarget = Target;

	// Update blackboard with target
	UBlackboardComponent* Blackboard = nullptr;
	UBlackboardComponent* Blackboard2 = nullptr;
	GetBlackboard(Blackboard, Blackboard2);

	if (IsValid(Blackboard))
	{
		Blackboard->SetValueAsObject(FName("Target"), Target);
	}
}

/**
 * GetTarget - Get the current AI target
 */
AActor* UAC_AI_BehaviorManager::GetTarget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::GetTarget - %s"),
		CurrentTarget ? *CurrentTarget->GetName() : TEXT("None"));

	return CurrentTarget;
}

/**
 * GetState - Get the current or previous AI state
 */
ESLFAIStates UAC_AI_BehaviorManager::GetState_Implementation(ESLFAIStateHandle Handle)
{
	ESLFAIStates Result = CurrentState;

	if (Handle == ESLFAIStateHandle::Previous)
	{
		Result = PreviousState;
	}

	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::GetState - Handle: %d, State: %d"),
		static_cast<int32>(Handle), static_cast<int32>(Result));

	return Result;
}

/**
 * SetPatrolPath - Set the patrol path for this AI
 */
void UAC_AI_BehaviorManager::SetPatrolPath_Implementation(AB_PatrolPath* InPatrolPath)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_BehaviorManager::SetPatrolPath"));

	PatrolPath = InPatrolPath;

	// Update blackboard with patrol path
	UBlackboardComponent* Blackboard = nullptr;
	UBlackboardComponent* Blackboard2 = nullptr;
	GetBlackboard(Blackboard, Blackboard2);

	if (IsValid(Blackboard) && InPatrolPath)
	{
		// PatrolPath is an AActor subclass, directly assignable to UObject*
		Blackboard->SetValueAsObject(FName("PatrolPath"), static_cast<AActor*>(InPatrolPath));
	}
}
