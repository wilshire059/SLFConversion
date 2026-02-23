// SLFEnemySentinel.cpp
// C++ implementation for B_Soulslike_Enemy_Sentinel
// Forensically distinct custom enemy using Ironbound Warlord mesh and ARP-retargeted animations

#include "SLFEnemySentinel.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "UObject/UnrealType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"
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
	UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] BeginPlay: %s"), *GetName());

	ApplySentinelConfig();
}

void ASLFEnemySentinel::ApplySentinelConfig()
{
	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Load Sentinel mesh — use Manny reskin (known working with ABP_Sentinel skeleton).
	// Ironbound Warlord mesh requires matching bone count; when ready, add as first choice.
	USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir + TEXT("SKM_Sentinel")));
	if (!SentinelMesh)
	{
		SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir + TEXT("SKM_Sentinel_Ironbound")));
	}
	if (!SentinelMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] No Sentinel mesh found, keeping default"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] Using mesh: %s"), *SentinelMesh->GetName());

	// Skeleton is set persistently on the mesh asset by SLF.SetMeshSkeleton commandlet.
	// Do NOT call SetSkeleton() at runtime — it modifies the shared asset in memory
	// but the mesh component doesn't rebuild its bone mapping, causing AnimBP init to fail.
	USkeleton* MeshSkeleton = SentinelMesh->GetSkeleton();
	UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] Mesh skeleton: %s (%d bones)"),
		MeshSkeleton ? *MeshSkeleton->GetName() : TEXT("NULL"),
		MeshSkeleton ? MeshSkeleton->GetReferenceSkeleton().GetNum() : 0);

	// CRITICAL: Set AnimBP class BEFORE changing the mesh.
	// SetSkeletalMeshAsset triggers a DEFERRED anim reinit on the next tick.
	// That reinit uses whatever AnimClass is on the component at that time.
	// If AnimClass is null, the deferred reinit creates no AnimInstance.
	// By setting it first, the deferred reinit picks up ABP_Sentinel automatically.
	SentinelAnimBPClass = LoadClass<UAnimInstance>(nullptr, *(SentinelDir + TEXT("ABP_Sentinel.ABP_Sentinel_C")));
	if (SentinelAnimBPClass)
	{
		MeshComp->SetAnimInstanceClass(SentinelAnimBPClass);
		MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] AnimBP set BEFORE mesh change: %s"), *SentinelAnimBPClass->GetName());
	}

	MeshComp->SetSkeletalMeshAsset(SentinelMesh);
	// Mesh is meter-scale (1 FBX unit = 1 UE unit = 1cm at import).
	// At scale 1.0, character is ~2.68cm tall. Scale MESH only (not actor) to preserve capsule physics.
	// 139x = 76 * 1.83 (user-requested size increase). Do NOT use SetActorScale3D - breaks capsule collision.
	MeshComp->SetRelativeScale3D(FVector(139.0f));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// Disable mesh collision - capsule handles all character collision.
	// At 76x scale, mesh physics bodies would conflict with the floor.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetBoundsScale(200.0f);
	MeshComp->bEnableUpdateRateOptimizations = false;
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

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
				UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Removed weapon child actor: %s"), *CAC->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Data assets applied"));
	}

	ConfigureAbilities();

	// AnimBP was set before mesh change (above). Log status.
	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] AnimBP class: %s"),
		SentinelAnimBPClass ? *SentinelAnimBPClass->GetName() : TEXT("NOT FOUND"));

	// Cache locomotion montages
	IdleMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Idle")));
	WalkMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Walk")));
	RunMontage = LoadObject<UAnimMontage>(nullptr, *(SentinelDir + TEXT("AM_Sentinel_Run")));
	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Locomotion montages: Idle=%s Walk=%s Run=%s"),
		IdleMontage ? TEXT("yes") : TEXT("no"),
		WalkMontage ? TEXT("yes") : TEXT("no"),
		RunMontage ? TEXT("yes") : TEXT("no"));

	// Log skeleton + animation summary
	if (USkeletalMeshComponent* MC = GetMesh())
	{
		if (USkeletalMesh* SM = MC->GetSkeletalMeshAsset())
		{
			USkeleton* Skel = SM->GetSkeleton();
			UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] Mesh=%s Skeleton=%s Bones=%d"),
				*SM->GetName(),
				Skel ? *Skel->GetName() : TEXT("NULL"),
				Skel ? Skel->GetReferenceSkeleton().GetNum() : 0);
		}
	}
}

void ASLFEnemySentinel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TicksAfterBeginPlay++;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Re-apply AnimBP until it sticks (deferred reinit from SetSkeletalMeshAsset kills it)
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
			UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] AnimInstance ready: %s (tick %d)"),
				*AnimInst->GetClass()->GetName(), TicksAfterBeginPlay);
			bAnimBPApplied = true;

			// Refresh AI state machine's cached AnimInstance (it cached NULL during BeginPlay)
			if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>())
			{
				AISMComp->RefreshCachedAnimInstance();
			}
		}
	}

	// Fix montage durations at runtime (PostLoad DataModel overwrites SequenceLength)
	if (bAnimBPApplied && !bMontageDurationsFixed)
	{
		FixMontageDurations();
	}

	// ---- Montage-Based Locomotion (matches Guard pattern exactly) ----
	// AI state machine handles movement/combat. We just switch locomotion montages
	// based on actual velocity, same as the Guard.
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
			bool bIsLocomotionMontage = (CurrentMontage == WalkMontage || CurrentMontage == RunMontage || CurrentMontage == IdleMontage);
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
				// Attack/other montage is playing - clear tracking so we re-evaluate after it ends
				ActiveLocomotionMontage = nullptr;
			}
		}
	}

	// One-shot diagnostic
	if (!bDiagDone && TicksAfterBeginPlay > 10 && MeshComp)
	{
		bDiagDone = true;
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] DIAG: AnimBP=%s AnimInst=%s Scale=%.1f Bones=%d"),
			bAnimBPApplied ? TEXT("true") : TEXT("false"),
			AnimInst ? *AnimInst->GetClass()->GetName() : TEXT("NULL"),
			MeshComp->GetRelativeScale3D().X,
			MeshComp->GetNumBones());
	}

	// Periodic movement diagnostic (limited)
	if (bAnimBPApplied && (TicksAfterBeginPlay % 120 == 0) && MeshComp && MoveDiagCount < 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			float Speed = GetCharacterMovement() ? GetCharacterMovement()->Velocity.Size2D() : 0.0f;
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();

			ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			float DistToPlayer = PlayerChar ? FVector::Dist(GetActorLocation(), PlayerChar->GetActorLocation()) : -1.0f;

			MoveDiagCount++;
			UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] MOVE[%d]: Speed=%.0f DistToPlayer=%.0f Montage=%s ActorZ=%.1f"),
				MoveDiagCount, Speed, DistToPlayer,
				CurrentMontage ? *CurrentMontage->GetName() : TEXT("none"),
				GetActorLocation().Z);
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

	struct FAbilityConfig { const TCHAR* AssetName; float MaxDist; bool bGapCloser; };
	const FAbilityConfig Configs[] = {
		{ TEXT("DA_Sentinel_Attack01"),      400.0f, false },
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
		Ability.Weight = 1.0f;
		Ability.MaxDistance = Cfg.MaxDist;
		Ability.bIsGapCloser = Cfg.bGapCloser;
		NewAbilities.Add(Ability);
	}

	CombatManagerComponent->Abilities = NewAbilities;
	UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Configured %d AI abilities"), NewAbilities.Num());
}

void ASLFEnemySentinel::ApplyMontageLocomotionFallback()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp || !IdleMontage) return;

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_Play(IdleMontage, 1.0f);
		bMontageLocomotionActive = true;
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Montage locomotion fallback activated"));
	}
}

void ASLFEnemySentinel::FixMontageDurations()
{
	bMontageDurationsFixed = true;

	static FProperty* SeqLenProp = UAnimSequenceBase::StaticClass()->FindPropertyByName(TEXT("SequenceLength"));
	if (!SeqLenProp) return;

	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");
	const TCHAR* MontageNames[] = {
		TEXT("AM_Sentinel_Attack01"), TEXT("AM_Sentinel_Attack02"),
		TEXT("AM_Sentinel_Attack03_Fast"),
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
		if (!Montage)
		{
			continue;
		}

		float CurrentLen = Montage->GetPlayLength();
		float CalcLen = Montage->CalculateSequenceLength();

		if (CalcLen > 0.01f && FMath::Abs(CurrentLen - CalcLen) > 0.001f)
		{
			float* LenPtr = SeqLenProp->ContainerPtrToValuePtr<float>(Montage);
			if (LenPtr)
			{
				*LenPtr = CalcLen;
				FixedCount++;
				UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Fixed montage %s: %.3f -> %.3f sec"),
					Name, CurrentLen, CalcLen);
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
