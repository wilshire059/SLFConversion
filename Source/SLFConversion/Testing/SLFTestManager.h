// SLFTestManager.h
// Test manager actor for automated PIE testing with screenshot capture,
// log export, AnimNotify monitoring, and enemy state validation.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "SLFTestManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSLFTest, Log, All);

/**
 * ASLFTestManager - Orchestrates automated PIE testing.
 *
 * Spawn in-level or via console command. Provides:
 * - Enemy spawning at configurable offsets
 * - Screenshot capture with HUD
 * - AnimNotify monitoring via delegate binding
 * - Filtered log export to timestamped files
 * - Enemy state validation (animation, mesh, sockets)
 */
UCLASS(Blueprintable)
class SLFCONVERSION_API ASLFTestManager : public AActor
{
	GENERATED_BODY()

public:
	ASLFTestManager();

	// --- Test Configuration ---

	/** Blueprint class to spawn for testing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Config")
	TSubclassOf<AActor> TestEnemyClass;

	/** Distance from player to spawn the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Config")
	float SpawnDistance = 500.f;

	/** Folder for screenshot output (relative to project Saved/) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Config")
	FString ScreenshotFolder = TEXT("Saved/Automation/");

	// --- Test State ---

	/** Currently spawned enemy actor */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Test State")
	AActor* SpawnedEnemy;

	/** Current screenshot index for sequencing */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Test State")
	int32 ScreenshotIndex;

	/** Accumulated test log entries */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Test State")
	TArray<FString> TestLog;

	/** Name of the current test being run */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Test State")
	FString CurrentTestName;

	// --- Core Functions ---

	/** Run a named test scenario */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	void RunTest(const FString& TestName);

	/** Spawn a test enemy at offset from player */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	AActor* SpawnTestEnemy(TSubclassOf<AActor> EnemyClass, FVector Offset = FVector(500.f, 0.f, 0.f));

	/** Capture a labeled screenshot */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	void CaptureScreenshot(const FString& Label);

	/** Export filtered log to a timestamped file */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	FString ExportFilteredLog(const FString& OutputPath = TEXT(""));

	/** Validate enemy state (animation, mesh, etc.) */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	FString ValidateEnemyState(const FString& ExpectedState = TEXT(""));

	/** Clear all test state and destroy spawned actors */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	void CleanupTest();

	// --- AnimNotify Monitoring ---

	/** Bind to enemy's AnimNotify events for monitoring */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	void BindToEnemyAnimNotifies(AActor* Enemy);

	/** Get a summary of received AnimNotifies */
	UFUNCTION(BlueprintCallable, Category = "SLF Test")
	FString GetAnimNotifySummary() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Build full screenshot path */
	FString GetScreenshotPath(const FString& Label) const;

	/** Add entry to test log */
	void AddLogEntry(const FString& Entry);

	/** Received AnimNotify names and counts */
	TMap<FName, int32> ReceivedNotifies;

	/** Timer handle for delayed operations */
	FTimerHandle TestTimerHandle;

	/** Handle montage notify begin events */
	UFUNCTION()
	void HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
};
