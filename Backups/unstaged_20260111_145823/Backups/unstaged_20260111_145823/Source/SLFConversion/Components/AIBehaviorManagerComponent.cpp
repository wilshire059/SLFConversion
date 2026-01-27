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
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "Interfaces/BPI_AIC.h"
#include "UObject/ConstructorHelpers.h"

UAIBehaviorManagerComponent::UAIBehaviorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f; // Tick every 0.5s for debug logging

	// Load default behavior tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> DefaultBehaviorTree(
		TEXT("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy.BT_Enemy"));
	if (DefaultBehaviorTree.Succeeded())
	{
		Behavior = DefaultBehaviorTree.Object;
	}
	else
	{
		Behavior = nullptr;
	}

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

void UAIBehaviorManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DebugLogTimer += DeltaTime;
	if (DebugLogTimer >= DebugLogInterval)
	{
		DebugLogTimer = 0.0f;

		// Log blackboard state
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			uint8 StateValue = BB->GetValueAsEnum(FName("State"));
			UObject* TargetValue = BB->GetValueAsObject(FName("Target"));
			bool bInCombatValue = BB->GetValueAsBool(FName("InCombat"));
			bool bCanAttack = BB->GetValueAsBool(FName("CanAttack"));
			bool bCanStrafe = BB->GetValueAsBool(FName("CanStrafe"));

			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] TICK DEBUG on %s:"),
				*GetOwner()->GetName());
			UE_LOG(LogTemp, Warning, TEXT("  Blackboard State=%d (Combat=4), InCombat=%s, Target=%s"),
				StateValue,
				bInCombatValue ? TEXT("TRUE") : TEXT("FALSE"),
				TargetValue ? *TargetValue->GetName() : TEXT("null"));
			UE_LOG(LogTemp, Warning, TEXT("  CanAttack=%s, CanStrafe=%s"),
				bCanAttack ? TEXT("TRUE") : TEXT("FALSE"),
				bCanStrafe ? TEXT("TRUE") : TEXT("FALSE"));

			// Check if behavior tree is running
			if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
			{
				if (AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController()))
				{
					if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
					{
						UE_LOG(LogTemp, Warning, TEXT("  BrainComponent: %s, IsRunning: %s, IsPaused: %s"),
							*BrainComp->GetClass()->GetName(),
							BrainComp->IsRunning() ? TEXT("YES") : TEXT("NO"),
							BrainComp->IsPaused() ? TEXT("YES") : TEXT("NO"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("  BrainComponent: NULL"));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] TICK DEBUG on %s: No blackboard!"),
				*GetOwner()->GetName());
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

		UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] SetState: %d -> %d"),
			static_cast<int32>(PreviousState), static_cast<int32>(CurrentState));

		// Update blackboard state key - use "State" which is the standard key name
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			BB->SetValueAsEnum(FName("State"), static_cast<uint8>(CurrentState));
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] Updated blackboard State to %d"), static_cast<int32>(CurrentState));

			// Also update InCombat bool when entering/leaving Combat state
			bool bInCombat = (CurrentState == ESLFAIStates::Combat);
			BB->SetValueAsBool(FName("InCombat"), bInCombat);
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] Updated blackboard InCombat to %s"), bInCombat ? TEXT("TRUE") : TEXT("FALSE"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] GetBlackboard() returned null!"));
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

	// Update blackboard target key (BB_Standard uses "Target" not "TargetActor")
	if (UBlackboardComponent* BB = GetBlackboard())
	{
		BB->SetValueAsObject(FName("Target"), NewTarget);
		UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] Updated blackboard Target"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] SetTarget: GetBlackboard() returned null!"));
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
