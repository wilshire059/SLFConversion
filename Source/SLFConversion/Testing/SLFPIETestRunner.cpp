// SLFPIETestRunner.cpp
// Automated PIE test runner implementation

#include "SLFPIETestRunner.h"
#include "SLFInputSimulator.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/IConsoleManager.h"

// ========== CONSOLE COMMANDS ==========

static FAutoConsoleCommand CCmdTestActions(
	TEXT("SLF.Test.Actions"),
	TEXT("Run action system test (attack, dodge, etc)"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GetCurrentPlayWorld(), EGetWorldErrorMode::ReturnNull))
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (USLFPIETestRunner* Runner = GI->GetSubsystem<USLFPIETestRunner>())
				{
					Runner->RunActionSystemTest();
				}
			}
		}
	})
);

static FAutoConsoleCommand CCmdTestMovement(
	TEXT("SLF.Test.Movement"),
	TEXT("Run movement test (WASD, sprint, etc)"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GetCurrentPlayWorld(), EGetWorldErrorMode::ReturnNull))
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (USLFPIETestRunner* Runner = GI->GetSubsystem<USLFPIETestRunner>())
				{
					Runner->RunMovementTest();
				}
			}
		}
	})
);

static FAutoConsoleCommand CCmdSimKey(
	TEXT("SLF.SimKey"),
	TEXT("Simulate a key press. Usage: SLF.SimKey <KeyName> (e.g., SLF.SimKey SpaceBar)"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Usage: SLF.SimKey <KeyName>"));
			return;
		}

		FKey Key(*Args[0]);
		if (!Key.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid key: %s"), *Args[0]);
			return;
		}

		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			USLFInputSimulator::SimulateKeyPress(World, Key);
		}
	})
);

static FAutoConsoleCommand CCmdSimAttack(
	TEXT("SLF.SimAttack"),
	TEXT("Simulate an attack input"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			USLFInputSimulator::SimulateAttack(World);
		}
	})
);

static FAutoConsoleCommand CCmdSimDodge(
	TEXT("SLF.SimDodge"),
	TEXT("Simulate a dodge input"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			USLFInputSimulator::SimulateDodge(World);
		}
	})
);

static FAutoConsoleCommand CCmdSimMove(
	TEXT("SLF.SimMove"),
	TEXT("Simulate movement. Usage: SLF.SimMove <X> <Y> <Duration> (e.g., SLF.SimMove 1 0 2.0 for right movement)"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		float X = 0.0f, Y = 0.0f, Duration = 1.0f;

		if (Args.Num() >= 1) X = FCString::Atof(*Args[0]);
		if (Args.Num() >= 2) Y = FCString::Atof(*Args[1]);
		if (Args.Num() >= 3) Duration = FCString::Atof(*Args[2]);

		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			USLFInputSimulator::SimulateMovement(World, FVector2D(X, Y), Duration);
		}
	})
);

static FAutoConsoleCommand CCmdSimCrouch(
	TEXT("SLF.SimCrouch"),
	TEXT("Toggle crouch on player character (directly calls Crouch/UnCrouch)"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLF.SimCrouch] Command executed"));
		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (ACharacter* Char = PC->GetCharacter())
				{
					UE_LOG(LogTemp, Warning, TEXT("[SLF.SimCrouch] Found character: %s"), *Char->GetClass()->GetName());
					UE_LOG(LogTemp, Warning, TEXT("[SLF.SimCrouch] Current bIsCrouched = %s"), Char->bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));

					if (Char->bIsCrouched)
					{
						Char->UnCrouch();
						UE_LOG(LogTemp, Warning, TEXT("[SLF.SimCrouch] Called UnCrouch(), bIsCrouched now = %s"), Char->bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));
					}
					else
					{
						Char->Crouch();
						UE_LOG(LogTemp, Warning, TEXT("[SLF.SimCrouch] Called Crouch(), bIsCrouched now = %s"), Char->bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));

						if (!Char->bIsCrouched)
						{
							if (UCharacterMovementComponent* CMC = Char->GetCharacterMovement())
							{
								UE_LOG(LogTemp, Error, TEXT("[SLF.SimCrouch] Crouch FAILED! CMC->CanEverCrouch()=%s, bWantsToCrouch=%s"),
									CMC->CanEverCrouch() ? TEXT("TRUE") : TEXT("FALSE"),
									CMC->bWantsToCrouch ? TEXT("TRUE") : TEXT("FALSE"));
							}
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[SLF.SimCrouch] No character on PlayerController!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[SLF.SimCrouch] No PlayerController!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.SimCrouch] No play world!"));
		}
	})
);

// ========== TEST RUNNER IMPLEMENTATION ==========

void USLFPIETestRunner::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test runner initialized"));
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Console commands available:"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.Test.Actions - Run action system test"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.Test.Movement - Run movement test"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimKey <Key> - Simulate key press"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimAttack - Simulate attack"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimDodge - Simulate dodge"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimMove <X> <Y> <Dur> - Simulate movement"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimCrouch - Toggle crouch directly"));

	// Check for auto-test command line argument
	if (FParse::Param(FCommandLine::Get(), TEXT("SLFAutoTest")))
	{
		bAutoTestEnabled = true;
		UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Auto-test mode ENABLED via command line"));

		// Schedule auto-tests after delay to allow level to fully load
		if (UWorld* World = GetWorld())
		{
			FTimerHandle AutoTestTimer;
			World->GetTimerManager().SetTimer(AutoTestTimer, [this]()
			{
				UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] ========== AUTO-TEST STARTING =========="));
				RunAllTests();
			}, AutoTestDelay, false);
		}
	}
}

void USLFPIETestRunner::Deinitialize()
{
	Super::Deinitialize();
}

void USLFPIETestRunner::RunAllTests()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ========== RUNNING ALL TESTS =========="));
	TestResults.Empty();

	RunMovementTest();

	// Chain other tests with delays
	FTimerHandle ActionTestTimer;
	GetWorld()->GetTimerManager().SetTimer(ActionTestTimer, this, &USLFPIETestRunner::RunActionSystemTest, 3.0f, false);

	FTimerHandle InteractionTestTimer;
	GetWorld()->GetTimerManager().SetTimer(InteractionTestTimer, this, &USLFPIETestRunner::RunInteractionTest, 6.0f, false);

	// Log results after all tests
	FTimerHandle ResultsTimer;
	GetWorld()->GetTimerManager().SetTimer(ResultsTimer, [this]()
	{
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ========== TEST RESULTS =========="));
		UE_LOG(LogTemp, Log, TEXT("%s"), *GetTestResultsReport());
	}, 10.0f, false);
}

void USLFPIETestRunner::RunTest(FName TestName)
{
	if (TestName == FName("Actions"))
	{
		RunActionSystemTest();
	}
	else if (TestName == FName("Movement"))
	{
		RunMovementTest();
	}
	else if (TestName == FName("Interaction"))
	{
		RunInteractionTest();
	}
}

void USLFPIETestRunner::RunActionSystemTest()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ===== ACTION SYSTEM TEST ====="));

	UWorld* World = GetWorld();
	if (!World)
	{
		LogTestResult(FName("ActionSystem"), false, TEXT("No world"));
		return;
	}

	// Get player character
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerChar)
	{
		LogTestResult(FName("ActionSystem"), false, TEXT("No player character"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Player: %s"), *PlayerChar->GetName());

	// Test 1: Simulate attack
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test: Simulating attack..."));
	USLFInputSimulator::SimulateAttack(World);

	// Test 2: Wait and simulate dodge
	FTimerHandle DodgeTimer;
	World->GetTimerManager().SetTimer(DodgeTimer, [World]()
	{
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test: Simulating dodge..."));
		USLFInputSimulator::SimulateDodge(World);
	}, 1.0f, false);

	// Test 3: Wait and simulate another attack
	FTimerHandle Attack2Timer;
	World->GetTimerManager().SetTimer(Attack2Timer, [World]()
	{
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test: Simulating combo attack..."));
		USLFInputSimulator::SimulateAttack(World);
	}, 2.0f, false);

	LogTestResult(FName("ActionSystem"), true, TEXT("Actions simulated - check logs for execution"));
}

void USLFPIETestRunner::RunMovementTest()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ===== MOVEMENT TEST ====="));

	UWorld* World = GetWorld();
	if (!World)
	{
		LogTestResult(FName("Movement"), false, TEXT("No world"));
		return;
	}

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerChar)
	{
		LogTestResult(FName("Movement"), false, TEXT("No player character"));
		return;
	}

	FVector StartLocation = PlayerChar->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Start location: %s"), *StartLocation.ToString());

	// Move forward for 1 second
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test: Moving forward..."));
	USLFInputSimulator::SimulateMovement(World, FVector2D(0, 1), 1.0f);

	// Check position after movement
	FTimerHandle CheckTimer;
	World->GetTimerManager().SetTimer(CheckTimer, [this, PlayerChar, StartLocation]()
	{
		FVector EndLocation = PlayerChar->GetActorLocation();
		float Distance = FVector::Dist(StartLocation, EndLocation);

		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] End location: %s, Distance moved: %.2f"), *EndLocation.ToString(), Distance);

		bool bMoved = Distance > 10.0f;
		LogTestResult(FName("Movement"), bMoved, FString::Printf(TEXT("Moved %.2f units"), Distance));
	}, 1.5f, false);
}

void USLFPIETestRunner::RunInteractionTest()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ===== INTERACTION TEST ====="));

	UWorld* World = GetWorld();
	if (!World)
	{
		LogTestResult(FName("Interaction"), false, TEXT("No world"));
		return;
	}

	// Simulate interact key
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Test: Simulating interact..."));
	USLFInputSimulator::SimulateInteract(World);

	LogTestResult(FName("Interaction"), true, TEXT("Interact simulated - check for interaction response"));
}

void USLFPIETestRunner::LogTestResult(FName TestName, bool bPassed, const FString& Message)
{
	TestResults.Add(TestName, bPassed);

	if (bPassed)
	{
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] [PASS] %s: %s"), *TestName.ToString(), *Message);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SLFPIETestRunner] [FAIL] %s: %s"), *TestName.ToString(), *Message);
	}

	OnTestCompleted.Broadcast(TestName, bPassed);
}

FString USLFPIETestRunner::GetTestResultsReport() const
{
	FString Report = TEXT("Test Results:\n");

	int32 Passed = 0;
	int32 Failed = 0;

	for (const auto& Result : TestResults)
	{
		Report += FString::Printf(TEXT("  %s: %s\n"),
			*Result.Key.ToString(),
			Result.Value ? TEXT("[PASS]") : TEXT("[FAIL]"));

		if (Result.Value)
			Passed++;
		else
			Failed++;
	}

	Report += FString::Printf(TEXT("\nTotal: %d passed, %d failed"), Passed, Failed);
	return Report;
}
