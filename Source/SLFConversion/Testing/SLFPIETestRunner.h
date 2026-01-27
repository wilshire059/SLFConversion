// SLFPIETestRunner.h
// Automated PIE test runner with input simulation
// Runs gameplay tests during PIE sessions

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLFPIETestRunner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestCompleted, FName, TestName, bool, bPassed);

/**
 * PIE Test Runner - Executes automated gameplay tests
 */
UCLASS()
class SLFCONVERSION_API USLFPIETestRunner : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Enable auto-test mode - runs tests automatically after level loads */
	UPROPERTY(BlueprintReadWrite, Category = "PIE Testing")
	bool bAutoTestEnabled = false;

	/** Delay before auto-tests start (allows level to fully load) */
	UPROPERTY(EditDefaultsOnly, Category = "PIE Testing")
	float AutoTestDelay = 3.0f;

	/** Run all registered tests */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunAllTests();

	/** Run a specific test by name */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunTest(FName TestName);

	/** Run action system test - tests combat actions */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunActionSystemTest();

	/** Run movement test - tests basic movement */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunMovementTest();

	/** Run interaction test - tests interact with objects */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunInteractionTest();

	/** Run save/load test - tests save data serialization and persistence */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	void RunSaveLoadTest();

	/** Event fired when a test completes */
	UPROPERTY(BlueprintAssignable, Category = "PIE Testing")
	FOnTestCompleted OnTestCompleted;

	/** Get test results as a string */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	FString GetTestResultsReport() const;

private:
	TMap<FName, bool> TestResults;
	void LogTestResult(FName TestName, bool bPassed, const FString& Message);
};

/**
 * Console command handler for PIE tests
 * Adds commands like: SLF.Test.Actions, SLF.Test.Movement, etc.
 */
class SLFCONVERSION_API FSLFTestCommands
{
public:
	static void Register();
	static void Unregister();

private:
	static void RunActionsTest(const TArray<FString>& Args);
	static void RunMovementTest(const TArray<FString>& Args);
	static void SimulateKey(const TArray<FString>& Args);
};
