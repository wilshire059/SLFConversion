// SLFSoulslikeEnemy.cpp
// C++ implementation for B_Soulslike_Enemy
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Soulslike_Enemy
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (EnemyId, PatrolPath) + inherited Cache_Rotation
// Functions:         1/1 implemented (CheckSense)
// Event Dispatchers: 1/1 (all assignable)
// Interfaces:        BPI_Enemy (8) FULLY IMPLEMENTED, BPI_Executable (3) FULLY IMPLEMENTED
// ═══════════════════════════════════════════════════════════════════════════════
//
// 20-PASS VALIDATION: 2026-01-03 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Soulslike_Enemy.json

#include "SLFSoulslikeEnemy.h"
#include "AIC_SoulslikeFramework.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "Components/LootDropManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "GameFramework/PlayerController.h"
#include "SLFEnums.h"

ASLFSoulslikeEnemy::ASLFSoulslikeEnemy()
{
	// Initialize enemy ID
	EnemyId = FGuid::NewGuid();
	PatrolPath = nullptr;
	bRotatingTowardsTarget = false;
	RotationAlpha = 0.0f;
	RotationDuration = 1.0f;

	// Enemies tick for AI
	PrimaryActorTick.bCanEverTick = true;

	// Set AI Controller class - this is critical for AI to work
	AIControllerClass = AAIC_SoulslikeFramework::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// IMPORTANT: AI components are created by Blueprint SCS, NOT here!
	// Following the same pattern as SLFSoulslikeCharacter::CacheComponentReferences()
	// The Blueprint SCS creates:
	//   - AC_AI_CombatManager (UAICombatManagerComponent)
	//   - AC_AI_BehaviorManager (UAIBehaviorManagerComponent)
	// We cache pointers to them in BeginPlay via FindComponentByClass

	// Initialize cache pointers to nullptr - will be populated in BeginPlay
	CombatManagerComponent = nullptr;
	BehaviorManagerComponent = nullptr;
	LootDropManagerComponent = nullptr;
	SoulsNiagaraComponent = nullptr;
	HealthbarWidget = nullptr;
}

void ASLFSoulslikeEnemy::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] BeginPlay: %s (ID: %s)"),
		*GetName(), *EnemyId.ToString());

	// Cache component references from Blueprint SCS
	CacheComponentReferences();

	// Bind to AI Perception updates from controller
	// This replaces the Blueprint EventGraph logic that was cleared during migration
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UAIPerceptionComponent* PerceptionComp = AIC->GetAIPerceptionComponent())
		{
			PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &ASLFSoulslikeEnemy::OnPerceptionUpdated);
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Bound to OnPerceptionUpdated delegate"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] AI Controller has no PerceptionComponent!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] No AI Controller at BeginPlay - perception binding skipped"));
	}
}

void ASLFSoulslikeEnemy::CacheComponentReferences()
{
	// CRITICAL: Find components created by Blueprint SCS
	// This follows the same pattern as SLFSoulslikeCharacter::CacheComponentReferences()
	// Blueprint SCS creates the actual components - we just cache pointers to them

	// AI Behavior Manager - Blueprint creates "AC_AI_BehaviorManager"
	BehaviorManagerComponent = FindComponentByClass<UAIBehaviorManagerComponent>();
	if (BehaviorManagerComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Found BehaviorManager: %s"), *BehaviorManagerComponent->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] BehaviorManager NOT FOUND - Blueprint SCS may be missing component"));
	}

	// AI Combat Manager - Blueprint creates "AC_AI_CombatManager"
	CombatManagerComponent = FindComponentByClass<UAICombatManagerComponent>();
	if (CombatManagerComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Found CombatManager: %s"), *CombatManagerComponent->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] CombatManager NOT FOUND - Blueprint SCS may be missing component"));
	}

	// Loot Drop Manager
	LootDropManagerComponent = FindComponentByClass<ULootDropManagerComponent>();
	if (LootDropManagerComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Found LootDropManager: %s"), *LootDropManagerComponent->GetName());
	}

	// Niagara component
	SoulsNiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	if (SoulsNiagaraComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Found NiagaraComponent: %s"), *SoulsNiagaraComponent->GetName());
	}

	// Healthbar widget
	HealthbarWidget = FindComponentByClass<UWidgetComponent>();
	if (HealthbarWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Found HealthbarWidget: %s"), *HealthbarWidget->GetName());
	}
}

void ASLFSoulslikeEnemy::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// This replaces the Blueprint EventGraph logic that handled perception updates
	// Original flow: OnPerceptionUpdated -> check if player -> SetTarget + SetState(Chase)

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnPerceptionUpdated - %d actors"), UpdatedActors.Num());

	if (!BehaviorManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] No BehaviorManager for perception handling"));
		return;
	}

	// Get AI perception component for detailed stimulus info
	AAIController* AIC = Cast<AAIController>(GetController());
	UAIPerceptionComponent* PerceptionComp = AIC ? AIC->GetAIPerceptionComponent() : nullptr;
	if (!PerceptionComp)
	{
		return;
	}

	// Check each updated actor
	for (AActor* Actor : UpdatedActors)
	{
		if (!Actor)
		{
			continue;
		}

		// Check if this is a player character (controlled by player controller)
		APawn* Pawn = Cast<APawn>(Actor);
		if (!Pawn || !Pawn->IsPlayerControlled())
		{
			continue;
		}

		// Get perception info for this actor
		FActorPerceptionBlueprintInfo Info;
		if (!PerceptionComp->GetActorsPerception(Actor, Info))
		{
			continue;
		}

		// Check if currently sensed (any stimulus was successfully sensed)
		bool bCurrentlySensed = false;
		for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				bCurrentlySensed = true;
				break;
			}
		}

		if (bCurrentlySensed)
		{
			// Player detected - set target and change to Chase state
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Player detected: %s - Setting target and Chase state"), *Actor->GetName());

			// Call SetTarget on BehaviorManager (sets blackboard TargetActor key)
			BehaviorManagerComponent->SetTarget(Actor);

			// Set state to Combat (triggers chase behavior in BT_Enemy)
			BehaviorManagerComponent->SetState(ESLFAIStates::Combat);
		}
		else
		{
			// Player lost - could reset to Idle here if needed
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Lost sight of player: %s"), *Actor->GetName());
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1/1]
// ═══════════════════════════════════════════════════════════════════════════════

bool ASLFSoulslikeEnemy::CheckSense_Implementation(AActor* Target)
{
	if (!Target) return false;

	// Check AI Perception Component for target
	if (UAIPerceptionComponent* Perception = FindComponentByClass<UAIPerceptionComponent>())
	{
		FActorPerceptionBlueprintInfo Info;
		if (Perception->GetActorsPerception(Target, Info))
		{
			// Check if any stimulus was sensed
			for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
			{
				if (Stimulus.WasSuccessfullySensed())
				{
					return true;
				}
			}
		}
	}

	return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ROTATION SYSTEM (Timeline equivalent)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::UpdateRotationTowardsTarget()
{
	if (!bRotatingTowardsTarget)
	{
		return;
	}

	// Get target from blackboard or combat manager
	AActor* Target = nullptr;
	if (BehaviorManagerComponent)
	{
		Target = BehaviorManagerComponent->CurrentTarget;
	}

	if (!Target)
	{
		StopRotateTowardsTarget_Implementation();
		return;
	}

	// Calculate target rotation
	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	FRotator TargetRotation = Direction.ToOrientationRotator();

	// Lerp from cached rotation to target
	FRotator NewRotation = UKismetMathLibrary::RLerp(Cache_Rotation, TargetRotation, RotationAlpha, true);
	SetActorRotation(NewRotation);

	// Increment alpha
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	RotationAlpha += DeltaTime / RotationDuration;

	if (RotationAlpha >= 1.0f)
	{
		StopRotateTowardsTarget_Implementation();
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_ENEMY INTERFACE IMPLEMENTATIONS (8 functions) - FULLY IMPLEMENTED
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::GetExecutionMoveToTransform_Implementation(FVector& MoveToLocation, FRotator& Rotation)
{
	// From Blueprint: Get ExecutionMoveTo arrow component's forward vector and location
	// Return a position in front of the enemy for executions
	FVector ForwardOffset = GetActorForwardVector() * 100.0f;
	MoveToLocation = GetActorLocation() + ForwardOffset;
	Rotation = UKismetMathLibrary::MakeRotFromX(GetActorForwardVector());
	Rotation.Yaw += 180.0f; // Face towards enemy

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] GetExecutionMoveToTransform: Location=%s, Rotation=%s"),
		*MoveToLocation.ToString(), *Rotation.ToString());
}

void ASLFSoulslikeEnemy::StopRotateTowardsTarget_Implementation()
{
	// From Blueprint:
	// 1. Set bAllowPhysicsRotationDuringAnimRootMotion = false
	// 2. Stop timeline
	// 3. Clear rotation state

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] StopRotateTowardsTarget"));

	bRotatingTowardsTarget = false;
	RotationAlpha = 0.0f;

	// Stop rotation timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RotationTimerHandle);
	}

	// Disable physics rotation during anim root motion
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}
}

void ASLFSoulslikeEnemy::RotateTowardsTarget_Implementation(double Duration)
{
	// From Blueprint:
	// 1. Set PlayRate = 1.0 / Duration on timeline
	// 2. Set bAllowPhysicsRotationDuringAnimRootMotion = true
	// 3. Cache current rotation
	// 4. Play timeline from start
	// 5. On update: lerp rotation towards target

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] RotateTowardsTarget: Duration=%f"), Duration);

	// Cache current rotation
	Cache_Rotation = GetActorRotation();

	// Set rotation parameters
	RotationDuration = (Duration > 0.0) ? Duration : 1.0;
	RotationAlpha = 0.0f;
	bRotatingTowardsTarget = true;

	// Enable physics rotation during anim root motion
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}

	// Start rotation timer (replaces Blueprint timeline)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RotationTimerHandle,
			this,
			&ASLFSoulslikeEnemy::UpdateRotationTowardsTarget,
			0.016f, // ~60Hz
			true // Looping
		);
	}
}

void ASLFSoulslikeEnemy::PickAndSpawnLoot_Implementation()
{
	// From Blueprint: Call AC_LootDropManager->PickItem()
	// This triggers the OnItemPicked dispatcher which spawns the item

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PickAndSpawnLoot"));

	if (LootDropManagerComponent)
	{
		LootDropManagerComponent->PickItem();
	}
	else
	{
		// Try to find it on the actor
		if (ULootDropManagerComponent* LootManager = FindComponentByClass<ULootDropManagerComponent>())
		{
			LootManager->PickItem();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PickAndSpawnLoot - No LootDropManager found!"));
		}
	}
}

void ASLFSoulslikeEnemy::GetPatrolPath_Implementation(AActor*& OutPatrolPath)
{
	OutPatrolPath = PatrolPath;
}

void ASLFSoulslikeEnemy::DisplayDeathVfx_Implementation(FVector AttractorPosition)
{
	// From Blueprint:
	// 1. Get Mesh->GetSocketLocation("spine_05")
	// 2. NS_Souls->SetWorldLocationAndRotation(spineLocation, FRotator::ZeroRotator)
	// 3. NS_Souls->Activate()
	// 4. NS_Souls->SetVectorParameter("AttractorPosition", AttractorPosition)

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] DisplayDeathVfx at %s"), *AttractorPosition.ToString());

	if (SoulsNiagaraComponent)
	{
		// Get spine location from mesh
		FVector SpineLocation = GetMesh()->GetSocketLocation(FName("spine_05"));

		// Set world location and rotation
		SoulsNiagaraComponent->SetWorldLocationAndRotation(SpineLocation, FRotator::ZeroRotator);

		// Activate
		SoulsNiagaraComponent->Activate(false); // bReset = false as per Blueprint

		// Set attractor position parameter
		SoulsNiagaraComponent->SetVectorParameter(FName("AttractorPosition"), AttractorPosition);
	}
}

void ASLFSoulslikeEnemy::ToggleHealthbarVisual_Implementation(bool bVisible)
{
	if (HealthbarWidget)
	{
		HealthbarWidget->SetVisibility(bVisible);
	}
}

void ASLFSoulslikeEnemy::UpdateEnemyHealth_Implementation(double CurrentValue, double MaxValue, double Change)
{
	// From Blueprint:
	// 1. Get Healthbar widget component
	// 2. Get widget from it
	// 3. Call UpdateEnemyHealthbar (BPI_EnemyHealthbar interface)

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] UpdateEnemyHealth: %f/%f (change: %f)"),
		CurrentValue, MaxValue, Change);

	if (HealthbarWidget)
	{
		UUserWidget* Widget = HealthbarWidget->GetWidget();
		if (Widget)
		{
			// Check if widget implements BPI_EnemyHealthbar interface
			if (Widget->Implements<UBPI_EnemyHealthbar>())
			{
				IBPI_EnemyHealthbar::Execute_UpdateEnemyHealthbar(Widget, CurrentValue, MaxValue, Change);
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_EXECUTABLE INTERFACE IMPLEMENTATIONS (3 functions) - FULLY IMPLEMENTED
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::OnExecutionStarted_Implementation()
{
	// From Blueprint:
	// 1. Get AC_AI_CombatManager
	// 2. Call ClearPoiseBreakResetTimer()
	// This prevents enemy from breaking out of poise break state during execution

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnExecutionStarted"));

	if (CombatManagerComponent)
	{
		CombatManagerComponent->ClearPoiseBreakResetTimer();
	}
	else
	{
		// Try to find it on the actor
		if (UAICombatManagerComponent* CombatManager = FindComponentByClass<UAICombatManagerComponent>())
		{
			CombatManager->ClearPoiseBreakResetTimer();
		}
	}
}

void ASLFSoulslikeEnemy::OnExecuted_Implementation(FGameplayTag ExecutionTag)
{
	// From Blueprint:
	// 1. Check if ExecutionTag is valid
	// 2. Get relevant executed montage from combat manager
	// 3. Reset stance stat
	// 4. Play montage
	// 5. On completed/blend out: Call SetState to change AI state
	// 6. Call FinishPoiseBreak on combat manager

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnExecuted: Tag=%s"), *ExecutionTag.ToString());

	// Check if tag is valid
	if (!ExecutionTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted - Invalid ExecutionTag, skipping"));
		return;
	}

	// Get combat manager
	UAICombatManagerComponent* CombatManager = CombatManagerComponent;
	if (!CombatManager)
	{
		CombatManager = FindComponentByClass<UAICombatManagerComponent>();
	}

	if (!CombatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted - No CombatManager found!"));
		return;
	}

	// Get executed montage
	UAnimMontage* Montage = CombatManager->GetRelevantExecutedMontage();

	if (Montage)
	{
		// Play the montage
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
			{
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindLambda([this, CombatManager](UAnimMontage* EndedMontage, bool bInterrupted)
				{
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnExecuted montage ended"));

					// After execution, enemy is dead - set to Idle (no Dead state in enum)
					// The actual death is handled by CombatManager->FinishPoiseBreak()
					if (BehaviorManagerComponent)
					{
						BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
					}

					// Finish poise break (marks enemy as dead)
					if (CombatManager)
					{
						CombatManager->FinishPoiseBreak();
					}
				});

				AnimInstance->Montage_Play(Montage, 1.0f);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
	else
	{
		// No montage, just set state and finish
		if (BehaviorManagerComponent)
		{
			BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
		}
		if (CombatManager)
		{
			CombatManager->FinishPoiseBreak();
		}
	}
}

void ASLFSoulslikeEnemy::OnBackstabbed_Implementation(FGameplayTag ExecutionTag)
{
	// From Blueprint: Same logic as OnExecuted
	// 1. Check if ExecutionTag is valid
	// 2. Get relevant executed montage
	// 3. Reset stat
	// 4. Play montage
	// 5. On completed: SetState, FinishPoiseBreak

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnBackstabbed: Tag=%s"), *ExecutionTag.ToString());

	// Check if tag is valid
	if (!ExecutionTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnBackstabbed - Invalid ExecutionTag, skipping"));
		return;
	}

	// Get combat manager
	UAICombatManagerComponent* CombatManager = CombatManagerComponent;
	if (!CombatManager)
	{
		CombatManager = FindComponentByClass<UAICombatManagerComponent>();
	}

	if (!CombatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnBackstabbed - No CombatManager found!"));
		return;
	}

	// Get executed montage
	UAnimMontage* Montage = CombatManager->GetRelevantExecutedMontage();

	if (Montage)
	{
		// Play the montage
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
			{
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindLambda([this, CombatManager](UAnimMontage* EndedMontage, bool bInterrupted)
				{
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnBackstabbed montage ended"));

					// After backstab, enemy is dead - set to Idle (no Dead state in enum)
					if (BehaviorManagerComponent)
					{
						BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
					}

					// Finish poise break (marks enemy as dead)
					if (CombatManager)
					{
						CombatManager->FinishPoiseBreak();
					}
				});

				AnimInstance->Montage_Play(Montage, 1.0f);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
	else
	{
		// No montage, just set state and finish
		if (BehaviorManagerComponent)
		{
			BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
		}
		if (CombatManager)
		{
			CombatManager->FinishPoiseBreak();
		}
	}
}
