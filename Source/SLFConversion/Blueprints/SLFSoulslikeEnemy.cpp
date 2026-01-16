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
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Interfaces/BPI_AIC.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "Components/LootDropManagerComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "SLFPrimaryDataAssets.h"
#include "KismetAnimationLibrary.h"
#include "Widgets/W_EnemyHealthbar.h"
#include "Interfaces/SLFExecutionIndicatorInterface.h"
#include "Blueprint/UserWidget.h"

ASLFSoulslikeEnemy::ASLFSoulslikeEnemy()
{
	// Initialize enemy ID
	EnemyId = FGuid::NewGuid();
	PatrolPath = nullptr;
	bRotatingTowardsTarget = false;
	RotationAlpha = 0.0f;
	RotationDuration = 1.0f;
	LastCombatEntryTime = 0.0f;

	// Enemies tick for AI
	PrimaryActorTick.bCanEverTick = true;

	// Set AI Controller class - this is critical for AI to work
	AIControllerClass = AAIC_SoulslikeFramework::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Component pointers - some may come from Blueprint SCS, some created here
	CombatManagerComponent = nullptr;
	BehaviorManagerComponent = nullptr;
	LootDropManagerComponent = nullptr;
	SoulsNiagaraComponent = nullptr;

	// Create Healthbar widget component - shows enemy HP bar above head
	// This ensures healthbar exists even if Blueprint SCS lost it during migration
	HealthbarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Healthbar"));
	if (HealthbarWidget)
	{
		HealthbarWidget->SetupAttachment(RootComponent);
		HealthbarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f)); // Above head
		HealthbarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		HealthbarWidget->SetDrawSize(FVector2D(200.0f, 30.0f));
		HealthbarWidget->SetVisibility(false); // Start hidden, show on damage

		// Set widget class to W_EnemyHealthbar
		static ConstructorHelpers::FClassFinder<UUserWidget> EnemyHealthbarClassFinder(
			TEXT("/Game/SoulslikeFramework/Widgets/World/W_EnemyHealthbar"));
		if (EnemyHealthbarClassFinder.Succeeded())
		{
			HealthbarWidget->SetWidgetClass(EnemyHealthbarClassFinder.Class);
		}
	}

	// Create AI State Machine component - this replaces Behavior Tree for combat AI
	AIStateMachine = CreateDefaultSubobject<USLFAIStateMachineComponent>(TEXT("AIStateMachine"));
}

void ASLFSoulslikeEnemy::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] BeginPlay: %s (ID: %s)"),
		*GetName(), *EnemyId.ToString());

	// Find components that were created by Blueprint SCS
	// These are NOT created in C++ constructor - they come from the Blueprint
	BehaviorManagerComponent = FindComponentByClass<UAIBehaviorManagerComponent>();
	CombatManagerComponent = FindComponentByClass<UAICombatManagerComponent>();
	LootDropManagerComponent = FindComponentByClass<ULootDropManagerComponent>();
	SoulsNiagaraComponent = FindComponentByClass<UNiagaraComponent>();

	// Find Healthbar by name - can't use FindComponentByClass because ExecutionWidget (from base class) is also a WidgetComponent
	TArray<UWidgetComponent*> WidgetComps;
	GetComponents<UWidgetComponent>(WidgetComps);
	for (UWidgetComponent* Comp : WidgetComps)
	{
		if (Comp->GetName().Contains(TEXT("Healthbar")))
		{
			HealthbarWidget = Comp;
			break;
		}
	}

	// Log what we found
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Components found:"));
	UE_LOG(LogTemp, Log, TEXT("  BehaviorManager: %s"), BehaviorManagerComponent ? *BehaviorManagerComponent->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  CombatManager: %s"), CombatManagerComponent ? *CombatManagerComponent->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  LootDropManager: %s"), LootDropManagerComponent ? *LootDropManagerComponent->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  Healthbar: %s"), HealthbarWidget ? *HealthbarWidget->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  SoulsNiagara: %s"), SoulsNiagaraComponent ? *SoulsNiagaraComponent->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  AIStateMachine: %s"), AIStateMachine ? *AIStateMachine->GetName() : TEXT("NULL"));

	// ═══════════════════════════════════════════════════════════════════════════════
	// BIND TO OnPoiseBroken EVENT (bp_only B_Soulslike_Enemy.json lines 10598-10604)
	// ═══════════════════════════════════════════════════════════════════════════════
	// When poise breaks, we show/hide the ExecutionWidget and call ToggleExecutionIcon
	if (CombatManagerComponent)
	{
		CombatManagerComponent->OnPoiseBroken.AddDynamic(this, &ASLFSoulslikeEnemy::HandleOnPoiseBroken);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] *** BOUND to OnPoiseBroken event on CombatManager ***"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeEnemy] CombatManagerComponent is NULL - cannot bind OnPoiseBroken!"));
	}

	// Setup healthbar widget if WidgetClass is not set (lost during reparenting)
	if (HealthbarWidget && !HealthbarWidget->GetWidgetClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] HealthbarWidget has no WidgetClass - loading W_EnemyHealthbar"));
		static TSubclassOf<UUserWidget> EnemyHealthbarClass = nullptr;
		if (!EnemyHealthbarClass)
		{
			EnemyHealthbarClass = LoadClass<UUserWidget>(nullptr,
				TEXT("/Game/SoulslikeFramework/Widgets/World/W_EnemyHealthbar.W_EnemyHealthbar_C"));
		}
		if (EnemyHealthbarClass)
		{
			HealthbarWidget->SetWidgetClass(EnemyHealthbarClass);
			HealthbarWidget->SetDrawSize(FVector2D(200.0f, 30.0f));
			HealthbarWidget->SetWidgetSpace(EWidgetSpace::Screen);
			HealthbarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f)); // Position above head
			HealthbarWidget->SetVisibility(false); // Start hidden, show on damage
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Set HealthbarWidget class to: %s"), *EnemyHealthbarClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SoulslikeEnemy] Failed to load W_EnemyHealthbar class!"));
		}
	}
	else if (HealthbarWidget)
	{
		// Widget class already set, but still need to fix position above head
		HealthbarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Fixed HealthbarWidget position above head"));
	}

	// Enable debug logging on state machine for testing
	if (AIStateMachine)
	{
		AIStateMachine->bDebugEnabled = true;
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] AI State Machine ENABLED - Combat AI will use C++ state machine instead of Behavior Tree!"));
	}

	if (BehaviorManagerComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("  BehaviorManager Behavior Tree: %s"),
			BehaviorManagerComponent->Behavior ? *BehaviorManagerComponent->Behavior->GetName() : TEXT("NULL"));

		// If no behavior tree is assigned, try to load the default one
		if (!BehaviorManagerComponent->Behavior)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] No BT assigned, loading default BT_Enemy"));
			static UBehaviorTree* DefaultBT = nullptr;
			if (!DefaultBT)
			{
				DefaultBT = LoadObject<UBehaviorTree>(nullptr,
					TEXT("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy.BT_Enemy"));
			}
			if (DefaultBT)
			{
				BehaviorManagerComponent->Behavior = DefaultBT;
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Assigned default BT: %s"), *DefaultBT->GetName());

				// Component BeginPlay already ran, so we need to manually initialize the behavior tree
				// This calls the AI Controller's InitializeBehavior via BPI_AIC interface
				if (AController* MyController = GetController())
				{
					if (AAIController* AIC = Cast<AAIController>(MyController))
					{
						if (AIC->GetClass()->ImplementsInterface(UBPI_AIC::StaticClass()))
						{
							UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Manually initializing behavior tree"));
							IBPI_AIC::Execute_InitializeBehavior(AIC, DefaultBT);
						}
						else
						{
							// Fallback: directly run behavior tree
							UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Running behavior tree directly"));
							AIC->RunBehaviorTree(DefaultBT);
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[SoulslikeEnemy] Failed to load default BT_Enemy!"));
			}
		}
	}

	// Setup perception callbacks after a short delay to ensure controller is ready
	SetupPerceptionCallbacks();
}

void ASLFSoulslikeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// NOTE: Animation variables are now updated by C++ AnimInstance NativeUpdateAnimation()
	// The AnimBPs have been reparented to C++ AnimInstance classes (UABP_SoulslikeEnemy, etc.)
	// which set GroundSpeed, Velocity, IsFalling, Direction directly via UPROPERTY
	//
	// The reflection-based UpdateAnimationVariables() is no longer needed and has been removed
}

void ASLFSoulslikeEnemy::UpdateAnimationVariables()
{
	// ═══════════════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLE BRIDGE PATTERN (AAA Standard)
	// ═══════════════════════════════════════════════════════════════════════════════
	//
	// This function sets Animation Blueprint variables from the owning Character.
	// This is the CORRECT pattern for AnimBPs that have Blueprint-defined variables.
	//
	// WHY THIS PATTERN:
	// - AnimBP has K2Node_VariableGet nodes referencing Blueprint variables by GUID
	// - AnimGraph reads these variables for locomotion blend spaces
	// - Simply reparenting to C++ AnimInstance doesn't work because nodes still
	//   reference Blueprint variables, not C++ properties
	// - The proper solution is to set Blueprint variables from the Character
	//
	// VARIABLES SET:
	// - GroundSpeed: 2D velocity magnitude for locomotion blend
	// - Velocity: Full 3D velocity vector
	// - IsFalling: Whether character is in air
	// - Direction: Movement direction relative to facing
	// - SoulslikeEnemy: Reference to owning actor
	// - SoulslikeCharacter: Reference to owning actor (alias)
	// - MovementComponent: Reference to character movement
	//
	// ═══════════════════════════════════════════════════════════════════════════════

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	// Calculate animation values from character state
	const FVector CurrentVelocity = GetVelocity();
	const float CurrentGroundSpeed = CurrentVelocity.Size2D();
	const bool bIsFalling = GetCharacterMovement() ? GetCharacterMovement()->IsFalling() : false;

	float CurrentDirection = 0.0f;
	if (CurrentGroundSpeed > 1.0f)
	{
		CurrentDirection = UKismetAnimationLibrary::CalculateDirection(CurrentVelocity, GetActorRotation());
	}

	// Get AnimInstance class for property lookup
	UClass* AnimClass = AnimInstance->GetClass();

	// Set GroundSpeed (double or float depending on AnimBP definition)
	if (FDoubleProperty* GroundSpeedProp = FindFProperty<FDoubleProperty>(AnimClass, TEXT("GroundSpeed")))
	{
		GroundSpeedProp->SetPropertyValue_InContainer(AnimInstance, CurrentGroundSpeed);
	}
	else if (FFloatProperty* GroundSpeedFloatProp = FindFProperty<FFloatProperty>(AnimClass, TEXT("GroundSpeed")))
	{
		GroundSpeedFloatProp->SetPropertyValue_InContainer(AnimInstance, static_cast<float>(CurrentGroundSpeed));
	}

	// Set Velocity (FVector)
	if (FStructProperty* VelocityProp = FindFProperty<FStructProperty>(AnimClass, TEXT("Velocity")))
	{
		if (VelocityProp->Struct == TBaseStructure<FVector>::Get())
		{
			*VelocityProp->ContainerPtrToValuePtr<FVector>(AnimInstance) = CurrentVelocity;
		}
	}

	// Set IsFalling (bool)
	if (FBoolProperty* IsFallingProp = FindFProperty<FBoolProperty>(AnimClass, TEXT("IsFalling")))
	{
		IsFallingProp->SetPropertyValue_InContainer(AnimInstance, bIsFalling);
	}

	// Set Direction (double or float)
	if (FDoubleProperty* DirectionProp = FindFProperty<FDoubleProperty>(AnimClass, TEXT("Direction")))
	{
		DirectionProp->SetPropertyValue_InContainer(AnimInstance, CurrentDirection);
	}
	else if (FFloatProperty* DirectionFloatProp = FindFProperty<FFloatProperty>(AnimClass, TEXT("Direction")))
	{
		DirectionFloatProp->SetPropertyValue_InContainer(AnimInstance, static_cast<float>(CurrentDirection));
	}

	// Set SoulslikeEnemy / SoulslikeCharacter (references to this actor)
	if (FObjectProperty* SoulslikeEnemyProp = FindFProperty<FObjectProperty>(AnimClass, TEXT("SoulslikeEnemy")))
	{
		SoulslikeEnemyProp->SetObjectPropertyValue_InContainer(AnimInstance, this);
	}
	if (FObjectProperty* SoulslikeCharacterProp = FindFProperty<FObjectProperty>(AnimClass, TEXT("SoulslikeCharacter")))
	{
		SoulslikeCharacterProp->SetObjectPropertyValue_InContainer(AnimInstance, this);
	}

	// Set MovementComponent
	if (FObjectProperty* MovementCompProp = FindFProperty<FObjectProperty>(AnimClass, TEXT("MovementComponent")))
	{
		MovementCompProp->SetObjectPropertyValue_InContainer(AnimInstance, GetCharacterMovement());
	}
}


// ═══════════════════════════════════════════════════════════════════════════════
// PERCEPTION HANDLING
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::SetupPerceptionCallbacks()
{
	// Get AI Controller
	AController* MyController = GetController();
	if (!MyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] SetupPerceptionCallbacks - No controller yet, will retry"));
		// Retry after a short delay
		FTimerHandle RetryHandle;
		GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &ASLFSoulslikeEnemy::SetupPerceptionCallbacks, 0.1f, false);
		return;
	}

	// Find AI Perception Component on the controller
	UAIPerceptionComponent* PerceptionComp = MyController->FindComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] SetupPerceptionCallbacks - No perception component on controller"));
		return;
	}

	// Bind to OnTargetPerceptionUpdated
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASLFSoulslikeEnemy::OnPerceptionUpdated);
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] SetupPerceptionCallbacks - Bound perception callback"));
}

void ASLFSoulslikeEnemy::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	// Check if this is a player or other target
	APawn* SensedPawn = Cast<APawn>(Actor);
	if (!SensedPawn)
	{
		return;
	}

	// Only react to players (check if controlled by a player controller)
	if (!SensedPawn->IsPlayerControlled())
	{
		return;
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// STATE MACHINE APPROACH (replaces Behavior Tree)
	// ═══════════════════════════════════════════════════════════════════════════

	// Check if we have the state machine
	if (AIStateMachine)
	{
		// Get current state from state machine
		ESLFAIState CurrentStateMachine = AIStateMachine->GetCurrentState();

		// AAA APPROACH: Combat state has hysteresis
		if (CurrentStateMachine == ESLFAIState::Combat)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[SLF_PERCEPTION] %s already in Combat (StateMachine), refreshing target"),
				*GetName());
			LastCombatEntryTime = GetWorld()->GetTimeSeconds();
			return;
		}

		// Not in combat - check if we should enter
		AAIController* AIC = Cast<AAIController>(GetController());
		if (!AIC)
		{
			return;
		}

		UAIPerceptionComponent* PerceptionComp = AIC->GetAIPerceptionComponent();
		if (!PerceptionComp)
		{
			return;
		}

		// Check if target is currently sensed
		FActorPerceptionBlueprintInfo PerceptionInfo;
		bool bCurrentlySensed = false;
		if (PerceptionComp->GetActorsPerception(Actor, PerceptionInfo))
		{
			for (const FAIStimulus& Stim : PerceptionInfo.LastSensedStimuli)
			{
				if (Stim.WasSuccessfullySensed())
				{
					bCurrentlySensed = true;
					break;
				}
			}
		}

		if (bCurrentlySensed)
		{
			// Target detected - tell state machine!
			UE_LOG(LogTemp, Warning, TEXT("[SLF_PERCEPTION_SENSED] Player detected on %s! Setting target on StateMachine"), *GetName());

			// SetTarget automatically transitions to Combat state
			AIStateMachine->SetTarget(Actor);
			LastCombatEntryTime = GetWorld()->GetTimeSeconds();
		}

		return; // State machine handled it
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// FALLBACK: Legacy BehaviorManager path (for backwards compatibility)
	// ═══════════════════════════════════════════════════════════════════════════

	// Get current state
	ESLFAIStates CurrentState = ESLFAIStates::Idle;
	if (BehaviorManagerComponent)
	{
		CurrentState = BehaviorManagerComponent->GetState();
	}

	// AAA APPROACH: Combat state has hysteresis - once in combat, STAY in combat
	if (CurrentState == ESLFAIStates::Combat)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SLF_PERCEPTION] %s already in Combat (BehaviorManager), ignoring"),
			*GetName());
		LastCombatEntryTime = GetWorld()->GetTimeSeconds();
		return;
	}

	// Not in combat - check if we should enter combat
	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC)
	{
		return;
	}

	UAIPerceptionComponent* PerceptionComp = AIC->GetAIPerceptionComponent();
	if (!PerceptionComp)
	{
		return;
	}

	// Check if target is currently sensed by ANY configured sense
	FActorPerceptionBlueprintInfo PerceptionInfo;
	bool bCurrentlySensed = false;
	if (PerceptionComp->GetActorsPerception(Actor, PerceptionInfo))
	{
		for (const FAIStimulus& Stim : PerceptionInfo.LastSensedStimuli)
		{
			if (Stim.WasSuccessfullySensed())
			{
				bCurrentlySensed = true;
				break;
			}
		}
	}

	if (bCurrentlySensed)
	{
		// Target detected - enter combat!
		UE_LOG(LogTemp, Warning, TEXT("[SLF_PERCEPTION_SENSED] Player detected on %s! Switching to Combat state (BehaviorManager)"), *GetName());

		if (BehaviorManagerComponent)
		{
			BehaviorManagerComponent->SetTarget(Actor);
			BehaviorManagerComponent->SetState(ESLFAIStates::Combat);
			LastCombatEntryTime = GetWorld()->GetTimeSeconds();

			if (UBlackboardComponent* BB = BehaviorManagerComponent->GetBlackboard())
			{
				BB->SetValueAsBool(FName("InCombat"), true);
			}
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

void ASLFSoulslikeEnemy::PerformAbility_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility called on %s"), *GetName());

	// Get Combat Manager
	UAICombatManagerComponent* CombatManager = CombatManagerComponent;
	if (!CombatManager)
	{
		CombatManager = FindComponentByClass<UAICombatManagerComponent>();
	}

	if (!CombatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PerformAbility - No CombatManager found!"));
		OnAttackEnd.Broadcast();
		return;
	}

	// Try to get an ability
	UDataAsset* Ability = nullptr;
	bool bGotAbility = CombatManager->TryGetAbility(Ability);

	if (!bGotAbility || !Ability)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility - No ability available"));
		OnAttackEnd.Broadcast();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility - Executing ability: %s (Class: %s)"),
		*Ability->GetName(), *Ability->GetClass()->GetName());

	// Get the ability montage from the ability asset
	// UPDA_AI_Ability uses "Montage" property with TSoftObjectPtr<UAnimMontage>
	UAnimMontage* AbilityMontage = nullptr;

	// First try: Cast to UPDA_AI_Ability directly (preferred - avoids reflection)
	if (UPDA_AI_Ability* AIAbility = Cast<UPDA_AI_Ability>(Ability))
	{
		AbilityMontage = AIAbility->Montage.LoadSynchronous();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility - Got montage from UPDA_AI_Ability: %s"),
			AbilityMontage ? *AbilityMontage->GetName() : TEXT("null"));
	}

	// Fallback: Try reflection for "Montage" property (TSoftObjectPtr)
	if (!AbilityMontage)
	{
		FProperty* MontageProp = Ability->GetClass()->FindPropertyByName(FName("Montage"));
		if (MontageProp)
		{
			if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(MontageProp))
			{
				FSoftObjectPtr* SoftPtr = SoftObjProp->GetPropertyValuePtr_InContainer(Ability);
				if (SoftPtr)
				{
					AbilityMontage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility - Got montage via reflection (Montage): %s"),
						AbilityMontage ? *AbilityMontage->GetName() : TEXT("null"));
				}
			}
		}
	}

	// Also try AbilityMontage (legacy)
	if (!AbilityMontage)
	{
		FProperty* MontageProp = Ability->GetClass()->FindPropertyByName(FName("AbilityMontage"));
		if (MontageProp)
		{
			if (FObjectProperty* ObjProp = CastField<FObjectProperty>(MontageProp))
			{
				UObject* MontageObj = ObjProp->GetObjectPropertyValue_InContainer(Ability);
				AbilityMontage = Cast<UAnimMontage>(MontageObj);
			}
		}
	}

	// Also try ActionMontage (legacy - for action data assets)
	if (!AbilityMontage)
	{
		FProperty* MontageProp = Ability->GetClass()->FindPropertyByName(FName("ActionMontage"));
		if (MontageProp)
		{
			if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(MontageProp))
			{
				FSoftObjectPtr* SoftPtr = SoftObjProp->GetPropertyValuePtr_InContainer(Ability);
				if (SoftPtr)
				{
					AbilityMontage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
				}
			}
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(MontageProp))
			{
				UObject* MontageObj = ObjProp->GetObjectPropertyValue_InContainer(Ability);
				AbilityMontage = Cast<UAnimMontage>(MontageObj);
			}
		}
	}

	if (!AbilityMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PerformAbility - No montage found in ability asset"));
		OnAttackEnd.Broadcast();
		return;
	}

	// Play the montage
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PerformAbility - No mesh component"));
		OnAttackEnd.Broadcast();
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PerformAbility - No anim instance"));
		OnAttackEnd.Broadcast();
		return;
	}

	// Play montage and set up completion callback
	FOnMontageEnded MontageEndDelegate;
	MontageEndDelegate.BindLambda([this](UAnimMontage* EndedMontage, bool bInterrupted)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility montage ended (interrupted=%d)"), bInterrupted);
		OnAttackEnd.Broadcast();
	});

	float PlayRate = AnimInstance->Montage_Play(AbilityMontage, 1.0f);
	if (PlayRate > 0.0f)
	{
		AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, AbilityMontage);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] PerformAbility - Playing montage: %s"), *AbilityMontage->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] PerformAbility - Failed to play montage"));
		OnAttackEnd.Broadcast();
	}
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
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] ToggleHealthbarVisual(%s) - HealthbarWidget: %s"),
		bVisible ? TEXT("true") : TEXT("false"),
		HealthbarWidget ? TEXT("Valid") : TEXT("NULL"));

	if (HealthbarWidget)
	{
		// Get widget class status
		TSubclassOf<UUserWidget> WidgetClass = HealthbarWidget->GetWidgetClass();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] WidgetClass: %s"), WidgetClass ? *WidgetClass->GetName() : TEXT("NULL"));

		// Get the actual widget
		UUserWidget* Widget = HealthbarWidget->GetWidget();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Widget instance: %s"), Widget ? *Widget->GetName() : TEXT("NULL"));

		// If no widget and we want visible, try to initialize
		if (bVisible && !Widget && WidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Initializing widget manually"));
			HealthbarWidget->InitWidget();
			Widget = HealthbarWidget->GetWidget();
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] After init - Widget: %s"), Widget ? *Widget->GetName() : TEXT("NULL"));
		}

		// Set visibility (inherited from USceneComponent)
		HealthbarWidget->SetVisibility(bVisible);

		// Also set hidden in game (sometimes needed)
		HealthbarWidget->SetHiddenInGame(!bVisible);

		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Set HealthbarWidget visibility=%s, hiddenInGame=%s"),
			bVisible ? TEXT("true") : TEXT("false"),
			!bVisible ? TEXT("true") : TEXT("false"));

		// Log draw size for debugging
		FVector2D DrawSize = HealthbarWidget->GetDrawSize();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Widget DrawSize: %s"), *DrawSize.ToString());
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
	// 2. Clear poise break timer (prevent early recovery during execution)
	// 3. Get relevant executed montage from combat manager
	// 4. Reset stance stat
	// 5. Play montage
	// 6. On completed/blend out: Call SetState to change AI state
	// 7. Call FinishPoiseBreak on combat manager

	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted: Tag=%s"), *ExecutionTag.ToString());

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

	// CRITICAL: Prepare for execution - stop poise break loop but DON'T resume AI yet
	// The AI should not attack while we're playing the executed montage
	// We'll broadcast OnPoiseBroken(false) AFTER the montage ends
	CombatManager->ClearPoiseBreakResetTimer();
	CombatManager->bPoiseBroken = false; // Set directly, don't broadcast yet

	// Stop the poise break loop animation
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.25f);
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted - Stopped all montages for execution"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted - Cleared poise break, preparing for execution montage"));

	// Get executed montage by looking up DataTable with ExecutionTag
	UAnimMontage* Montage = CombatManager->GetRelevantExecutedMontage(ExecutionTag);

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
					UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted montage ended - NOW resuming AI"));

					// After execution montage ends, NOW we can resume AI
					// Broadcast OnPoiseBroken(false) to signal recovery
					if (CombatManager)
					{
						CombatManager->OnPoiseBroken.Broadcast(false);
						UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Broadcasted OnPoiseBroken(false) - AI can resume"));
					}

					// Set AI state to Idle
					if (BehaviorManagerComponent)
					{
						BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
					}
				});

				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Playing executed montage: %s"), *Montage->GetName());
				AnimInstance->Montage_Play(Montage, 1.0f);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
	else
	{
		// No montage found, broadcast immediately and set state to Idle
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnExecuted - No montage, resuming AI immediately"));
		CombatManager->OnPoiseBroken.Broadcast(false);
		if (BehaviorManagerComponent)
		{
			BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
		}
	}
}

void ASLFSoulslikeEnemy::OnBackstabbed_Implementation(FGameplayTag ExecutionTag)
{
	// From Blueprint: Similar to OnExecuted but for backstabs
	// Backstab happens to unsuspecting enemies (not poise broken)
	// 1. Check if ExecutionTag is valid
	// 2. Get relevant executed montage
	// 3. Play montage
	// 4. On completed: SetState(Idle)

	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnBackstabbed: Tag=%s"), *ExecutionTag.ToString());

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

	// Get executed montage by looking up DataTable with ExecutionTag
	UAnimMontage* Montage = CombatManager->GetRelevantExecutedMontage(ExecutionTag);

	if (Montage)
	{
		// Play the montage
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
			{
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindLambda([this](UAnimMontage* EndedMontage, bool bInterrupted)
				{
					UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnBackstabbed montage ended - setting AI state to Idle"));

					// After backstab montage ends, just set AI state to Idle
					// Enemy will recover and resume normal behavior
					if (BehaviorManagerComponent)
					{
						BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
					}
				});

				AnimInstance->Montage_Play(Montage, 1.0f);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
	else
	{
		// No montage found, just set state to Idle
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] OnBackstabbed - No montage, setting AI state to Idle"));
		if (BehaviorManagerComponent)
		{
			BehaviorManagerComponent->SetState(ESLFAIStates::Idle);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// POISE BREAK / EXECUTION INDICATOR HANDLING
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeEnemy::HandleOnPoiseBroken(bool bBroken)
{
	// ═══════════════════════════════════════════════════════════════════════════════
	// bp_only flow (B_Soulslike_Enemy.json lines 10598-10899):
	// On Poise Broken (AC_AI_CombatManager) event fires with "Broken?" parameter
	// then → SetVisibility(ExecutionWidget, bNewVisibility = Broken?)
	// then → GetWidget() from ExecutionWidget → ToggleExecutionIcon(Visible = Broken?)
	// ═══════════════════════════════════════════════════════════════════════════════

	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] *** HandleOnPoiseBroken *** - bBroken=%s - Enemy: %s"),
		bBroken ? TEXT("true") : TEXT("false"), *GetName());

	// CachedExecutionWidget is inherited from ASLFBaseCharacter
	if (CachedExecutionWidget)
	{
		// Step 1: SetVisibility on the WidgetComponent based on bBroken
		// This is the bp_only SetVisibility node (lines 10640-10674)
		CachedExecutionWidget->SetVisibility(bBroken);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] ExecutionWidget visibility = %s"),
			bBroken ? TEXT("VISIBLE") : TEXT("HIDDEN"));

		// Step 2: Get the UserWidget and call ToggleExecutionIcon on it
		// This is the bp_only GetWidget → ToggleExecutionIcon flow (lines 10679-10899)
		UUserWidget* Widget = CachedExecutionWidget->GetWidget();

		// Widget may be null if component was hidden and never initialized - force init
		if (!Widget && bBroken && CachedExecutionWidget->GetWidgetClass())
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Widget is null, forcing InitWidget()..."));
			CachedExecutionWidget->InitWidget();
			Widget = CachedExecutionWidget->GetWidget();
		}

		if (Widget)
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeEnemy] Widget class: %s"), *Widget->GetClass()->GetName());

			// W_TargetExecutionIndicator implements ISLFExecutionIndicatorInterface
			if (Widget->GetClass()->ImplementsInterface(USLFExecutionIndicatorInterface::StaticClass()))
			{
				ISLFExecutionIndicatorInterface::Execute_ToggleExecutionIcon(Widget, bBroken);
				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Called ToggleExecutionIcon(%s) on ExecutionWidget"),
					bBroken ? TEXT("true") : TEXT("false"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] Widget %s doesn't implement ISLFExecutionIndicatorInterface"),
					*Widget->GetClass()->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeEnemy] ExecutionWidget->GetWidget() returned null - is WidgetClass set? WidgetClass=%s"),
				CachedExecutionWidget->GetWidgetClass() ? *CachedExecutionWidget->GetWidgetClass()->GetName() : TEXT("NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeEnemy] CachedExecutionWidget is NULL - execution indicator won't show!"));
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeEnemy] Check if ASLFBaseCharacter creates ExecutionWidget in constructor."));
	}
}
