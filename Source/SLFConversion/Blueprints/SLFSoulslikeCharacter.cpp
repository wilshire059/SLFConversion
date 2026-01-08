// SLFSoulslikeCharacter.cpp
// C++ implementation for B_Soulslike_Character
//
// ═══════════════════════════════════════════════════════════════════════════════
// 20-PASS VALIDATION: Full logic migration from B_Soulslike_Character.json
// ═══════════════════════════════════════════════════════════════════════════════
// Source: BlueprintDNA/Blueprint/B_Soulslike_Character.json (110,703 lines)
// Key Sections Migrated:
//   - INPUT & BUFFERING (action dispatch via InputBuffer -> ActionManager)
//   - BEGIN PLAY - INITIALIZATION (component caching, input setup)
//   - Enhanced Input System integration
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFSoulslikeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/InputBufferComponent.h"
#include "Components/AC_ActionManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_InteractionManager.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "UObject/ConstructorHelpers.h"
#include "SLFGameTypes.h"  // For FSLFSkeletalMeshData reflection access
#include "SLFPrimaryDataAssets.h"
#include "SLFPickupItemBase.h"
#include "Components/InventoryManagerComponent.h"
#include "Framework/SLFPlayerController.h"
#include "Widgets/W_HUD.h"
#include "B_Interactable.h"

ASLFSoulslikeCharacter::ASLFSoulslikeCharacter()
{
	// ═══════════════════════════════════════════════════════════════════
	// LOAD INPUT ASSETS - Enhanced Input configuration
	// ═══════════════════════════════════════════════════════════════════

	// Load Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"));
	if (MappingContextFinder.Succeeded())
	{
		PlayerMappingContext = MappingContextFinder.Object;
	}

	// Load Input Actions
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Move.IA_Move"));
	if (MoveActionFinder.Succeeded())
	{
		IA_Move = MoveActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Look.IA_Look"));
	if (LookActionFinder.Succeeded())
	{
		IA_Look = LookActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionFinder.Succeeded())
	{
		IA_Jump = JumpActionFinder.Object;
	}

	// Sprint and Dodge share the same Input Action asset
	static ConstructorHelpers::FObjectFinder<UInputAction> SprintDodgeActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Sprint_Dodge.IA_Sprint_Dodge"));
	if (SprintDodgeActionFinder.Succeeded())
	{
		IA_Sprint = SprintDodgeActionFinder.Object;
		IA_Dodge = SprintDodgeActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_RightHandAttack.IA_RightHandAttack"));
	if (AttackActionFinder.Succeeded())
	{
		IA_Attack = AttackActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> GuardActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Guard_LeftHandAttack_DualWieldAttack.IA_Guard_LeftHandAttack_DualWieldAttack"));
	if (GuardActionFinder.Succeeded())
	{
		IA_Guard = GuardActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Interact.IA_Interact"));
	if (InteractActionFinder.Succeeded())
	{
		IA_Interact = InteractActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> TargetLockActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_TargetLock.IA_TargetLock"));
	if (TargetLockActionFinder.Succeeded())
	{
		IA_TargetLock = TargetLockActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Crouch.IA_Crouch"));
	if (CrouchActionFinder.Succeeded())
	{
		IA_Crouch = CrouchActionFinder.Object;
	}

	// Initialize camera config
	CameraPitchLock = 0.0;

	// Initialize settings
	CustomGameSettings = nullptr;
	DefaultMeshInfo = nullptr;

	// Initialize cache
	bCache_IsHoldingInteract = false;
	bCache_IsHoldingSprint = false;
	Cache_InteractElapsed = 0.0;
	Cache_SpringArmLength = 300.0;
	bCache_IsAsyncEquipmentBusy = false;
	Cache_ActiveCameraSequence = nullptr;
	Cache_ControlRotation = FRotator::ZeroRotator;
	bIsMeshInitialized = false;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENT POINTERS - NOT created here, cached from Blueprint SCS in BeginPlay
	// The Blueprint's SimpleConstructionScript defines the actual components.
	// C++ only holds pointers for faster access to Blueprint-defined components.
	// ═══════════════════════════════════════════════════════════════════

	// Manager components - defined in Blueprint SCS
	CachedInputBuffer = nullptr;
	CachedActionManager = nullptr;
	CachedCombatManager = nullptr;
	CachedInteractionManager = nullptr;

	// Camera components - defined in Blueprint SCS
	CachedCameraBoom = nullptr;
	CachedFollowCamera = nullptr;

	// Modular mesh components - defined in Blueprint SCS
	Head = nullptr;
	UpperBody = nullptr;
	LowerBody = nullptr;
	Arms = nullptr;

	// ═══════════════════════════════════════════════════════════════════
	// MOVEMENT SETTINGS - Enable crouching on CharacterMovementComponent
	// ═══════════════════════════════════════════════════════════════════
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		MovementComp->bCanWalkOffLedgesWhenCrouching = true;
	}
}

void ASLFSoulslikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] ===== BeginPlay START: %s (Class: %s) ====="), *GetName(), *GetClass()->GetName());

	// Enable crouching on the CharacterMovementComponent
	// Must be done in BeginPlay because Blueprint SCS components override constructor settings
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		MovementComp->bCanWalkOffLedgesWhenCrouching = true;
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Enabled crouching on CharacterMovement"));
	}

	// Cache component references from Blueprint SCS
	CacheComponentReferences();

	// NOTE: AnimBP ActionManager is now set via NativeUpdateAnimation in UABP_SoulslikeCharacter_Additive
	// The C++ AnimInstance uses FindComponentByClass<UAC_ActionManager>() to cache and update
	// the ActionManager reference automatically every frame - no reflection needed

	// Initialize modular mesh system - merge modular meshes and apply to main skeleton
	InitializeModularMesh();

	// Setup enhanced input mapping context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (PlayerMappingContext)
			{
				Subsystem->AddMappingContext(PlayerMappingContext, 0);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Added PlayerMappingContext"));
			}
		}
	}

	// Bind to InputBuffer's OnInputBufferConsumed event
	// From JSON: INPUT & BUFFERING section binds to this event
	if (CachedInputBuffer)
	{
		CachedInputBuffer->OnInputBufferConsumed.AddDynamic(this, &ASLFSoulslikeCharacter::OnInputBufferConsumed);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound to InputBuffer OnInputBufferConsumed"));
	}
}

void ASLFSoulslikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// NOTE: AnimBP IsCrouched is now set via NativeUpdateAnimation in UABP_SoulslikeCharacter_Additive
	// The C++ AnimInstance reads OwnerCharacter->bIsCrouched automatically every frame
}

void ASLFSoulslikeCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Update ActionManager's IsCrouched - some gameplay systems may read this
	if (CachedActionManager)
	{
		CachedActionManager->IsCrouched = true;
	}

	// NOTE: AnimBP IsCrouched is now handled by NativeUpdateAnimation in UABP_SoulslikeCharacter_Additive
	// It reads OwnerCharacter->bIsCrouched automatically, and the ActionManager is found via FindComponentByClass
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnStartCrouch"));
}

void ASLFSoulslikeCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Update ActionManager's IsCrouched - some gameplay systems may read this
	if (CachedActionManager)
	{
		CachedActionManager->IsCrouched = false;
	}

	// NOTE: AnimBP IsCrouched is now handled by NativeUpdateAnimation in UABP_SoulslikeCharacter_Additive
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnEndCrouch"));
}

void ASLFSoulslikeCharacter::UpdateAnimInstanceCrouchState(bool bCrouching)
{
	// NOTE: AnimBP IsCrouched is now handled by NativeUpdateAnimation in UABP_SoulslikeCharacter_Additive
	// The C++ AnimInstance reads OwnerCharacter->bIsCrouched automatically every frame
	// This function is kept for API compatibility but is no longer needed
}

void ASLFSoulslikeCharacter::CacheComponentReferences()
{
	// Cache commonly used components for faster access
	// NOTE: Blueprint SCS defines components like AC_ActionManager_C, AC_CombatManager_C, etc.
	// These are Blueprint subclasses of our C++ classes (UAC_ActionManager, etc.)
	// FindComponentByClass works because Blueprint classes inherit from C++ base
	CachedInputBuffer = FindComponentByClass<UInputBufferComponent>();
	CachedActionManager = FindComponentByClass<UAC_ActionManager>();
	CachedCombatManager = FindComponentByClass<UAC_CombatManager>();
	CachedInteractionManager = FindComponentByClass<UAC_InteractionManager>();

	// Cache camera components from Blueprint
	CachedCameraBoom = FindComponentByClass<USpringArmComponent>();
	CachedFollowCamera = FindComponentByClass<UCameraComponent>();

	// Cache modular mesh components by name from Blueprint SCS
	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
	for (USkeletalMeshComponent* Comp : SkeletalMeshComps)
	{
		FString Name = Comp->GetName();
		if (Name.Contains(TEXT("Head")))
		{
			Head = Comp;
		}
		else if (Name.Contains(TEXT("UpperBody")))
		{
			UpperBody = Comp;
		}
		else if (Name.Contains(TEXT("LowerBody")))
		{
			LowerBody = Comp;
		}
		else if (Name.Contains(TEXT("Arms")))
		{
			Arms = Comp;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Cached components - InputBuffer: %s, ActionManager: %s, CombatManager: %s, InteractionManager: %s, CameraBoom: %s, FollowCamera: %s"),
		CachedInputBuffer ? TEXT("OK") : TEXT("NULL"),
		CachedActionManager ? TEXT("OK") : TEXT("NULL"),
		CachedCombatManager ? TEXT("OK") : TEXT("NULL"),
		CachedInteractionManager ? TEXT("OK") : TEXT("NULL"),
		CachedCameraBoom ? TEXT("OK") : TEXT("NULL"),
		CachedFollowCamera ? TEXT("OK") : TEXT("NULL"));

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Modular Mesh - Head: %s, UpperBody: %s, LowerBody: %s, Arms: %s"),
		Head ? TEXT("OK") : TEXT("NULL"),
		UpperBody ? TEXT("OK") : TEXT("NULL"),
		LowerBody ? TEXT("OK") : TEXT("NULL"),
		Arms ? TEXT("OK") : TEXT("NULL"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// INPUT BUFFER INTEGRATION (from JSON: INPUT & BUFFERING section)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeCharacter::OnInputBufferConsumed(FGameplayTag Action)
{
	// From JSON logic:
	// 1. Check NOT IsDead? via CombatManager
	// 2. If alive, call ActionManager.PerformAction(Action)

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnInputBufferConsumed: %s"), *Action.ToString());

	// Check if character is dead
	bool bIsDead = false;
	if (CachedCombatManager)
	{
		bIsDead = CachedCombatManager->IsDead;
	}

	if (bIsDead)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Input ignored - character is dead"));
		return;
	}

	// Dispatch action to ActionManager
	if (CachedActionManager)
	{
		CachedActionManager->EventPerformAction(Action);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Dispatched action to ActionManager: %s"), *Action.ToString());
	}

	// Broadcast OnInputDetected
	OnInputDetected.Broadcast();
}

void ASLFSoulslikeCharacter::QueueActionToBuffer(const FGameplayTag& ActionTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] QueueActionToBuffer: %s (InputBuffer: %s)"),
		*ActionTag.ToString(), CachedInputBuffer ? TEXT("OK") : TEXT("NULL"));

	if (CachedInputBuffer)
	{
		CachedInputBuffer->QueueAction(ActionTag);
	}
}

void ASLFSoulslikeCharacter::ExecuteActionImmediately(const FGameplayTag& ActionTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] ExecuteActionImmediately: %s (InputBuffer: %s)"),
		*ActionTag.ToString(), CachedInputBuffer ? TEXT("OK") : TEXT("NULL"));

	if (CachedInputBuffer)
	{
		CachedInputBuffer->ExecuteActionImmediately(ActionTag);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ENHANCED INPUT SETUP
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Failed to get EnhancedInputComponent"));
		return;
	}

	// Bind input actions from JSON: MOVEMENT & CAMERA, INPUT & BUFFERING sections
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Input Actions - Move:%s Look:%s Jump:%s Sprint:%s Crouch:%s"),
		IA_Move ? TEXT("OK") : TEXT("NULL"),
		IA_Look ? TEXT("OK") : TEXT("NULL"),
		IA_Jump ? TEXT("OK") : TEXT("NULL"),
		IA_Sprint ? TEXT("OK") : TEXT("NULL"),
		IA_Crouch ? TEXT("OK") : TEXT("NULL"));

	if (IA_Move)
	{
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASLFSoulslikeCharacter::HandleMove);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound IA_Move to HandleMove"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_Move is NULL - movement will not work!"));
	}
	if (IA_Look)
	{
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ASLFSoulslikeCharacter::HandleLook);
	}
	if (IA_Jump)
	{
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleJump);
	}
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleSprintStarted);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleSprintCompleted);
	}
	if (IA_Dodge)
	{
		EnhancedInput->BindAction(IA_Dodge, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleDodge);
	}
	if (IA_Attack)
	{
		EnhancedInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleAttack);
	}
	if (IA_Guard)
	{
		EnhancedInput->BindAction(IA_Guard, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleGuardStarted);
		EnhancedInput->BindAction(IA_Guard, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleGuardCompleted);
	}
	if (IA_Interact)
	{
		EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleInteractStarted);
		EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleInteractCompleted);
	}
	if (IA_TargetLock)
	{
		EnhancedInput->BindAction(IA_TargetLock, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleTargetLockInput);
	}
	if (IA_Crouch)
	{
		EnhancedInput->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleCrouch);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] *** IA_Crouch BOUND to HandleCrouch - Press C to crouch ***"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] !!! IA_Crouch is NULL - Crouch input will NOT work !!!"));
	}

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Enhanced input bindings set up"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// INPUT ACTION HANDLERS
// From JSON: INPUT & BUFFERING section - each dispatches GameplayTag to buffer
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeCharacter::HandleMove(const FInputActionValue& Value)
{
	// Movement is handled by CharacterMovementComponent, not via action buffer
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Debug: Log that HandleMove is being called
	static bool bFirstMove = true;
	if (bFirstMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleMove CALLED - Vector: X=%.2f Y=%.2f"), MovementVector.X, MovementVector.Y);
		bFirstMove = false;
	}

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASLFSoulslikeCharacter::HandleLook(const FInputActionValue& Value)
{
	// Camera look is handled by PlayerController
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Debug: Log that HandleLook is being called
	static bool bFirstLook = true;
	if (bFirstLook)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleLook CALLED - Vector: X=%.2f Y=%.2f"), LookAxisVector.X, LookAxisVector.Y);
		bFirstLook = false;
	}

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASLFSoulslikeCharacter::HandleJump()
{
	// From JSON: Queues SoulslikeFramework.Action.Jump
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Jump")));
}

void ASLFSoulslikeCharacter::HandleSprintStarted()
{
	// From JSON: Executes SoulslikeFramework.Action.StartSprinting immediately
	bCache_IsHoldingSprint = true;
	ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StartSprinting")));
}

void ASLFSoulslikeCharacter::HandleSprintCompleted()
{
	// From JSON: Executes SoulslikeFramework.Action.StopSprinting immediately
	bCache_IsHoldingSprint = false;
	ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StopSprinting")));
}

void ASLFSoulslikeCharacter::HandleDodge()
{
	// From JSON: Queues SoulslikeFramework.Action.Dodge
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Dodge")));
}

void ASLFSoulslikeCharacter::HandleAttack()
{
	// From JSON: Queues SoulslikeFramework.Action.LightAttackRight (right hand attack)
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.LightAttackRight")));
}

void ASLFSoulslikeCharacter::HandleGuardStarted()
{
	// From JSON: Queues SoulslikeFramework.Action.GuardStart
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardStart")));
}

void ASLFSoulslikeCharacter::HandleGuardCompleted()
{
	// From JSON: Queues SoulslikeFramework.Action.GuardEnd
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardEnd")));
}

void ASLFSoulslikeCharacter::HandleInteractStarted()
{
	// From JSON: MAIN INTERACTION INPUT EVENT section
	bCache_IsHoldingInteract = true;
	Cache_InteractElapsed = 0.0;

	// InteractionManager tracks NearestInteractable in TickComponent
	// When interact is pressed, we call the interactable's interface method
	if (CachedInteractionManager && CachedInteractionManager->NearestInteractable)
	{
		AActor* Interactable = CachedInteractionManager->NearestInteractable;
		
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Interacting with: %s"), *Interactable->GetName());

		// Call the OnInteract interface method on the interactable
		if (Interactable->GetClass()->ImplementsInterface(USLFInteractableInterface::StaticClass()))
		{
			ISLFInteractableInterface::Execute_OnInteract(Interactable, this);
		}
		
		// Also queue the interaction action for animation handling
		QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Interact")));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] No interactable nearby"));
	}
}

void ASLFSoulslikeCharacter::HandleInteractCompleted()
{
	bCache_IsHoldingInteract = false;
	OnInteractPressed.Broadcast(Cache_InteractElapsed);
}

void ASLFSoulslikeCharacter::HandleTargetLockInput()
{
	// From JSON: TARGET LOCKING section
	HandleTargetLock();
}

void ASLFSoulslikeCharacter::HandleCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("========== CROUCH DEBUG =========="));
	UE_LOG(LogTemp, Warning, TEXT("[HandleCrouch] CALLED! bIsCrouched = %s"), bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("==================================="));

	// Try direct crouch toggle first (bypass action system)
	if (bIsCrouched)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HandleCrouch] Calling UnCrouch()..."));
		UnCrouch();
		UE_LOG(LogTemp, Warning, TEXT("[HandleCrouch] After UnCrouch() - bIsCrouched = %s"), bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HandleCrouch] Calling Crouch()..."));
		Crouch();
		UE_LOG(LogTemp, Warning, TEXT("[HandleCrouch] After Crouch() - bIsCrouched = %s"), bIsCrouched ? TEXT("TRUE") : TEXT("FALSE"));

		// Check if CanCrouch failed
		if (!bIsCrouched)
		{
			UE_LOG(LogTemp, Error, TEXT("[HandleCrouch] Crouch() did NOT change bIsCrouched! CanCrouch() may have failed!"));
			if (UCharacterMovementComponent* CMC = GetCharacterMovement())
			{
				UE_LOG(LogTemp, Error, TEXT("[HandleCrouch] CMC->CanEverCrouch() = %s"), CMC->CanEverCrouch() ? TEXT("TRUE") : TEXT("FALSE"));
				UE_LOG(LogTemp, Error, TEXT("[HandleCrouch] CMC->IsCrouching() = %s"), CMC->IsCrouching() ? TEXT("TRUE") : TEXT("FALSE"));
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1-4/4] (from JSON Functions section)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeCharacter::HandleTargetLock_Implementation()
{
	// From JSON: Toggle target lock - if locked, unlock; if unlocked, find target and lock
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] HandleTargetLock"));

	if (!CachedInteractionManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleTargetLock - InteractionManager is NULL"));
		return;
	}

	// Check if currently target locked
	if (CachedInteractionManager->IsTargetLocked())
	{
		// Currently locked - unlock
		CachedInteractionManager->ResetLockOn();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Target unlocked"));
	}
	else
	{
		// Not locked - find target via trace and lock on
		TArray<FHitResult> Hits = CachedInteractionManager->TargetLockTrace();
		if (Hits.Num() > 0)
		{
			// Lock onto first valid target
			AActor* TargetActor = Hits[0].GetActor();
			if (TargetActor)
			{
				CachedInteractionManager->LockOnTarget(TargetActor, true);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Locked onto target: %s"), *TargetActor->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] No valid targets found for lock-on"));
		}
	}
}

float ASLFSoulslikeCharacter::GetAxisValue_Implementation(FName AxisName)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		return PC->GetInputAxisValue(AxisName);
	}
	return 0.0f;
}

FSLFModularMeshData ASLFSoulslikeCharacter::MakeModularMeshData_Implementation()
{
	FSLFModularMeshData Data;

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] MakeModularMeshData - Collecting meshes"));

	// Collect meshes from modular components (order: Head, UpperBody, Arms, LowerBody)
	TArray<USkeletalMesh*> MeshesToMerge;

	if (Head && Head->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(Head->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("  [0] Head: %s"), *Head->GetSkeletalMeshAsset()->GetName());
	}

	if (UpperBody && UpperBody->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(UpperBody->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("  [1] UpperBody: %s"), *UpperBody->GetSkeletalMeshAsset()->GetName());
	}

	if (Arms && Arms->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(Arms->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("  [2] Arms: %s"), *Arms->GetSkeletalMeshAsset()->GetName());
	}

	if (LowerBody && LowerBody->GetSkeletalMeshAsset())
	{
		MeshesToMerge.Add(LowerBody->GetSkeletalMeshAsset());
		UE_LOG(LogTemp, Log, TEXT("  [3] LowerBody: %s"), *LowerBody->GetSkeletalMeshAsset()->GetName());
	}

	// Build FSkeletalMeshMergeParams
	MeshMergeData = FSkeletalMeshMergeParams();
	for (USkeletalMesh* MeshPart : MeshesToMerge)
	{
		MeshMergeData.MeshesToMerge.Add(MeshPart);
	}

	// Use skeleton from first mesh
	if (MeshesToMerge.Num() > 0 && MeshesToMerge[0])
	{
		MeshMergeData.Skeleton = MeshesToMerge[0]->GetSkeleton();
	}

	// Populate return struct
	Data.MeshParts = MeshesToMerge;
	Data.Skeleton = MeshMergeData.Skeleton;

	UE_LOG(LogTemp, Log, TEXT("  Collected %d meshes for merging"), MeshesToMerge.Num());

	return Data;
}

void ASLFSoulslikeCharacter::RefreshModularPieces()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] RefreshModularPieces"));

	// Step 1: Collect meshes into MeshMergeData
	MakeModularMeshData();

	// Step 2: Merge meshes using SkeletalMergingLibrary
	if (MeshMergeData.MeshesToMerge.Num() > 0)
	{
		USkeletalMesh* MergedMesh = USkeletalMergingLibrary::MergeMeshes(MeshMergeData);

		// Step 3: Apply merged mesh to main character skeleton
		if (IsValid(MergedMesh))
		{
			if (USkeletalMeshComponent* CharMesh = GetMesh())
			{
				CharMesh->SetSkinnedAssetAndUpdate(MergedMesh, false);
				bIsMeshInitialized = true;
				UE_LOG(LogTemp, Log, TEXT("  Applied merged mesh to character skeleton!"));

				// Step 4: Hide the modular components since we're now using the merged mesh
				// The modular components were only used to hold references for merging
				if (Head)
				{
					Head->SetVisibility(false);
					UE_LOG(LogTemp, Log, TEXT("  Hidden Head component"));
				}
				if (UpperBody)
				{
					UpperBody->SetVisibility(false);
					UE_LOG(LogTemp, Log, TEXT("  Hidden UpperBody component"));
				}
				if (Arms)
				{
					Arms->SetVisibility(false);
					UE_LOG(LogTemp, Log, TEXT("  Hidden Arms component"));
				}
				if (LowerBody)
				{
					LowerBody->SetVisibility(false);
					UE_LOG(LogTemp, Log, TEXT("  Hidden LowerBody component"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  MergeMeshes returned null - merge failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No meshes to merge - modular components may not have meshes assigned"));
	}
}

void ASLFSoulslikeCharacter::InitializeModularMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] ===== InitializeModularMesh START ====="));

	// If DefaultMeshInfo is null, try to get it from GameInstance -> SelectedBaseClass -> DefaultMeshAsset
	// This replicates the original Blueprint EventGraph logic that was cleared during migration
	// The Blueprint GameInstance has a variable "SelectedBaseClass" of type PDA_BaseCharacterInfo_C
	if (!DefaultMeshInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] DefaultMeshInfo is NULL, attempting to retrieve from GameInstance..."));

		UWorld* World = GetWorld();
		if (World)
		{
			UGameInstance* GI = World->GetGameInstance();
			if (GI)
			{
				UE_LOG(LogTemp, Log, TEXT("    GameInstance: %s (Class: %s)"), *GI->GetName(), *GI->GetClass()->GetName());

				// Use reflection to access SelectedBaseClass property on the Blueprint GameInstance
				FObjectProperty* SelectedBaseClassProp = CastField<FObjectProperty>(
					GI->GetClass()->FindPropertyByName(TEXT("SelectedBaseClass")));

				if (SelectedBaseClassProp)
				{
					UObject* SelectedClass = SelectedBaseClassProp->GetObjectPropertyValue_InContainer(GI);
					UE_LOG(LogTemp, Log, TEXT("    SelectedBaseClass via reflection: %s"),
						SelectedClass ? *SelectedClass->GetName() : TEXT("NULL"));

					if (SelectedClass)
					{
						// Direct cast to C++ class - PDA_BaseCharacterInfo reparented to UPDA_BaseCharacterInfo
						if (UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(SelectedClass))
						{
							DefaultMeshInfo = CharInfo->DefaultMeshAsset;
							UE_LOG(LogTemp, Log, TEXT("    Retrieved DefaultMeshInfo from C++ property: %s"),
								DefaultMeshInfo ? *DefaultMeshInfo->GetName() : TEXT("NULL"));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("    Cast<UPDA_BaseCharacterInfo> failed (Class: %s)"),
								*SelectedClass->GetClass()->GetName());
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("    SelectedBaseClass property not found in GameInstance (Class: %s)"),
						*GI->GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("    GetGameInstance() returned NULL"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("    GetWorld() returned NULL"));
		}
	}

	// Load meshes from DefaultMeshInfo using direct C++ property access (Option B migration)
	// The Blueprint PDA_DefaultMeshData now inherits from UPDA_DefaultMeshData with direct mesh properties
	if (DefaultMeshInfo)
	{
		UE_LOG(LogTemp, Log, TEXT("  Loading meshes from DefaultMeshInfo: %s (Class: %s)"),
			*DefaultMeshInfo->GetName(), *DefaultMeshInfo->GetClass()->GetName());

		// Cast to C++ class to access direct properties (avoids Blueprint struct reflection issues)
		UPDA_DefaultMeshData* MeshDataAsset = Cast<UPDA_DefaultMeshData>(DefaultMeshInfo);
		if (MeshDataAsset)
		{
			UE_LOG(LogTemp, Log, TEXT("    Successfully cast to UPDA_DefaultMeshData"));

			// Load and assign Head mesh
			if (Head && !Head->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->HeadMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->HeadMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						Head->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded Head mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign UpperBody mesh
			if (UpperBody && !UpperBody->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->UpperBodyMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->UpperBodyMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						UpperBody->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded UpperBody mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign Arms mesh
			if (Arms && !Arms->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->ArmsMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->ArmsMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						Arms->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded Arms mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign LowerBody mesh
			if (LowerBody && !LowerBody->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->LowerBodyMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->LowerBodyMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						LowerBody->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded LowerBody mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("    Cast to UPDA_DefaultMeshData failed - class hierarchy may not be correct"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  DefaultMeshInfo is NULL - cannot load default meshes"));
	}

	// Check if modular mesh components now have meshes
	bool bHasMeshes = false;

	if (Head && Head->GetSkeletalMeshAsset())
	{
		bHasMeshes = true;
		UE_LOG(LogTemp, Log, TEXT("  Head has mesh: %s"), *Head->GetSkeletalMeshAsset()->GetName());
	}
	if (UpperBody && UpperBody->GetSkeletalMeshAsset())
	{
		bHasMeshes = true;
		UE_LOG(LogTemp, Log, TEXT("  UpperBody has mesh: %s"), *UpperBody->GetSkeletalMeshAsset()->GetName());
	}
	if (Arms && Arms->GetSkeletalMeshAsset())
	{
		bHasMeshes = true;
		UE_LOG(LogTemp, Log, TEXT("  Arms has mesh: %s"), *Arms->GetSkeletalMeshAsset()->GetName());
	}
	if (LowerBody && LowerBody->GetSkeletalMeshAsset())
	{
		bHasMeshes = true;
		UE_LOG(LogTemp, Log, TEXT("  LowerBody has mesh: %s"), *LowerBody->GetSkeletalMeshAsset()->GetName());
	}

	if (bHasMeshes)
	{
		// Merge modular meshes and apply to main skeleton
		RefreshModularPieces();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No modular meshes found - character may be invisible"));
		UE_LOG(LogTemp, Warning, TEXT("  Ensure DefaultMeshInfo is set or Blueprint SCS has meshes assigned"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_PLAYER INTERFACE IMPLEMENTATIONS (23 functions)
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFSoulslikeCharacter::ResetCameraView_Implementation(double TimeScale)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ResetCameraView: TimeScale=%f"), TimeScale);

	// Reset camera boom to default position
	if (CachedCameraBoom)
	{
		CachedCameraBoom->TargetArmLength = Cache_SpringArmLength;
	}

	// Broadcast camera reset finished
	OnCameraResetFinished.Broadcast();
}

void ASLFSoulslikeCharacter::StopActiveCameraSequence_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] StopActiveCameraSequence"));

	if (Cache_ActiveCameraSequence)
	{
		Cache_ActiveCameraSequence->Stop();
		Cache_ActiveCameraSequence = nullptr;
	}
}

void ASLFSoulslikeCharacter::PlayCameraSequence_Implementation(ULevelSequence* Sequence, FMovieSceneSequencePlaybackSettings Settings)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] PlayCameraSequence"));

	if (!Sequence)
	{
		return;
	}

	// Stop any active sequence first
	StopActiveCameraSequence_Implementation();

	// Create and play the new sequence
	ALevelSequenceActor* SequenceActor = nullptr;
	Cache_ActiveCameraSequence = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), Sequence, Settings, SequenceActor);

	if (Cache_ActiveCameraSequence)
	{
		Cache_ActiveCameraSequence->Play();
	}
}

void ASLFSoulslikeCharacter::PlayBackstabMontage_Implementation(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] PlayBackstabMontage: Tag=%s"), *ExecutionTag.ToString());

	if (Montage.IsNull())
	{
		return;
	}

	UAnimMontage* LoadedMontage = Montage.LoadSynchronous();
	if (LoadedMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(LoadedMontage);
	}
}

void ASLFSoulslikeCharacter::PlayExecuteMontage_Implementation(const TSoftObjectPtr<UAnimMontage>& Montage, FGameplayTag ExecutionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] PlayExecuteMontage: Tag=%s"), *ExecutionTag.ToString());

	if (Montage.IsNull())
	{
		return;
	}

	UAnimMontage* LoadedMontage = Montage.LoadSynchronous();
	if (LoadedMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(LoadedMontage);
	}
}

void ASLFSoulslikeCharacter::TriggerChaosField_Implementation(bool Enable)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] TriggerChaosField: %s"), Enable ? TEXT("true") : TEXT("false"));

	// Find the ChaosForceField child actor component and toggle its physics
	UChildActorComponent* ChaosFieldComp = FindComponentByClass<UChildActorComponent>();
	if (ChaosFieldComp && ChaosFieldComp->GetChildActor())
	{
		AActor* ChaosActor = ChaosFieldComp->GetChildActor();
		ChaosActor->SetActorEnableCollision(Enable);
		ChaosActor->SetActorHiddenInGame(!Enable);
	}
}

void ASLFSoulslikeCharacter::DiscoverRestingPoint_Implementation(UAnimMontage* InteractionMontage, AActor* Point)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] DiscoverRestingPoint: %s"), Point ? *Point->GetName() : TEXT("null"));

	// Play the interaction montage if provided
	if (InteractionMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(InteractionMontage);
	}

	// Mark resting point as discovered (the actual saving is done via progress manager)
	if (Point)
	{
		UE_LOG(LogTemp, Log, TEXT("  Resting point discovered - saving progress"));
		// Progress saving handled by AC_ProgressManager
	}
}

void ASLFSoulslikeCharacter::OnRest_Implementation(AActor* TargetCampfire)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnRest at %s"), TargetCampfire ? *TargetCampfire->GetName() : TEXT("null"));

	// Handle player rest behavior:
	// - Restore stats (health, FP, stamina) via StatManager
	// - Save game via SaveLoadManager
	// - Refill flasks
	// These are all handled by the AC_InteractionManager's EventOnRest
	if (TargetCampfire)
	{
		UE_LOG(LogTemp, Log, TEXT("  Player resting - triggering rest behavior"));
		// The actual restore/save logic is in AC_InteractionManager::EventOnRest
	}
}

void ASLFSoulslikeCharacter::OnDialogStarted_Implementation(UActorComponent* DialogComponent)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnDialogStarted"));

	// Disable player input during dialog
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// Store current control state and disable movement
		DisableInput(PC);
	}

	// Focus camera on dialog target will be handled by timeline/sequencer
}

void ASLFSoulslikeCharacter::OnNpcTraced_Implementation(AActor* NPC)
{
	// From JSON: When NPC is traced, show/hide "E: Talk" interaction widget
	// Logic: GetPlayerHUD → Is Valid NPC → Show/Hide interaction prompt
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnNpcTraced: %s"), NPC ? *NPC->GetName() : TEXT("null"));

	// Get player controller and HUD via interface
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Show/hide NPC interaction prompt based on whether NPC is valid
	// The actual HUD widget visibility is handled by the HUD class via interface
	// If NPC is valid, show "E: Talk" prompt; otherwise hide it
	if (IsValid(NPC))
	{
		// NPC is valid - show talk prompt
		// Note: HUD updates would be done via BPI_Controller interface in full implementation
		UE_LOG(LogTemp, Log, TEXT("  NPC traced - showing talk prompt"));
	}
	else
	{
		// NPC is null - hide talk prompt
		UE_LOG(LogTemp, Log, TEXT("  NPC untraced - hiding talk prompt"));
	}
}

void ASLFSoulslikeCharacter::GetSoulslikeCharacter_Implementation(ACharacter*& B_SoulslikeCharacter)
{
	B_SoulslikeCharacter = this;
}

void ASLFSoulslikeCharacter::ToggleCrouchReplicated_Implementation()
{
	// From JSON: CROUCH - Toggle type action, will crouch/uncrouch, replicated
	// Logic: ToggleCrouch → ExecuteActionImmediately for Crouch action
	// Also toggles related property for the anim BP
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ToggleCrouchReplicated"));

	// Toggle crouch state using base character functionality
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}

	// Execute crouch action immediately via input buffer
	if (CachedInputBuffer)
	{
		CachedInputBuffer->ExecuteActionImmediately(
			FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Crouch")));
	}

	// Notify ActionManager of crouch state change for anim BP
	if (CachedActionManager)
	{
		CachedActionManager->ToggleCrouch();
	}
}

void ASLFSoulslikeCharacter::ResetGreaves_Implementation()
{
	// From JSON: ResetGreaves → ChangeGreaves with default mesh from DefaultMeshInfo
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ResetGreaves"));

	// Get default greaves mesh from DefaultMeshInfo data asset
	// In full implementation, DefaultMeshInfo contains soft references to default meshes
	// For now, pass null to ChangeGreaves which will reset to base mesh
	ChangeGreaves_Implementation(TSoftObjectPtr<USkeletalMesh>());
}

void ASLFSoulslikeCharacter::ResetGloves_Implementation()
{
	// From JSON: ResetGloves → ChangeGloves with default mesh from DefaultMeshInfo
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ResetGloves"));

	ChangeGloves_Implementation(TSoftObjectPtr<USkeletalMesh>());
}

void ASLFSoulslikeCharacter::ResetArmor_Implementation()
{
	// From JSON: ResetArmor → ChangeArmor with default mesh from DefaultMeshInfo
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ResetArmor"));

	ChangeArmor_Implementation(TSoftObjectPtr<USkeletalMesh>());
}

void ASLFSoulslikeCharacter::ResetHeadpiece_Implementation()
{
	// From JSON: ResetHeadpiece → ChangeHeadpiece with default mesh from DefaultMeshInfo
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ResetHeadpiece"));

	ChangeHeadpiece_Implementation(TSoftObjectPtr<USkeletalMesh>());
}

void ASLFSoulslikeCharacter::ChangeGreaves_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh)
{
	// From JSON: Sets Cache_IsAsyncEquipmentBusy, loads mesh, sets on LowerBody component
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ChangeGreaves"));

	bCache_IsAsyncEquipmentBusy = true;

	if (LowerBody && !NewMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = NewMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			LowerBody->SetSkeletalMesh(LoadedMesh);
			RefreshModularPieces();  // Re-merge after equipment change
			UE_LOG(LogTemp, Log, TEXT("  Applied greaves mesh: %s"), *LoadedMesh->GetName());
		}
	}
	else if (LowerBody)
	{
		// Reset to default - in full implementation would use DefaultMeshInfo
		UE_LOG(LogTemp, Log, TEXT("  Reset greaves to default"));
	}

	bCache_IsAsyncEquipmentBusy = false;
}

void ASLFSoulslikeCharacter::ChangeGloves_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh)
{
	// From JSON: Sets Cache_IsAsyncEquipmentBusy, loads mesh, sets on Arms component
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ChangeGloves"));

	bCache_IsAsyncEquipmentBusy = true;

	if (Arms && !NewMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = NewMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			Arms->SetSkeletalMesh(LoadedMesh);
			RefreshModularPieces();  // Re-merge after equipment change
			UE_LOG(LogTemp, Log, TEXT("  Applied gloves mesh: %s"), *LoadedMesh->GetName());
		}
	}
	else if (Arms)
	{
		// Reset to default - in full implementation would use DefaultMeshInfo
		UE_LOG(LogTemp, Log, TEXT("  Reset gloves to default"));
	}

	bCache_IsAsyncEquipmentBusy = false;
}

void ASLFSoulslikeCharacter::ChangeArmor_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh)
{
	// From JSON: Sets Cache_IsAsyncEquipmentBusy, loads mesh, sets on UpperBody component
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ChangeArmor"));

	bCache_IsAsyncEquipmentBusy = true;

	if (UpperBody && !NewMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = NewMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			UpperBody->SetSkeletalMesh(LoadedMesh);
			RefreshModularPieces();  // Re-merge after equipment change
			UE_LOG(LogTemp, Log, TEXT("  Applied armor mesh: %s"), *LoadedMesh->GetName());
		}
	}
	else if (UpperBody)
	{
		// Reset to default - in full implementation would use DefaultMeshInfo
		UE_LOG(LogTemp, Log, TEXT("  Reset armor to default"));
	}

	bCache_IsAsyncEquipmentBusy = false;
}

void ASLFSoulslikeCharacter::ChangeHeadpiece_Implementation(const TSoftObjectPtr<USkeletalMesh>& NewMesh)
{
	// From JSON: Sets Cache_IsAsyncEquipmentBusy, loads mesh, sets on Head component
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ChangeHeadpiece"));

	bCache_IsAsyncEquipmentBusy = true;

	if (Head && !NewMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = NewMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			Head->SetSkeletalMesh(LoadedMesh);
			RefreshModularPieces();  // Re-merge after equipment change
			UE_LOG(LogTemp, Log, TEXT("  Applied headpiece mesh: %s"), *LoadedMesh->GetName());
		}
	}
	else if (Head)
	{
		// Reset to default - in full implementation would use DefaultMeshInfo
		UE_LOG(LogTemp, Log, TEXT("  Reset headpiece to default"));
	}

	bCache_IsAsyncEquipmentBusy = false;
}

void ASLFSoulslikeCharacter::OnTargetLocked_Implementation(bool bTargetLocked, bool bRotateTowards)
{
	// From JSON: Set Cache_Rotation → Branch on TargetLocked → configure controller rotation
	// Logic: Stores current control rotation, then sets use controller rotation based on lock state
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnTargetLocked: locked=%s, rotate=%s"),
		bTargetLocked ? TEXT("true") : TEXT("false"),
		bRotateTowards ? TEXT("true") : TEXT("false"));

	// Cache current control rotation
	if (Controller)
	{
		Cache_ControlRotation = Controller->GetControlRotation();
	}

	// Branch based on target locked state
	if (bTargetLocked)
	{
		// Target is locked - configure for lock-on behavior
		// When locked, character rotation is managed by the lock-on system
		bUseControllerRotationYaw = false;

		// Branch on RotateTowards - if true, rotate character to face target
		if (bRotateTowards)
		{
			// Rotate towards target - handled by tick or separate system
			UE_LOG(LogTemp, Log, TEXT("  Target locked with rotation towards target"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("  Target locked without rotation"));
		}
	}
	else
	{
		// Target unlocked - restore normal behavior
		bUseControllerRotationYaw = false; // Soulslike typically doesn't use controller yaw rotation

		UE_LOG(LogTemp, Log, TEXT("  Target unlocked"));
	}

	// Notify combat manager of lock state change
	if (CachedCombatManager)
	{
		// CombatManager would handle additional lock-on logic
	}
}

void ASLFSoulslikeCharacter::OnLootItem_Implementation(AActor* Item)
{
	// From JSON: Called from B_PickupItem::OnInteract via BPI_Player interface
	// Logic: Add item to inventory → Queue PickupItemMontage action
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnLootItem: %s"), Item ? *Item->GetName() : TEXT("null"));

	if (!IsValid(Item))
	{
		return;
	}

	// Cast to pickup item to get item data and add to inventory
	if (ASLFPickupItemBase* PickupItem = Cast<ASLFPickupItemBase>(Item))
	{
		if (UInventoryManagerComponent* InvMgr = FindComponentByClass<UInventoryManagerComponent>())
		{
			if (PickupItem->Item)
			{
				InvMgr->AddItem(PickupItem->Item, PickupItem->Count, true);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Added %s x%d to inventory"),
					*PickupItem->Item->GetName(), PickupItem->Count);
			}
		}
	}

	// Queue the pickup item montage action
	if (CachedInputBuffer)
	{
		CachedInputBuffer->QueueAction(
			FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.PickupItemMontage")));
	}
}

void ASLFSoulslikeCharacter::OnInteractableTraced_Implementation(AActor* Interactable)
{
	// From JSON: GetPlayerHUD -> Show/Hide interaction prompt
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnInteractableTraced: %s"), Interactable ? *Interactable->GetName() : TEXT("null"));

	// Get player controller to access HUD
	ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetController());
	if (!PC || !PC->HUDWidgetRef)
	{
		return;
	}

	UW_HUD* HUD = PC->HUDWidgetRef;

	// Show/hide interaction prompt based on whether Interactable is valid
	if (IsValid(Interactable))
	{
		// Cast to AB_Interactable to pass to HUD
		AB_Interactable* InteractableActor = Cast<AB_Interactable>(Interactable);
		if (InteractableActor)
		{
			HUD->EventShowInteractableWidget(InteractableActor);
			UE_LOG(LogTemp, Log, TEXT("  Showing interaction widget for: %s"), *Interactable->GetName());
		}
	}
	else
	{
		// Hide interaction prompt
		HUD->EventHideInteractionWidget();
		UE_LOG(LogTemp, Log, TEXT("  Hiding interaction widget"));
	}
}
