// SLFInputSimulator.h
// Input simulation utility for PIE testing
// Can simulate keyboard, mouse, and gamepad input during gameplay

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputCoreTypes.h"
#include "SLFInputSimulator.generated.h"

/**
 * Input Simulator Subsystem
 * Provides functions to simulate input during PIE for automated testing.
 * Access via: GetGameInstance()->GetSubsystem<USLFInputSimulator>()
 */
UCLASS()
class SLFCONVERSION_API USLFInputSimulator : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========== KEY SIMULATION ==========

	/** Simulate a key press (down then up after delay) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateKeyPress(const UObject* WorldContextObject, FKey Key, float HoldDuration = 0.1f);

	/** Simulate key down event */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateKeyDown(const UObject* WorldContextObject, FKey Key);

	/** Simulate key up event */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateKeyUp(const UObject* WorldContextObject, FKey Key);

	/** Simulate a sequence of key presses with delays between them */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateKeySequence(const UObject* WorldContextObject, const TArray<FKey>& Keys, float DelayBetweenKeys = 0.2f);

	// ========== ACTION SIMULATION ==========

	/** Simulate an input action by name (looks up the action mapping) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateInputAction(const UObject* WorldContextObject, FName ActionName);

	/** Simulate pressing attack (left mouse / gamepad right trigger) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateAttack(const UObject* WorldContextObject);

	/** Simulate pressing dodge/roll (space / gamepad A) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateDodge(const UObject* WorldContextObject);

	/** Simulate pressing jump */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateJump(const UObject* WorldContextObject);

	/** Simulate pressing interact (E key) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateInteract(const UObject* WorldContextObject);

	/** Simulate movement in a direction for a duration */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateMovement(const UObject* WorldContextObject, FVector2D Direction, float Duration = 0.5f);

	// ========== MOUSE SIMULATION ==========

	/** Simulate mouse movement (delta) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation")
	static void SimulateMouseMove(FVector2D Delta);

	/** Simulate mouse button press */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateMouseButton(const UObject* WorldContextObject, FKey MouseButton, bool bPressed);

	// ========== GAMEPAD SIMULATION ==========

	/** Simulate gamepad axis input */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateGamepadAxis(const UObject* WorldContextObject, FKey AxisKey, float Value);

	/** Simulate gamepad button */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void SimulateGamepadButton(const UObject* WorldContextObject, FKey Button, bool bPressed);

	// ========== CONSOLE COMMANDS ==========

	/** Execute a console command (can be used from Python) */
	UFUNCTION(BlueprintCallable, Category = "Input Simulation", meta = (WorldContext = "WorldContextObject"))
	static void ExecuteConsoleCommand(const UObject* WorldContextObject, const FString& Command);

private:
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject);
	static void InjectKeyEvent(FKey Key, bool bIsDown);
};

/**
 * Static library for input simulation - can be called from anywhere
 */
UCLASS()
class SLFCONVERSION_API USLFInputSimulatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Start a PIE session (Editor only) */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	static bool StartPIE();

	/** Stop the current PIE session (Editor only) */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	static void StopPIE();

	/** Check if PIE is currently running */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	static bool IsPIERunning();

	/** Get the PIE world (returns nullptr if not in PIE) */
	UFUNCTION(BlueprintCallable, Category = "PIE Testing")
	static UWorld* GetPIEWorld();
};
