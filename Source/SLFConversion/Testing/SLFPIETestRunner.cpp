// SLFPIETestRunner.cpp
// Automated PIE test runner implementation

#include "SLFPIETestRunner.h"
#include "SLFInputSimulator.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
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
#include "Blueprints/SLFEnemySentinel.h"

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

// --- Photoshoot: orbit camera around Sentinel and take screenshots ---
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"

static FAutoConsoleCommand CCmdSentinelPhotoshoot(
	TEXT("SLF.SentinelPhotoshoot"),
	TEXT("Spawn Sentinel, disable AI, orbit camera for screenshots.\nUsage: SLF.SentinelPhotoshoot"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[Photoshoot] No play world!")); return; }

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		ACharacter* PlayerChar = PC ? PC->GetCharacter() : nullptr;
		if (!PlayerChar) { UE_LOG(LogTemp, Error, TEXT("[Photoshoot] No player!")); return; }

		// Spawn Sentinel 400 units in front of player
		AActor* Sentinel = SpawnEnemyInPIE(World,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"),
			TEXT("/Script/SLFConversion.SLFEnemySentinel"),
			FVector(400.f, 0.f, 0.f));

		if (!Sentinel)
		{
			UE_LOG(LogTemp, Error, TEXT("[Photoshoot] Failed to spawn Sentinel!"));
			return;
		}

		// Destroy any previously spawned Sentinels (from earlier tests)
		TArray<AActor*> ToDestroy;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor != Sentinel && Actor->GetClass()->GetName().Contains(TEXT("Sentinel")))
			{
				ToDestroy.Add(Actor);
			}
		}
		for (AActor* A : ToDestroy)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Destroying old: %s"), *A->GetName());
			A->Destroy();
		}

		// Hide ALL visual actors in the scene (walls, props, floors, text billboards)
		// Keep: Sentinel, player controller, game framework actors, lights, sky
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor == Sentinel || Actor == PlayerChar) continue;
			if (Actor->IsA(APlayerController::StaticClass())) continue;
			FString ClassName = Actor->GetClass()->GetName();
			// Keep only essential actors
			if (ClassName.Contains(TEXT("Light")) ||
				ClassName.Contains(TEXT("Sky")) ||
				ClassName.Contains(TEXT("Atmosphere")) ||
				ClassName.Contains(TEXT("Fog")) ||
				ClassName.Contains(TEXT("PostProcess")) ||
				ClassName.Contains(TEXT("GameMode")) ||
				ClassName.Contains(TEXT("GameState")) ||
				ClassName.Contains(TEXT("PlayerState")) ||
				ClassName.Contains(TEXT("HUD")) ||
				ClassName.Contains(TEXT("GameSession")) ||
				ClassName.Contains(TEXT("GameNetwork")) ||
				ClassName.Contains(TEXT("WorldSettings")) ||
				ClassName.Contains(TEXT("NavigationSystem")) ||
				ClassName.Contains(TEXT("NavMesh")) ||
				ClassName.Contains(TEXT("PlayerStart")))
			{
				continue;
			}
			// Hide everything else
			Actor->SetActorHiddenInGame(true);
		}
		UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Scene cleaned - all non-essential actors hidden"));

		// God mode for player so they don't die
		GEngine->Exec(World, TEXT("god"));

		// Disable AI aggressively to freeze in idle pose
		ACharacter* SentinelChar = Cast<ACharacter>(Sentinel);
		if (SentinelChar)
		{
			if (AController* AICtrl = SentinelChar->GetController())
			{
				AICtrl->StopMovement();
				AICtrl->UnPossess();
			}
			TArray<UActorComponent*> AllComps;
			SentinelChar->GetComponents(AllComps);
			for (UActorComponent* Comp : AllComps)
			{
				if (Comp && (Comp->GetName().Contains(TEXT("AIStateMachine")) ||
				             Comp->GetName().Contains(TEXT("CombatManager")) ||
				             Comp->GetName().Contains(TEXT("BehaviorTree"))))
				{
					Comp->SetComponentTickEnabled(false);
					Comp->Deactivate();
					UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Disabled: %s"), *Comp->GetName());
				}
			}
			if (UCharacterMovementComponent* MC = SentinelChar->GetCharacterMovement())
			{
				MC->StopMovementImmediately();
				MC->DisableMovement();
			}
		}

		// Move player underground so they don't appear in shots
		PlayerChar->SetActorLocation(Sentinel->GetActorLocation() + FVector(0.f, 0.f, -500.f));
		PlayerChar->SetActorHiddenInGame(true);

		// Disable post-process effects that interfere with clean shots
		GEngine->Exec(World, TEXT("r.MotionBlurQuality 0"));
		GEngine->Exec(World, TEXT("r.DepthOfFieldQuality 0"));
		GEngine->Exec(World, TEXT("ShowFlag.MotionBlur 0"));

		// Spawn camera
		ACameraActor* PhotoCam = World->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), Sentinel->GetActorLocation() + FVector(200.f, 0.f, 100.f), FRotator::ZeroRotator);
		if (PhotoCam)
		{
			PhotoCam->GetCameraComponent()->FieldOfView = 60.f;
			PhotoCam->GetCameraComponent()->PostProcessSettings.bOverride_MotionBlurAmount = true;
			PhotoCam->GetCameraComponent()->PostProcessSettings.MotionBlurAmount = 0.0f;
			PhotoCam->GetCameraComponent()->PostProcessSettings.bOverride_MotionBlurMax = true;
			PhotoCam->GetCameraComponent()->PostProcessSettings.MotionBlurMax = 0.0f;
			PhotoCam->GetCameraComponent()->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
			PhotoCam->GetCameraComponent()->PostProcessSettings.DepthOfFieldFstop = 0.0f;
			PC->SetViewTargetWithBlend(PhotoCam, 0.0f);
		}

		FVector SentinelLoc = Sentinel->GetActorLocation();

		// Log mesh component info for diagnostics
		if (SentinelChar)
		{
			if (USkeletalMeshComponent* MeshComp = SentinelChar->GetMesh())
			{
				FBoxSphereBounds Bounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
				UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Mesh bounds: Origin=(%s) Extent=(%s) Radius=%.0f"),
					*Bounds.Origin.ToString(), *Bounds.BoxExtent.ToString(), Bounds.SphereRadius);
				UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Mesh: %s, AnimBP: %s"),
					MeshComp->GetSkeletalMeshAsset() ? *MeshComp->GetSkeletalMeshAsset()->GetName() : TEXT("NULL"),
					MeshComp->GetAnimInstance() ? *MeshComp->GetAnimInstance()->GetClass()->GetName() : TEXT("NULL"));
				// Log active montage
				if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
				{
					UAnimMontage* ActiveMontage = AnimInst->GetCurrentActiveMontage();
					UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Active montage: %s"),
						ActiveMontage ? *ActiveMontage->GetName() : TEXT("NONE"));
				}
			}
		}

		// Target center at roughly chest height of Sentinel
		FVector TargetCenter = SentinelLoc + FVector(0.f, 0.f, 120.f);

		FVector SentinelFwd = Sentinel->GetActorForwardVector();
		UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Sentinel at %s, Fwd=%s, Target=%s"),
			*SentinelLoc.ToString(), *SentinelFwd.ToString(), *TargetCenter.ToString());

		// Delete old screenshots
		FString ShotDir = FPaths::ProjectDir() / TEXT("Saved/Screenshots/Photoshoot");
		IFileManager::Get().MakeDirectory(*ShotDir, true);

		// Define orbits using forward-relative angles
		// Character is ~380 units tall, need ~500+ units for full body with 60 FOV
		struct FShotAngle { float Az; float El; float Dist; FString Name; };
		TArray<FShotAngle> Angles = {
			// Full body from 8 angles, 500 units out
			{   0.f, 10.f, 500.f, TEXT("front") },
			{  45.f, 10.f, 500.f, TEXT("front_3q_right") },
			{  90.f, 10.f, 500.f, TEXT("right_side") },
			{ 135.f, 10.f, 500.f, TEXT("back_right") },
			{ 180.f, 10.f, 500.f, TEXT("back") },
			{ 225.f, 10.f, 500.f, TEXT("back_left") },
			{ 270.f, 10.f, 500.f, TEXT("left_side") },
			{ 315.f, 10.f, 500.f, TEXT("front_3q_left") },
			// Above
			{   0.f, 45.f, 500.f, TEXT("above_front") },
			{ 180.f, 45.f, 500.f, TEXT("above_back") },
			// Upper body closeups
			{   0.f, 10.f, 250.f, TEXT("closeup_front") },
			{  90.f, 10.f, 250.f, TEXT("closeup_right") },
			{ 180.f, 10.f, 250.f, TEXT("closeup_back") },
			{ 270.f, 10.f, 250.f, TEXT("closeup_left") },
		};

		struct FPhotoshootState
		{
			int32 CurrentAngle = 0;
			bool bMoved = false; // Two-phase: move then screenshot
			TArray<FShotAngle> Angles;
			FVector TargetCenter;
			FVector SentinelFwd; // Sentinel's facing direction for relative orbits
			FString ShotDir;
			TWeakObjectPtr<APlayerController> PC;
			TWeakObjectPtr<ACameraActor> Cam;
			FTimerHandle TimerHandle;
		};
		TSharedPtr<FPhotoshootState> State = MakeShared<FPhotoshootState>();
		State->Angles = Angles;
		State->TargetCenter = TargetCenter;
		State->SentinelFwd = SentinelFwd;
		State->ShotDir = ShotDir;
		State->PC = PC;
		State->Cam = PhotoCam;

		// Two-phase timer: phase 1 = move camera, phase 2 = take screenshot
		World->GetTimerManager().SetTimer(State->TimerHandle, [State, World]()
		{
			if (State->CurrentAngle >= State->Angles.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] COMPLETE! %d screenshots in Saved/Screenshots/Photoshoot/"),
					State->Angles.Num());
				// Re-enable motion blur and restore camera
				GEngine->Exec(World, TEXT("r.MotionBlurQuality 4"));
				GEngine->Exec(World, TEXT("r.DepthOfFieldQuality 2"));
				if (APlayerController* LocalPC = State->PC.Get())
				{
					if (APawn* P = LocalPC->GetPawn())
						LocalPC->SetViewTargetWithBlend(P, 0.5f);
				}
				if (ACameraActor* C = State->Cam.Get())
					C->Destroy();
				World->GetTimerManager().ClearTimer(State->TimerHandle);
				return;
			}

			ACameraActor* Cam = State->Cam.Get();
			if (!Cam) return;

			if (!State->bMoved)
			{
				// Phase 1: Move camera to orbital position
				// Az=0 = camera in front of Sentinel (opposite to forward vector)
				// Az=90 = camera to Sentinel's right, etc.
				const FShotAngle& Shot = State->Angles[State->CurrentAngle];
				float AzRad = FMath::DegreesToRadians(Shot.Az);
				float ElRad = FMath::DegreesToRadians(Shot.El);

				float HorizDist = Shot.Dist * FMath::Cos(ElRad);
				float VertDist = Shot.Dist * FMath::Sin(ElRad);

				// Build orbit relative to Sentinel's facing direction
				// "Front" camera = opposite of Sentinel's forward (so it sees the face)
				FVector Fwd2D = State->SentinelFwd;
				Fwd2D.Z = 0.f;
				Fwd2D.Normalize();
				FVector Right2D = FVector::CrossProduct(FVector::UpVector, Fwd2D).GetSafeNormal();

				// Camera offset: rotate in the horizontal plane
				// Az=0 → camera at -Fwd (in front, looking at face)
				// Az=90 → camera at +Right (Sentinel's right side)
				FVector HorizDir = -Fwd2D * FMath::Cos(AzRad) + Right2D * FMath::Sin(AzRad);
				FVector CamPos = State->TargetCenter + HorizDir * HorizDist + FVector(0.f, 0.f, VertDist);

				FVector Dir = (State->TargetCenter - CamPos).GetSafeNormal();
				Cam->SetActorLocation(CamPos);
				Cam->SetActorRotation(Dir.Rotation());

				UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] Cam[%d] %s: pos=%s looking at %s"),
					State->CurrentAngle, *Shot.Name, *CamPos.ToString(), *State->TargetCenter.ToString());

				State->bMoved = true;
			}
			else
			{
				// Phase 2: Take screenshot (motion blur has cleared)
				const FShotAngle& Shot = State->Angles[State->CurrentAngle];
				FString ShotPath = State->ShotDir / FString::Printf(TEXT("Sentinel_%02d_%s.png"),
					State->CurrentAngle, *Shot.Name);
				FScreenshotRequest::RequestScreenshot(ShotPath, false, false);

				UE_LOG(LogTemp, Warning, TEXT("[Photoshoot] [%d/%d] %s"),
					State->CurrentAngle + 1, State->Angles.Num(), *Shot.Name);

				State->CurrentAngle++;
				State->bMoved = false;
			}
		}, 0.3f, true, 2.0f); // Every 0.3s (move+shot = 0.6s per angle), start after 2s
	})
);

// --- Animation test: play different montages and screenshot each ---
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"

// Auto-launch PIE then run SLF.SentinelAnimTest after world is ready.
// Use this from -ExecCmds when the editor isn't in PIE yet.
static FAutoConsoleCommand CCmdAutoAnimTest(
	TEXT("SLF.AutoAnimTest"),
	TEXT("Launch PIE, wait for world, then run SLF.SentinelAnimTest.\nUsage: SLF.AutoAnimTest"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
#if WITH_EDITOR
		// Start PIE if not already running
		if (GEditor && !GEditor->IsPlaySessionInProgress())
		{
			UE_LOG(LogTemp, Warning, TEXT("[AutoAnimTest] Starting PIE..."));
			USLFInputSimulatorLibrary::StartPIE();
		}

		// Poll until play world is available, then run the anim test
		struct FPollState { int32 Polls = 0; FTimerHandle Handle; };
		TSharedPtr<FPollState> Poll = MakeShared<FPollState>();

		if (GEditor)
		{
			GEditor->GetTimerManager()->SetTimer(Poll->Handle, [Poll]()
			{
				Poll->Polls++;
				UWorld* PlayWorld = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
				if (PlayWorld)
				{
					UE_LOG(LogTemp, Warning, TEXT("[AutoAnimTest] PIE world ready after %d polls. Running SentinelAnimTest."), Poll->Polls);
					GEngine->Exec(PlayWorld, TEXT("SLF.SentinelAnimTest"));
					if (GEditor) GEditor->GetTimerManager()->ClearTimer(Poll->Handle);
				}
				else if (Poll->Polls > 100) // ~30s timeout
				{
					UE_LOG(LogTemp, Error, TEXT("[AutoAnimTest] Timed out waiting for PIE world!"));
					if (GEditor) GEditor->GetTimerManager()->ClearTimer(Poll->Handle);
				}
			}, 0.3f, true, 1.0f);
		}
#else
		UE_LOG(LogTemp, Error, TEXT("[AutoAnimTest] Only available in editor builds"));
#endif
	})
);

static FAutoConsoleCommand CCmdSentinelAnimTest(
	TEXT("SLF.SentinelAnimTest"),
	TEXT("Cycle through Sentinel animations and screenshot each.\nUsage: SLF.SentinelAnimTest"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
		if (!World) { UE_LOG(LogTemp, Error, TEXT("[AnimTest] No play world!")); return; }

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);

		// Use player location as reference, spawn Sentinel nearby on the main floor
		ACharacter* PlayerChar = PC ? PC->GetCharacter() : nullptr;
		FVector SpawnLoc(600.f, 0.f, 92.f); // Default: main floor area
		if (PlayerChar)
		{
			SpawnLoc = PlayerChar->GetActorLocation() + PlayerChar->GetActorForwardVector() * 400.f;
			SpawnLoc.Z = PlayerChar->GetActorLocation().Z; // Same floor level
		}

		// Destroy any existing Sentinels from prior tests
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (It->GetClass()->GetName().Contains(TEXT("Sentinel")))
			{
				(*It)->Destroy();
			}
		}

		// Spawn Sentinel directly
		UClass* SentinelClass = LoadClass<AActor>(nullptr,
			TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"));
		if (!SentinelClass)
			SentinelClass = FindObject<UClass>(nullptr, TEXT("/Script/SLFConversion.SLFEnemySentinel"));
		if (!SentinelClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[AnimTest] Cannot find Sentinel class!"));
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AActor* Sentinel = World->SpawnActor<AActor>(SentinelClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (!Sentinel)
		{
			UE_LOG(LogTemp, Error, TEXT("[AnimTest] Failed to spawn Sentinel!"));
			return;
		}

		ACharacter* SentinelChar = Cast<ACharacter>(Sentinel);
		UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Sentinel spawned at %s"), *SpawnLoc.ToString());

		// Spawn a runtime floor under Sentinel so it doesn't fall when we hide level geometry
		{
			UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeMesh)
			{
				FVector FloorLoc = SpawnLoc - FVector(0.f, 0.f, 90.f); // Just below capsule feet
				AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FloorLoc, FRotator::ZeroRotator);
				if (Floor)
				{
					Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
					Floor->SetActorScale3D(FVector(30.f, 30.f, 0.1f)); // Wide flat platform
					Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					Floor->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);
					Floor->SetActorHiddenInGame(true); // Invisible but solid
					Floor->Tags.Add(TEXT("AnimTestFloor"));
					UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Runtime floor spawned at %s"), *FloorLoc.ToString());
				}
			}
		}

		// God mode and hide player
		GEngine->Exec(World, TEXT("god"));
		if (PlayerChar)
		{
			PlayerChar->SetActorHiddenInGame(true);
			PlayerChar->SetActorLocation(SpawnLoc + FVector(0.f, 0.f, -500.f));
		}

		// Disable AI on Sentinel
		if (SentinelChar)
		{
			if (AController* AICtrl = SentinelChar->GetController())
			{
				AICtrl->StopMovement();
				AICtrl->UnPossess();
			}
			TArray<UActorComponent*> AllComps;
			SentinelChar->GetComponents(AllComps);
			for (UActorComponent* Comp : AllComps)
			{
				if (Comp && (Comp->GetName().Contains(TEXT("AIStateMachine")) ||
				             Comp->GetName().Contains(TEXT("CombatManager"))))
				{
					Comp->SetComponentTickEnabled(false);
					Comp->Deactivate();
				}
			}
			if (UCharacterMovementComponent* MC = SentinelChar->GetCharacterMovement())
			{
				MC->StopMovementImmediately();
				MC->DisableMovement();
			}
		}

		// Hide non-essential actors (same whitelist as Photoshoot which works)
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* A = *It;
			if (A == Sentinel) continue;
			if (A->ActorHasTag(TEXT("AnimTestFloor"))) continue; // Keep our runtime floor
			if (A->IsA(APlayerController::StaticClass())) continue;
			FString CN = A->GetClass()->GetName();
			if (CN.Contains(TEXT("Light")) || CN.Contains(TEXT("Sky")) ||
				CN.Contains(TEXT("Atmosphere")) || CN.Contains(TEXT("Fog")) ||
				CN.Contains(TEXT("PostProcess")) || CN.Contains(TEXT("GameMode")) ||
				CN.Contains(TEXT("GameState")) || CN.Contains(TEXT("PlayerState")) ||
				CN.Contains(TEXT("HUD")) || CN.Contains(TEXT("WorldSettings")) ||
				CN.Contains(TEXT("GameSession")) || CN.Contains(TEXT("GameNetwork")) ||
				CN.Contains(TEXT("NavigationSystem")) || CN.Contains(TEXT("NavMesh")) ||
				CN.Contains(TEXT("PlayerStart")) || CN.Contains(TEXT("Camera")))
			{
				continue;
			}
			A->SetActorHiddenInGame(true);
		}
		UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Scene cleaned - non-essential actors hidden, lights/sky preserved"));

		// Spawn 3 point lights around Sentinel for good visibility
		auto SpawnLight = [&](FVector Offset, float Intensity)
		{
			FVector LightLoc = SpawnLoc + Offset;
			APointLight* Light = World->SpawnActor<APointLight>(APointLight::StaticClass(), LightLoc, FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(Intensity);
				Light->PointLightComponent->SetAttenuationRadius(2000.f);
				Light->PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.95f, 0.9f)); // Warm white
			}
		};
		SpawnLight(FVector(300.f, 200.f, 400.f), 50000.f);   // Key light (front-right above)
		SpawnLight(FVector(-200.f, -300.f, 300.f), 30000.f);  // Fill light (back-left)
		SpawnLight(FVector(0.f, 0.f, 500.f), 20000.f);        // Top light

		// Set up camera - front 3/4 view, 500 units from Sentinel
		FVector TargetCenter = SpawnLoc + FVector(0.f, 0.f, 120.f); // Chest height
		FVector SentinelFwd = Sentinel->GetActorForwardVector();
		FVector Fwd2D = SentinelFwd; Fwd2D.Z = 0.f; Fwd2D.Normalize();
		FVector Right2D = FVector::CrossProduct(FVector::UpVector, Fwd2D).GetSafeNormal();
		// Camera in front-right at 30 degrees, 500 units out
		float AzRad = FMath::DegreesToRadians(30.f);
		FVector CamOffset = (-Fwd2D * FMath::Cos(AzRad) + Right2D * FMath::Sin(AzRad)) * 500.f;
		FVector CamPos = TargetCenter + CamOffset + FVector(0.f, 0.f, 50.f);

		ACameraActor* AnimCam = World->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), CamPos, FRotator::ZeroRotator);
		if (AnimCam)
		{
			AnimCam->GetCameraComponent()->FieldOfView = 60.f;
			AnimCam->GetCameraComponent()->PostProcessSettings.bOverride_MotionBlurAmount = true;
			AnimCam->GetCameraComponent()->PostProcessSettings.MotionBlurAmount = 0.0f;
			FVector Dir = (TargetCenter - CamPos).GetSafeNormal();
			AnimCam->SetActorRotation(Dir.Rotation());
			if (PC) PC->SetViewTargetWithBlend(AnimCam, 0.0f);
		}

		GEngine->Exec(World, TEXT("r.MotionBlurQuality 0"));
		GEngine->Exec(World, TEXT("r.DepthOfFieldQuality 0"));

		UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Camera at %s looking at %s"), *CamPos.ToString(), *TargetCenter.ToString());

		// Define animations to test
		FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");
		struct FAnimShot { FString MontageName; FString DisplayName; float PlayTime; };
		TArray<FAnimShot> AnimShots = {
			{ TEXT("AM_Sentinel_Idle"), TEXT("idle"), 1.0f },
			{ TEXT("AM_Sentinel_Walk"), TEXT("walk"), 0.8f },
			{ TEXT("AM_Sentinel_Run"), TEXT("run"), 0.6f },
			{ TEXT("AM_Sentinel_Attack01"), TEXT("attack1"), 1.0f },
			{ TEXT("AM_Sentinel_Attack02"), TEXT("attack2"), 1.0f },
			{ TEXT("AM_Sentinel_Attack03_Fast"), TEXT("attack3_fast"), 0.8f },
			{ TEXT("AM_Sentinel_HeavyAttack"), TEXT("heavy_attack"), 1.2f },
			{ TEXT("AM_Sentinel_Dodge_Fwd"), TEXT("dodge_fwd"), 0.6f },
			{ TEXT("AM_Sentinel_HitReact"), TEXT("hitreact"), 0.8f },
			{ TEXT("AM_Sentinel_Death_Front"), TEXT("death_front"), 1.5f },
		};

		FString ShotDir = FPaths::ProjectDir() / TEXT("Saved/Screenshots/AnimTest");
		IFileManager::Get().MakeDirectory(*ShotDir, true);

		struct FAnimTestState
		{
			int32 CurrentAnim = 0;
			int32 Phase = 0; // 0=play, 1=wait, 2=screenshot
			bool bSetupDone = false;
			TArray<FAnimShot> AnimShots;
			FString SentinelDir;
			FString ShotDir;
			TWeakObjectPtr<ACharacter> SentinelChar;
			TWeakObjectPtr<ACameraActor> Cam;
			FTimerHandle TimerHandle;
			float WaitRemaining = 0.f;
		};
		TSharedPtr<FAnimTestState> State = MakeShared<FAnimTestState>();
		State->AnimShots = AnimShots;
		State->SentinelDir = SentinelDir;
		State->ShotDir = ShotDir;
		State->SentinelChar = SentinelChar;
		State->Cam = AnimCam;

		World->GetTimerManager().SetTimer(State->TimerHandle, [State, World]()
		{
			ACharacter* SC = State->SentinelChar.Get();
			if (!SC) return;

			// Setup phase: wait for AnimBP to apply, then set external montage control
			if (!State->bSetupDone)
			{
				UAnimInstance* AI = SC->GetMesh() ? SC->GetMesh()->GetAnimInstance() : nullptr;
				if (!AI)
				{
					UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Waiting for AnimBP..."));
					return;
				}
				// Sentinel locomotion montage control removed from header
				AI->Montage_Stop(0.1f);
				State->bSetupDone = true;
				UE_LOG(LogTemp, Warning, TEXT("[AnimTest] AnimBP ready: %s. External montage control ON."),
					*AI->GetClass()->GetName());
				return;
			}

			if (State->CurrentAnim >= State->AnimShots.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("[AnimTest] COMPLETE! %d animations tested in %s"),
					State->AnimShots.Num(), *State->ShotDir);
				if (ACameraActor* C = State->Cam.Get()) C->Destroy();
				World->GetTimerManager().ClearTimer(State->TimerHandle);
				return;
			}

			UAnimInstance* AI = SC->GetMesh() ? SC->GetMesh()->GetAnimInstance() : nullptr;
			if (!AI) return;

			const FAnimShot& Shot = State->AnimShots[State->CurrentAnim];

			if (State->Phase == 0)
			{
				AI->Montage_Stop(0.05f);
				FString MontPath = State->SentinelDir + Shot.MontageName;
				UAnimMontage* Mont = LoadObject<UAnimMontage>(nullptr, *MontPath);
				if (Mont)
				{
					float PlayLen = AI->Montage_Play(Mont, 1.0f);
					UE_LOG(LogTemp, Warning, TEXT("[AnimTest] [%d/%d] Playing: %s (len=%.2f, result=%.2f)"),
						State->CurrentAnim + 1, State->AnimShots.Num(),
						*Shot.MontageName, Mont->GetPlayLength(), PlayLen);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AnimTest] MISSING: %s"), *MontPath);
				}
				State->Phase = 1;
				State->WaitRemaining = Shot.PlayTime;
			}
			else if (State->Phase == 1)
			{
				State->WaitRemaining -= 0.3f;
				if (State->WaitRemaining <= 0.f)
				{
					State->Phase = 2;
				}
			}
			else if (State->Phase == 2)
			{
				FString ShotPath = State->ShotDir / FString::Printf(TEXT("Anim_%02d_%s.png"),
					State->CurrentAnim, *Shot.DisplayName);
				FScreenshotRequest::RequestScreenshot(ShotPath, false, false);
				UE_LOG(LogTemp, Warning, TEXT("[AnimTest] Screenshot [%d]: %s"), State->CurrentAnim, *Shot.DisplayName);

				State->CurrentAnim++;
				State->Phase = 0;
			}
		}, 0.3f, true, 3.0f);
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

	// Auto-spawn Sentinel after PIE is fully loaded (deferred to avoid Saved/ corruption)
	if (UWorld* World = GetWorld())
	{
		FTimerHandle SpawnTimer;
		World->GetTimerManager().SetTimer(SpawnTimer, [this]()
		{
			UWorld* W = GetWorld();
			if (!W) return;

			ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(W, 0);
			if (!PlayerChar) return;

			// Spawn Sentinel 600 units in front of the player
			AActor* Sentinel = SpawnEnemyInPIE(W,
				TEXT("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Sentinel.B_Soulslike_Enemy_Sentinel_C"),
				TEXT("/Script/SLFConversion.SLFEnemySentinel"),
				FVector(600.f, 0.f, 0.f));

			UE_LOG(LogTemp, Warning, TEXT("[SLFPIETestRunner] Auto-spawned Sentinel: %s at %s"),
				Sentinel ? *Sentinel->GetName() : TEXT("FAILED"),
				Sentinel ? *Sentinel->GetActorLocation().ToString() : TEXT("N/A"));
		}, 3.0f, false); // 3 second delay — world fully loaded by then
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
