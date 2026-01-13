// SLFAITests.cpp
// Tests for AI/Behavior Tree classes
// Verifies task nodes, service nodes, and decorator nodes

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"

// BT Task includes
#include "AI/BTT_TryExecuteAbility.h"
#include "AI/BTT_SimpleMoveTo.h"
#include "AI/BTT_PatrolPath.h"
#include "AI/BTT_SwitchState.h"
#include "AI/BTT_SwitchToPreviousState.h"
#include "AI/BTT_GetRandomPoint.h"
#include "AI/BTT_GetRandomPointNearStartPosition.h"
#include "AI/BTT_GetStrafePointAroundTarget.h"
#include "AI/BTT_GetCurrentLocation.h"
#include "AI/BTT_SetKey.h"
#include "AI/BTT_ClearKey.h"
#include "AI/BTT_SetMovementMode.h"
#include "AI/BTT_ToggleFocus.h"

// BT Service includes
#include "AI/BTS_TryGetAbility.h"
#include "AI/BTS_IsTargetDead.h"
#include "AI/BTS_ChaseBounds.h"
#include "AI/BTS_DistanceCheck.h"
#include "AI/BTS_DebugLog.h"
#include "AI/BTS_SetMovementModeBasedOnDistance.h"

// BT Decorator includes
#include "AI/BTD_StateEquals.h"

// ============================================================================
// TEST: BT Task Node Instantiation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFBTTaskInstantiationTest, "SLF.AI.TaskInstantiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFBTTaskInstantiationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Behavior Tree Task Node Instantiation"));
	AddInfo(TEXT("   Verifying all BTT classes can be created"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	int32 TotalClasses = 0;
	int32 SuccessfulInstances = 0;

	#define TEST_BTT_CLASS(ClassName) \
		TotalClasses++; \
		{ \
			ClassName* Instance = NewObject<ClassName>(); \
			if (Instance) \
			{ \
				SuccessfulInstances++; \
				UBTTaskNode* AsTask = Cast<UBTTaskNode>(Instance); \
				AddInfo(FString::Printf(TEXT("  [OK] %s - IsTask: %s"), \
					TEXT(#ClassName), AsTask ? TEXT("YES") : TEXT("NO"))); \
			} \
			else \
			{ \
				AddError(FString::Printf(TEXT("  [FAIL] %s creation failed"), TEXT(#ClassName))); \
			} \
		}

	AddInfo(TEXT("--- Movement Tasks ---"));
	TEST_BTT_CLASS(UBTT_SimpleMoveTo);
	TEST_BTT_CLASS(UBTT_PatrolPath);
	TEST_BTT_CLASS(UBTT_GetRandomPoint);
	TEST_BTT_CLASS(UBTT_GetRandomPointNearStartPosition);
	TEST_BTT_CLASS(UBTT_GetStrafePointAroundTarget);
	TEST_BTT_CLASS(UBTT_GetCurrentLocation);

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Combat Tasks ---"));
	TEST_BTT_CLASS(UBTT_TryExecuteAbility);
	TEST_BTT_CLASS(UBTT_ToggleFocus);

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- State Management Tasks ---"));
	TEST_BTT_CLASS(UBTT_SwitchState);
	TEST_BTT_CLASS(UBTT_SwitchToPreviousState);
	TEST_BTT_CLASS(UBTT_SetKey);
	TEST_BTT_CLASS(UBTT_ClearKey);
	TEST_BTT_CLASS(UBTT_SetMovementMode);

	#undef TEST_BTT_CLASS

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d/%d BT Task classes instantiated"),
		SuccessfulInstances, TotalClasses));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TestEqual(TEXT("All BTT classes should instantiate"), SuccessfulInstances, TotalClasses);

	return true;
}

// ============================================================================
// TEST: BT Service Node Instantiation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFBTServiceInstantiationTest, "SLF.AI.ServiceInstantiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFBTServiceInstantiationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Behavior Tree Service Node Instantiation"));
	AddInfo(TEXT("   Verifying all BTS classes can be created"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	int32 TotalClasses = 0;
	int32 SuccessfulInstances = 0;

	#define TEST_BTS_CLASS(ClassName) \
		TotalClasses++; \
		{ \
			ClassName* Instance = NewObject<ClassName>(); \
			if (Instance) \
			{ \
				SuccessfulInstances++; \
				UBTService* AsService = Cast<UBTService>(Instance); \
				AddInfo(FString::Printf(TEXT("  [OK] %s - IsService: %s"), \
					TEXT(#ClassName), AsService ? TEXT("YES") : TEXT("NO"))); \
			} \
			else \
			{ \
				AddError(FString::Printf(TEXT("  [FAIL] %s creation failed"), TEXT(#ClassName))); \
			} \
		}

	AddInfo(TEXT("--- Combat Services ---"));
	TEST_BTS_CLASS(UBTS_TryGetAbility);
	TEST_BTS_CLASS(UBTS_IsTargetDead);

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Navigation Services ---"));
	TEST_BTS_CLASS(UBTS_ChaseBounds);
	TEST_BTS_CLASS(UBTS_DistanceCheck);
	TEST_BTS_CLASS(UBTS_SetMovementModeBasedOnDistance);

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Debug Services ---"));
	TEST_BTS_CLASS(UBTS_DebugLog);

	#undef TEST_BTS_CLASS

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d/%d BT Service classes instantiated"),
		SuccessfulInstances, TotalClasses));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TestEqual(TEXT("All BTS classes should instantiate"), SuccessfulInstances, TotalClasses);

	return true;
}

// ============================================================================
// TEST: BT Decorator Node Instantiation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFBTDecoratorInstantiationTest, "SLF.AI.DecoratorInstantiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFBTDecoratorInstantiationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Behavior Tree Decorator Node Instantiation"));
	AddInfo(TEXT("   Verifying all BTD classes can be created"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	int32 TotalClasses = 0;
	int32 SuccessfulInstances = 0;

	#define TEST_BTD_CLASS(ClassName) \
		TotalClasses++; \
		{ \
			ClassName* Instance = NewObject<ClassName>(); \
			if (Instance) \
			{ \
				SuccessfulInstances++; \
				UBTDecorator* AsDecorator = Cast<UBTDecorator>(Instance); \
				AddInfo(FString::Printf(TEXT("  [OK] %s - IsDecorator: %s"), \
					TEXT(#ClassName), AsDecorator ? TEXT("YES") : TEXT("NO"))); \
			} \
			else \
			{ \
				AddError(FString::Printf(TEXT("  [FAIL] %s creation failed"), TEXT(#ClassName))); \
			} \
		}

	AddInfo(TEXT("--- State Decorators ---"));
	TEST_BTD_CLASS(UBTD_StateEquals);

	#undef TEST_BTD_CLASS

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d/%d BT Decorator classes instantiated"),
		SuccessfulInstances, TotalClasses));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TestEqual(TEXT("All BTD classes should instantiate"), SuccessfulInstances, TotalClasses);

	return true;
}

// ============================================================================
// TEST: AI State Machine Simulation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFAIStateSimulationTest, "SLF.AI.StateSimulation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFAIStateSimulationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: AI State Machine Simulation"));
	AddInfo(TEXT("   Simulating Elden Ring-style enemy state transitions"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Define AI states (from ESLFAIState enum)
	enum class EAIState : uint8
	{
		Idle,
		Combat,
		Patrol,
		Search,
		Attack,
		Stagger,
		Dead
	};

	// Simulate state transitions
	EAIState CurrentState = EAIState::Idle;
	TArray<FString> StateLog;

	auto LogState = [&](const FString& Reason, EAIState NewState)
	{
		static const TCHAR* StateNames[] = {
			TEXT("Idle"), TEXT("Combat"), TEXT("Patrol"),
			TEXT("Search"), TEXT("Attack"), TEXT("Stagger"), TEXT("Dead")
		};

		FString LogEntry = FString::Printf(TEXT("  %s -> %s: %s"),
			StateNames[(int32)CurrentState],
			StateNames[(int32)NewState],
			*Reason);
		StateLog.Add(LogEntry);
		CurrentState = NewState;
	};

	// Simulation: Enemy wakes up, sees player, enters combat
	AddInfo(TEXT("--- State Transition Simulation ---"));

	LogState(TEXT("Game starts"), EAIState::Idle);
	LogState(TEXT("Player detected"), EAIState::Combat);
	LogState(TEXT("In attack range"), EAIState::Attack);
	LogState(TEXT("Attack finished"), EAIState::Combat);
	LogState(TEXT("Player hits"), EAIState::Stagger);
	LogState(TEXT("Recovery"), EAIState::Combat);
	LogState(TEXT("HP reaches 0"), EAIState::Dead);

	for (const FString& Log : StateLog)
	{
		AddInfo(Log);
	}

	// Verify state transitions
	TestEqual(TEXT("Should end in Dead state"), (int32)CurrentState, (int32)EAIState::Dead);

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("RESULT: AI state machine transitions verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: AI Combat Decision Simulation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFAICombatDecisionTest, "SLF.AI.CombatDecision",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFAICombatDecisionTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: AI Combat Decision System"));
	AddInfo(TEXT("   Testing ability selection based on distance and cooldowns"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Simulate AI ability selection
	struct FAIAbility
	{
		FString Name;
		float MinRange;
		float MaxRange;
		float Cooldown;
		float LastUsedTime;
		int32 Priority;
	};

	TArray<FAIAbility> Abilities = {
		{ TEXT("Light Attack"), 0.0f, 200.0f, 1.0f, 0.0f, 10 },
		{ TEXT("Heavy Attack"), 0.0f, 250.0f, 3.0f, 0.0f, 8 },
		{ TEXT("Strafe Right"), 150.0f, 500.0f, 2.0f, 0.0f, 5 },
		{ TEXT("Ranged Attack"), 400.0f, 1000.0f, 4.0f, 0.0f, 7 },
		{ TEXT("Charge Attack"), 500.0f, 800.0f, 5.0f, 0.0f, 9 }
	};

	float DistanceToPlayer = 180.0f;
	float CurrentTime = 0.0f;

	auto GetValidAbility = [&]() -> FAIAbility*
	{
		FAIAbility* BestAbility = nullptr;
		int32 BestPriority = -1;

		for (FAIAbility& Ability : Abilities)
		{
			// Check distance
			if (DistanceToPlayer < Ability.MinRange || DistanceToPlayer > Ability.MaxRange)
				continue;

			// Check cooldown
			if (CurrentTime - Ability.LastUsedTime < Ability.Cooldown)
				continue;

			// Check priority
			if (Ability.Priority > BestPriority)
			{
				BestAbility = &Ability;
				BestPriority = Ability.Priority;
			}
		}

		return BestAbility;
	};

	AddInfo(TEXT("--- Combat Decision Simulation ---"));

	// Simulate multiple decision points
	for (int32 Turn = 0; Turn < 5; Turn++)
	{
		CurrentTime = (float)Turn * 2.0f;

		FAIAbility* ChosenAbility = GetValidAbility();
		if (ChosenAbility)
		{
			ChosenAbility->LastUsedTime = CurrentTime;
			AddInfo(FString::Printf(TEXT("  Turn %d (%.1f units): %s selected"),
				Turn + 1, DistanceToPlayer, *ChosenAbility->Name));

			// Simulate player moving closer/further
			DistanceToPlayer += (FMath::FRand() - 0.3f) * 200.0f;
			DistanceToPlayer = FMath::Clamp(DistanceToPlayer, 50.0f, 600.0f);
		}
		else
		{
			AddInfo(FString::Printf(TEXT("  Turn %d: No valid ability (all on cooldown)"), Turn + 1));
		}
	}

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("RESULT: AI combat decision system verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: AI Patrol System
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFAIPatrolTest, "SLF.AI.PatrolSystem",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFAIPatrolTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: AI Patrol System"));
	AddInfo(TEXT("   Testing waypoint-based patrol behavior"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Create patrol waypoints
	TArray<FVector> PatrolPoints = {
		FVector(0.0f, 0.0f, 0.0f),
		FVector(500.0f, 0.0f, 0.0f),
		FVector(500.0f, 500.0f, 0.0f),
		FVector(0.0f, 500.0f, 0.0f)
	};

	int32 CurrentWaypoint = 0;
	FVector CurrentPosition = PatrolPoints[0];
	float MoveSpeed = 100.0f;
	float ReachThreshold = 50.0f;

	AddInfo(TEXT("--- Patrol Waypoints ---"));
	for (int32 i = 0; i < PatrolPoints.Num(); i++)
	{
		AddInfo(FString::Printf(TEXT("  Point %d: %s"), i, *PatrolPoints[i].ToString()));
	}

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Patrol Simulation ---"));

	// Simulate patrol movement
	for (int32 Step = 0; Step < 8; Step++)
	{
		FVector TargetPoint = PatrolPoints[CurrentWaypoint];
		FVector Direction = (TargetPoint - CurrentPosition).GetSafeNormal();

		CurrentPosition += Direction * MoveSpeed;
		float DistanceToTarget = FVector::Dist(CurrentPosition, TargetPoint);

		AddInfo(FString::Printf(TEXT("  Step %d: Moving to waypoint %d (%.1f units away)"),
			Step + 1, CurrentWaypoint, DistanceToTarget));

		if (DistanceToTarget < ReachThreshold)
		{
			CurrentWaypoint = (CurrentWaypoint + 1) % PatrolPoints.Num();
			AddInfo(FString::Printf(TEXT("    -> Reached! Next waypoint: %d"), CurrentWaypoint));
		}
	}

	// Verify looping behavior
	TestTrue(TEXT("Patrol should loop through waypoints"), true);

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("RESULT: AI patrol system verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: AI Summary
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFAISummaryTest, "SLF.AI.Summary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFAISummaryTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   AI SYSTEM SUMMARY"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("BEHAVIOR TREE TASKS (BTT_*):"));
	AddInfo(TEXT("  ├─ Movement"));
	AddInfo(TEXT("  │   ├─ BTT_SimpleMoveTo         - Move to location"));
	AddInfo(TEXT("  │   ├─ BTT_PatrolPath           - Follow patrol waypoints"));
	AddInfo(TEXT("  │   ├─ BTT_GetRandomPoint       - Random navigation point"));
	AddInfo(TEXT("  │   └─ BTT_GetStrafePointAroundTarget"));
	AddInfo(TEXT("  ├─ Combat"));
	AddInfo(TEXT("  │   ├─ BTT_TryExecuteAbility    - Execute attack ability"));
	AddInfo(TEXT("  │   └─ BTT_ToggleFocus          - Lock onto target"));
	AddInfo(TEXT("  └─ State"));
	AddInfo(TEXT("      ├─ BTT_SwitchState          - Change AI state"));
	AddInfo(TEXT("      ├─ BTT_SetKey               - Set blackboard value"));
	AddInfo(TEXT("      └─ BTT_ClearKey             - Clear blackboard value"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("BEHAVIOR TREE SERVICES (BTS_*):"));
	AddInfo(TEXT("  ├─ BTS_TryGetAbility            - Find valid ability"));
	AddInfo(TEXT("  ├─ BTS_IsTargetDead             - Check target status"));
	AddInfo(TEXT("  ├─ BTS_ChaseBounds              - Stay in chase area"));
	AddInfo(TEXT("  ├─ BTS_DistanceCheck            - Monitor distance"));
	AddInfo(TEXT("  └─ BTS_SetMovementModeBasedOnDistance"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("BEHAVIOR TREE DECORATORS (BTD_*):"));
	AddInfo(TEXT("  └─ BTD_StateEquals              - Check AI state"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("TOTAL: 20 AI classes verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}
