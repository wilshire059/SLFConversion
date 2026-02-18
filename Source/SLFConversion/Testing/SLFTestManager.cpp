// SLFTestManager.cpp
// Automated PIE test manager implementation

#include "SLFTestManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformFileManager.h"
#include "TimerManager.h"
#include "UnrealClient.h"

DEFINE_LOG_CATEGORY(LogSLFTest);

ASLFTestManager::ASLFTestManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SpawnedEnemy = nullptr;
	ScreenshotIndex = 0;
}

void ASLFTestManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogSLFTest, Log, TEXT("SLFTestManager ready"));
}

void ASLFTestManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CleanupTest();
	Super::EndPlay(EndPlayReason);
}

void ASLFTestManager::RunTest(const FString& TestName)
{
	CurrentTestName = TestName;
	ScreenshotIndex = 0;
	TestLog.Empty();
	ReceivedNotifies.Empty();

	AddLogEntry(FString::Printf(TEXT("=== TEST START: %s ==="), *TestName));
	UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Running test: %s"), *TestName);

	// Capture initial screenshot
	CaptureScreenshot(TEXT("Initial"));
}

AActor* ASLFTestManager::SpawnTestEnemy(TSubclassOf<AActor> EnemyClass, FVector Offset)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSLFTest, Error, TEXT("[SLFTestManager] No world available"));
		return nullptr;
	}

	// Clean up existing enemy
	if (SpawnedEnemy && !SpawnedEnemy->IsPendingKillPending())
	{
		SpawnedEnemy->Destroy();
		SpawnedEnemy = nullptr;
	}

	if (!EnemyClass)
	{
		EnemyClass = TestEnemyClass;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogSLFTest, Error, TEXT("[SLFTestManager] No enemy class specified"));
		return nullptr;
	}

	// Calculate spawn location relative to player
	FVector SpawnLocation = FVector::ZeroVector;
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (PlayerChar)
	{
		FVector PlayerForward = PlayerChar->GetActorForwardVector();
		SpawnLocation = PlayerChar->GetActorLocation() + PlayerForward * Offset.X + PlayerChar->GetActorRightVector() * Offset.Y;
		SpawnLocation.Z = PlayerChar->GetActorLocation().Z + Offset.Z;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedEnemy = World->SpawnActor<AActor>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedEnemy)
	{
		// Face enemy toward player
		if (PlayerChar)
		{
			FVector DirectionToPlayer = (PlayerChar->GetActorLocation() - SpawnedEnemy->GetActorLocation()).GetSafeNormal();
			SpawnedEnemy->SetActorRotation(DirectionToPlayer.Rotation());
		}

		AddLogEntry(FString::Printf(TEXT("Spawned enemy: %s at %s"),
			*SpawnedEnemy->GetName(), *SpawnLocation.ToString()));

		UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Spawned %s at %s"),
			*SpawnedEnemy->GetName(), *SpawnLocation.ToString());

		// Auto-bind AnimNotifies
		BindToEnemyAnimNotifies(SpawnedEnemy);
	}
	else
	{
		UE_LOG(LogSLFTest, Error, TEXT("[SLFTestManager] Failed to spawn enemy of class %s"),
			*EnemyClass->GetName());
	}

	return SpawnedEnemy;
}

void ASLFTestManager::CaptureScreenshot(const FString& Label)
{
	FString ScreenshotPath = GetScreenshotPath(Label);

	// Use FScreenshotRequest for gameplay screenshots (includes HUD)
	FScreenshotRequest::RequestScreenshot(ScreenshotPath, false, false);

	ScreenshotIndex++;

	AddLogEntry(FString::Printf(TEXT("Screenshot [%d]: %s -> %s"), ScreenshotIndex, *Label, *ScreenshotPath));
	UE_LOG(LogSLFTest, Log, TEXT("[SLFTestManager] Screenshot captured: %s"), *ScreenshotPath);
}

FString ASLFTestManager::ExportFilteredLog(const FString& OutputPath)
{
	FString FinalPath = OutputPath;
	if (FinalPath.IsEmpty())
	{
		FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
		FinalPath = FPaths::ProjectSavedDir() / TEXT("Automation") / FString::Printf(TEXT("TestLog_%s_%s.txt"),
			*CurrentTestName, *Timestamp);
	}

	// Build log content
	FString LogContent;
	LogContent += FString::Printf(TEXT("=== SLF Test Log ===\n"));
	LogContent += FString::Printf(TEXT("Test: %s\n"), *CurrentTestName);
	LogContent += FString::Printf(TEXT("Time: %s\n"), *FDateTime::Now().ToString());
	LogContent += FString::Printf(TEXT("Screenshots: %d\n"), ScreenshotIndex);
	LogContent += TEXT("\n--- Test Log ---\n");

	for (const FString& Entry : TestLog)
	{
		LogContent += Entry + TEXT("\n");
	}

	// Add AnimNotify summary
	LogContent += TEXT("\n--- AnimNotify Summary ---\n");
	LogContent += GetAnimNotifySummary();

	// Add enemy state
	if (SpawnedEnemy)
	{
		LogContent += TEXT("\n--- Enemy State ---\n");
		LogContent += ValidateEnemyState();
	}

	// Ensure directory exists
	FString Directory = FPaths::GetPath(FinalPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*Directory);

	// Write file
	if (FFileHelper::SaveStringToFile(LogContent, *FinalPath))
	{
		UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Log exported to: %s"), *FinalPath);
		return FinalPath;
	}
	else
	{
		UE_LOG(LogSLFTest, Error, TEXT("[SLFTestManager] Failed to export log to: %s"), *FinalPath);
		return TEXT("");
	}
}

FString ASLFTestManager::ValidateEnemyState(const FString& ExpectedState)
{
	FString Report;

	if (!SpawnedEnemy)
	{
		Report = TEXT("No enemy spawned");
		AddLogEntry(TEXT("VALIDATE: No enemy spawned"));
		return Report;
	}

	Report += FString::Printf(TEXT("Enemy: %s\n"), *SpawnedEnemy->GetName());
	Report += FString::Printf(TEXT("Class: %s\n"), *SpawnedEnemy->GetClass()->GetName());
	Report += FString::Printf(TEXT("Location: %s\n"), *SpawnedEnemy->GetActorLocation().ToString());
	Report += FString::Printf(TEXT("PendingKill: %s\n"), SpawnedEnemy->IsPendingKillPending() ? TEXT("YES") : TEXT("NO"));

	// Check SkeletalMesh
	ACharacter* EnemyChar = Cast<ACharacter>(SpawnedEnemy);
	if (EnemyChar)
	{
		USkeletalMeshComponent* MeshComp = EnemyChar->GetMesh();
		if (MeshComp)
		{
			Report += FString::Printf(TEXT("SkeletalMesh: %s\n"),
				MeshComp->GetSkeletalMeshAsset() ? *MeshComp->GetSkeletalMeshAsset()->GetName() : TEXT("NONE"));
			Report += FString::Printf(TEXT("MeshVisible: %s\n"),
				MeshComp->IsVisible() ? TEXT("YES") : TEXT("NO"));

			// AnimInstance
			UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
			if (AnimInst)
			{
				Report += FString::Printf(TEXT("AnimInstance: %s\n"), *AnimInst->GetClass()->GetName());
				UAnimMontage* ActiveMontage = AnimInst->GetCurrentActiveMontage();
				if (ActiveMontage)
				{
					Report += FString::Printf(TEXT("ActiveMontage: %s\n"), *ActiveMontage->GetName());
				}
			}
			else
			{
				Report += TEXT("AnimInstance: NONE\n");
			}

			// Socket check
			TArray<FName> SocketNames = MeshComp->GetAllSocketNames();
			Report += FString::Printf(TEXT("Sockets: %d\n"), SocketNames.Num());
		}
		else
		{
			Report += TEXT("SkeletalMesh: NO MESH COMPONENT\n");
		}
	}

	// Compare with expected state if provided
	if (!ExpectedState.IsEmpty())
	{
		bool bMatch = Report.Contains(ExpectedState);
		Report += FString::Printf(TEXT("\nExpected '%s': %s\n"),
			*ExpectedState, bMatch ? TEXT("FOUND") : TEXT("NOT FOUND"));
	}

	AddLogEntry(FString::Printf(TEXT("VALIDATE:\n%s"), *Report));
	UE_LOG(LogSLFTest, Log, TEXT("[SLFTestManager] Validation:\n%s"), *Report);

	return Report;
}

void ASLFTestManager::CleanupTest()
{
	if (SpawnedEnemy && !SpawnedEnemy->IsPendingKillPending())
	{
		SpawnedEnemy->Destroy();
		SpawnedEnemy = nullptr;
	}

	// Clear timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TestTimerHandle);
	}

	AddLogEntry(TEXT("=== TEST CLEANUP ==="));
	UE_LOG(LogSLFTest, Log, TEXT("[SLFTestManager] Test cleaned up"));
}

void ASLFTestManager::BindToEnemyAnimNotifies(AActor* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	ACharacter* EnemyChar = Cast<ACharacter>(Enemy);
	if (!EnemyChar)
	{
		UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Enemy is not a Character, cannot bind AnimNotifies"));
		return;
	}

	USkeletalMeshComponent* MeshComp = EnemyChar->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Enemy has no SkeletalMeshComponent"));
		return;
	}

	// Bind to PlayMontageNotify events on AnimInstance (not MeshComp)
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		UE_LOG(LogSLFTest, Warning, TEXT("[SLFTestManager] Enemy has no AnimInstance, cannot bind AnimNotifies"));
		return;
	}
	AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &ASLFTestManager::HandleMontageNotifyBegin);

	AddLogEntry(TEXT("Bound to enemy AnimNotify events"));
	UE_LOG(LogSLFTest, Log, TEXT("[SLFTestManager] Bound to AnimNotify events on %s"), *Enemy->GetName());
}

FString ASLFTestManager::GetAnimNotifySummary() const
{
	FString Summary;

	if (ReceivedNotifies.Num() == 0)
	{
		Summary = TEXT("No AnimNotifies received\n");
		return Summary;
	}

	Summary += FString::Printf(TEXT("AnimNotifies received (%d types):\n"), ReceivedNotifies.Num());
	for (const auto& Pair : ReceivedNotifies)
	{
		Summary += FString::Printf(TEXT("  %s: %d times\n"), *Pair.Key.ToString(), Pair.Value);
	}

	return Summary;
}

FString ASLFTestManager::GetScreenshotPath(const FString& Label) const
{
	FString SafeLabel = Label.Replace(TEXT(" "), TEXT("_"));
	FString SafeTestName = CurrentTestName.IsEmpty() ? TEXT("Test") : CurrentTestName.Replace(TEXT(" "), TEXT("_"));
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));

	return FPaths::ProjectDir() / ScreenshotFolder /
		FString::Printf(TEXT("%s_%03d_%s_%s.png"), *SafeTestName, ScreenshotIndex, *SafeLabel, *Timestamp);
}

void ASLFTestManager::AddLogEntry(const FString& Entry)
{
	FString TimestampedEntry = FString::Printf(TEXT("[%s] %s"),
		*FDateTime::Now().ToString(TEXT("%H:%M:%S")), *Entry);
	TestLog.Add(TimestampedEntry);
}

// Dynamic delegate handler for montage notifies
void ASLFTestManager::HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	int32& Count = ReceivedNotifies.FindOrAdd(NotifyName, 0);
	Count++;

	AddLogEntry(FString::Printf(TEXT("AnimNotify: %s (count: %d)"), *NotifyName.ToString(), Count));
	UE_LOG(LogSLFTest, Log, TEXT("[SLFTestManager] AnimNotify received: %s"), *NotifyName.ToString());
}
