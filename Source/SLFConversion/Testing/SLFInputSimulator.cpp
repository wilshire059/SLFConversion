// SLFInputSimulator.cpp
// Input simulation implementation for PIE testing

#include "SLFInputSimulator.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

void USLFInputSimulator::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Subsystem initialized - Input simulation ready"));
}

void USLFInputSimulator::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Subsystem deinitialized"));
}

APlayerController* USLFInputSimulator::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	return World->GetFirstPlayerController();
}

void USLFInputSimulator::InjectKeyEvent(FKey Key, bool bIsDown)
{
	if (!FSlateApplication::IsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFInputSimulator] Slate not initialized"));
		return;
	}

	FSlateApplication& SlateApp = FSlateApplication::Get();

	// Create key event
	const uint32 CharCode = 0;
	const uint32 KeyCode = Key.IsValid() ? (uint32)Key.GetFName().GetNumber() : 0;
	const FModifierKeysState ModifierKeys;

	if (bIsDown)
	{
		FKeyEvent KeyEvent(Key, ModifierKeys, 0, false, CharCode, KeyCode);
		SlateApp.ProcessKeyDownEvent(KeyEvent);
		UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Key DOWN: %s"), *Key.ToString());
	}
	else
	{
		FKeyEvent KeyEvent(Key, ModifierKeys, 0, false, CharCode, KeyCode);
		SlateApp.ProcessKeyUpEvent(KeyEvent);
		UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Key UP: %s"), *Key.ToString());
	}
}

void USLFInputSimulator::SimulateKeyPress(const UObject* WorldContextObject, FKey Key, float HoldDuration)
{
	// Send key down immediately
	SimulateKeyDown(WorldContextObject, Key);

	// Schedule key up after hold duration
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [WorldContextObject, Key]()
		{
			SimulateKeyUp(WorldContextObject, Key);
		}, HoldDuration, false);
	}
}

void USLFInputSimulator::SimulateKeyDown(const UObject* WorldContextObject, FKey Key)
{
	// Method 1: Inject via Slate (works for UI)
	InjectKeyEvent(Key, true);

	// Method 2: Also inject via PlayerController for gameplay input
	if (APlayerController* PC = GetLocalPlayerController(WorldContextObject))
	{
		PC->InputKey(FInputKeyParams(Key, EInputEvent::IE_Pressed, 1.0, false));
	}
}

void USLFInputSimulator::SimulateKeyUp(const UObject* WorldContextObject, FKey Key)
{
	// Method 1: Inject via Slate
	InjectKeyEvent(Key, false);

	// Method 2: Also inject via PlayerController
	if (APlayerController* PC = GetLocalPlayerController(WorldContextObject))
	{
		PC->InputKey(FInputKeyParams(Key, EInputEvent::IE_Released, 0.0, false));
	}
}

void USLFInputSimulator::SimulateKeySequence(const UObject* WorldContextObject, const TArray<FKey>& Keys, float DelayBetweenKeys)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World || Keys.Num() == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Starting key sequence with %d keys"), Keys.Num());

	// Process keys with delays
	float CurrentDelay = 0.0f;
	for (int32 i = 0; i < Keys.Num(); i++)
	{
		FKey Key = Keys[i];
		FTimerHandle TimerHandle;

		World->GetTimerManager().SetTimer(TimerHandle, [WorldContextObject, Key]()
		{
			SimulateKeyPress(WorldContextObject, Key, 0.1f);
		}, CurrentDelay, false);

		CurrentDelay += DelayBetweenKeys;
	}
}

void USLFInputSimulator::SimulateInputAction(const UObject* WorldContextObject, FName ActionName)
{
	APlayerController* PC = GetLocalPlayerController(WorldContextObject);
	if (!PC || !PC->PlayerInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFInputSimulator] No PlayerController or PlayerInput"));
		return;
	}

	// Look up action mapping and simulate the first key
	const TArray<FInputActionKeyMapping>& ActionMappings = PC->PlayerInput->ActionMappings;
	for (const FInputActionKeyMapping& Mapping : ActionMappings)
	{
		if (Mapping.ActionName == ActionName)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Simulating action '%s' via key '%s'"), *ActionName.ToString(), *Mapping.Key.ToString());
			SimulateKeyPress(WorldContextObject, Mapping.Key);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[SLFInputSimulator] Action '%s' not found in mappings"), *ActionName.ToString());
}

void USLFInputSimulator::SimulateAttack(const UObject* WorldContextObject)
{
	// Try left mouse button first
	SimulateKeyPress(WorldContextObject, EKeys::LeftMouseButton);
}

void USLFInputSimulator::SimulateDodge(const UObject* WorldContextObject)
{
	// Space bar for dodge/roll
	SimulateKeyPress(WorldContextObject, EKeys::SpaceBar);
}

void USLFInputSimulator::SimulateJump(const UObject* WorldContextObject)
{
	// Space bar for jump (same as dodge in many configs)
	SimulateKeyPress(WorldContextObject, EKeys::SpaceBar);
}

void USLFInputSimulator::SimulateInteract(const UObject* WorldContextObject)
{
	// E key for interact
	SimulateKeyPress(WorldContextObject, EKeys::E);
}

void USLFInputSimulator::SimulateMovement(const UObject* WorldContextObject, FVector2D Direction, float Duration)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return;
	}

	// Determine which keys to press based on direction
	TArray<FKey> MovementKeys;

	if (Direction.Y > 0.5f)
	{
		MovementKeys.Add(EKeys::W);
	}
	else if (Direction.Y < -0.5f)
	{
		MovementKeys.Add(EKeys::S);
	}

	if (Direction.X > 0.5f)
	{
		MovementKeys.Add(EKeys::D);
	}
	else if (Direction.X < -0.5f)
	{
		MovementKeys.Add(EKeys::A);
	}

	// Press all movement keys
	for (const FKey& Key : MovementKeys)
	{
		SimulateKeyDown(WorldContextObject, Key);
	}

	// Release after duration
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [WorldContextObject, MovementKeys]()
	{
		for (const FKey& Key : MovementKeys)
		{
			SimulateKeyUp(WorldContextObject, Key);
		}
	}, Duration, false);

	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Simulating movement Dir(%.1f, %.1f) for %.2fs"), Direction.X, Direction.Y, Duration);
}

void USLFInputSimulator::SimulateMouseMove(FVector2D Delta)
{
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	FSlateApplication& SlateApp = FSlateApplication::Get();
	FVector2D CurrentPos = SlateApp.GetCursorPos();
	FVector2D NewPos = CurrentPos + Delta;

	SlateApp.SetCursorPos(NewPos);

	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Mouse moved by (%.1f, %.1f)"), Delta.X, Delta.Y);
}

void USLFInputSimulator::SimulateMouseButton(const UObject* WorldContextObject, FKey MouseButton, bool bPressed)
{
	if (bPressed)
	{
		SimulateKeyDown(WorldContextObject, MouseButton);
	}
	else
	{
		SimulateKeyUp(WorldContextObject, MouseButton);
	}
}

void USLFInputSimulator::SimulateGamepadAxis(const UObject* WorldContextObject, FKey AxisKey, float Value)
{
	APlayerController* PC = GetLocalPlayerController(WorldContextObject);
	if (!PC)
	{
		return;
	}

	FInputKeyParams Params(AxisKey, Value, 0.0, false);
	PC->InputKey(Params);

	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Gamepad axis %s = %.2f"), *AxisKey.ToString(), Value);
}

void USLFInputSimulator::SimulateGamepadButton(const UObject* WorldContextObject, FKey Button, bool bPressed)
{
	APlayerController* PC = GetLocalPlayerController(WorldContextObject);
	if (!PC)
	{
		return;
	}

	FInputKeyParams Params(Button, bPressed ? EInputEvent::IE_Pressed : EInputEvent::IE_Released, bPressed ? 1.0 : 0.0, false);
	PC->InputKey(Params);

	UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Gamepad button %s %s"), *Button.ToString(), bPressed ? TEXT("PRESSED") : TEXT("RELEASED"));
}

void USLFInputSimulator::ExecuteConsoleCommand(const UObject* WorldContextObject, const FString& Command)
{
	APlayerController* PC = GetLocalPlayerController(WorldContextObject);
	if (PC)
	{
		PC->ConsoleCommand(Command);
		UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] Executed console command: %s"), *Command);
	}
}

// ========== PIE CONTROL (EDITOR ONLY) ==========

bool USLFInputSimulatorLibrary::StartPIE()
{
#if WITH_EDITOR
	if (GEditor && !GEditor->IsPlaySessionInProgress())
	{
		FRequestPlaySessionParams Params;
		Params.WorldType = EPlaySessionWorldType::PlayInEditor;
		Params.DestinationSlateViewport = nullptr; // Use active viewport

		GEditor->RequestPlaySession(Params);
		UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] PIE session requested"));
		return true;
	}
#endif
	return false;
}

void USLFInputSimulatorLibrary::StopPIE()
{
#if WITH_EDITOR
	if (GEditor && GEditor->IsPlaySessionInProgress())
	{
		GEditor->RequestEndPlayMap();
		UE_LOG(LogTemp, Log, TEXT("[SLFInputSimulator] PIE session end requested"));
	}
#endif
}

bool USLFInputSimulatorLibrary::IsPIERunning()
{
#if WITH_EDITOR
	return GEditor && GEditor->IsPlaySessionInProgress();
#else
	return false;
#endif
}

UWorld* USLFInputSimulatorLibrary::GetPIEWorld()
{
#if WITH_EDITOR
	if (GEditor)
	{
		FWorldContext* PIEContext = GEditor->GetPIEWorldContext();
		if (PIEContext)
		{
			return PIEContext->World();
		}
	}
#endif
	return nullptr;
}
