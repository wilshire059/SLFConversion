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
#include "EngineUtils.h"
#include "Components/ChildActorComponent.h"
#include "Components/InputBufferComponent.h"
#include "Components/AC_ActionManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_InteractionManager.h"
#include "Components/StatManagerComponent.h"  // For CachedStatManager->IsStatMoreThan()
#include "Components/AC_EquipmentManager.h"
#include "Components/ProgressManagerComponent.h"  // For dialog GameplayEvents
#include "Interfaces/SLFInteractableInterface.h"
#include "Interfaces/SLFDestructibleHelperInterface.h"  // For EnableChaosDestroy/DisableChaosDestroy
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "UObject/ConstructorHelpers.h"
#include "SLFGameTypes.h"  // For FSLFSkeletalMeshData reflection access
#include "SLFPrimaryDataAssets.h"
#include "SLFPickupItemBase.h"
#include "B_PickupItem.h"
#include "Components/InventoryManagerComponent.h"
#include "Framework/SLFPlayerController.h"
#include "GameFramework/PC_SoulslikeFramework.h"
#include "Widgets/W_HUD.h"
#include "B_Interactable.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Interfaces/BPI_Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "BFL_Helper.h"

ASLFSoulslikeCharacter::ASLFSoulslikeCharacter()
{
	// ═══════════════════════════════════════════════════════════════════
	// LOAD INPUT ASSETS - Enhanced Input configuration
	// ═══════════════════════════════════════════════════════════════════

	// Load Input Mapping Contexts
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"));
	if (MappingContextFinder.Succeeded())
	{
		PlayerMappingContext = MappingContextFinder.Object;
	}

	// Load Dialog Input Mapping Context (used during NPC dialog)
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DialogMappingContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_Dialog.IMC_Dialog"));
	if (DialogMappingContextFinder.Succeeded())
	{
		DialogMappingContext = DialogMappingContextFinder.Object;
	}

	// Load Navigable Menu Input Mapping Context (contains IA_NavigableMenu_Ok key mappings)
	// This enables E key / SpaceBar to trigger menu navigation during dialog
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> NavigableMenuMappingContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_NavigableMenu.IMC_NavigableMenu"));
	if (NavigableMenuMappingContextFinder.Succeeded())
	{
		NavigableMenuMappingContext = NavigableMenuMappingContextFinder.Object;
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

	// Load Scroll Wheel Input Actions (for weapon/item wheel cycling)
	static ConstructorHelpers::FObjectFinder<UInputAction> ScrollRightHandActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Scroll_RightHand.IA_Scroll_RightHand"));
	if (ScrollRightHandActionFinder.Succeeded())
	{
		IA_Scroll_RightHand = ScrollRightHandActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ScrollLeftHandActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Scroll_LeftHand.IA_Scroll_LeftHand"));
	if (ScrollLeftHandActionFinder.Succeeded())
	{
		IA_Scroll_LeftHand = ScrollLeftHandActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ScrollToolsActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Scroll_Tools.IA_Scroll_Tools"));
	if (ScrollToolsActionFinder.Succeeded())
	{
		IA_Scroll_Tools = ScrollToolsActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> UseEquippedItemActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_UseEquippedItem.IA_UseEquippedItem"));
	if (UseEquippedItemActionFinder.Succeeded())
	{
		IA_UseEquippedItem = UseEquippedItemActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> WeaponSkillActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_WeaponSkill.IA_WeaponSkill"));
	if (WeaponSkillActionFinder.Succeeded())
	{
		IA_WeaponSkill = WeaponSkillActionFinder.Object;
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
	CachedNpcInteractionManager = nullptr;  // Set during OnDialogStarted
	// NOTE: CachedStatManager is inherited from ASLFBaseCharacter and initialized there
	SprintDodgeStartTime = 0.0;

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

	// Collision diagnostics removed — terrain collision confirmed working

	// Add "Player" tag - required for B_StatusEffectArea overlap detection
	// Blueprint checks ActorHasTag("Player") before applying status effects
	Tags.AddUnique(FName("Player"));
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Added 'Player' tag"));

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

	// Update AnimBP overlay states from EquipmentManager
	// This syncs C++ overlay values to Blueprint enum variables for weapon animations
	UpdateAnimInstanceOverlayStates();

	// Handle continuous target lock rotation
	// This rotates the character towards the locked target each frame
	TickTargetLockRotation(DeltaTime);
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
	// NOTE: CachedStatManager is already cached in ASLFBaseCharacter::BeginPlay()

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
	// NOTE: IA_Sprint and IA_Dodge point to the SAME input action asset (IA_Sprint_Dodge)
	// Sprint/Dodge behavior is determined by input timing:
	// - TAP (<=0.2s): Dodge
	// - HOLD (>0.2s): Sprint
	// HandleSprintCompleted checks elapsed time and calls HandleDodge for taps
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleSprintStarted);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASLFSoulslikeCharacter::HandleSprintCompleted);
	}
	// DO NOT bind IA_Dodge separately - it's the same asset as IA_Sprint!
	// Dodge is triggered via HandleSprintCompleted when tap duration <= 0.2s
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

	// Scroll Wheel Input Bindings (for weapon/item wheel cycling)
	if (IA_Scroll_RightHand)
	{
		EnhancedInput->BindAction(IA_Scroll_RightHand, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleScrollRightHand);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] IA_Scroll_RightHand BOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_Scroll_RightHand is NULL!"));
	}

	if (IA_Scroll_LeftHand)
	{
		EnhancedInput->BindAction(IA_Scroll_LeftHand, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleScrollLeftHand);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] IA_Scroll_LeftHand BOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_Scroll_LeftHand is NULL!"));
	}

	if (IA_Scroll_Tools)
	{
		EnhancedInput->BindAction(IA_Scroll_Tools, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleScrollTools);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] IA_Scroll_Tools BOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_Scroll_Tools is NULL!"));
	}

	if (IA_UseEquippedItem)
	{
		EnhancedInput->BindAction(IA_UseEquippedItem, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleUseEquippedItem);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] IA_UseEquippedItem BOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_UseEquippedItem is NULL!"));
	}

	// Weapon Skill / Special Attack (L2 on gamepad with different modifier)
	if (IA_WeaponSkill)
	{
		EnhancedInput->BindAction(IA_WeaponSkill, ETriggerEvent::Started, this, &ASLFSoulslikeCharacter::HandleWeaponSkill);
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] IA_WeaponSkill BOUND (Special Attack)"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SoulslikeCharacter] IA_WeaponSkill is NULL! Special attacks will not work."));
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
	// From JSON: Jump requires stamina check (RequiredStatAmount: 5.0)
	// Original Blueprint: IsStatMoreThan(Stamina, 5.0) before QueueAction
	static const FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
	static const double JumpStaminaRequired = 5.0;

	if (CachedStatManager)
	{
		if (CachedStatManager->IsStatMoreThan(StaminaTag, JumpStaminaRequired))
		{
			QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Jump")));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Jump blocked - insufficient stamina (need %.1f)"), JumpStaminaRequired);
		}
	}
	else
	{
		// No stat manager - allow jump anyway (fallback for testing)
		QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Jump")));
	}
}

void ASLFSoulslikeCharacter::HandleSprintStarted()
{
	// From JSON: IA_Sprint_Dodge handles both sprint AND dodge
	// Record start time for tap vs hold detection
	SprintDodgeStartTime = GetWorld()->GetTimeSeconds();

	// Start sprinting (will be stopped if this turns out to be a dodge tap)
	bCache_IsHoldingSprint = true;
	ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StartSprinting")));
}

void ASLFSoulslikeCharacter::HandleSprintCompleted()
{
	// From JSON: IA_Sprint_Dodge Completed - check elapsed time for tap vs hold
	// Original Blueprint: ElapsedSeconds <= 0.2 means dodge TAP, otherwise sprint RELEASE
	bCache_IsHoldingSprint = false;

	double ElapsedSeconds = GetWorld()->GetTimeSeconds() - SprintDodgeStartTime;

	if (ElapsedSeconds <= 0.2)
	{
		// TAP - This is a dodge, not sprint
		// First stop the sprint that was started
		ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StopSprinting")));

		// Then try to dodge (with stamina check)
		HandleDodge();
	}
	else
	{
		// HOLD release - stop sprinting normally
		ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StopSprinting")));
	}
}

void ASLFSoulslikeCharacter::HandleDodge()
{
	// From JSON: Dodge requires stamina check (RequiredStatAmount: 5.0)
	// Original Blueprint: IsStatMoreThan(Stamina, 5.0) before QueueAction
	// Also checks IsCrouched - if crouched, do crouch action instead
	static const FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
	static const double DodgeStaminaRequired = 5.0;

	// Check if crouched - from Blueprint, crouched characters do different action
	if (bIsCrouched)
	{
		// Original Blueprint: If crouched, do crouch action (crouch-dodge/roll)
		QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Crouch")));
		return;
	}

	if (CachedStatManager)
	{
		if (CachedStatManager->IsStatMoreThan(StaminaTag, DodgeStaminaRequired))
		{
			QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Dodge")));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Dodge blocked - insufficient stamina (need %.1f)"), DodgeStaminaRequired);
			// Original Blueprint: ExecuteActionImmediately for out-of-stamina feedback
			// This could play a "can't do" animation or sound
		}
	}
	else
	{
		// No stat manager - allow dodge anyway (fallback for testing)
		QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Dodge")));
	}
}

void ASLFSoulslikeCharacter::HandleAttack()
{
	// From Blueprint: If holding interact key, trigger two-hand stance instead of attack
	if (bCache_IsHoldingInteract)
	{
		// Two-hand stance right hand (like Elden Ring - hold Y + RT)
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Interact held + Attack -> TwoHandStanceRight"));
		ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.TwoHandStanceRight")));
		return;
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// CHECK FOR EXECUTION TARGET (poise-broken enemy)
	// bp_only: B_Soulslike_Character checks if target is poise-broken and chooses:
	//   - Backstab (if behind enemy)
	//   - Frontal Execution (if in front of enemy)
	//   - Normal attack (if no poise-broken target)
	// ═══════════════════════════════════════════════════════════════════════════════
	if (CachedCombatManager && CachedCombatManager->ExecutionTarget)
	{
		AActor* Target = CachedCombatManager->ExecutionTarget;

		// Check if player is behind target (backstab) or in front (frontal execution)
		// bp_only uses 0.9 tolerance (very precise - only directly behind counts as backstab)
		// 0.9 means dot product must be < -0.9, which is a ~25 degree cone directly behind
		bool bIsBehind = UBFL_Helper::GetIsBehindTarget(this, Target, 0.9, GetWorld());

		if (bIsBehind)
		{
			// BACKSTAB - player is behind the poise-broken enemy
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] *** BACKSTAB *** Target: %s"), *Target->GetName());
			ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Backstab")));
		}
		else
		{
			// FRONTAL EXECUTION - player is in front of poise-broken enemy
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] *** FRONTAL EXECUTION *** Target: %s"), *Target->GetName());
			ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Execute")));
		}
		return;
	}

	// Normal attack (no execution target)
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.LightAttackRight")));
}

void ASLFSoulslikeCharacter::HandleGuardStarted()
{
	// From Blueprint: If holding interact key, trigger two-hand stance instead of guard
	if (bCache_IsHoldingInteract)
	{
		// Two-hand stance left hand (like Elden Ring - hold Y + LT)
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Interact held + Guard -> TwoHandStanceLeft"));
		ExecuteActionImmediately(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.TwoHandStanceLeft")));
	}
	else
	{
		// Normal guard
		QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardStart")));
	}
}

void ASLFSoulslikeCharacter::HandleGuardCompleted()
{
	// From JSON: Queues SoulslikeFramework.Action.GuardEnd
	QueueActionToBuffer(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardEnd")));
}

void ASLFSoulslikeCharacter::HandleInteractStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleInteractStarted CALLED"));

	// From JSON: MAIN INTERACTION INPUT EVENT section
	bCache_IsHoldingInteract = true;
	Cache_InteractElapsed = 0.0;

	// ═══════════════════════════════════════════════════════════════════════════
	// DIALOG ADVANCEMENT - Check if dialog is active first
	// If user presses E while dialog is showing, advance the dialog instead of
	// normal interaction behavior. This allows using the same key (E) for both.
	// ═══════════════════════════════════════════════════════════════════════════
	APlayerController* PC = Cast<APlayerController>(GetController());
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleInteractStarted - PC: %s, Implements BPI_Controller: %s"),
		PC ? TEXT("OK") : TEXT("NULL"),
		(PC && PC->Implements<UBPI_Controller>()) ? TEXT("YES") : TEXT("NO"));

	if (PC && PC->Implements<UBPI_Controller>())
	{
		UUserWidget* HUDWidget = nullptr;
		IBPI_Controller::Execute_GetPlayerHUD(PC, HUDWidget);

		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleInteractStarted - HUDWidget: %s, Cast to UW_HUD: %s"),
			HUDWidget ? TEXT("OK") : TEXT("NULL"),
			Cast<UW_HUD>(HUDWidget) ? TEXT("OK") : TEXT("FAIL"));

		if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleInteractStarted - IsDialogActive: %s"),
				HUD->IsDialogActive ? TEXT("TRUE") : TEXT("FALSE"));

			if (HUD->IsDialogActive)
			{
				// Dialog is active - advance it instead of interacting
				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Dialog active - advancing dialog via OnDialogWindowClosed"));
				HUD->OnDialogWindowClosed.Broadcast();
				return;
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// NORMAL INTERACTION
	// InteractionManager tracks NearestInteractable in TickComponent
	// When interact is pressed, we call the interactable's interface method
	// ═══════════════════════════════════════════════════════════════════════════
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

void ASLFSoulslikeCharacter::HandleScrollRightHand()
{
	// Scroll right hand weapon wheel (cycles through equipped right hand weapons)
	// bp_only: IA_Scroll_RightHand triggers ActionManager.EventPerformAction(ScrollWheel.Right)
	// ActionManager broadcasts OnScrollWheel, HUD's W_ItemWheelSlot listens and calls EventScrollWheel
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleScrollRightHand CALLED"));

	static const FGameplayTag ScrollRightTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.ScrollWheel.Right"));
	ExecuteActionImmediately(ScrollRightTag);
}

void ASLFSoulslikeCharacter::HandleScrollLeftHand()
{
	// Scroll left hand weapon wheel (cycles through equipped left hand items/shields)
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleScrollLeftHand CALLED"));

	static const FGameplayTag ScrollLeftTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.ScrollWheel.Left"));
	ExecuteActionImmediately(ScrollLeftTag);
}

void ASLFSoulslikeCharacter::HandleScrollTools()
{
	// Scroll tools wheel (cycles through consumables like health flasks)
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleScrollTools CALLED"));

	static const FGameplayTag ScrollToolsTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.ScrollWheel.Bottom"));
	ExecuteActionImmediately(ScrollToolsTag);
}

void ASLFSoulslikeCharacter::HandleUseEquippedItem()
{
	// Use equipped tool/consumable (e.g., health flask)
	// From bp_only JSON: First cancel guard, then queue use equipped tool action
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleUseEquippedItem CALLED"));

	// 1. Execute GuardCancel immediately (stops guarding if guarding)
	static const FGameplayTag GuardCancelTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardCancel"));
	ExecuteActionImmediately(GuardCancelTag);

	// 2. Queue UseEquippedTool action (uses the flask)
	static const FGameplayTag UseToolTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.UseEquippedTool"));
	QueueActionToBuffer(UseToolTag);
}

void ASLFSoulslikeCharacter::HandleWeaponSkill()
{
	// SPECIAL ATTACK - Weapon ability (L2 without guard, triggered by IA_WeaponSkill)
	// From bp_only JSON: "SPECIAL ATTACK - Weapon ability"
	// Triggers SoulslikeFramework.Action.SpecialAttack immediately
	// The action system then finds the weapon ability data and consumes FP
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] HandleWeaponSkill CALLED - Special Attack"));

	static const FGameplayTag SpecialAttackTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.SpecialAttack"));
	ExecuteActionImmediately(SpecialAttackTag);
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

void ASLFSoulslikeCharacter::EventOnDialogExit()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] EventOnDialogExit"));

	// Advance dialog or finish dialog sequence
	if (CachedNpcInteractionManager)
	{
		CachedNpcInteractionManager->AdjustIndexForExit();
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Called AdjustIndexForExit on CachedNpcInteractionManager"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] EventOnDialogExit - CachedNpcInteractionManager is null!"));
	}
}

void ASLFSoulslikeCharacter::OnDialogFinished()
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnDialogFinished - Cleaning up dialog bindings"));

	// Check if this NPC has a vendor asset - if so, the NPC menu handles input, not us
	bool bIsVendorNpc = CachedNpcInteractionManager && CachedNpcInteractionManager->VendorAsset != nullptr;

	APlayerController* PC = Cast<APlayerController>(GetController());

	// Only switch input context if NOT a vendor NPC
	// Vendor NPCs have their menu open, which manages its own input context
	if (!bIsVendorNpc)
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Non-vendor NPC - switching to gameplay input"));

		if (PC && PC->Implements<UBPI_Controller>())
		{
			TArray<UInputMappingContext*> ContextsToEnable;
			TArray<UInputMappingContext*> ContextsToDisable;

			if (PlayerMappingContext)
			{
				ContextsToEnable.Add(PlayerMappingContext);
			}
			if (DialogMappingContext)
			{
				ContextsToDisable.Add(DialogMappingContext);
			}
			// Remove NavigableMenu context that was added for gamepad dialog progression
			if (NavigableMenuMappingContext)
			{
				ContextsToDisable.Add(NavigableMenuMappingContext);
			}

			IBPI_Controller::Execute_SwitchInputContext(PC, ContextsToEnable, ContextsToDisable);
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Switched to gameplay input context"));
		}

		// Toggle UI mode off and finish dialog only for non-vendor NPCs
		if (PC && PC->Implements<UBPI_Controller>())
		{
			UUserWidget* HUDWidget = nullptr;
			IBPI_Controller::Execute_GetPlayerHUD(PC, HUDWidget);

			if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
			{
				// Unbind OnDialogWindowClosed
				HUD->OnDialogWindowClosed.RemoveDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Unbound OnDialogWindowClosed"));

				// Toggle UI mode off
				HUD->EventToggleUiMode(false);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Toggled UI mode off"));

				// Finish dialog on HUD (hides dialog widget)
				HUD->EventFinishDialog();
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Called EventFinishDialog on HUD"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Vendor NPC - menu is open, skipping input context switch"));

		// For vendor NPCs, just unbind OnDialogWindowClosed but DON'T:
		// - Switch input context (menu manages it)
		// - Toggle UI mode off (menu is still open)
		// - Call EventFinishDialog (would clear ActiveWidgetTag)
		if (PC && PC->Implements<UBPI_Controller>())
		{
			UUserWidget* HUDWidget = nullptr;
			IBPI_Controller::Execute_GetPlayerHUD(PC, HUDWidget);

			if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
			{
				// Unbind OnDialogWindowClosed - we don't need it anymore
				HUD->OnDialogWindowClosed.RemoveDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Unbound OnDialogWindowClosed (vendor)"));
			}
		}
	}

	// Unbind OnDialogFinished from the NPC's interaction manager
	if (CachedNpcInteractionManager)
	{
		CachedNpcInteractionManager->OnDialogFinished.RemoveDynamic(this, &ASLFSoulslikeCharacter::OnDialogFinished);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Unbound OnDialogFinished"));
	}

	// Clear cached reference
	CachedNpcInteractionManager = nullptr;
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

	// FALLBACK: If still no DefaultMeshInfo, try to load the default PDA_DefaultMeshData asset directly
	// This provides a working character when GameInstance.SelectedBaseClass is not set
	if (!DefaultMeshInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Attempting FALLBACK: Loading default PDA_DefaultMeshData directly..."));

		// Try to load the default mesh data asset from the known path
		static const TCHAR* DefaultMeshDataPath = TEXT("/Game/SoulslikeFramework/Data/PDA_DefaultMeshData.PDA_DefaultMeshData");
		DefaultMeshInfo = Cast<UDataAsset>(StaticLoadObject(UDataAsset::StaticClass(), nullptr, DefaultMeshDataPath));

		if (DefaultMeshInfo)
		{
			UE_LOG(LogTemp, Warning, TEXT("    FALLBACK SUCCESS: Loaded DefaultMeshInfo from: %s"), DefaultMeshDataPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("    FALLBACK FAILED: Could not load asset from: %s"), DefaultMeshDataPath);
			UE_LOG(LogTemp, Error, TEXT("    Character will be INVISIBLE. Please ensure PDA_DefaultMeshData exists or set DefaultMeshInfo manually."));
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

	// ULTIMATE FALLBACK: If still no meshes, load Quinn body parts directly (Quinn is default)
	if (!bHasMeshes)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No meshes loaded - attempting ULTIMATE FALLBACK with hard-coded Quinn paths"));

		static const TCHAR* QuinnHeadPath = TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_Head.SKM_QuinnCape_Head");
		static const TCHAR* QuinnUpperBodyPath = TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_UpperBody.SKM_QuinnCape_UpperBody");
		static const TCHAR* QuinnArmsPath = TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_Arms.SKM_Quinn_Arms");
		static const TCHAR* QuinnLowerBodyPath = TEXT("/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_LowerBody.SKM_QuinnCape_LowerBody");

		if (Head && !Head->GetSkeletalMeshAsset())
		{
			USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, QuinnHeadPath));
			if (LoadedMesh)
			{
				Head->SetSkeletalMesh(LoadedMesh);
				bHasMeshes = true;
				UE_LOG(LogTemp, Warning, TEXT("    FALLBACK: Loaded Head mesh: %s"), *LoadedMesh->GetName());
			}
		}

		if (UpperBody && !UpperBody->GetSkeletalMeshAsset())
		{
			USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, QuinnUpperBodyPath));
			if (LoadedMesh)
			{
				UpperBody->SetSkeletalMesh(LoadedMesh);
				bHasMeshes = true;
				UE_LOG(LogTemp, Warning, TEXT("    FALLBACK: Loaded UpperBody mesh: %s"), *LoadedMesh->GetName());
			}
		}

		if (Arms && !Arms->GetSkeletalMeshAsset())
		{
			USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, QuinnArmsPath));
			if (LoadedMesh)
			{
				Arms->SetSkeletalMesh(LoadedMesh);
				bHasMeshes = true;
				UE_LOG(LogTemp, Warning, TEXT("    FALLBACK: Loaded Arms mesh: %s"), *LoadedMesh->GetName());
			}
		}

		if (LowerBody && !LowerBody->GetSkeletalMeshAsset())
		{
			USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, QuinnLowerBodyPath));
			if (LoadedMesh)
			{
				LowerBody->SetSkeletalMesh(LoadedMesh);
				bHasMeshes = true;
				UE_LOG(LogTemp, Warning, TEXT("    FALLBACK: Loaded LowerBody mesh: %s"), *LoadedMesh->GetName());
			}
		}
	}

	if (bHasMeshes)
	{
		// Merge modular meshes and apply to main skeleton
		RefreshModularPieces();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("  CRITICAL: No modular meshes found after all fallbacks - character WILL be invisible"));
		UE_LOG(LogTemp, Error, TEXT("  Check that SKM_Quinn* meshes exist at /Game/SoulslikeFramework/Meshes/SKM/"));
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
	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] PlayCameraSequence called with: %s"),
		Sequence ? *Sequence->GetName() : TEXT("NULL"));

	if (!Sequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] PlayCameraSequence - Sequence is NULL, aborting"));
		return;
	}

	// Stop any active sequence first
	StopActiveCameraSequence_Implementation();

	// Create and play the new sequence
	ALevelSequenceActor* SequenceActor = nullptr;
	Cache_ActiveCameraSequence = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), Sequence, Settings, SequenceActor);

	UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] CreateLevelSequencePlayer result: Player=%s, Actor=%s"),
		Cache_ActiveCameraSequence ? TEXT("Valid") : TEXT("NULL"),
		SequenceActor ? *SequenceActor->GetName() : TEXT("NULL"));

	// bp_only: Set TransformOriginActor to player so camera movements are relative to player position
	// This is CRITICAL - without this, the sequence uses absolute world coordinates
	if (SequenceActor)
	{
		// Enable instance data override
		SequenceActor->bOverrideInstanceData = true;

		// Get or create the instance data and set the transform origin to self (player)
		if (UDefaultLevelSequenceInstanceData* InstanceData = Cast<UDefaultLevelSequenceInstanceData>(SequenceActor->DefaultInstanceData))
		{
			InstanceData->TransformOriginActor = this;
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Set TransformOriginActor to player"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] DefaultInstanceData is not UDefaultLevelSequenceInstanceData"));
		}
	}

	if (Cache_ActiveCameraSequence)
	{
		Cache_ActiveCameraSequence->Play();
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Camera sequence PLAYING"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Failed to create LevelSequencePlayer!"));
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

	// Find the ChaosForceField child actor component by name (Blueprint has it named "ChaosForceField")
	// Must search by name since character may have multiple ChildActorComponents
	UChildActorComponent* ChaosFieldComp = nullptr;
	TArray<UChildActorComponent*> ChildActorComps;
	GetComponents<UChildActorComponent>(ChildActorComps);
	for (UChildActorComponent* Comp : ChildActorComps)
	{
		if (Comp && Comp->GetName().Contains(TEXT("ChaosForceField")))
		{
			ChaosFieldComp = Comp;
			break;
		}
	}

	if (!ChaosFieldComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] TriggerChaosField - ChaosForceField component not found!"));
		return;
	}

	AActor* ChaosActor = ChaosFieldComp->GetChildActor();
	if (!ChaosActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] TriggerChaosField - ChaosForceField has no child actor!"));
		return;
	}

	// Call EnableChaosDestroy or DisableChaosDestroy via direct cast to ISLFDestructibleHelperInterface
	// NOTE: We use direct _Implementation() call because Execute_() dispatch doesn't route to C++
	// when the Blueprint generated class has stale function overrides from old BPI interface
	ISLFDestructibleHelperInterface* DestructibleInterface = Cast<ISLFDestructibleHelperInterface>(ChaosActor);
	if (DestructibleInterface)
	{
		if (Enable)
		{
			DestructibleInterface->EnableChaosDestroy_Implementation();
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] TriggerChaosField - Called EnableChaosDestroy_Implementation on %s"), *ChaosActor->GetName());
		}
		else
		{
			DestructibleInterface->DisableChaosDestroy_Implementation();
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] TriggerChaosField - Called DisableChaosDestroy_Implementation on %s"), *ChaosActor->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] TriggerChaosField - ChaosActor %s does not implement ISLFDestructibleHelperInterface!"), *ChaosActor->GetName());
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
	// - Open rest menu via HUD
	// All handled by AC_InteractionManager::EventOnRest
	if (TargetCampfire)
	{
		// Get InteractionManager component and call EventOnRest
		UAC_InteractionManager* InteractionMgr = FindComponentByClass<UAC_InteractionManager>();
		if (InteractionMgr)
		{
			InteractionMgr->EventOnRest(TargetCampfire);
			UE_LOG(LogTemp, Log, TEXT("  Called AC_InteractionManager::EventOnRest"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  No AC_InteractionManager found on player!"));
		}
	}
}

void ASLFSoulslikeCharacter::OnDialogStarted_Implementation(UActorComponent* DialogComponent)
{
	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnDialogStarted"));

	// Cast to AIInteractionManagerComponent
	UAIInteractionManagerComponent* AIInteractionMgr = Cast<UAIInteractionManagerComponent>(DialogComponent);
	if (!AIInteractionMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnDialogStarted - DialogComponent is not UAIInteractionManagerComponent!"));
		return;
	}

	// Cache for EventOnDialogExit to call AdjustIndexForExit
	CachedNpcInteractionManager = AIInteractionMgr;

	// Switch to dialog input context (matches bp_only behavior)
	// This enables IMC_Dialog AND IMC_NavigableMenu (for menu navigation), disables IMC_Gameplay (movement/combat)
	// IMC_NavigableMenu is needed so PC_SoulslikeFramework's HandleNavigateOk gets called when player presses E/SpaceBar
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->Implements<UBPI_Controller>())
	{
		TArray<UInputMappingContext*> ContextsToEnable;
		TArray<UInputMappingContext*> ContextsToDisable;

		if (DialogMappingContext)
		{
			ContextsToEnable.Add(DialogMappingContext);
		}
		if (NavigableMenuMappingContext)
		{
			ContextsToEnable.Add(NavigableMenuMappingContext);
		}
		if (PlayerMappingContext)
		{
			ContextsToDisable.Add(PlayerMappingContext);
		}

		IBPI_Controller::Execute_SwitchInputContext(PC, ContextsToEnable, ContextsToDisable);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Switched to dialog input context (IMC_Dialog + IMC_NavigableMenu enabled, IMC_Gameplay disabled)"));
	}

	// Get HUD via BPI_Controller interface on the PlayerController
	if (PC && PC->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		UUserWidget* HUDWidget = nullptr;
		IBPI_Controller::Execute_GetPlayerHUD(PC, HUDWidget);

		if (UW_HUD* HUD = Cast<UW_HUD>(HUDWidget))
		{
			// Get NPC name and vendor asset from dialog component
			FText NpcName = AIInteractionMgr->Name;

			// Check if vendor asset exists (don't cast - Blueprint PDA_Vendor may not inherit from UPDA_Vendor)
			// We just need to know if this NPC has a vendor asset, not access its specific properties here
			bool bHasVendorAsset = (AIInteractionMgr->VendorAsset != nullptr);

			// DIAGNOSTIC: Trace exact component state
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] VENDOR DEBUG - AIInteractionMgr: %s, Owner: %s"),
				*AIInteractionMgr->GetName(),
				AIInteractionMgr->GetOwner() ? *AIInteractionMgr->GetOwner()->GetName() : TEXT("NULL"));
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] VENDOR DEBUG - VendorAsset exists: %s"),
				bHasVendorAsset ? TEXT("YES") : TEXT("NO"));
			if (bHasVendorAsset)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] VENDOR DEBUG - VendorAsset: %s (%s)"),
					*AIInteractionMgr->VendorAsset->GetName(),
					*AIInteractionMgr->VendorAsset->GetClass()->GetName());
			}

			// ═══════════════════════════════════════════════════════════════════
			// STEP 1: Get dialog text from DialogAsset and show W_Dialog FIRST
			// bp_only shows dialogue text at bottom of screen BEFORE any menu
			// ═══════════════════════════════════════════════════════════════════
			FText DialogText = FText::GetEmpty();

			// CRITICAL: Get ProgressManager from PlayerController and store it on the AIInteractionMgr
			// This is needed for AdjustIndexForExit to execute GameplayEvents
			// Note: Can be either UProgressManagerComponent or UAC_ProgressManager
			UActorComponent* ProgressManagerComp = nullptr;
			IBPI_Controller::Execute_GetProgressManager(PC, ProgressManagerComp);

			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] PC class: %s"), *PC->GetClass()->GetName());
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Execute_GetProgressManager returned: %s"),
				ProgressManagerComp ? *ProgressManagerComp->GetClass()->GetName() : TEXT("NULL"));

			// Store the component directly (AIInteractionMgr handles both class types)
			AIInteractionMgr->ProgressManager = ProgressManagerComp;
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] ProgressManager stored: %s"),
				ProgressManagerComp ? *ProgressManagerComp->GetName() : TEXT("NULL"));

			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] DialogAsset: %s"),
				AIInteractionMgr->DialogAsset ? *AIInteractionMgr->DialogAsset->GetName() : TEXT("NULL"));

			if (UPDA_Dialog* DialogData = Cast<UPDA_Dialog>(AIInteractionMgr->DialogAsset))
			{
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Cast to UPDA_Dialog succeeded"));

				// Get dialog table based on player's progress (uses the Requirements array)
				TSoftObjectPtr<UDataTable> DialogTable;
				DialogData->GetDialogTableBasedOnProgress(ProgressManagerComp, DialogTable);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] GetDialogTableBasedOnProgress returned: %s"),
					DialogTable.IsNull() ? TEXT("NULL") : *DialogTable.ToString());

				// Fallback to DefaultDialogTable if no progress-based table found
				if (DialogTable.IsNull())
				{
					DialogTable = DialogData->DefaultDialogTable;
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Using DefaultDialogTable: %s"),
						DialogTable.IsNull() ? TEXT("NULL") : *DialogTable.ToString());

					// FALLBACK: If DefaultDialogTable is the generic one, try to load the actual NPC table
					// This handles the case where Requirement array data was lost during migration
					if (!DialogTable.IsNull())
					{
						FString TablePath = DialogTable.ToString();
						if (TablePath.Contains(TEXT("DT_GenericDefaultDialog")))
						{
							// Check if this is for the Guide NPC and try to load the correct table
							FString DialogAssetName = DialogData->GetName();
							UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Generic dialog detected for: %s, attempting fallback"), *DialogAssetName);

							if (DialogAssetName.Contains(TEXT("ExampleDialog")) && !DialogAssetName.Contains(TEXT("Vendor")))
							{
								// Guide NPC fallback - load the NoProgress dialog table
								static const FSoftObjectPath GuideDialogPath(TEXT("/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.DT_ShowcaseGuideNpc_NoProgress"));
								UDataTable* FallbackTable = Cast<UDataTable>(GuideDialogPath.TryLoad());
								if (FallbackTable)
								{
									DialogTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(FallbackTable));
									UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] FALLBACK: Using DT_ShowcaseGuideNpc_NoProgress"));
								}
							}
						}
					}
				}

				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] DefaultDialogTable IsNull: %s, Path: %s"),
					DialogTable.IsNull() ? TEXT("YES") : TEXT("NO"),
					DialogTable.IsNull() ? TEXT("N/A") : *DialogTable.ToString());

				if (!DialogTable.IsNull())
				{
					UDataTable* LoadedTable = DialogTable.LoadSynchronous();
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] LoadedTable: %s"),
						LoadedTable ? *LoadedTable->GetName() : TEXT("NULL"));

					if (LoadedTable)
					{
						// Get row names
						TArray<FName> RowNames = LoadedTable->GetRowNames();
						UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Row count: %d"), RowNames.Num());

						// CRITICAL: Set ActiveTable and CachedHUD so AdjustIndexForExit can show subsequent entries
						AIInteractionMgr->ActiveTable = DialogTable;
						AIInteractionMgr->CachedHUD = HUD;  // Required for AdjustIndexForExit to call EventSetupDialog
						AIInteractionMgr->MaxIndex = RowNames.Num();  // Total count, not count-1
						AIInteractionMgr->CurrentIndex = 0;

						if (RowNames.Num() > 0)
						{
							// Get first dialog entry
							FSLFDialogEntry FirstEntry = AIInteractionMgr->GetCurrentDialogEntry(LoadedTable, RowNames);
							DialogText = FirstEntry.Entry;

							UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Got dialog text: '%s' (Index 0/%d)"),
								*DialogText.ToString(), AIInteractionMgr->MaxIndex);
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] DataTable has no rows!"));
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] Cast to UPDA_Dialog FAILED! Asset class: %s"),
					AIInteractionMgr->DialogAsset ? *AIInteractionMgr->DialogAsset->GetClass()->GetName() : TEXT("NULL"));
			}

			// Show dialog text at bottom of screen (W_Dialog)
			if (!DialogText.IsEmpty())
			{
				HUD->EventSetupDialog(DialogText);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Called EventSetupDialog with text: %s"), *DialogText.ToString());
			}
			else
			{
				// Fallback: show NPC name as greeting
				DialogText = FText::Format(NSLOCTEXT("Dialog", "Greeting", "{0}"), NpcName);
				HUD->EventSetupDialog(DialogText);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] No dialog data, showing NPC name: %s"), *DialogText.ToString());
			}

			// ═══════════════════════════════════════════════════════════════════
			// STEP 2: Bind delegates for dialog advancement - same for all NPCs
			// NOTE: Do NOT show W_NPC_Window here! It's shown by AdjustIndexForExit
			// when dialog finishes (if VendorAsset exists)
			// ═══════════════════════════════════════════════════════════════════
			if (bHasVendorAsset)
			{
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Vendor NPC detected - menu will show AFTER dialog completes"));
				// NOTE: EventSetupNpcWindow is now called by AdjustIndexForExit when dialog finishes

				// Bind OnDialogWindowClosed so pressing E advances dialog
				HUD->OnDialogWindowClosed.RemoveDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				HUD->OnDialogWindowClosed.AddDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound OnDialogWindowClosed to EventOnDialogExit (vendor)"));

				// Bind OnDialogFinished for cleanup when dialog ends
				AIInteractionMgr->OnDialogFinished.RemoveDynamic(this, &ASLFSoulslikeCharacter::OnDialogFinished);
				AIInteractionMgr->OnDialogFinished.AddDynamic(this, &ASLFSoulslikeCharacter::OnDialogFinished);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound OnDialogFinished for cleanup (vendor)"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Non-vendor NPC (Guide) - dialogue only, NO menu"));
				// For non-vendor NPCs: just show dialog, player presses E to advance/close
				// Dialog is now handled in HandleInteractStarted - when E is pressed and
				// IsDialogActive is true, it broadcasts OnDialogWindowClosed to advance.

				// NOTE: Do NOT call EventToggleUiMode(true) here!
				// It sets FInputModeUIOnly which blocks ALL game input including E key.
				// We need game input to remain active so HandleInteractStarted receives E presses.

				// Bind OnDialogWindowClosed to EventOnDialogExit so pressing E advances dialog
				// Remove any existing binding first to prevent duplicate calls
				HUD->OnDialogWindowClosed.RemoveDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				HUD->OnDialogWindowClosed.AddDynamic(this, &ASLFSoulslikeCharacter::EventOnDialogExit);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound OnDialogWindowClosed to EventOnDialogExit"));

				// Bind OnDialogFinished for cleanup when dialog ends
				AIInteractionMgr->OnDialogFinished.RemoveDynamic(this, &ASLFSoulslikeCharacter::OnDialogFinished);
				AIInteractionMgr->OnDialogFinished.AddDynamic(this, &ASLFSoulslikeCharacter::OnDialogFinished);
				UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Bound OnDialogFinished for cleanup"));
			}

			// ═══════════════════════════════════════════════════════════════════
			// STEP 3: Add NavigableMenu input context for gamepad dialog progression
			// This allows gamepad "confirm" button (IA_NavigableMenu_Ok) to work
			// alongside E key (IA_Interact) during dialog
			// ═══════════════════════════════════════════════════════════════════
			if (NavigableMenuMappingContext)
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
				{
					// Add with higher priority (1) so it works alongside gameplay context (0)
					Subsystem->AddMappingContext(NavigableMenuMappingContext, 1);
					UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Added NavigableMenuMappingContext for gamepad dialog input"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnDialogStarted - Failed to cast HUD to UW_HUD!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnDialogStarted - PlayerController does not implement BPI_Controller!"));
	}
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
	// Souls-like lock-on configuration:
	// When locked: Character faces enemy (handled by TickTargetLockRotation), strafes instead of turning
	// When unlocked: Character rotates in movement direction (normal souls-like movement)

	UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] OnTargetLocked: locked=%s, rotate=%s"),
		bTargetLocked ? TEXT("true") : TEXT("false"),
		bRotateTowards ? TEXT("true") : TEXT("false"));

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	if (bTargetLocked)
	{
		// === LOCKED ON ===
		// Disable movement-based rotation - we manually rotate to face target in Tick
		// This enables strafing (moving sideways while facing enemy)
		if (MoveComp)
		{
			MoveComp->bOrientRotationToMovement = false;
		}
		bUseControllerRotationYaw = false;

		UE_LOG(LogTemp, Log, TEXT("  Lock-on enabled: bOrientRotationToMovement=false, strafing mode"));
	}
	else
	{
		// === UNLOCKED ===
		// Re-enable movement-based rotation (character rotates toward movement direction)
		// This is standard souls-like movement when not locked on
		if (MoveComp)
		{
			MoveComp->bOrientRotationToMovement = true;
		}
		bUseControllerRotationYaw = false;

		UE_LOG(LogTemp, Log, TEXT("  Lock-on disabled: bOrientRotationToMovement=true, normal movement"));
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

	// Get inventory manager from PlayerController
	UInventoryManagerComponent* InvMgr = nullptr;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		InvMgr = PC->FindComponentByClass<UInventoryManagerComponent>();
	}

	// Try to get item data and add to inventory
	// Handle BOTH class hierarchies: ASLFPickupItemBase (newer) and AB_PickupItem (older/Blueprint)
	UPrimaryDataAsset* ItemData = nullptr;
	int32 ItemCount = 1;

	if (ASLFPickupItemBase* SLFPickup = Cast<ASLFPickupItemBase>(Item))
	{
		// Newer hierarchy (ASLFPickupItemBase) - Item is UDataAsset*, cast to UPrimaryDataAsset*
		ItemData = Cast<UPrimaryDataAsset>(SLFPickup->Item);
		ItemCount = SLFPickup->Count;
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Found ASLFPickupItemBase"));
	}
	else if (AB_PickupItem* BPickup = Cast<AB_PickupItem>(Item))
	{
		// Older hierarchy (AB_PickupItem from B_PickupItem Blueprint)
		ItemData = BPickup->Item;
		ItemCount = BPickup->Count;
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Found AB_PickupItem"));
	}

	// Add to inventory if we found valid item data
	if (InvMgr && ItemData)
	{
		InvMgr->AddItem(ItemData, ItemCount, true);
		UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Added %s x%d to inventory"),
			*ItemData->GetName(), ItemCount);
	}
	else if (!InvMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnLootItem - InventoryManagerComponent NOT FOUND on PlayerController!"));
	}
	else if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnLootItem - ItemData is null on pickup actor!"));
	}

	// Queue the pickup item montage action
	// NOTE: The tag is "PickupItem" which maps to "DA_Action_PickupItemMontage" in AC_ActionManager
	if (CachedInputBuffer)
	{
		FGameplayTag PickupTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.PickupItem"), false);
		if (PickupTag.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("[SoulslikeCharacter] Queuing PickupItem action (plays pickup montage)"));
			CachedInputBuffer->QueueAction(PickupTag);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] PickupItem action tag is NOT VALID!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoulslikeCharacter] OnLootItem - CachedInputBuffer is NULL! Cannot queue pickup animation."));
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
		UE_LOG(LogTemp, Warning, TEXT("  No PC or HUD widget ref!"));
		return;
	}

	UW_HUD* HUD = PC->HUDWidgetRef;

	// Show/hide interaction prompt based on whether Interactable is valid
	if (IsValid(Interactable))
	{
		// Pass the actor directly - HUD now accepts AActor*
		HUD->EventShowInteractableWidget(Interactable);
		UE_LOG(LogTemp, Log, TEXT("  Showing interaction widget for: %s"), *Interactable->GetName());
	}
	else
	{
		// Hide interaction prompt
		HUD->EventHideInteractionWidget();
		UE_LOG(LogTemp, Log, TEXT("  Hiding interaction widget"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// OVERLAY STATE SYNC - Updates AnimBP Blueprint enum variables from EquipmentManager
// ═══════════════════════════════════════════════════════════════════════════════
void ASLFSoulslikeCharacter::UpdateAnimInstanceOverlayStates()
{
	// Get AnimInstance from skeletal mesh
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();

	// === MOVEMENT VARIABLES (Speed, Direction for locomotion/strafe animations) ===
	// Calculate Direction using same method as Blueprint: CalculateDirection(Velocity, ActorRotation)
	// This is needed for strafe animations during target lock
	FVector Velocity = GetVelocity();
	FRotator ActorRotation = GetActorRotation();
	float Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
	float Speed = Velocity.Size2D(); // Ground speed (XY only)

	// Set Direction(Angle) - note the parentheses in the Blueprint variable name
	if (FDoubleProperty* DirectionProp = FindFProperty<FDoubleProperty>(AnimClass, TEXT("Direction(Angle)")))
	{
		DirectionProp->SetPropertyValue_InContainer(AnimInstance, Direction);
	}
	else if (FFloatProperty* DirectionPropF = FindFProperty<FFloatProperty>(AnimClass, TEXT("Direction(Angle)")))
	{
		DirectionPropF->SetPropertyValue_InContainer(AnimInstance, Direction);
	}

	// Set GroundSpeed
	if (FDoubleProperty* SpeedProp = FindFProperty<FDoubleProperty>(AnimClass, TEXT("GroundSpeed")))
	{
		SpeedProp->SetPropertyValue_InContainer(AnimInstance, Speed);
	}
	else if (FFloatProperty* SpeedPropF = FindFProperty<FFloatProperty>(AnimClass, TEXT("GroundSpeed")))
	{
		SpeedPropF->SetPropertyValue_InContainer(AnimInstance, Speed);
	}

	// === OVERLAY STATES (weapon hold/attack animations) ===
	// Get EquipmentManager from controller (NOT character!)
	UAC_EquipmentManager* EquipMgr = nullptr;
	if (AController* PC = GetController())
	{
		EquipMgr = PC->FindComponentByClass<UAC_EquipmentManager>();
	}
	if (!EquipMgr)
	{
		return;
	}

	// Find and set LeftHandOverlayState (Blueprint enum property)
	if (FEnumProperty* LeftProp = FindFProperty<FEnumProperty>(AnimClass, TEXT("LeftHandOverlayState")))
	{
		// Get the integer value from C++ enum
		int64 Value = static_cast<int64>(EquipMgr->LeftHandOverlayState);
		// Set using the numeric property (works for both Blueprint and C++ enums)
		FNumericProperty* UnderlyingProp = LeftProp->GetUnderlyingProperty();
		if (UnderlyingProp)
		{
			UnderlyingProp->SetIntPropertyValue(LeftProp->ContainerPtrToValuePtr<void>(AnimInstance), Value);
		}
	}

	// Find and set RightHandOverlayState
	if (FEnumProperty* RightProp = FindFProperty<FEnumProperty>(AnimClass, TEXT("RightHandOverlayState")))
	{
		int64 Value = static_cast<int64>(EquipMgr->RightHandOverlayState);
		FNumericProperty* UnderlyingProp = RightProp->GetUnderlyingProperty();
		if (UnderlyingProp)
		{
			UnderlyingProp->SetIntPropertyValue(RightProp->ContainerPtrToValuePtr<void>(AnimInstance), Value);
		}
	}

	// Find and set ActiveOverlayState
	if (FEnumProperty* ActiveProp = FindFProperty<FEnumProperty>(AnimClass, TEXT("ActiveOverlayState")))
	{
		int64 Value = static_cast<int64>(EquipMgr->ActiveOverlayState);
		FNumericProperty* UnderlyingProp = ActiveProp->GetUnderlyingProperty();
		if (UnderlyingProp)
		{
			UnderlyingProp->SetIntPropertyValue(ActiveProp->ContainerPtrToValuePtr<void>(AnimInstance), Value);
		}
	}
}

void ASLFSoulslikeCharacter::TickTargetLockRotation(float DeltaTime)
{
	// Souls-like target lock rotation:
	// - CHARACTER rotates to face enemy (strafing behavior)
	// - CAMERA rotates to look at enemy (via control rotation + spring arm)
	// - Movement input becomes relative to enemy direction

	// Early out if no InteractionManager
	if (!CachedInteractionManager)
	{
		return;
	}

	// Check if target is locked
	if (!CachedInteractionManager->IsTargetLocked())
	{
		return;
	}

	// Get the locked target component (for accurate look-at position)
	USceneComponent* LockedOnComponent = CachedInteractionManager->LockedOnComponent;
	if (!IsValid(LockedOnComponent))
	{
		return;
	}

	// Get target lock strength from InteractionManager (interpolation speed)
	float LockStrength = CachedInteractionManager->TargetLockStrength;

	// Get player location (from Actor)
	FVector PlayerLocation = GetActorLocation();

	// Get target location (from locked component)
	FVector TargetLocation = LockedOnComponent->GetComponentLocation();

	// Calculate look-at rotation from player to target (full 3D for camera)
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);

	// Get current rotations
	FRotator CurrentActorRotation = GetActorRotation();

	AController* PC = GetController();
	FRotator CurrentControlRotation = PC ? PC->GetControlRotation() : CurrentActorRotation;

	// === CHARACTER ROTATION (Yaw only - face enemy horizontally) ===
	FRotator CharacterTargetRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
	FRotator NewActorRotation = FMath::RInterpTo(
		CurrentActorRotation,
		CharacterTargetRotation,
		DeltaTime,
		LockStrength
	);
	SetActorRotation(FRotator(0.0f, NewActorRotation.Yaw, 0.0f));

	// === CAMERA ROTATION (Yaw + Pitch - look at enemy including height) ===
	// Camera follows control rotation via spring arm
	// Keep current pitch within reasonable bounds, interpolate yaw to face target
	FRotator CameraTargetRotation = FRotator(
		FMath::Clamp(LookAtRotation.Pitch, -45.0f, 45.0f), // Clamp pitch to avoid extreme angles
		LookAtRotation.Yaw,
		0.0f
	);
	FRotator NewControlRotation = FMath::RInterpTo(
		CurrentControlRotation,
		CameraTargetRotation,
		DeltaTime,
		LockStrength
	);

	if (PC)
	{
		PC->SetControlRotation(NewControlRotation);
	}
}
