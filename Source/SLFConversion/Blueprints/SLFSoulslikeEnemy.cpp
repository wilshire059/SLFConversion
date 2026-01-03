// SLFSoulslikeEnemy.cpp
// C++ implementation for B_Soulslike_Enemy
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Soulslike_Enemy
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (initialized in constructor)
// Functions:         1/1 implemented
// Event Dispatchers: 1/1 (all assignable)
// Interfaces:        BPI_Enemy (8), BPI_Executable (3)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFSoulslikeEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "NiagaraFunctionLibrary.h"

ASLFSoulslikeEnemy::ASLFSoulslikeEnemy()
{
	// Initialize enemy ID
	EnemyId = FGuid::NewGuid();
	PatrolPath = nullptr;

	// Enemies tick for AI
	PrimaryActorTick.bCanEverTick = true;

	// Create components - use different FNames than Blueprint SCS to avoid collision
	HealthbarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthbarWidget"));
	if (HealthbarWidget)
	{
		HealthbarWidget->SetupAttachment(GetCapsuleComponent());
		HealthbarWidget->SetVisibility(false); // Hidden by default
	}

	CombatManagerComponent = CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("CombatManagerComponent"));
	BehaviorManagerComponent = CreateDefaultSubobject<UAIBehaviorManagerComponent>(TEXT("BehaviorManagerComponent"));

	SoulsNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SoulsNiagaraComponent"));
	if (SoulsNiagaraComponent)
	{
		SoulsNiagaraComponent->SetupAttachment(RootComponent);
		SoulsNiagaraComponent->SetAutoActivate(false);
	}
}

void ASLFSoulslikeEnemy::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] BeginPlay: %s (ID: %s)"),
		*GetName(), *EnemyId.ToString());
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
// BPI_ENEMY INTERFACE IMPLEMENTATIONS (8 functions)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::GetExecutionMoveToTransform_Implementation(FVector& MoveToLocation, FRotator& Rotation)
{
	// Return a position in front of the enemy for executions
	FVector ForwardOffset = GetActorForwardVector() * 100.0f;
	MoveToLocation = GetActorLocation() + ForwardOffset;
	Rotation = GetActorRotation();
	Rotation.Yaw += 180.0f; // Face towards enemy

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] GetExecutionMoveToTransform"));
}

void ASLFSoulslikeEnemy::StopRotateTowardsTarget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] StopRotateTowardsTarget"));
	// TODO: Stop any active rotation towards target
}

void ASLFSoulslikeEnemy::RotateTowardsTarget_Implementation(double Duration)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] RotateTowardsTarget: Duration=%f"), Duration);
	// TODO: Start rotating towards current target over duration
}

void ASLFSoulslikeEnemy::PickAndSpawnLoot_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PickAndSpawnLoot"));
	// TODO: Query loot table and spawn items at death location
}

void ASLFSoulslikeEnemy::GetPatrolPath_Implementation(AActor*& OutPatrolPath)
{
	OutPatrolPath = PatrolPath;
}

void ASLFSoulslikeEnemy::DisplayDeathVfx_Implementation(FVector AttractorPosition)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] DisplayDeathVfx at %s"), *AttractorPosition.ToString());

	// Activate souls VFX if available
	if (SoulsNiagaraComponent)
	{
		SoulsNiagaraComponent->Activate(true);
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
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] UpdateEnemyHealth: %f/%f (change: %f)"),
		CurrentValue, MaxValue, Change);

	// TODO: Update healthbar widget with new values via interface
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_EXECUTABLE INTERFACE IMPLEMENTATIONS (3 functions)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::OnExecutionStarted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnExecutionStarted"));
	// TODO: Disable AI, play stagger animation
}

void ASLFSoulslikeEnemy::OnExecuted_Implementation(FGameplayTag ExecutionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnExecuted: Tag=%s"), *ExecutionTag.ToString());
	// TODO: Play death animation, spawn loot, destroy actor
}

void ASLFSoulslikeEnemy::OnBackstabbed_Implementation(FGameplayTag ExecutionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] OnBackstabbed: Tag=%s"), *ExecutionTag.ToString());
	// TODO: Play backstab reaction animation
}
