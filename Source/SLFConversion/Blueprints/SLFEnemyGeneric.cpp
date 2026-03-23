// SLFEnemyGeneric.cpp
// Data-driven enemy that configures from EnemyTypeName property.
// Pattern copied from SLFEnemySentinel but generalized for any enemy.

#include "SLFEnemyGeneric.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ChildActorComponent.h"
#include "NavigationInvokerComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Animation/AnimSequence.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceSkeletalMesh.h"

ASLFEnemyGeneric::ASLFEnemyGeneric()
{
	if (!FindComponentByClass<UAICombatManagerComponent>())
	{
		CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("GenericCombatManager"));
	}
	if (!FindComponentByClass<USLFAIStateMachineComponent>())
	{
		CreateDefaultSubobject<USLFAIStateMachineComponent>(TEXT("GenericAIStateMachine"));
	}
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(3000.0f, 4000.0f);

	// Enable RVO avoidance so enemies don't walk into terrain/each other
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->bUseRVOAvoidance = true;
		CMC->AvoidanceWeight = 0.5f;
		CMC->SetAvoidanceEnabled(true);
	}
}

void ASLFEnemyGeneric::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyTypeName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[EnemyGeneric] EnemyTypeName is empty! Set it in Blueprint CDO or per-instance."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] BeginPlay: %s (Type=%s)"), *GetName(), *EnemyTypeName);
	ApplyEnemyConfig();
}

void ASLFEnemyGeneric::ApplyEnemyConfig()
{
	const FString Dir = FString::Printf(TEXT("/Game/CustomEnemies/%s/"), *EnemyTypeName);

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Load mesh
	FString MeshPath = FString::Printf(TEXT("%sSKM_%s"), *Dir, *EnemyTypeName);
	USkeletalMesh* EnemyMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
	if (!EnemyMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] No mesh found at %s"), *MeshPath);
		return;
	}

	// Set AnimBP before mesh change (Bug #8: SetSkeletalMeshAsset nulls AnimInstance)
	FString ABPPath = FString::Printf(TEXT("%sABP_%s.ABP_%s_C"), *Dir, *EnemyTypeName, *EnemyTypeName);
	EnemyAnimBPClass = LoadClass<UAnimInstance>(nullptr, *ABPPath);
	if (EnemyAnimBPClass)
	{
		MeshComp->SetAnimInstanceClass(EnemyAnimBPClass);
		MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	// Enable CPU access via GetLODInfo (NOT GetLODInfoArray — different backing store in UE5.5+)
	for (int32 LODIdx = 0; LODIdx < EnemyMesh->GetLODNum(); LODIdx++)
	{
		if (FSkeletalMeshLODInfo* Info = EnemyMesh->GetLODInfo(LODIdx))
		{
			Info->bAllowCPUAccess = true;
		}
	}
	MeshComp->SetSkeletalMeshAsset(EnemyMesh);
	MeshComp->SetForcedLOD(1); // Force LOD index 0 (1-based: 1 = LOD0)
	// Mesh is meter-scale (same as Sentinel) — scale MESH only, not actor
	MeshComp->SetRelativeScale3D(FVector(MeshScale));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, MeshZOffset));
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetBoundsScale(200.0f);
	MeshComp->bEnableUpdateRateOptimizations = false;
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// Capsule sizing (same as Sentinel)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCapsuleHalfHeight(90.0f);
		Capsule->SetCapsuleRadius(35.0f);
	}

	// Material: the commandlet bakes M_ into the mesh asset directly.
	// Only override if the mesh has no material or has the default material.
	if (MeshComp->GetNumMaterials() > 0)
	{
		UMaterialInterface* ExistingMat = MeshComp->GetMaterial(0);
		if (ExistingMat && !ExistingMat->GetName().Contains(TEXT("Default")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] Mesh already has material: %s"), *ExistingMat->GetName());
		}
		else
		{
			// Try loading M_ from commandlet
			UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr,
				*FString::Printf(TEXT("%sM_%s"), *Dir, *EnemyTypeName));
			if (Mat)
			{
				for (int32 i = 0; i < MeshComp->GetNumMaterials(); i++)
					MeshComp->SetMaterial(i, Mat);
			}
		}
	}

	// Remove inherited weapon child actor
	{
		TArray<UChildActorComponent*> ChildActorComps;
		GetComponents<UChildActorComponent>(ChildActorComps);
		for (UChildActorComponent* CAC : ChildActorComps)
		{
			if (CAC && CAC->GetName().Contains(TEXT("Weapon")))
			{
				CAC->DestroyChildActor();
				CAC->DestroyComponent();
			}
		}
	}

	// Set data assets on CombatManager
	if (CombatManagerComponent)
	{
		FString ReactionPath = FString::Printf(TEXT("%sPDA_%s_CombatReaction"), *Dir, *EnemyTypeName);
		FString PoisePath = FString::Printf(TEXT("%sPDA_%s_PoiseBreak"), *Dir, *EnemyTypeName);

		UPDA_CombatReactionAnimData* ReactionData = LoadObject<UPDA_CombatReactionAnimData>(nullptr, *ReactionPath);
		UPDA_PoiseBreakAnimData* PoiseData = LoadObject<UPDA_PoiseBreakAnimData>(nullptr, *PoisePath);

		if (ReactionData) CombatManagerComponent->ReactionAnimset = ReactionData;
		if (PoiseData) CombatManagerComponent->PoiseBreakAsset = PoiseData;
	}

	ConfigureAbilities();

	// Tune AI state machine for Elden Ring-style aggression
	if (USLFAIStateMachineComponent* AISM = FindComponentByClass<USLFAIStateMachineComponent>())
	{
		AISM->Config.WalkSpeed = 350.0f;           // Elden Ring standard enemy walk
		AISM->Config.RunSpeed = 650.0f;             // Sprint when chasing (was 550, too slow)
		AISM->Config.StrafeSpeed = 250.0f;          // Faster strafe
		AISM->Config.SprintThresholdDistance = 250.0f; // Run sooner (was 400, enemies just walked)
		AISM->Config.AttackRange = 250.0f;          // Slightly longer reach
		AISM->Config.BaseAggression = 0.92f;        // Very aggressive (8% strafe chance)
		AISM->Config.ComboChance = 0.5f;            // 50% chance to chain attacks
		AISM->Config.MaxComboLength = 3;            // Up to 3 hit combos
		AISM->Config.PostAttackRecovery = 0.15f;    // Quick recovery
		AISM->Config.RepositionInterval = 0.8f;     // Short repositioning
		AISM->Config.MinAttackDelay = 0.1f;         // Almost instant follow-up
		AISM->Config.MaxAttackDelay = 0.4f;         // Short max delay
		AISM->Config.GapCloserChance = 0.8f;        // Often lunge when far (was 0.6)
		AISM->Config.bCanDodge = true;               // Enable dodge evasion
		AISM->Config.DodgeChance = 0.7f;             // 70% chance for testing (lower to 0.3 for production)
		AISM->Config.DodgeCooldown = 1.5f;           // 1.5s between dodges (testing)

		// Load dodge montages from content folder
		for (int32 i = 1; i <= 3; i++)
		{
			FString MontPath = FString::Printf(TEXT("%sAM_%s_Dodge%02d"), *Dir, *EnemyTypeName, i);
			if (UAnimMontage* DodgeMont = LoadObject<UAnimMontage>(nullptr, *MontPath))
			{
				AISM->DodgeMontages.Add(DodgeMont);
				UE_LOG(LogTemp, Warning, TEXT("[DODGE SETUP] %s: Loaded %s"), *GetName(), *MontPath);
			}
		}
		if (AISM->DodgeMontages.Num() == 0)
		{
			AISM->Config.bCanDodge = false;
			UE_LOG(LogTemp, Warning, TEXT("[DODGE SETUP] %s: No dodge montages found — dodge disabled"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[DODGE SETUP] %s: %d dodge montages, chance=%.0f%%, cooldown=%.1fs"),
				*GetName(), AISM->DodgeMontages.Num(), AISM->Config.DodgeChance * 100.0f, AISM->Config.DodgeCooldown);
		}
	}

	// Cache locomotion montages and disable root motion on them
	IdleMontage = LoadObject<UAnimMontage>(nullptr, *FString::Printf(TEXT("%sAM_%s_Idle"), *Dir, *EnemyTypeName));
	WalkMontage = LoadObject<UAnimMontage>(nullptr, *FString::Printf(TEXT("%sAM_%s_Walk"), *Dir, *EnemyTypeName));
	RunMontage = LoadObject<UAnimMontage>(nullptr, *FString::Printf(TEXT("%sAM_%s_Run"), *Dir, *EnemyTypeName));

	// Force root lock on all animations to prevent root motion sliding (Bug #17)
	// Interchange importer in UE5.7 ignores FbxFactory root lock settings
	FString AnimDir = FString::Printf(TEXT("%sAnimations/"), *Dir);
	TArray<FAssetData> AnimAssets;
	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	ARM.Get().GetAssetsByPath(FName(*AnimDir), AnimAssets, true);
	for (const FAssetData& AD : AnimAssets)
	{
		if (UAnimSequence* Seq = Cast<UAnimSequence>(AD.GetAsset()))
		{
			if (!Seq->bForceRootLock)
			{
				Seq->bForceRootLock = true;
				Seq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;
			}
		}
	}

	// Character Body FX Vol. 3 — mesh-surface particle effect
	// NS_Cosmic = energy aura, NS_BlackOoze = dark corruption
	// Character Body FX Vol.3 — full diagnostics
	// Body VFX is deferred to Tick — mesh must be fully initialized first (same as AnimBP)

	UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] %s configured: Mesh=%s AnimBP=%s Idle=%s Walk=%s Run=%s"),
		*EnemyTypeName,
		EnemyMesh ? TEXT("y") : TEXT("n"),
		EnemyAnimBPClass ? TEXT("y") : TEXT("n"),
		IdleMontage ? TEXT("y") : TEXT("n"),
		WalkMontage ? TEXT("y") : TEXT("n"),
		RunMontage ? TEXT("y") : TEXT("n"));
}

void ASLFEnemyGeneric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TicksAfterBeginPlay++;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Deferred AnimBP application (Bug #8: mesh change nulls AnimInstance)
	if (!bAnimBPApplied && TicksAfterBeginPlay > 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst && EnemyAnimBPClass)
		{
			MeshComp->SetAnimInstanceClass(EnemyAnimBPClass);
			MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MeshComp->InitAnim(true);
			AnimInst = MeshComp->GetAnimInstance();
		}

		if (AnimInst)
		{
			bAnimBPApplied = true;

			// Start idle to suppress BlendSpacePlayer (Bug #16)
			if (IdleMontage)
			{
				AnimInst->Montage_Play(IdleMontage, 1.0f);
				AnimInst->Montage_SetNextSection(FName(TEXT("Default")), FName(TEXT("Default")), IdleMontage);
				ActiveLocomotionMontage = IdleMontage;
			}

			if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>())
			{
				AISMComp->RefreshCachedAnimInstance();
			}

			UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] %s AnimBP applied (tick %d)"), *EnemyTypeName, TicksAfterBeginPlay);
		}
	}

	// Deferred body VFX — wait for mesh to be fully initialized
	if (!bBodyVFXSpawned && bAnimBPApplied && TicksAfterBeginPlay > 10)
	{
		TrySpawnBodyVFX();
	}

	// Montage-based locomotion (Guard pattern: seamless loop)
	if (bAnimBPApplied && MeshComp)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			float Speed = GetCharacterMovement() ? GetCharacterMovement()->Velocity.Size2D() : 0.0f;

			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			bool bIsLocomotion = (CurrentMontage == IdleMontage || CurrentMontage == WalkMontage || CurrentMontage == RunMontage);
			bool bIsOther = (CurrentMontage != nullptr && !bIsLocomotion);

			if (!bIsOther)
			{
				UAnimMontage* DesiredMontage = nullptr;
				if (Speed > 300.0f && RunMontage) DesiredMontage = RunMontage;
				else if (Speed > 30.0f && WalkMontage) DesiredMontage = WalkMontage;
				else if (IdleMontage) DesiredMontage = IdleMontage;

				if (DesiredMontage && DesiredMontage != ActiveLocomotionMontage)
				{
					UE_LOG(LogTemp, Warning, TEXT("[Loco] Speed=%.0f Switching: %s -> %s"),
						Speed,
						ActiveLocomotionMontage ? *ActiveLocomotionMontage->GetName() : TEXT("none"),
						*DesiredMontage->GetName());
					if (ActiveLocomotionMontage)
					{
						AnimInst->Montage_Stop(0.2f, ActiveLocomotionMontage);
					}
					AnimInst->Montage_Play(DesiredMontage, 1.0f);
					AnimInst->Montage_SetNextSection(FName(TEXT("Default")), FName(TEXT("Default")), DesiredMontage);
					ActiveLocomotionMontage = DesiredMontage;
				}
			}
			else
			{
				// Attack/reaction playing — clear so we re-evaluate after
				ActiveLocomotionMontage = nullptr;
			}
		}
	}
}

void ASLFEnemyGeneric::ConfigureAbilities()
{
	if (!CombatManagerComponent) return;

	const FString Dir = FString::Printf(TEXT("/Game/CustomEnemies/%s/"), *EnemyTypeName);

	TArray<FSLFAIAbility> NewAbilities;

	// Auto-discover attack abilities: DA_<Name>_Attack01 through Attack07
	// (Attack08 was a long-range BulletBehavior anim — excluded for melee enemies)
	for (int32 i = 1; i <= 7; i++)
	{
		FString DAName = FString::Printf(TEXT("DA_%s_Attack%02d"), *EnemyTypeName, i);
		UDataAsset* AbilityAsset = LoadObject<UDataAsset>(nullptr, *(Dir + DAName));
		if (!AbilityAsset) break;

		FSLFAIAbility Ability;
		Ability.AbilityAsset = AbilityAsset;
		Ability.Weight = 1.0f;
		// First 2 attacks double as gap closers (lunge from range)
		if (i <= 2)
		{
			Ability.MaxDistance = 350.0f;
			Ability.bIsGapCloser = true;
		}
		else
		{
			Ability.MaxDistance = 250.0f;
			Ability.bIsGapCloser = false;
		}
		NewAbilities.Add(Ability);
	}

	// Auto-discover heavy attack abilities
	for (int32 i = 1; i <= 2; i++)
	{
		FString DAName = FString::Printf(TEXT("DA_%s_HeavyAttack%02d"), *EnemyTypeName, i);
		UDataAsset* AbilityAsset = LoadObject<UDataAsset>(nullptr, *(Dir + DAName));
		if (!AbilityAsset) break;

		FSLFAIAbility Ability;
		Ability.AbilityAsset = AbilityAsset;
		Ability.Weight = 0.5f;
		Ability.MaxDistance = 350.0f;
		Ability.bIsGapCloser = true;
		NewAbilities.Add(Ability);
	}

	CombatManagerComponent->Abilities = NewAbilities;
	UE_LOG(LogTemp, Warning, TEXT("[EnemyGeneric] %s: %d AI abilities configured"), *EnemyTypeName, NewAbilities.Num());
}

void ASLFEnemyGeneric::TrySpawnBodyVFX()
{
	bBodyVFXSpawned = true;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	USkeletalMesh* SkelMesh = MeshComp->GetSkeletalMeshAsset();
	if (!SkelMesh) return;

	// CPU access is baked at import time via commandlet (Build() + Save)

	static const TCHAR* FXPaths[] = {
		TEXT("/Game/CharacterBodyFX3/FX/NS_Furnace.NS_Furnace"),
		TEXT("/Game/CharacterBodyFX3/FX/NS_Lightning.NS_Lightning"),
	};

	UNiagaraSystem* BodyFX = nullptr;
	for (const TCHAR* Path : FXPaths)
	{
		BodyFX = LoadObject<UNiagaraSystem>(nullptr, Path);
		if (BodyFX) break;
	}
	if (!BodyFX) return;

	UE_LOG(LogTemp, Warning, TEXT("[BodyFX] %s: Spawning %s (tick %d)"),
		*EnemyTypeName, *BodyFX->GetName(), TicksAfterBeginPlay);
	UE_LOG(LogTemp, Warning, TEXT("[BodyFX] MeshComp registered=%s, HasBegunPlay=%s, RenderData=%s"),
		MeshComp->IsRegistered() ? TEXT("Y") : TEXT("N"),
		MeshComp->HasBegunPlay() ? TEXT("Y") : TEXT("N"),
		MeshComp->GetSkeletalMeshRenderData() ? TEXT("Y") : TEXT("N"));

	// The system's DI has MeshUserParameter binding to "User.Skeletal Mesh".
	// This binding resolves the user param to get the mesh. It expects a
	// USkeletalMeshComponent or AActor, NOT a NiagaraDataInterface.
	// Use SetVariableObject to bind our MeshComp directly as the user param object.
	UNiagaraComponent* VFXComp = NewObject<UNiagaraComponent>(this);
	VFXComp->SetupAttachment(MeshComp);
	VFXComp->SetAutoActivate(false);
	VFXComp->RegisterComponent();
	VFXComp->SetAsset(BodyFX);

	// Bind the skeletal mesh COMPONENT as the user parameter object
	VFXComp->SetVariableObject(FName(TEXT("User.Skeletal Mesh")), MeshComp);

	// Scale up the effect for visibility
	VFXComp->SetRelativeScale3D(FVector(2.0f));

	VFXComp->Activate();

	UE_LOG(LogTemp, Warning, TEXT("[BodyFX] Post-Activate: IsActive=%s, IsComplete=%s"),
		VFXComp->IsActive() ? TEXT("Y") : TEXT("N"),
		VFXComp->IsComplete() ? TEXT("Y") : TEXT("N"));

	BodyVFXComponents.Add(VFXComp);
}
