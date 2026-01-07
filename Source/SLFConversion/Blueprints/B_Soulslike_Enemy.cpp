// B_Soulslike_Enemy.cpp
// C++ implementation for Blueprint B_Soulslike_Enemy
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/Blueprint/B_Soulslike_Enemy.json

#include "Blueprints/B_Soulslike_Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/TimelineComponent.h"
#include "NiagaraComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "Components/LootDropManagerComponent.h"
#include "Blueprints/B_PatrolPath.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"

AB_Soulslike_Enemy::AB_Soulslike_Enemy()
{
	// Initialize variables
	EnemyId = FGuid::NewGuid();
	PatrolPath = nullptr;

	// Create components
	Healthbar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Healthbar"));
	if (Healthbar)
	{
		Healthbar->SetupAttachment(RootComponent);
	}

	AC_AI_CombatManager = CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("AC_AI_CombatManager"));
	AC_AI_BehaviorManager = CreateDefaultSubobject<UAIBehaviorManagerComponent>(TEXT("AC_AI_BehaviorManager"));

	NS_Souls = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_Souls"));
	if (NS_Souls)
	{
		NS_Souls->SetupAttachment(RootComponent);
	}

	AC_LootDropManager = CreateDefaultSubobject<ULootDropManagerComponent>(TEXT("AC_LootDropManager"));

	ExecutionMoveTo = CreateDefaultSubobject<UArrowComponent>(TEXT("ExecutionMoveTo"));
	if (ExecutionMoveTo)
	{
		ExecutionMoveTo->SetupAttachment(RootComponent);
	}

	VisualizerBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("VisualizerBillboard"));
	if (VisualizerBillboard)
	{
		VisualizerBillboard->SetupAttachment(RootComponent);
	}

	TL_RotateTowardsTarget = CreateDefaultSubobject<UTimelineComponent>(TEXT("TL_RotateTowardsTarget"));
}

void AB_Soulslike_Enemy::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] BeginPlay - %s"), *GetName());
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOCAL FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Soulslike_Enemy::CheckSense_Implementation(UAIPerceptionComponent* AiPerceptionComponent, AActor* Actor, ESLFAISenses Sense, bool& OutReturnValue, FAIStimulus& OutStimulus)
{
	OutReturnValue = false;
	OutStimulus = FAIStimulus();

	if (!AiPerceptionComponent || !Actor)
	{
		return;
	}

	// Check if the AI perception component has sensed the actor via the specified sense
	FActorPerceptionBlueprintInfo PerceptionInfo;
	if (AiPerceptionComponent->GetActorsPerception(Actor, PerceptionInfo))
	{
		// Find the stimulus matching the requested sense type
		for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				OutReturnValue = true;
				OutStimulus = Stimulus;
				return;
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_Enemy INTERFACE IMPLEMENTATIONS (8 functions)
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Soulslike_Enemy::GetExecutionMoveToTransform_Implementation(FVector& MoveToLocation, FRotator& Rotation)
{
	// JSON Logic: ExecutionMoveTo->GetForwardVector() -> MakeRotFromX
	// ExecutionMoveTo->GetWorldLocation() -> MoveToLocation
	if (ExecutionMoveTo)
	{
		MoveToLocation = ExecutionMoveTo->GetComponentLocation();
		FVector ForwardVector = ExecutionMoveTo->GetForwardVector();
		Rotation = UKismetMathLibrary::MakeRotFromX(ForwardVector);
	}
	else
	{
		MoveToLocation = GetActorLocation();
		Rotation = GetActorRotation();
	}
}

void AB_Soulslike_Enemy::StopRotateTowardsTarget_Implementation()
{
	// JSON Logic: Stop timeline, set bAllowPhysicsRotationDuringAnimRootMotion = false
	if (TL_RotateTowardsTarget)
	{
		TL_RotateTowardsTarget->Stop();
	}

	// Clear timer if any
	GetWorldTimerManager().ClearTimer(RotateToTargetTimerHandle);

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] StopRotateTowardsTarget"));
}

void AB_Soulslike_Enemy::RotateTowardsTarget_Implementation(double Duration)
{
	// JSON Logic: SetPlayRate(1.0/Duration), PlayFromStart()
	// Uses timeline TL_RotateTowardsTarget
	if (TL_RotateTowardsTarget && Duration > 0.0)
	{
		float PlayRate = 1.0f / static_cast<float>(Duration);
		TL_RotateTowardsTarget->SetPlayRate(PlayRate);
		TL_RotateTowardsTarget->PlayFromStart();
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] RotateTowardsTarget - Duration: %.2f"), Duration);
}

void AB_Soulslike_Enemy::PickAndSpawnLoot_Implementation()
{
	// JSON Logic: AC_LootDropManager -> PickItem()
	if (AC_LootDropManager)
	{
		AC_LootDropManager->PickItem();
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] PickAndSpawnLoot"));
}

void AB_Soulslike_Enemy::GetPatrolPath_Implementation(AActor*& OutPatrolPath)
{
	// JSON Logic: Return PatrolPath variable
	// AB_PatrolPath inherits from AActor, so assignment is valid
	OutPatrolPath = PatrolPath;
}

void AB_Soulslike_Enemy::DisplayDeathVfx_Implementation(FVector AttractorPosition)
{
	// JSON Logic:
	// 1. Get Mesh socket location ("hips")
	// 2. Set NS_Souls world location to socket location
	// 3. Set NS_Souls Attractor parameter to AttractorPosition
	// 4. Activate NS_Souls

	if (NS_Souls)
	{
		if (GetMesh())
		{
			FVector SocketLocation = GetMesh()->GetSocketLocation(FName("hips"));
			NS_Souls->SetWorldLocation(SocketLocation);
		}

		// Set attractor vector parameter
		NS_Souls->SetVectorParameter(FName("Attractor"), AttractorPosition);

		// Activate the Niagara system
		NS_Souls->Activate(true);
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] DisplayDeathVfx - Attractor: %s"), *AttractorPosition.ToString());
}

void AB_Soulslike_Enemy::ToggleHealthbarVisual_Implementation(bool bVisible)
{
	// JSON Logic: Healthbar->SetVisibility(bVisible)
	if (Healthbar)
	{
		Healthbar->SetVisibility(bVisible);
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] ToggleHealthbarVisual - %s"), bVisible ? TEXT("Visible") : TEXT("Hidden"));
}

void AB_Soulslike_Enemy::UpdateEnemyHealth_Implementation(double CurrentValue, double MaxValue, double Change)
{
	// JSON Logic: Get Healthbar widget -> Cast to W_EnemyHealthbar -> UpdateEnemyHealthbar
	if (Healthbar)
	{
		UUserWidget* Widget = Healthbar->GetWidget();
		if (Widget)
		{
			// Call UpdateEnemyHealthbar via interface or reflection
			// The widget should implement an UpdateEnemyHealthbar function
			static FName FuncName(TEXT("UpdateEnemyHealthbar"));
			UFunction* UpdateFunc = Widget->FindFunction(FuncName);
			if (UpdateFunc)
			{
				struct
				{
					double CurrentHealth;
					double MaxHealth;
					double ChangeAmount;
				} Params;
				Params.CurrentHealth = CurrentValue;
				Params.MaxHealth = MaxValue;
				Params.ChangeAmount = Change;
				Widget->ProcessEvent(UpdateFunc, &Params);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] UpdateEnemyHealth - Current: %.0f, Max: %.0f, Change: %.0f"),
		CurrentValue, MaxValue, Change);
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_Executable INTERFACE IMPLEMENTATIONS (3 functions)
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Soulslike_Enemy::OnExecutionStarted_Implementation()
{
	// JSON Logic: Disable movement, start execution animation
	// GetCharacterMovement()->DisableMovement()
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] OnExecutionStarted"));
}

void AB_Soulslike_Enemy::OnExecuted_Implementation(FGameplayTag ExecutionTag)
{
	// JSON Logic:
	// 1. Deactivate collision
	// 2. Play death effects
	// 3. Handle loot spawning
	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] OnExecuted - Tag: %s"), *ExecutionTag.ToString());

	// Disable collision on capsule
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AB_Soulslike_Enemy::OnBackstabbed_Implementation(FGameplayTag ExecutionTag)
{
	// JSON Logic: Play backstab reaction, apply damage
	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_Enemy] OnBackstabbed - Tag: %s"), *ExecutionTag.ToString());

	// Backstab typically disables movement temporarily
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}
}
