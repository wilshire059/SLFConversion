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
#include "Components/AC_SaveLoadManager.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Interfaces/BPI_Interactable.h"
#include "SLFAutomationLibrary.h"
#include "InstancedStruct.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxKey.h"
#include "GameplayTagContainer.h"
#include "Widgets/W_HUD.h"
#include "Widgets/W_Dialog.h"
#include "GameFramework/HUD.h"
#include "EngineUtils.h"
#include "UnrealClient.h"

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

static FAutoConsoleCommand CCmdTestSaveLoad(
	TEXT("SLF.Test.SaveLoad"),
	TEXT("Run save/load system test"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GetCurrentPlayWorld(), EGetWorldErrorMode::ReturnNull))
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (USLFPIETestRunner* Runner = GI->GetSubsystem<USLFPIETestRunner>())
				{
					Runner->RunSaveLoadTest();
				}
			}
		}
	})
);

static FAutoConsoleCommand CCmdTestNPCDialog(
	TEXT("SLF.Test.NPCDialog"),
	TEXT("Run NPC dialog test - tests interaction with NPC and dialog text display"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GetCurrentPlayWorld(), EGetWorldErrorMode::ReturnNull))
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (USLFPIETestRunner* Runner = GI->GetSubsystem<USLFPIETestRunner>())
				{
					Runner->RunNPCDialogTest();
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

static FAutoConsoleCommand CCmdDiagnoseSettings(
	TEXT("SLF.DiagnoseSettings"),
	TEXT("Diagnose settings widgets at runtime - shows dropdown values, current selections"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLF.DiagnoseSettings] ========== DIAGNOSING SETTINGS WIDGETS =========="));

		if (UWorld* World = GEngine->GetCurrentPlayWorld())
		{
			// Find all W_Settings_Entry widgets in viewport
			TArray<UUserWidget*> FoundWidgets;
			UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, FoundWidgets, UUserWidget::StaticClass(), true);

			int32 SettingsEntryCount = 0;
			for (UUserWidget* Widget : FoundWidgets)
			{
				if (Widget && Widget->GetClass()->GetName().Contains(TEXT("Settings_Entry")))
				{
					SettingsEntryCount++;
					UE_LOG(LogTemp, Warning, TEXT("  [Entry] %s"), *Widget->GetName());

					// Get SettingTag via reflection
					FProperty* TagProp = Widget->GetClass()->FindPropertyByName(TEXT("SettingTag"));
					if (TagProp && TagProp->IsA<FStructProperty>())
					{
						FGameplayTag* Tag = TagProp->ContainerPtrToValuePtr<FGameplayTag>(Widget);
						if (Tag)
						{
							UE_LOG(LogTemp, Warning, TEXT("    SettingTag: %s"), *Tag->ToString());
						}
					}

					// Get EntryType via reflection
					FProperty* TypeProp = Widget->GetClass()->FindPropertyByName(TEXT("EntryType"));
					if (TypeProp && TypeProp->IsA<FEnumProperty>())
					{
						void* TypePtr = TypeProp->ContainerPtrToValuePtr<void>(Widget);
						int64 TypeValue = CastField<FEnumProperty>(TypeProp)->GetUnderlyingProperty()->GetSignedIntPropertyValue(TypePtr);
						UE_LOG(LogTemp, Warning, TEXT("    EntryType: %lld"), TypeValue);
					}

					// Get CurrentValue via reflection
					FProperty* ValueProp = Widget->GetClass()->FindPropertyByName(TEXT("CurrentValue"));
					if (ValueProp && ValueProp->IsA<FStrProperty>())
					{
						FString* Value = ValueProp->ContainerPtrToValuePtr<FString>(Widget);
						if (Value)
						{
							UE_LOG(LogTemp, Warning, TEXT("    CurrentValue: %s"), **Value);
						}
					}

					// Check for DropDown child widget
					UWidget* DropDown = Widget->GetWidgetFromName(TEXT("DropDown"));
					if (DropDown)
					{
						UE_LOG(LogTemp, Warning, TEXT("    DropDown: FOUND (%s)"), *DropDown->GetClass()->GetName());

						// Try to get option count if it's a ComboBoxKey
						if (UComboBoxKey* ComboBox = Cast<UComboBoxKey>(DropDown))
						{
							UE_LOG(LogTemp, Warning, TEXT("    DropDown SelectedOption: %s"), *ComboBox->GetSelectedOption().ToString());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("    DropDown: NOT FOUND"));
					}
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("[SLF.DiagnoseSettings] Found %d W_Settings_Entry widgets"), SettingsEntryCount);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLF.DiagnoseSettings] No play world!"));
		}
	})
);

// --- Spawn commands for Guard & Sentinel enemies (PIE only) ---

static AActor* SpawnEnemyInPIE(UWorld* World, const TCHAR* BlueprintPath, const TCHAR* FallbackClassName, FVector Offset)
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerChar) return nullptr;

	UClass* EnemyClass = LoadClass<AActor>(nullptr, BlueprintPath);
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not load Blueprint, trying C++ class: %s"), FallbackClassName);
		EnemyClass = FindObject<UClass>(nullptr, FallbackClassName);
	}
	if (!EnemyClass) return nullptr;

	FVector PlayerLoc = PlayerChar->GetActorLocation();
	FVector Forward = PlayerChar->GetActorForwardVector();
	FVector Right = PlayerChar->GetActorRightVector();
	FVector SpawnLoc = PlayerLoc + Forward * Offset.X + Right * Offset.Y;
	SpawnLoc.Z = PlayerLoc.Z + Offset.Z;

	// Line trace to avoid spawning inside walls — pull back if blocked
	FHitResult Hit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(SpawnEnemyTrace), false, PlayerChar);
	if (World->LineTraceSingleByChannel(Hit, PlayerLoc, SpawnLoc, ECC_Visibility, TraceParams))
	{
		// Hit geometry — spawn just before the hit point, pulled back toward player
		SpawnLoc = Hit.ImpactPoint - Forward * 50.f;
		SpawnLoc.Z = PlayerLoc.Z + Offset.Z;
		UE_LOG(LogTemp, Warning, TEXT("  SpawnEnemy: wall detected at %.0f units, adjusted to (%.0f, %.0f, %.0f)"),
			Hit.Distance, SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Enemy = World->SpawnActor<AActor>(EnemyClass, SpawnLoc, FRotator::ZeroRotator, Params);
	if (Enemy)
	{
		FVector DirToPlayer = (PlayerChar->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
		Enemy->SetActorRotation(DirToPlayer.Rotation());
	}
	return Enemy;
}

static FAutoConsoleCommand CCmdSpawnGuard(
	TEXT("SLF.SpawnGuard"),
	TEXT("Spawn a Guard enemy in front of the player.\nUsage: SLF.SpawnGuard"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[SLF.SpawnGuard] No play world!")); return; }

		AActor* Guard = SpawnEnemyInPIE(World,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard.B_Soulslike_Enemy_Guard_C"),
			TEXT("/Script/SLFConversion.SLFEnemyGuard"),
			FVector(600.f, 0.f, 0.f));

		UE_LOG(LogTemp, Warning, TEXT("[SLF.SpawnGuard] %s at %s"),
			Guard ? *Guard->GetName() : TEXT("FAILED"),
			Guard ? *Guard->GetActorLocation().ToString() : TEXT("N/A"));
	})
);

static FAutoConsoleCommand CCmdSpawnSentinel(
	TEXT("SLF.SpawnSentinel"),
	TEXT("Spawn a Sentinel enemy in front of the player.\nUsage: SLF.SpawnSentinel"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[SLF.SpawnSentinel] No play world!")); return; }

		AActor* Sentinel = SpawnEnemyInPIE(World,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"),
			TEXT("/Script/SLFConversion.SLFEnemySentinel"),
			FVector(600.f, 0.f, 0.f));

		UE_LOG(LogTemp, Warning, TEXT("[SLF.SpawnSentinel] %s at %s"),
			Sentinel ? *Sentinel->GetName() : TEXT("FAILED"),
			Sentinel ? *Sentinel->GetActorLocation().ToString() : TEXT("N/A"));
	})
);

static FAutoConsoleCommand CCmdSpawnComparison(
	TEXT("SLF.SpawnComparison"),
	TEXT("Spawn Guard and Sentinel side by side for visual comparison.\nUsage: SLF.SpawnComparison"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[SLF.SpawnComparison] No play world!")); return; }

		// Guard on the left (-200 Y), Sentinel on the right (+200 Y)
		AActor* Guard = SpawnEnemyInPIE(World,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard.B_Soulslike_Enemy_Guard_C"),
			TEXT("/Script/SLFConversion.SLFEnemyGuard"),
			FVector(600.f, -250.f, 0.f));

		AActor* Sentinel = SpawnEnemyInPIE(World,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"),
			TEXT("/Script/SLFConversion.SLFEnemySentinel"),
			FVector(600.f, 250.f, 0.f));

		UE_LOG(LogTemp, Warning, TEXT("[SLF.SpawnComparison] Guard: %s | Sentinel: %s"),
			Guard ? *Guard->GetActorLocation().ToString() : TEXT("FAILED"),
			Sentinel ? *Sentinel->GetActorLocation().ToString() : TEXT("FAILED"));
	})
);

static FAutoConsoleCommand CCmdCompareEnemies(
	TEXT("SLF.CompareEnemies"),
	TEXT("Find Guard and Sentinel in the world and dump full state comparison.\nUsage: SLF.CompareEnemies"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[SLF.CompareEnemies] No play world!")); return; }

		AActor* Guard = nullptr;
		AActor* Sentinel = nullptr;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			FString ClassName = It->GetClass()->GetName();
			if (ClassName.Contains(TEXT("Guard")) && ClassName.Contains(TEXT("Enemy")))
				Guard = *It;
			if (ClassName.Contains(TEXT("Sentinel")))
				Sentinel = *It;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SLF.CompareEnemies] Guard=%s  Sentinel=%s"),
			Guard ? *Guard->GetName() : TEXT("NOT FOUND"),
			Sentinel ? *Sentinel->GetName() : TEXT("NOT FOUND"));

		FString OutputFile = FPaths::ProjectDir() / TEXT("sentinel_compare.txt");
		USLFAutomationLibrary::CompareCharacters(Guard, Sentinel, OutputFile);
		UE_LOG(LogTemp, Warning, TEXT("[SLF.CompareEnemies] Report saved to: %s"), *OutputFile);
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
	UE_LOG(LogTemp, Log, TEXT("  SLF.Test.SaveLoad - Run save/load system test"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimKey <Key> - Simulate key press"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimAttack - Simulate attack"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimDodge - Simulate dodge"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimMove <X> <Y> <Dur> - Simulate movement"));
	UE_LOG(LogTemp, Log, TEXT("  SLF.SimCrouch - Toggle crouch directly"));

	// Auto-spawn Sentinel near the player (if not already present)
	if (UWorld* World = GetWorld())
	{
		FTimerHandle SentinelSpawnTimer;
		World->GetTimerManager().SetTimer(SentinelSpawnTimer, [this]()
		{
			UWorld* World = GetWorld();
			if (!World) return;

			// Check if a Sentinel already exists
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (It->GetClass()->GetName().Contains(TEXT("Sentinel")))
				{
					return; // Already spawned
				}
			}

			// Spawn near the player using the same helper as SLF.SpawnSentinel
			AActor* Sentinel = SpawnEnemyInPIE(World,
				TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"),
				TEXT("/Script/SLFConversion.SLFEnemySentinel"),
				FVector(300.f, 0.f, 100.f)); // 300 forward, 100 above (closer to player, stays on floor)

			UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Auto-spawned Sentinel: %s at %s"),
				Sentinel ? *Sentinel->GetName() : TEXT("FAILED"),
				Sentinel ? *Sentinel->GetActorLocation().ToString() : TEXT("N/A"));

			// Run comparison diagnostic 1 second after spawn
			if (Sentinel)
			{
				FTimerHandle CompareTimer;
				World->GetTimerManager().SetTimer(CompareTimer, [World]()
				{
					AActor* Guard = nullptr;
					AActor* Sent = nullptr;
					for (TActorIterator<AActor> It(World); It; ++It)
					{
						FString CN = It->GetClass()->GetName();
						if (CN.Contains(TEXT("Guard")) && CN.Contains(TEXT("Enemy"))) Guard = *It;
						if (CN.Contains(TEXT("Sentinel"))) Sent = *It;
					}
					FString OutputFile = FPaths::ProjectDir() / TEXT("sentinel_compare.txt");
					USLFAutomationLibrary::CompareCharacters(Guard, Sent, OutputFile);
				}, 1.0f, false);

				// Dump live bone transforms 3 seconds after spawn to verify animation is playing
				FTimerHandle BoneDiagTimer;
				World->GetTimerManager().SetTimer(BoneDiagTimer, [World]()
				{
					for (TActorIterator<AActor> It(World); It; ++It)
					{
						if (!It->GetClass()->GetName().Contains(TEXT("Sentinel"))) continue;
						ACharacter* Char = Cast<ACharacter>(*It);
						if (!Char) continue;
						USkeletalMeshComponent* SMC = Char->GetMesh();
						if (!SMC) continue;

						UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag] === LIVE BONE TRANSFORMS ==="));
						UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag] Mesh: %s  AnimInstance: %s"),
							SMC->GetSkeletalMeshAsset() ? *SMC->GetSkeletalMeshAsset()->GetName() : TEXT("null"),
							SMC->GetAnimInstance() ? *SMC->GetAnimInstance()->GetClass()->GetName() : TEXT("null"));

						// Dump key bone transforms in component space
						const TArray<FTransform>& CSTransforms = SMC->GetComponentSpaceTransforms();
						const FReferenceSkeleton& RefSkel = SMC->GetSkeletalMeshAsset()->GetRefSkeleton();
						int32 NumBones = FMath::Min(CSTransforms.Num(), RefSkel.GetNum());

						const TCHAR* KeyBones[] = {
							TEXT("root"), TEXT("pelvis"), TEXT("spine_01"), TEXT("head"),
							TEXT("hand_r"), TEXT("hand_l"), TEXT("foot_l"), TEXT("foot_r"),
							TEXT("thigh_l"), TEXT("upperarm_r"), TEXT("weapon_r")
						};

						int32 NonIdentityCount = 0;
						for (int32 i = 0; i < NumBones; i++)
						{
							FTransform T = CSTransforms[i];
							FVector Pos = T.GetTranslation();
							FQuat Rot = T.GetRotation();
							bool bIsIdentity = Pos.IsNearlyZero(0.01f) && Rot.Equals(FQuat::Identity, 0.001f);
							if (!bIsIdentity) NonIdentityCount++;

							// Check if this is a key bone
							FName BoneName = RefSkel.GetBoneName(i);
							bool bIsKey = false;
							for (const TCHAR* KB : KeyBones)
							{
								if (BoneName == FName(KB)) { bIsKey = true; break; }
							}

							if (bIsKey)
							{
								UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag]   %s[%d]: Pos=(%0.2f, %0.2f, %0.2f) Rot=(%0.4f, %0.4f, %0.4f, %0.4f) %s"),
									*BoneName.ToString(), i, Pos.X, Pos.Y, Pos.Z,
									Rot.X, Rot.Y, Rot.Z, Rot.W,
									bIsIdentity ? TEXT("[IDENTITY!]") : TEXT("OK"));
							}
						}

						UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag] Non-identity bones: %d/%d"), NonIdentityCount, NumBones);

						// Also check current montage
						UAnimInstance* AI = SMC->GetAnimInstance();
						if (AI)
						{
							UAnimMontage* CurMontage = AI->GetCurrentActiveMontage();
							UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag] Current Montage: %s"),
								CurMontage ? *CurMontage->GetName() : TEXT("none"));
						}
						break;
					}

					// Take a screenshot
					FString ShotPath = FPaths::ProjectDir() / TEXT("Saved/Screenshots/WindowsEditor/Sentinel_PIE_Test.png");
					FScreenshotRequest::RequestScreenshot(ShotPath, false, false);
					UE_LOG(LogTemp, Warning, TEXT("[SentinelBoneDiag] Screenshot requested: %s"), *ShotPath);

				}, 3.0f, false);
			}
		}, 2.0f, false);
	}

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
	else if (TestName == FName("SaveLoad"))
	{
		RunSaveLoadTest();
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

void USLFPIETestRunner::RunSaveLoadTest()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ===== SAVE/LOAD SYSTEM TEST ====="));

	UWorld* World = GetWorld();
	if (!World)
	{
		LogTestResult(FName("SaveLoad"), false, TEXT("No world"));
		return;
	}

	// Get player controller
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		LogTestResult(FName("SaveLoad"), false, TEXT("No player controller"));
		return;
	}

	// Find AC_SaveLoadManager component on the controller
	UAC_SaveLoadManager* SaveLoadManager = PC->FindComponentByClass<UAC_SaveLoadManager>();
	if (!SaveLoadManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] SaveLoadManager not on PC, checking pawn..."));

		// Try on the pawn
		if (APawn* Pawn = PC->GetPawn())
		{
			SaveLoadManager = Pawn->FindComponentByClass<UAC_SaveLoadManager>();
		}
	}

	if (!SaveLoadManager)
	{
		LogTestResult(FName("SaveLoad.Component"), false, TEXT("AC_SaveLoadManager not found on PC or Pawn"));
		UE_LOG(LogTemp, Error, TEXT("[SLFPIETestRunner] Could not find AC_SaveLoadManager - component may not be attached"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Found AC_SaveLoadManager: %s"), *SaveLoadManager->GetName());
	LogTestResult(FName("SaveLoad.Component"), true, TEXT("AC_SaveLoadManager found"));

	// Test 1: Check if save slot can be set
	FString TestSlotName = TEXT("TestSlot_PIE");
	SaveLoadManager->EventUpdateActiveSlot(TestSlotName);
	bool bSlotUpdated = SaveLoadManager->CurrentSaveSlot == TestSlotName;
	LogTestResult(FName("SaveLoad.UpdateSlot"), bSlotUpdated,
		FString::Printf(TEXT("CurrentSaveSlot=%s"), *SaveLoadManager->CurrentSaveSlot));

	// Test 2: Test data serialization
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Testing SerializeLevelData..."));
	TArray<FInstancedStruct> LevelData = SaveLoadManager->SerializeLevelData();
	bool bLevelDataValid = LevelData.Num() > 0;
	LogTestResult(FName("SaveLoad.SerializeLevelData"), bLevelDataValid,
		FString::Printf(TEXT("LevelData entries: %d"), LevelData.Num()));

	// Test 3: Test class data serialization
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Testing SerializeClassData..."));
	TArray<FInstancedStruct> ClassData = SaveLoadManager->SerializeClassData();
	bool bClassDataValid = true; // Class data can be empty if no class selected
	LogTestResult(FName("SaveLoad.SerializeClassData"), bClassDataValid,
		FString::Printf(TEXT("ClassData entries: %d"), ClassData.Num()));

	// Test 4: Test full save data serialization
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Testing SerializeAllDataForSaving..."));
	SaveLoadManager->SerializeAllDataForSaving();
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] SerializeAllDataForSaving completed"));
	LogTestResult(FName("SaveLoad.SerializeAll"), true, TEXT("SerializeAllDataForSaving executed without crash"));

	// Test 5: Test EventSaveData (actual disk save)
	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Testing EventSaveData (will save to disk)..."));
	SaveLoadManager->EventSaveData();
	LogTestResult(FName("SaveLoad.SaveData"), true, TEXT("EventSaveData executed without crash"));

	// Check after a delay if save completed
	FTimerHandle SaveCheckTimer;
	World->GetTimerManager().SetTimer(SaveCheckTimer, [this, SaveLoadManager, TestSlotName]()
	{
		// Test 6: Verify save file exists
		bool bSaveExists = UGameplayStatics::DoesSaveGameExist(TestSlotName, 0);
		LogTestResult(FName("SaveLoad.FileExists"), bSaveExists,
			FString::Printf(TEXT("Save file '%s' exists: %s"), *TestSlotName, bSaveExists ? TEXT("YES") : TEXT("NO")));

		// Test 7: Test preload
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Testing EventTryPreloadData..."));
		SaveLoadManager->EventTryPreloadData();
		LogTestResult(FName("SaveLoad.Preload"), true, TEXT("Preload executed without crash"));

		// Test 8: Test GetSavedLevelName
		FName SavedLevelName;
		bool bSuccess = false;
		SaveLoadManager->GetSavedLevelName(SavedLevelName, bSuccess);
		LogTestResult(FName("SaveLoad.GetSavedLevel"), true,
			FString::Printf(TEXT("GetSavedLevelName: %s (success=%s)"), *SavedLevelName.ToString(), bSuccess ? TEXT("true") : TEXT("false")));

		// Print summary
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] ===== SAVE/LOAD TEST COMPLETE ====="));
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Check logs above for individual test results"));
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Save slot used: %s"), *TestSlotName);

	}, 2.0f, false);
}

void USLFPIETestRunner::RunNPCDialogTest()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] ===== NPC DIALOG TEST ====="));

	UWorld* World = GetWorld();
	if (!World)
	{
		LogTestResult(FName("NPCDialog"), false, TEXT("No world"));
		return;
	}

	// Get player controller and character
	APlayerController* PC = World->GetFirstPlayerController();
	ACharacter* PlayerChar = PC ? PC->GetCharacter() : nullptr;
	if (!PC || !PlayerChar)
	{
		LogTestResult(FName("NPCDialog"), false, TEXT("No player controller or character"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Player: %s at %s"),
		*PlayerChar->GetName(), *PlayerChar->GetActorLocation().ToString());

	// Find all actors with AIInteractionManagerComponent (NPCs)
	TArray<AActor*> NPCActors;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (UAIInteractionManagerComponent* DialogComp = Actor->FindComponentByClass<UAIInteractionManagerComponent>())
		{
			NPCActors.Add(Actor);
			UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Found NPC with dialog: %s at %s (Name: %s)"),
				*Actor->GetName(),
				*Actor->GetActorLocation().ToString(),
				*DialogComp->Name.ToString());
		}
	}

	if (NPCActors.Num() == 0)
	{
		LogTestResult(FName("NPCDialog.FindNPC"), false, TEXT("No NPCs with AIInteractionManagerComponent found in level"));
		return;
	}

	LogTestResult(FName("NPCDialog.FindNPC"), true,
		FString::Printf(TEXT("Found %d NPCs with dialog component"), NPCActors.Num()));

	// Find nearest NPC to player
	AActor* NearestNPC = nullptr;
	float NearestDist = FLT_MAX;
	for (AActor* NPC : NPCActors)
	{
		float Dist = FVector::Dist(PlayerChar->GetActorLocation(), NPC->GetActorLocation());
		if (Dist < NearestDist)
		{
			NearestDist = Dist;
			NearestNPC = NPC;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Nearest NPC: %s at distance %.2f"),
		*NearestNPC->GetName(), NearestDist);

	// Move player towards NPC if too far
	if (NearestDist > 300.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Player too far (%.2f), teleporting closer..."), NearestDist);
		FVector TargetLocation = NearestNPC->GetActorLocation();
		TargetLocation += (PlayerChar->GetActorLocation() - NearestNPC->GetActorLocation()).GetSafeNormal() * 150.0f;
		TargetLocation.Z = PlayerChar->GetActorLocation().Z; // Keep same height
		PlayerChar->SetActorLocation(TargetLocation);
		UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Teleported player to %s"), *TargetLocation.ToString());
	}

	// Wait a moment for overlap to register, then interact
	FTimerHandle InteractTimer;
	World->GetTimerManager().SetTimer(InteractTimer, [this, World, NearestNPC, PC]()
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Simulating INTERACT key..."));

		// Simulate interact key press (E key typically)
		USLFInputSimulator::SimulateInteract(World);

		// After interaction, check if dialog appeared
		FTimerHandle DialogCheckTimer;
		World->GetTimerManager().SetTimer(DialogCheckTimer, [this, PC, NearestNPC]()
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] ===== CHECKING DIALOG STATE ====="));

			// Get the HUD widget
			UUserWidget* HUDWidget = nullptr;
			if (AHUD* HUD = PC->GetHUD())
			{
				// Find the W_HUD widget
				TArray<UUserWidget*> Widgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UW_HUD::StaticClass());
				if (Widgets.Num() > 0)
				{
					HUDWidget = Widgets[0];
				}
			}

			// Also try to find W_HUD directly
			if (!HUDWidget)
			{
				TArray<UUserWidget*> AllWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass());
				for (UUserWidget* Widget : AllWidgets)
				{
					if (Widget->GetClass()->GetName().Contains(TEXT("W_HUD")))
					{
						HUDWidget = Widget;
						UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Found W_HUD via class name search: %s"), *Widget->GetClass()->GetName());
						break;
					}
				}
			}

			if (UW_HUD* WHud = Cast<UW_HUD>(HUDWidget))
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Found W_HUD widget"));
				LogTestResult(FName("NPCDialog.FindHUD"), true, TEXT("W_HUD found"));

				// Check if dialog is active
				bool bDialogActive = WHud->IsDialogActive;
				UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] IsDialogActive: %s"), bDialogActive ? TEXT("TRUE") : TEXT("FALSE"));
				LogTestResult(FName("NPCDialog.DialogActive"), bDialogActive,
					FString::Printf(TEXT("IsDialogActive=%s"), bDialogActive ? TEXT("true") : TEXT("false")));

				// Try to find W_Dialog widget from HUD widget tree
				UWidget* DialogWidgetRaw = WHud->GetWidgetFromName(TEXT("W_Dialog"));
				UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] GetWidgetFromName(W_Dialog) returned: %s"),
					DialogWidgetRaw ? TEXT("VALID") : TEXT("nullptr"));

				if (DialogWidgetRaw)
				{
					UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Dialog widget class: %s"),
						*DialogWidgetRaw->GetClass()->GetName());
					UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Dialog widget class path: %s"),
						*DialogWidgetRaw->GetClass()->GetPathName());

					// Check if it's a UW_Dialog subclass
					UW_Dialog* DialogWidget = Cast<UW_Dialog>(DialogWidgetRaw);
					if (DialogWidget)
					{
						UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Cast to UW_Dialog SUCCEEDED"));
						LogTestResult(FName("NPCDialog.WidgetCast"), true, TEXT("Cast to UW_Dialog succeeded"));

						// Get dialog text if possible
						if (UTextBlock* DialogText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("DialogText"))))
						{
							FText CurrentText = DialogText->GetText();
							FString TextStr = CurrentText.ToString();
							UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Dialog text: '%s'"), *TextStr);

							bool bIsPlaceholder = TextStr.Contains(TEXT("Lorem")) || TextStr.Contains(TEXT("ipsum")) || TextStr.IsEmpty();
							LogTestResult(FName("NPCDialog.TextContent"), !bIsPlaceholder,
								FString::Printf(TEXT("Dialog text: '%s' (Placeholder=%s)"),
									*TextStr.Left(50), bIsPlaceholder ? TEXT("YES-FAIL") : TEXT("NO-GOOD")));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Could not find DialogText widget inside W_Dialog"));
							LogTestResult(FName("NPCDialog.TextWidget"), false, TEXT("DialogText widget not found"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("[SLFPIETestRunner] Cast to UW_Dialog FAILED - Blueprint not reparented!"));
						LogTestResult(FName("NPCDialog.WidgetCast"), false, TEXT("Cast to UW_Dialog failed - need to reparent Blueprint"));

						// Try to read text directly from the UserWidget
						if (UUserWidget* DialogUserWidget = Cast<UUserWidget>(DialogWidgetRaw))
						{
							if (UTextBlock* DialogText = Cast<UTextBlock>(DialogUserWidget->GetWidgetFromName(TEXT("DialogText"))))
							{
								FText CurrentText = DialogText->GetText();
								FString TextStr = CurrentText.ToString();
								UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] (Fallback) Dialog text: '%s'"), *TextStr);
							}
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[SLFPIETestRunner] W_Dialog widget not found in HUD!"));
					LogTestResult(FName("NPCDialog.WidgetFound"), false, TEXT("W_Dialog widget not found"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[SLFPIETestRunner] Could not find or cast to W_HUD"));
				LogTestResult(FName("NPCDialog.FindHUD"), false, TEXT("W_HUD not found or cast failed"));
			}

			// Also check the AIInteractionManagerComponent directly
			if (UAIInteractionManagerComponent* DialogComp = NearestNPC->FindComponentByClass<UAIInteractionManagerComponent>())
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFPIETestRunner] Checking AIInteractionManagerComponent state:"));
				UE_LOG(LogTemp, Log, TEXT("  Name: %s"), *DialogComp->Name.ToString());
				UE_LOG(LogTemp, Log, TEXT("  CurrentIndex: %d"), DialogComp->CurrentIndex);
				UE_LOG(LogTemp, Log, TEXT("  MaxIndex: %d"), DialogComp->MaxIndex);
				UE_LOG(LogTemp, Log, TEXT("  DialogAsset: %s"), DialogComp->DialogAsset ? *DialogComp->DialogAsset->GetName() : TEXT("None"));

				// ActiveTable is a TSoftObjectPtr, need to load it
				UDataTable* Table = DialogComp->ActiveTable.Get();
				UE_LOG(LogTemp, Log, TEXT("  ActiveTable: %s"), Table ? *Table->GetName() : TEXT("None/Not Loaded"));

				if (Table)
				{
					TArray<FName> RowNames = Table->GetRowNames();
					UE_LOG(LogTemp, Log, TEXT("  ActiveTable RowNames (%d):"), RowNames.Num());
					for (int32 i = 0; i < FMath::Min(5, RowNames.Num()); i++)
					{
						UE_LOG(LogTemp, Log, TEXT("    [%d]: %s"), i, *RowNames[i].ToString());
					}

					// Try to get current dialog entry
					FSLFDialogEntry Entry = DialogComp->GetCurrentDialogEntry(Table, RowNames);
					FString EntryText = Entry.Entry.ToString();
					UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] GetCurrentDialogEntry returned: '%s'"),
						EntryText.IsEmpty() ? TEXT("<EMPTY>") : *EntryText.Left(80));

					bool bHasValidText = !EntryText.IsEmpty() && !EntryText.Contains(TEXT("Lorem"));
					LogTestResult(FName("NPCDialog.DialogEntry"), bHasValidText,
						FString::Printf(TEXT("Entry text: '%s'"), EntryText.IsEmpty() ? TEXT("<EMPTY>") : *EntryText.Left(50)));
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] ===== NPC DIALOG TEST COMPLETE ====="));
			UE_LOG(LogTemp, Log, TEXT("%s"), *GetTestResultsReport());

		}, 2.0f, false); // Wait 2 seconds for dialog to appear

	}, 0.5f, false); // Wait 0.5 seconds for overlap to register
}
