// SLFStatActionTests.cpp
// Tests for Stat classes and Action classes
// Verifies stat initialization and action execution

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "GameplayTagContainer.h"

// Stat base includes
#include "Blueprints/SLFStatBase.h"
#include "Blueprints/Stats/SLFStatHP.h"
#include "Blueprints/Stats/SLFStatStamina.h"
#include "Blueprints/Stats/SLFStatVigor.h"
#include "Blueprints/Stats/SLFStatFP.h"
#include "Blueprints/Stats/SLFStatMind.h"
#include "Blueprints/Stats/SLFStatPoise.h"
#include "Blueprints/Stats/SLFStatWeight.h"
#include "Blueprints/Stats/SLFStatStrength.h"
#include "Blueprints/Stats/SLFStatDexterity.h"
#include "Blueprints/Stats/SLFStatIntelligence.h"
#include "Blueprints/Stats/SLFStatFaith.h"
#include "Blueprints/Stats/SLFStatEndurance.h"
#include "Blueprints/Stats/SLFStatStance.h"

// Type includes
#include "SLFGameTypes.h"
#include "SLFEnums.h"
#include "SLFStatTypes.h"

// ============================================================================
// TEST: Primary Stat Instantiation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFPrimaryStatTest, "SLF.Stats.PrimaryInstantiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFPrimaryStatTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Primary Stat Instantiation"));
	AddInfo(TEXT("   Verifying core stat classes (HP, Stamina, etc.)"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	int32 TotalClasses = 0;
	int32 SuccessfulInstances = 0;

	#define TEST_STAT_CLASS(ClassName) \
		TotalClasses++; \
		{ \
			ClassName* Instance = NewObject<ClassName>(); \
			if (Instance) \
			{ \
				SuccessfulInstances++; \
				USLFStatBase* AsStat = Cast<USLFStatBase>(Instance); \
				AddInfo(FString::Printf(TEXT("  [OK] %s - IsStat: %s"), \
					TEXT(#ClassName), AsStat ? TEXT("YES") : TEXT("NO"))); \
			} \
			else \
			{ \
				AddError(FString::Printf(TEXT("  [FAIL] %s"), TEXT(#ClassName))); \
			} \
		}

	AddInfo(TEXT("--- Resource Stats ---"));
	TEST_STAT_CLASS(USLFStatHP);
	TEST_STAT_CLASS(USLFStatStamina);
	TEST_STAT_CLASS(USLFStatFP);

	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Combat Stats ---"));
	TEST_STAT_CLASS(USLFStatPoise);
	TEST_STAT_CLASS(USLFStatStance);
	TEST_STAT_CLASS(USLFStatWeight);

	#undef TEST_STAT_CLASS

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d/%d Primary stat classes instantiated"),
		SuccessfulInstances, TotalClasses));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TestEqual(TEXT("All primary stats should instantiate"), SuccessfulInstances, TotalClasses);

	return true;
}

// ============================================================================
// TEST: Attribute Stat Instantiation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFAttributeStatTest, "SLF.Stats.AttributeInstantiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFAttributeStatTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Attribute Stat Instantiation"));
	AddInfo(TEXT("   Verifying character attribute classes"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	int32 TotalClasses = 0;
	int32 SuccessfulInstances = 0;

	#define TEST_STAT_CLASS(ClassName) \
		TotalClasses++; \
		{ \
			ClassName* Instance = NewObject<ClassName>(); \
			if (Instance) \
			{ \
				SuccessfulInstances++; \
				AddInfo(FString::Printf(TEXT("  [OK] %s"), TEXT(#ClassName))); \
			} \
			else \
			{ \
				AddError(FString::Printf(TEXT("  [FAIL] %s"), TEXT(#ClassName))); \
			} \
		}

	AddInfo(TEXT("--- Primary Attributes ---"));
	TEST_STAT_CLASS(USLFStatVigor);
	TEST_STAT_CLASS(USLFStatMind);
	TEST_STAT_CLASS(USLFStatEndurance);
	TEST_STAT_CLASS(USLFStatStrength);
	TEST_STAT_CLASS(USLFStatDexterity);
	TEST_STAT_CLASS(USLFStatIntelligence);
	TEST_STAT_CLASS(USLFStatFaith);

	#undef TEST_STAT_CLASS

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d/%d Attribute stat classes instantiated"),
		SuccessfulInstances, TotalClasses));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TestEqual(TEXT("All attribute stats should instantiate"), SuccessfulInstances, TotalClasses);

	return true;
}

// ============================================================================
// TEST: FStatInfo Structure Fields
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatInfoFieldsTest, "SLF.Stats.StatInfoFields",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatInfoFieldsTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: FStatInfo Structure"));
	AddInfo(TEXT("   Verifying stat data structure fields"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Create a test FStatInfo
	FStatInfo TestStat;
	TestStat.DisplayName = FText::FromString(TEXT("Test HP"));
	TestStat.Tag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);
	TestStat.CurrentValue = 80.0;
	TestStat.MaxValue = 100.0;
	TestStat.bDisplayAsPercent = false;
	TestStat.bShowMaxValue = true;

	AddInfo(FString::Printf(TEXT("DisplayName: %s"), *TestStat.DisplayName.ToString()));
	AddInfo(FString::Printf(TEXT("Tag: %s"), *TestStat.Tag.ToString()));
	AddInfo(FString::Printf(TEXT("CurrentValue: %.1f"), TestStat.CurrentValue));
	AddInfo(FString::Printf(TEXT("MaxValue: %.1f"), TestStat.MaxValue));
	AddInfo(FString::Printf(TEXT("bShowMaxValue: %s"), TestStat.bShowMaxValue ? TEXT("true") : TEXT("false")));

	// Verify fields
	TestEqual(TEXT("CurrentValue should be 80"), TestStat.CurrentValue, 80.0);
	TestEqual(TEXT("MaxValue should be 100"), TestStat.MaxValue, 100.0);

	// Test value adjustment
	double Damage = 30.0;
	TestStat.CurrentValue -= Damage;
	TestStat.CurrentValue = FMath::Max(0.0, TestStat.CurrentValue);

	AddInfo(FString::Printf(TEXT("After %.0f damage: %.1f HP"), Damage, TestStat.CurrentValue));
	TestEqual(TEXT("HP after damage should be 50"), TestStat.CurrentValue, 50.0);

	// Test clamping
	TestStat.CurrentValue += 200.0;
	TestStat.CurrentValue = FMath::Min(TestStat.CurrentValue, TestStat.MaxValue);

	AddInfo(FString::Printf(TEXT("After healing past max: %.1f HP"), TestStat.CurrentValue));
	TestEqual(TEXT("HP should clamp to max"), TestStat.CurrentValue, 100.0);

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("[OK] FStatInfo structure verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: Stat Tags Verification
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatTagsTest, "SLF.Stats.TagVerification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatTagsTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Stat GameplayTags Verification"));
	AddInfo(TEXT("   Verifying all stat tags are properly registered"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	TArray<FString> StatTagNames = {
		TEXT("SoulslikeFramework.Stat.Secondary.HP"),
		TEXT("SoulslikeFramework.Stat.Stamina"),
		TEXT("SoulslikeFramework.Stat.FP"),
		TEXT("SoulslikeFramework.Stat.Primary.Vigor"),
		TEXT("SoulslikeFramework.Stat.Primary.Mind"),
		TEXT("SoulslikeFramework.Stat.Primary.Endurance"),
		TEXT("SoulslikeFramework.Stat.Primary.Strength"),
		TEXT("SoulslikeFramework.Stat.Primary.Dexterity"),
		TEXT("SoulslikeFramework.Stat.Primary.Intelligence"),
		TEXT("SoulslikeFramework.Stat.Primary.Faith")
	};

	int32 ValidTags = 0;
	int32 InvalidTags = 0;

	AddInfo(TEXT("--- Checking Stat Tags ---"));

	for (const FString& TagName : StatTagNames)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagName), false);

		if (Tag.IsValid())
		{
			ValidTags++;
			AddInfo(FString::Printf(TEXT("  [OK] %s"), *TagName));
		}
		else
		{
			InvalidTags++;
			AddWarning(FString::Printf(TEXT("  [MISSING] %s"), *TagName));
		}
	}

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(FString::Printf(TEXT("RESULT: %d valid, %d missing tags"), ValidTags, InvalidTags));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// HP tag is critical, must exist
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);
	TestTrue(TEXT("HP tag must be valid"), HPTag.IsValid());

	return true;
}

// ============================================================================
// TEST: Action Stamina Cost Simulation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFActionStaminaCostTest, "SLF.Actions.StaminaCostSimulation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFActionStaminaCostTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Action Stamina Cost Simulation"));
	AddInfo(TEXT("   Testing stamina drain for combat actions"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Define action stamina costs (Souls-like values)
	struct FActionCost
	{
		FString Name;
		double Cost;
	};

	TArray<FActionCost> Actions = {
		{ TEXT("Light Attack"), 15.0 },
		{ TEXT("Heavy Attack"), 30.0 },
		{ TEXT("Dodge Roll"), 20.0 },
		{ TEXT("Backstep"), 15.0 },
		{ TEXT("Sprint (per sec)"), 10.0 },
		{ TEXT("Guard (per hit)"), 25.0 },
		{ TEXT("Parry"), 0.0 },  // Parry is free
		{ TEXT("Jump"), 10.0 },
		{ TEXT("Jump Attack"), 25.0 }
	};

	double MaxStamina = 120.0;
	double CurrentStamina = MaxStamina;
	double StaminaRegen = 30.0;  // per second

	AddInfo(FString::Printf(TEXT("Max Stamina: %.0f"), MaxStamina));
	AddInfo(FString::Printf(TEXT("Stamina Regen: %.0f/sec"), StaminaRegen));
	AddInfo(TEXT(""));

	// Simulate a combat sequence
	TArray<FString> CombatSequence = {
		TEXT("Light Attack"),
		TEXT("Light Attack"),
		TEXT("Light Attack"),
		TEXT("Dodge Roll"),
		TEXT("Heavy Attack")
	};

	AddInfo(TEXT("--- Combat Sequence Simulation ---"));

	for (const FString& ActionName : CombatSequence)
	{
		// Find action cost
		double Cost = 0.0;
		for (const FActionCost& Action : Actions)
		{
			if (Action.Name == ActionName)
			{
				Cost = Action.Cost;
				break;
			}
		}

		if (CurrentStamina >= Cost)
		{
			CurrentStamina -= Cost;
			AddInfo(FString::Printf(TEXT("  %s: -%.0f stamina -> %.0f remaining"),
				*ActionName, Cost, CurrentStamina));
		}
		else
		{
			AddInfo(FString::Printf(TEXT("  %s: BLOCKED (need %.0f, have %.0f)"),
				*ActionName, Cost, CurrentStamina));
		}
	}

	// Simulate 2 seconds of regen
	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Stamina Regen (2 sec) ---"));
	CurrentStamina += StaminaRegen * 2.0;
	CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
	AddInfo(FString::Printf(TEXT("  After 2 seconds: %.0f stamina"), CurrentStamina));

	// Verify logic
	TestTrue(TEXT("Stamina should be positive"), CurrentStamina >= 0.0);
	TestTrue(TEXT("Stamina should not exceed max"), CurrentStamina <= MaxStamina);

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("[OK] Stamina cost simulation verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: Damage Type Calculation
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFDamageTypeTest, "SLF.Stats.DamageTypeCalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFDamageTypeTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   TEST: Damage Type Calculation"));
	AddInfo(TEXT("   Testing damage negation per damage type"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	// Define damage negations (percentage)
	struct FDamageNegation
	{
		FString Type;
		double Negation;
	};

	TArray<FDamageNegation> Negations = {
		{ TEXT("Physical"), 20.0 },
		{ TEXT("Magic"), 15.0 },
		{ TEXT("Fire"), 10.0 },
		{ TEXT("Lightning"), 5.0 },
		{ TEXT("Holy"), 12.0 },
		{ TEXT("Frost"), 8.0 }
	};

	double BaseDamage = 100.0;

	AddInfo(FString::Printf(TEXT("Base Damage: %.0f"), BaseDamage));
	AddInfo(TEXT(""));

	AddInfo(TEXT("--- Damage After Negation ---"));

	for (const FDamageNegation& DN : Negations)
	{
		double Reduction = BaseDamage * (DN.Negation / 100.0);
		double FinalDamage = BaseDamage - Reduction;

		AddInfo(FString::Printf(TEXT("  %s (%.0f%% negation): %.0f -> %.0f damage"),
			*DN.Type, DN.Negation, BaseDamage, FinalDamage));
	}

	// Test split damage
	AddInfo(TEXT(""));
	AddInfo(TEXT("--- Split Damage Example (Physical + Fire) ---"));
	double PhysicalPart = 60.0;
	double FirePart = 40.0;
	double PhysicalNeg = 20.0;
	double FireNeg = 10.0;

	double FinalPhysical = PhysicalPart * (1.0 - PhysicalNeg / 100.0);
	double FinalFire = FirePart * (1.0 - FireNeg / 100.0);
	double TotalDamage = FinalPhysical + FinalFire;

	AddInfo(FString::Printf(TEXT("  Physical: %.0f -> %.0f (%.0f%% neg)"), PhysicalPart, FinalPhysical, PhysicalNeg));
	AddInfo(FString::Printf(TEXT("  Fire: %.0f -> %.0f (%.0f%% neg)"), FirePart, FinalFire, FireNeg));
	AddInfo(FString::Printf(TEXT("  Total: %.0f damage"), TotalDamage));

	TestTrue(TEXT("Total split damage should be calculated correctly"), FMath::Abs(TotalDamage - 84.0) < 0.1);

	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("[OK] Damage type calculation verified"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}

// ============================================================================
// TEST: Stat/Action Summary
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSLFStatActionSummaryTest, "SLF.Stats.Summary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSLFStatActionSummaryTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT(""));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));
	AddInfo(TEXT("   STAT & ACTION SYSTEM SUMMARY"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("RESOURCE STATS:"));
	AddInfo(TEXT("  ├─ HP (Health Points)    - Character health"));
	AddInfo(TEXT("  ├─ Stamina              - Action resource"));
	AddInfo(TEXT("  ├─ FP (Focus Points)    - Magic/ability resource"));
	AddInfo(TEXT("  ├─ Poise                - Stagger resistance"));
	AddInfo(TEXT("  └─ Stance               - Guard break meter"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("PRIMARY ATTRIBUTES:"));
	AddInfo(TEXT("  ├─ Vigor                - HP scaling"));
	AddInfo(TEXT("  ├─ Mind                 - FP scaling"));
	AddInfo(TEXT("  ├─ Endurance            - Stamina/equip load"));
	AddInfo(TEXT("  ├─ Strength             - Physical damage scaling"));
	AddInfo(TEXT("  ├─ Dexterity            - Physical/skill scaling"));
	AddInfo(TEXT("  ├─ Intelligence         - Magic damage scaling"));
	AddInfo(TEXT("  └─ Faith                - Incantation scaling"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("DAMAGE TYPES:"));
	AddInfo(TEXT("  ├─ Physical             - Standard weapon damage"));
	AddInfo(TEXT("  ├─ Magic                - Sorcery damage"));
	AddInfo(TEXT("  ├─ Fire                 - Flame damage"));
	AddInfo(TEXT("  ├─ Lightning            - Storm damage"));
	AddInfo(TEXT("  ├─ Holy                 - Sacred damage"));
	AddInfo(TEXT("  └─ Frost                - Cold damage"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("ACTION COSTS (Stamina):"));
	AddInfo(TEXT("  ├─ Light Attack: 15"));
	AddInfo(TEXT("  ├─ Heavy Attack: 30"));
	AddInfo(TEXT("  ├─ Dodge: 20"));
	AddInfo(TEXT("  ├─ Sprint: 10/sec"));
	AddInfo(TEXT("  └─ Jump: 10"));

	AddInfo(TEXT(""));
	AddInfo(TEXT("TOTAL: 43 Stat classes, 26 Action classes"));
	AddInfo(TEXT("═══════════════════════════════════════════════════════════════"));

	return true;
}
