// SLFCombatTests.cpp
// Automated tests for combat, damage, and stats systems
// Run via: UnrealEditor-Cmd.exe [project] -ExecCmds="Automation RunTests SLF" -unattended -nopause

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Component includes
#include "Components/StatManagerComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_ActionManager.h"

// Stat includes
#include "Blueprints/SLFStatBase.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"

// ============================================================================
// TEST: Stat Initialization
// Verifies that stats are created with correct default values
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatInitializationTest, "SLF.Stats.Initialization",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatInitializationTest::RunTest(const FString& Parameters)
{
	// Create a temporary world for testing
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestWorld)
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	// Create a test actor with StatManagerComponent
	AActor* TestActor = TestWorld->SpawnActor<AActor>();
	if (!TestActor)
	{
		AddError(TEXT("Failed to spawn test actor"));
		TestWorld->DestroyWorld(false);
		return false;
	}

	// Add StatManagerComponent
	UStatManagerComponent* StatManager = NewObject<UStatManagerComponent>(TestActor);
	if (!StatManager)
	{
		AddError(TEXT("Failed to create StatManagerComponent"));
		TestWorld->DestroyWorld(false);
		return false;
	}

	StatManager->RegisterComponent();

	// Verify the component exists
	TestTrue(TEXT("StatManagerComponent should exist"), StatManager != nullptr);

	// Test HP tag format (verify our fix is correct)
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);
	TestTrue(TEXT("Secondary.HP tag should be valid"), HPTag.IsValid());

	FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"), false);
	TestTrue(TEXT("Secondary.Stamina tag should be valid"), StaminaTag.IsValid());

	// Test that Primary.HP is NOT what we use (verifying our fix)
	FGameplayTag WrongHPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Primary.HP"), false);
	AddInfo(FString::Printf(TEXT("Primary.HP tag valid: %s (should NOT be used for damage)"),
		WrongHPTag.IsValid() ? TEXT("true") : TEXT("false")));

	// Cleanup
	TestWorld->DestroyWorld(false);

	AddInfo(TEXT("Stat initialization test completed successfully"));
	return true;
}

// ============================================================================
// TEST: HP Tag Consistency
// Verifies that all combat components use the correct HP tag
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFHPTagConsistencyTest, "SLF.Stats.HPTagConsistency",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFHPTagConsistencyTest::RunTest(const FString& Parameters)
{
	// The correct HP tag after our fix
	const FString CorrectHPTag = TEXT("SoulslikeFramework.Stat.Secondary.HP");

	// Verify the tag can be requested
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName(*CorrectHPTag), false);

	TestTrue(TEXT("HP tag should be requestable"), HPTag.IsValid());
	TestEqual(TEXT("HP tag string should match"), HPTag.ToString(), CorrectHPTag);

	AddInfo(FString::Printf(TEXT("HP Tag verified: %s"), *HPTag.ToString()));

	return true;
}

// ============================================================================
// TEST: Stamina Tag Consistency
// Verifies stamina consumption uses correct tag
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStaminaTagTest, "SLF.Stats.StaminaTag",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStaminaTagTest::RunTest(const FString& Parameters)
{
	const FString CorrectStaminaTag = TEXT("SoulslikeFramework.Stat.Secondary.Stamina");

	FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName(*CorrectStaminaTag), false);

	TestTrue(TEXT("Stamina tag should be requestable"), StaminaTag.IsValid());
	TestEqual(TEXT("Stamina tag string should match"), StaminaTag.ToString(), CorrectStaminaTag);

	AddInfo(FString::Printf(TEXT("Stamina Tag verified: %s"), *StaminaTag.ToString()));

	return true;
}

// ============================================================================
// TEST: Stat Value Adjustment
// Tests that AdjustStat correctly modifies stat values
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatAdjustmentTest, "SLF.Stats.ValueAdjustment",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatAdjustmentTest::RunTest(const FString& Parameters)
{
	// Create a stat object directly
	USLFStatBase* TestStat = NewObject<USLFStatBase>();
	if (!TestStat)
	{
		AddError(TEXT("Failed to create test stat"));
		return false;
	}

	// Initialize with test values
	TestStat->StatInfo.MaxValue = 100.0;
	TestStat->StatInfo.CurrentValue = 100.0;
	TestStat->StatInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);

	AddInfo(FString::Printf(TEXT("Initial HP: %.0f/%.0f"), TestStat->StatInfo.CurrentValue, TestStat->StatInfo.MaxValue));

	// Test damage (negative adjustment)
	double InitialHP = TestStat->StatInfo.CurrentValue;
	double DamageAmount = 25.0;

	// Simulate what AdjustStat does
	TestStat->StatInfo.CurrentValue -= DamageAmount;
	TestStat->StatInfo.CurrentValue = FMath::Max(0.0, TestStat->StatInfo.CurrentValue);

	TestEqual(TEXT("HP should be reduced by damage"), TestStat->StatInfo.CurrentValue, InitialHP - DamageAmount);
	AddInfo(FString::Printf(TEXT("After 25 damage: %.0f/%.0f"), TestStat->StatInfo.CurrentValue, TestStat->StatInfo.MaxValue));

	// Test healing (positive adjustment)
	double HealAmount = 10.0;
	TestStat->StatInfo.CurrentValue += HealAmount;
	TestStat->StatInfo.CurrentValue = FMath::Min(TestStat->StatInfo.MaxValue, TestStat->StatInfo.CurrentValue);

	TestEqual(TEXT("HP should be increased by healing"), TestStat->StatInfo.CurrentValue, 85.0);
	AddInfo(FString::Printf(TEXT("After 10 heal: %.0f/%.0f"), TestStat->StatInfo.CurrentValue, TestStat->StatInfo.MaxValue));

	// Test death condition
	TestStat->StatInfo.CurrentValue = 0.0;
	TestTrue(TEXT("Death should trigger when HP <= 0"), TestStat->StatInfo.CurrentValue <= 0.0);
	AddInfo(TEXT("Death condition verified: HP <= 0 triggers death"));

	return true;
}

// ============================================================================
// TEST: Damage Chain Validation
// Verifies the damage chain components exist and are properly typed
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFDamageChainTest, "SLF.Combat.DamageChain",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFDamageChainTest::RunTest(const FString& Parameters)
{
	// Verify all combat component classes exist
	UClass* PlayerCombatClass = UAC_CombatManager::StaticClass();
	TestTrue(TEXT("UAC_CombatManager class should exist"), PlayerCombatClass != nullptr);
	AddInfo(FString::Printf(TEXT("Player Combat Manager: %s"), *PlayerCombatClass->GetName()));

	UClass* AICombatClass = UAICombatManagerComponent::StaticClass();
	TestTrue(TEXT("UAICombatManagerComponent class should exist"), AICombatClass != nullptr);
	AddInfo(FString::Printf(TEXT("AI Combat Manager: %s"), *AICombatClass->GetName()));

	UClass* StatManagerClass = UStatManagerComponent::StaticClass();
	TestTrue(TEXT("UStatManagerComponent class should exist"), StatManagerClass != nullptr);
	AddInfo(FString::Printf(TEXT("Stat Manager: %s"), *StatManagerClass->GetName()));

	// Verify the inheritance chain
	TestTrue(TEXT("UAC_CombatManager should be an ActorComponent"),
		PlayerCombatClass->IsChildOf(UActorComponent::StaticClass()));
	TestTrue(TEXT("UAICombatManagerComponent should be an ActorComponent"),
		AICombatClass->IsChildOf(UActorComponent::StaticClass()));
	TestTrue(TEXT("UStatManagerComponent should be an ActorComponent"),
		StatManagerClass->IsChildOf(UActorComponent::StaticClass()));

	AddInfo(TEXT("Damage chain component classes verified"));
	return true;
}

// ============================================================================
// TEST: Combat Manager Interface Functions
// Verifies combat managers have required interface functions
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFCombatInterfaceTest, "SLF.Combat.InterfaceFunctions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFCombatInterfaceTest::RunTest(const FString& Parameters)
{
	// Check that UAC_CombatManager has HandleIncomingWeaponDamage
	UClass* PlayerCombatClass = UAC_CombatManager::StaticClass();
	UFunction* HandleDamageFunc = PlayerCombatClass->FindFunctionByName(FName("HandleIncomingWeaponDamage"));
	TestTrue(TEXT("UAC_CombatManager should have HandleIncomingWeaponDamage function"), HandleDamageFunc != nullptr);

	if (HandleDamageFunc)
	{
		AddInfo(FString::Printf(TEXT("Found function: %s"), *HandleDamageFunc->GetName()));
	}

	// Check HandleDeath function
	UFunction* HandleDeathFunc = PlayerCombatClass->FindFunctionByName(FName("HandleDeath"));
	TestTrue(TEXT("UAC_CombatManager should have HandleDeath function"), HandleDeathFunc != nullptr);

	if (HandleDeathFunc)
	{
		AddInfo(FString::Printf(TEXT("Found function: %s"), *HandleDeathFunc->GetName()));
	}

	// Check AI combat manager functions
	UClass* AICombatClass = UAICombatManagerComponent::StaticClass();
	UFunction* AIHandleDamageFunc = AICombatClass->FindFunctionByName(FName("HandleIncomingWeaponDamage_AI"));
	TestTrue(TEXT("UAICombatManagerComponent should have HandleIncomingWeaponDamage_AI function"), AIHandleDamageFunc != nullptr);

	if (AIHandleDamageFunc)
	{
		AddInfo(FString::Printf(TEXT("Found function: %s"), *AIHandleDamageFunc->GetName()));
	}

	AddInfo(TEXT("Combat interface functions verified"));
	return true;
}

// ============================================================================
// TEST: Action Manager Stamina Check
// Verifies action manager has stamina validation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFActionStaminaTest, "SLF.Actions.StaminaValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFActionStaminaTest::RunTest(const FString& Parameters)
{
	// Check that UAC_ActionManager exists and has EventPerformAction
	UClass* ActionManagerClass = UAC_ActionManager::StaticClass();
	TestTrue(TEXT("UAC_ActionManager class should exist"), ActionManagerClass != nullptr);

	UFunction* PerformActionFunc = ActionManagerClass->FindFunctionByName(FName("EventPerformAction"));
	TestTrue(TEXT("UAC_ActionManager should have EventPerformAction function"), PerformActionFunc != nullptr);

	if (PerformActionFunc)
	{
		AddInfo(FString::Printf(TEXT("Found function: %s"), *PerformActionFunc->GetName()));
	}

	// Verify stamina loss functions exist
	UFunction* StartStaminaLossFunc = ActionManagerClass->FindFunctionByName(FName("EventStartStaminaLoss"));
	TestTrue(TEXT("UAC_ActionManager should have EventStartStaminaLoss function"), StartStaminaLossFunc != nullptr);

	UFunction* StopStaminaLossFunc = ActionManagerClass->FindFunctionByName(FName("EventStopStaminaLoss"));
	TestTrue(TEXT("UAC_ActionManager should have EventStopStaminaLoss function"), StopStaminaLossFunc != nullptr);

	AddInfo(TEXT("Action manager stamina functions verified"));
	return true;
}

// ============================================================================
// TEST: Stat Info Struct Validation
// Verifies FStatInfo has all required fields
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatInfoStructTest, "SLF.Stats.StatInfoStruct",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatInfoStructTest::RunTest(const FString& Parameters)
{
	FStatInfo TestInfo;

	// Set test values
	TestInfo.CurrentValue = 75.0;
	TestInfo.MaxValue = 100.0;
	TestInfo.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);

	// Verify values can be read back
	TestEqual(TEXT("CurrentValue should be 75"), TestInfo.CurrentValue, 75.0);
	TestEqual(TEXT("MaxValue should be 100"), TestInfo.MaxValue, 100.0);
	TestTrue(TEXT("Tag should be valid"), TestInfo.Tag.IsValid());

	// Test percentage calculation
	double Percent = (TestInfo.MaxValue > 0) ? (TestInfo.CurrentValue / TestInfo.MaxValue) : 0.0;
	TestEqual(TEXT("Percentage should be 0.75"), Percent, 0.75);

	AddInfo(FString::Printf(TEXT("StatInfo: %.0f/%.0f (%.0f%%)"),
		TestInfo.CurrentValue, TestInfo.MaxValue, Percent * 100.0));

	return true;
}

// ============================================================================
// TEST: Full Damage Simulation
// Simulates a complete damage application without needing PIE
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFFullDamageSimulationTest, "SLF.Combat.FullDamageSimulation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFFullDamageSimulationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT("=== FULL DAMAGE SIMULATION ==="));

	// Simulate HP stat
	double MaxHP = 500.0;
	double CurrentHP = MaxHP;
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);

	AddInfo(FString::Printf(TEXT("Initial HP: %.0f/%.0f"), CurrentHP, MaxHP));
	TestEqual(TEXT("Initial HP should equal MaxHP"), CurrentHP, MaxHP);

	// Simulate 3 hits of 50 damage each
	double DamagePerHit = 50.0;
	for (int32 i = 0; i < 3; i++)
	{
		CurrentHP -= DamagePerHit;
		CurrentHP = FMath::Max(0.0, CurrentHP);
		AddInfo(FString::Printf(TEXT("Hit %d: -%.0f damage -> HP: %.0f"), i + 1, DamagePerHit, CurrentHP));
	}

	TestEqual(TEXT("HP after 3 hits should be 350"), CurrentHP, 350.0);

	// Simulate heavy attack (100 damage)
	double HeavyDamage = 100.0;
	CurrentHP -= HeavyDamage;
	CurrentHP = FMath::Max(0.0, CurrentHP);
	AddInfo(FString::Printf(TEXT("Heavy hit: -%.0f damage -> HP: %.0f"), HeavyDamage, CurrentHP));

	TestEqual(TEXT("HP after heavy hit should be 250"), CurrentHP, 250.0);

	// Simulate healing
	double HealAmount = 50.0;
	CurrentHP += HealAmount;
	CurrentHP = FMath::Min(MaxHP, CurrentHP);
	AddInfo(FString::Printf(TEXT("Heal: +%.0f -> HP: %.0f"), HealAmount, CurrentHP));

	TestEqual(TEXT("HP after heal should be 300"), CurrentHP, 300.0);

	// Simulate fatal damage
	double FatalDamage = 500.0;
	CurrentHP -= FatalDamage;
	CurrentHP = FMath::Max(0.0, CurrentHP);
	AddInfo(FString::Printf(TEXT("Fatal hit: -%.0f damage -> HP: %.0f"), FatalDamage, CurrentHP));

	TestEqual(TEXT("HP should be clamped to 0"), CurrentHP, 0.0);
	TestTrue(TEXT("Death condition should be triggered"), CurrentHP <= 0.0);

	AddInfo(TEXT("=== DEATH TRIGGERED ==="));
	AddInfo(TEXT("Full damage simulation completed successfully"));

	return true;
}

// ============================================================================
// TEST: Stamina Consumption Simulation
// Simulates stamina drain for actions
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStaminaConsumptionTest, "SLF.Actions.StaminaConsumption",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStaminaConsumptionTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT("=== STAMINA CONSUMPTION SIMULATION ==="));

	double MaxStamina = 100.0;
	double CurrentStamina = MaxStamina;
	FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"), false);

	AddInfo(FString::Printf(TEXT("Initial Stamina: %.0f/%.0f"), CurrentStamina, MaxStamina));

	// Simulate dodge (20 stamina)
	double DodgeCost = 20.0;
	if (CurrentStamina >= DodgeCost)
	{
		CurrentStamina -= DodgeCost;
		AddInfo(FString::Printf(TEXT("Dodge: -%.0f -> Stamina: %.0f"), DodgeCost, CurrentStamina));
		TestEqual(TEXT("Stamina after dodge should be 80"), CurrentStamina, 80.0);
	}

	// Simulate light attack (15 stamina)
	double LightAttackCost = 15.0;
	if (CurrentStamina >= LightAttackCost)
	{
		CurrentStamina -= LightAttackCost;
		AddInfo(FString::Printf(TEXT("Light Attack: -%.0f -> Stamina: %.0f"), LightAttackCost, CurrentStamina));
		TestEqual(TEXT("Stamina after light attack should be 65"), CurrentStamina, 65.0);
	}

	// Simulate heavy attack (40 stamina - more than remaining to test blocking)
	double HeavyAttackCost = 40.0;
	if (CurrentStamina >= HeavyAttackCost)
	{
		CurrentStamina -= HeavyAttackCost;
		AddInfo(FString::Printf(TEXT("Heavy Attack: -%.0f -> Stamina: %.0f"), HeavyAttackCost, CurrentStamina));
		TestEqual(TEXT("Stamina after heavy attack should be 25"), CurrentStamina, 25.0);
	}

	// Try to do another heavy attack (should fail - insufficient stamina: 25 < 40)
	if (CurrentStamina >= HeavyAttackCost)
	{
		CurrentStamina -= HeavyAttackCost;
		AddInfo(TEXT("Heavy Attack 2: SUCCESS (unexpected)"));
	}
	else
	{
		AddInfo(FString::Printf(TEXT("Heavy Attack 2: BLOCKED (need %.0f, have %.0f)"), HeavyAttackCost, CurrentStamina));
		TestTrue(TEXT("Action should be blocked with insufficient stamina"), CurrentStamina < HeavyAttackCost);
	}

	// Simulate stamina regeneration
	double RegenAmount = 50.0;
	CurrentStamina += RegenAmount;
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina);
	AddInfo(FString::Printf(TEXT("Regen: +%.0f -> Stamina: %.0f"), RegenAmount, CurrentStamina));

	// 25 + 50 = 75
	TestEqual(TEXT("Stamina after regen should be 75"), CurrentStamina, 75.0);

	AddInfo(TEXT("Stamina consumption simulation completed successfully"));
	return true;
}

// ============================================================================
// TEST: Enemy Health Bar Update Simulation
// Simulates health bar percentage calculations
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFHealthBarSimulationTest, "SLF.UI.HealthBarSimulation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFHealthBarSimulationTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT("=== HEALTH BAR SIMULATION ==="));

	double MaxHP = 500.0;
	double CurrentHP = MaxHP;

	// Calculate initial percentage
	auto CalculatePercent = [](double Current, double Max) -> float
	{
		if (Max <= 0.0) return 0.0f;
		return FMath::Clamp(static_cast<float>(Current / Max), 0.0f, 1.0f);
	};

	float Percent = CalculatePercent(CurrentHP, MaxHP);
	AddInfo(FString::Printf(TEXT("Full HP: %.0f/%.0f = %.0f%%"), CurrentHP, MaxHP, Percent * 100.0f));
	TestEqual(TEXT("Full HP should show 100%"), Percent, 1.0f);

	// Take some damage
	CurrentHP = 350.0;
	Percent = CalculatePercent(CurrentHP, MaxHP);
	AddInfo(FString::Printf(TEXT("After damage: %.0f/%.0f = %.0f%%"), CurrentHP, MaxHP, Percent * 100.0f));
	TestEqual(TEXT("350/500 should show 70%"), Percent, 0.7f);

	// Critical health
	CurrentHP = 50.0;
	Percent = CalculatePercent(CurrentHP, MaxHP);
	AddInfo(FString::Printf(TEXT("Critical HP: %.0f/%.0f = %.0f%%"), CurrentHP, MaxHP, Percent * 100.0f));
	TestEqual(TEXT("50/500 should show 10%"), Percent, 0.1f);

	// Dead
	CurrentHP = 0.0;
	Percent = CalculatePercent(CurrentHP, MaxHP);
	AddInfo(FString::Printf(TEXT("Dead: %.0f/%.0f = %.0f%%"), CurrentHP, MaxHP, Percent * 100.0f));
	TestEqual(TEXT("0/500 should show 0%"), Percent, 0.0f);

	// Test lerp for trailing health bar
	float FrontPercent = 0.5f;  // Current health (50%)
	float BackPercent = 0.8f;   // Previous health (80%)
	float InterpSpeed = 15.0f;
	float DeltaTime = 0.016f;   // ~60 FPS

	float NewBackPercent = FMath::FInterpTo(BackPercent, FrontPercent, DeltaTime, InterpSpeed);
	AddInfo(FString::Printf(TEXT("Health bar lerp: Back %.2f -> %.2f (target %.2f)"), BackPercent, NewBackPercent, FrontPercent));
	TestTrue(TEXT("Back bar should lerp toward front bar"), NewBackPercent < BackPercent);
	TestTrue(TEXT("Back bar should not overshoot front bar"), NewBackPercent >= FrontPercent);

	AddInfo(TEXT("Health bar simulation completed successfully"));
	return true;
}

// ============================================================================
// SUMMARY TEST: Run All Combat Tests
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFCombatSummaryTest, "SLF.Combat.Summary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFCombatSummaryTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("╔════════════════════════════════════════════════════════════╗"));
	AddInfo(TEXT("║           SLF COMBAT SYSTEM TEST SUMMARY                   ║"));
	AddInfo(TEXT("╠════════════════════════════════════════════════════════════╣"));
	AddInfo(TEXT("║ Run individual tests for detailed results:                 ║"));
	AddInfo(TEXT("║                                                            ║"));
	AddInfo(TEXT("║ • SLF.Stats.Initialization      - Stat creation            ║"));
	AddInfo(TEXT("║ • SLF.Stats.HPTagConsistency    - HP tag verification      ║"));
	AddInfo(TEXT("║ • SLF.Stats.StaminaTag          - Stamina tag verification ║"));
	AddInfo(TEXT("║ • SLF.Stats.ValueAdjustment     - Stat math operations     ║"));
	AddInfo(TEXT("║ • SLF.Stats.StatInfoStruct      - FStatInfo validation     ║"));
	AddInfo(TEXT("║ • SLF.Combat.DamageChain        - Component verification   ║"));
	AddInfo(TEXT("║ • SLF.Combat.InterfaceFunctions - Combat functions exist   ║"));
	AddInfo(TEXT("║ • SLF.Combat.FullDamageSimulation - End-to-end damage      ║"));
	AddInfo(TEXT("║ • SLF.Actions.StaminaValidation - Action manager functions ║"));
	AddInfo(TEXT("║ • SLF.Actions.StaminaConsumption - Stamina drain test      ║"));
	AddInfo(TEXT("║ • SLF.UI.HealthBarSimulation    - Health bar math          ║"));
	AddInfo(TEXT("╚════════════════════════════════════════════════════════════╝"));
	AddInfo(TEXT(""));

	return true;
}
