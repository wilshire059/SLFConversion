// SLFEnemySentinel.cpp
// C++ implementation for B_Soulslike_Enemy_Sentinel
// Forensically distinct custom enemy using Ironbound Warlord mesh and ARP-retargeted animations

#include "SLFEnemySentinel.h"
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

ASLFEnemySentinel::ASLFEnemySentinel()
{
	// Fresh Blueprint has no SCS components (not inherited from B_Soulslike_Enemy).
	// Create all components the AI system needs — Guard gets these from Blueprint SCS.
	if (!FindComponentByClass<UAICombatManagerComponent>())
	{
		CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("SentinelCombatManager"));
	}

	// AI state machine drives chasing, attacking, dodging — this is what makes the Guard work.
	// Without it, the enemy just stands there.
	if (!FindComponentByClass<USLFAIStateMachineComponent>())
	{
		CreateDefaultSubobject<USLFAIStateMachineComponent>(TEXT("SentinelAIStateMachine"));
	}

	// Navigation invoker: triggers dynamic NavMesh tile generation around this actor
	// so the AI state machine's MoveToLocation works without a placed NavMeshBoundsVolume
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(3000.0f, 4000.0f); // generate 3000 units, remove at 4000
}

void ASLFEnemySentinel::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] BeginPlay: %s BUILD=20260225A"), *GetName());

	ApplySentinelConfig();
}

void ASLFEnemySentinel::ApplySentinelConfig()
{
	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Load Sentinel mesh — SKM_Sentinel IS the Ironbound Warlord (properly rigged, 55 ARP bones).
	USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir + TEXT("SKM_Sentinel")));
	if (!SentinelMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] No Sentinel mesh found, keeping default"));
		return;
	}

	// Skeleton is set persistently on the mesh asset by SLF.SetMeshSkeleton commandlet.
	USkeleton* MeshSkeleton = SentinelMesh->GetSkeleton();
	UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] Mesh=%s Skeleton=%s Bones=%d"),
		*SentinelMesh->GetName(),
		MeshSkeleton ? *MeshSkeleton->GetName() : TEXT("NULL"),
		MeshSkeleton ? MeshSkeleton->GetReferenceSkeleton().GetNum() : 0);

	// Set AnimBP before mesh change
	SentinelAnimBPClass = LoadClass<UAnimInstance>(nullptr, *(SentinelDir + TEXT("ABP_Sentinel.ABP_Sentinel_C")));
	if (SentinelAnimBPClass)
	{
		MeshComp->SetAnimInstanceClass(SentinelAnimBPClass);
		MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	MeshComp->SetSkeletalMeshAsset(SentinelMesh);
	// Mesh is meter-scale (1 FBX unit = 1 UE unit = 1cm at import).
	// At scale 1.0, character is ~2.68cm tall. Scale MESH only (not actor) to preserve capsule physics.
	// 139x = 76 * 1.83 (user-requested size increase). Do NOT use SetActorScale3D - breaks capsule collision.
	MeshComp->SetRelativeScale3D(FVector(139.0f));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// Disable mesh collision - capsule handles all character collision.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetBoundsScale(200.0f);
	MeshComp->bEnableUpdateRateOptimizations = false;
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// Capsule sizing for Sentinel (taller than default)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCapsuleHalfHeight(90.0f);
		Capsule->SetCapsuleRadius(35.0f);
	}

	// AI mesh FBX was imported with embedded textures - materials created automatically.
	// If no materials exist, apply a default.
	if (MeshComp->GetNumMaterials() == 0 || !MeshComp->GetMaterial(0))
	{
		UMaterialInterface* DefaultMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (DefaultMat)
		{
			for (int32 i = 0; i < FMath::Max(1, MeshComp->GetNumMaterials()); i++)
			{
				MeshComp->SetMaterial(i, DefaultMat);
			}
		}
	}

	// Remove weapon child actor - Sentinel doesn't need the default Blueprint weapon
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
		UPDA_CombatReactionAnimData* ReactionData = LoadObject<UPDA_CombatReactionAnimData>(nullptr, *(SentinelDir + TEXT("PDA_Sentinel_CombatReaction")));
		UPDA_PoiseBreakAnimData* PoiseData = LoadObject<UPDA_PoiseBreakAnimData>(nullptr, *(SentinelDir + TEXT("PDA_Sentinel_PoiseBreak")));

		if (ReactionData) CombatManagerComponent->ReactionAnimset = ReactionData;
		if (PoiseData) CombatManagerComponent->PoiseBreakAsset = PoiseData;

		// Apply bleed status effect to all attacks
		UPrimaryDataAsset* BleedAsset = LoadObject<UPrimaryDataAsset>(nullptr,
			TEXT("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed"));
		if (BleedAsset)
		{
			FSLFStatusEffectApplication BleedApp;
			BleedApp.Rank = 1;
			BleedApp.BuildupAmount = 40.0;
			CombatManagerComponent->DefaultAttackStatusEffects.Add(BleedAsset, BleedApp);
		}

		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] CombatManager configured: Reaction=%s Poise=%s"),
			ReactionData ? TEXT("y") : TEXT("n"), PoiseData ? TEXT("y") : TEXT("n"));
	}

	ConfigureAbilities();

	// Cache locomotion montages
	IdleMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Idle")));
	WalkMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Walk")));
	RunMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Run")));

	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Locomotion montages: Idle=%s Walk=%s Run=%s AnimBP=%s"),
		IdleMontage ? TEXT("y") : TEXT("n"), WalkMontage ? TEXT("y") : TEXT("n"), RunMontage ? TEXT("y") : TEXT("n"),
		SentinelAnimBPClass ? TEXT("y") : TEXT("n"));
}

void ASLFEnemySentinel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TicksAfterBeginPlay++;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Deferred AnimBP application — wait a few ticks for mesh to fully initialize
	if (!bAnimBPApplied && TicksAfterBeginPlay > 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst && SentinelAnimBPClass)
		{
			MeshComp->SetAnimInstanceClass(SentinelAnimBPClass);
			MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MeshComp->InitAnim(true);
			AnimInst = MeshComp->GetAnimInstance();
		}

		if (AnimInst)
		{
			bAnimBPApplied = true;

			// Start idle montage immediately to suppress base AnimGraph BlendSpacePlayer (Bug #16)
			if (IdleMontage)
			{
				AnimInst->Montage_Play(IdleMontage, 1.0f);
				AnimInst->Montage_SetNextSection(FName(TEXT("Default")), FName(TEXT("Default")), IdleMontage);
				ActiveLocomotionMontage = IdleMontage;
				bMontageLocomotionActive = true;
			}

			if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>())
			{
				AISMComp->RefreshCachedAnimInstance();
			}

			UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] AnimBP applied, idle montage started (tick %d)"), TicksAfterBeginPlay);
		}
	}

	// Fix montage durations at runtime (PostLoad DataModel overwrites SequenceLength)
	if (bAnimBPApplied && !bMontageDurationsFixed)
	{
		FixMontageDurations();
	}

	// ---- Montage-Based Locomotion (Guard pattern: seamless loop) ----
	// Montage_SetNextSection("Default","Default") makes montages loop without crossfade hitch.
	// Identity comparison on montage pointers detects locomotion vs attack montages.
	if (bAnimBPApplied && MeshComp)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			float Speed = 0.0f;
			if (GetCharacterMovement())
			{
				Speed = GetCharacterMovement()->Velocity.Size2D();
			}

			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			bool bIsLocomotionMontage = (CurrentMontage == IdleMontage || CurrentMontage == WalkMontage || CurrentMontage == RunMontage);
			bool bIsAttackOrOther = (CurrentMontage != nullptr && !bIsLocomotionMontage);

			// Don't interrupt attack/hit-react/dodge montages
			if (!bIsAttackOrOther)
			{
				UAnimMontage* DesiredMontage = nullptr;
				if (Speed > 300.0f && RunMontage)
				{
					DesiredMontage = RunMontage;
				}
				else if (Speed > 30.0f && WalkMontage)
				{
					DesiredMontage = WalkMontage;
				}
				else if (IdleMontage)
				{
					DesiredMontage = IdleMontage;
				}

				if (DesiredMontage && DesiredMontage != ActiveLocomotionMontage)
				{
					if (ActiveLocomotionMontage)
					{
						AnimInst->Montage_Stop(0.2f, ActiveLocomotionMontage);
					}
					AnimInst->Montage_Play(DesiredMontage, 1.0f);
					AnimInst->Montage_SetNextSection(FName(TEXT("Default")), FName(TEXT("Default")), DesiredMontage);
					ActiveLocomotionMontage = DesiredMontage;

					if (!bMontageLocomotionActive)
					{
						bMontageLocomotionActive = true;
						UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Montage locomotion started (tick %d, speed=%.0f)"),
							TicksAfterBeginPlay, Speed);
					}
				}
			}
			else
			{
				// Attack/other montage is playing — clear tracking so we re-evaluate after it ends
				ActiveLocomotionMontage = nullptr;
			}
		}
	}
}

void ASLFEnemySentinel::ConfigureAbilities()
{
	if (!CombatManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] No CombatManagerComponent for abilities"));
		return;
	}

	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");

	struct FAbilityConfig { const TCHAR* AssetName; float MaxDist; float Weight; bool bGapCloser; };
	const FAbilityConfig Configs[] = {
		{ TEXT("DA_Sentinel_Attack01"),      400.0f, 1.0f,  false },
		{ TEXT("DA_Sentinel_Attack02"),      400.0f, 1.0f,  false },
		{ TEXT("DA_Sentinel_Attack03_Fast"), 350.0f, 1.5f,  false },
		{ TEXT("DA_Sentinel_HeavyAttack"),   500.0f, 0.5f,  true  },
	};

	TArray<FSLFAIAbility> NewAbilities;
	for (const auto& Cfg : Configs)
	{
		UDataAsset* AbilityAsset = LoadObject<UDataAsset>(nullptr, *(SentinelDir + Cfg.AssetName));
		if (!AbilityAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Could not load ability: %s"), Cfg.AssetName);
			continue;
		}

		FSLFAIAbility Ability;
		Ability.AbilityAsset = AbilityAsset;
		Ability.Weight = Cfg.Weight;
		Ability.MaxDistance = Cfg.MaxDist;
		Ability.bIsGapCloser = Cfg.bGapCloser;
		NewAbilities.Add(Ability);
	}

	CombatManagerComponent->Abilities = NewAbilities;
	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Configured %d AI abilities"), NewAbilities.Num());
}

void ASLFEnemySentinel::FixMontageDurations()
{
	bMontageDurationsFixed = true;

	static FProperty* SeqLenProp = UAnimSequenceBase::StaticClass()->FindPropertyByName(TEXT("SequenceLength"));
	if (!SeqLenProp) return;

	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");
	const TCHAR* MontageNames[] = {
		TEXT("AM_Sentinel_Attack01"), TEXT("AM_Sentinel_Attack02"),
		TEXT("AM_Sentinel_Attack03_Fast"), TEXT("AM_Sentinel_HeavyAttack"),
		TEXT("AM_Sentinel_HitReact"), TEXT("AM_Sentinel_HitReact_Light"),
		TEXT("AM_Sentinel_GuardHit"),
		TEXT("AM_Sentinel_Death_Front"), TEXT("AM_Sentinel_Death_Back"), TEXT("AM_Sentinel_Death_Left"),
		TEXT("AM_Sentinel_Dodge_Fwd"), TEXT("AM_Sentinel_Dodge_Bwd"),
		TEXT("AM_Sentinel_Dodge_Left"), TEXT("AM_Sentinel_Dodge_Right"),
		TEXT("AM_Sentinel_Idle"), TEXT("AM_Sentinel_Walk"), TEXT("AM_Sentinel_Run"),
	};

	int32 FixedCount = 0;
	for (const TCHAR* Name : MontageNames)
	{
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + Name));
		if (!Montage) continue;

		float CurrentLen = Montage->GetPlayLength();
		float CalcLen = Montage->CalculateSequenceLength();

		if (CalcLen > 0.01f && FMath::Abs(CurrentLen - CalcLen) > 0.001f)
		{
			float* LenPtr = SeqLenProp->ContainerPtrToValuePtr<float>(Montage);
			if (LenPtr)
			{
				*LenPtr = CalcLen;
				FixedCount++;
			}
		}
		else if (CalcLen <= 0.01f)
		{
			for (int32 t = 0; t < Montage->SlotAnimTracks.Num(); t++)
			{
				for (int32 s = 0; s < Montage->SlotAnimTracks[t].AnimTrack.AnimSegments.Num(); s++)
				{
					FAnimSegment& Seg = Montage->SlotAnimTracks[t].AnimTrack.AnimSegments[s];
					UAnimSequenceBase* Ref = Seg.GetAnimReference();
					if (Ref && Ref->GetPlayLength() > 0.01f)
					{
						float RefLen = Ref->GetPlayLength();
						Seg.AnimEndTime = RefLen;
						float NewCalcLen = Montage->CalculateSequenceLength();
						if (NewCalcLen > 0.01f)
						{
							float* LenPtr = SeqLenProp->ContainerPtrToValuePtr<float>(Montage);
							if (LenPtr) *LenPtr = NewCalcLen;
							FixedCount++;
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Montage duration fix: %d/%d montages patched"),
		FixedCount, UE_ARRAY_COUNT(MontageNames));
}
