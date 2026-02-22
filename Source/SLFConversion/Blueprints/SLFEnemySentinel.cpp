// SLFEnemySentinel.cpp
// C++ implementation for B_Soulslike_Enemy_Sentinel
// Forensically distinct clone of the Guard enemy using Manny mesh and baked animations

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
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationInvokerComponent.h"
#include "AIController.h"

ASLFEnemySentinel::ASLFEnemySentinel()
{
	// Fresh Blueprint has no SCS components - create essential ones in C++
	// (Bug #3 doesn't apply here: Blueprint SCS is empty, not inherited from Guard)
	if (!FindComponentByClass<UAICombatManagerComponent>())
	{
		CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("SentinelCombatManager"));
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

	// Fix capsule collision - Sentinel BP was created from C++ (CreateBlueprint), so it
	// doesn't inherit the SCS collision overrides from B_Soulslike_Enemy Blueprint.
	// The Guard's capsule has Profile=Custom with BlockAll. Without this, the Sentinel
	// falls through the floor because the default Pawn profile doesn't match level geometry.
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCapsuleHalfHeight(90.0f);
		Capsule->SetCapsuleRadius(35.0f);
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionObjectType(ECC_Pawn);
		Capsule->SetCollisionResponseToAllChannels(ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] Capsule collision configured: R=35 HH=90 BlockAll"));
	}

	// Log NavSystem status (NavigationInvokerComponent handles dynamic NavMesh)
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		UE_LOG(LogTemp, Log, TEXT("[EnemySentinel] NavSystem found, invoker active"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] No NavigationSystem - will use direct movement"));
	}

	// Set up movement component for direct movement
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 300.0f;
		MoveComp->MaxAcceleration = 2048.0f;
		MoveComp->BrakingDecelerationWalking = 1000.0f;
		MoveComp->bOrientRotationToMovement = false; // We handle rotation manually
	}

	ApplySentinelConfig();
}

void ASLFEnemySentinel::ApplySentinelConfig()
{
	const FString SentinelDir = TEXT("/Game/CustomEnemies/Sentinel/");

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	USkeletalMesh* SentinelMesh = LoadObject<USkeletalMesh>(nullptr, *(SentinelDir + TEXT("SKM_Sentinel")));
	if (!SentinelMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] SKM_Sentinel not found, keeping default"));
		return;
	}

	// Fix skeleton if missing (Bug #12: skeleton lives in separate package, can lose link)
	if (!SentinelMesh->GetSkeleton())
	{
		// Try common auto-names from FBX import
		const TCHAR* SkelNames[] = { TEXT("SKM_Sentinel_Skeleton"), TEXT("Sentinel_Skeleton") };
		for (const TCHAR* SkelName : SkelNames)
		{
			USkeleton* SentinelSkel = LoadObject<USkeleton>(nullptr, *(SentinelDir + SkelName));
			if (SentinelSkel)
			{
				SentinelMesh->SetSkeleton(SentinelSkel);
				UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] Fixed missing skeleton -> %s"), SkelName);
				break;
			}
		}
		if (!SentinelMesh->GetSkeleton())
		{
			UE_LOG(LogTemp, Error, TEXT("[EnemySentinel] No skeleton found! AnimBP will fail."));
		}
	}

	MeshComp->SetSkeletalMeshAsset(SentinelMesh);
	// Mesh is meter-scale (1 FBX unit = 1 UE unit = 1cm at import).
	// At scale 1.0, character is ~2.68cm tall. Scale MESH only (not actor) to preserve capsule physics.
	// 76x gives SkeletonH ~204 (Guard is ~212). Do NOT use SetActorScale3D - it breaks capsule collision.
	MeshComp->SetRelativeScale3D(FVector(76.0f));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
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

	// Cache the AnimBP class - apply in Tick after deferred reinit (Bug #8)
	SentinelAnimBPClass = LoadClass<UAnimInstance>(nullptr, *(SentinelDir + TEXT("ABP_Sentinel.ABP_Sentinel_C")));
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

	// Re-apply AnimBP until it sticks (matches Guard pattern - deferred reinit kills it)
	if (SentinelAnimBPClass && TicksAfterBeginPlay > 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst || !AnimInst->IsA(SentinelAnimBPClass))
		{
			MeshComp->SetAnimInstanceClass(SentinelAnimBPClass);
			MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MeshComp->InitAnim(true);

			AnimInst = MeshComp->GetAnimInstance();
			if (AnimInst && !bAnimBPApplied)
			{
				bAnimBPApplied = true;
				UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] AnimBP applied: %s (tick %d)"),
					*AnimInst->GetClass()->GetName(), TicksAfterBeginPlay);

				// Refresh AI state machine's cached AnimInstance (it cached NULL during BeginPlay)
				if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>())
				{
					AISMComp->RefreshCachedAnimInstance();
				}

				// Kick off idle montage
				if (IdleMontage)
				{
					AnimInst->Montage_Play(IdleMontage, 1.0f);
					ActiveLocomotionMontage = IdleMontage;
					bMontageLocomotionActive = true;
				}
			}
		}
	}

	// Fix montage durations at runtime
	if (bAnimBPApplied && !bMontageDurationsFixed)
	{
		FixMontageDurations();
	}

	// ---- Direct Movement + Montage Locomotion (combined) ----
	// Montage selection is driven by TickDirectMovement since it knows the chase state
	if (bAnimBPApplied)
	{
		TickDirectMovement(DeltaTime);
	}

	// One-shot diagnostic at tick 15
	if (!bDiagDone && TicksAfterBeginPlay > 14 && MeshComp)
	{
		bDiagDone = true;
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] DIAG: AnimBP=%s AnimInst=%s Scale=%.1f Bones=%d AnimMode=%d MoveMode=%d MaxSpeed=%.0f"),
			bAnimBPApplied ? TEXT("yes") : TEXT("no"),
			AnimInst ? *AnimInst->GetClass()->GetName() : TEXT("NULL"),
			MeshComp->GetRelativeScale3D().X,
			MeshComp->GetNumBones(),
			(int32)MeshComp->GetAnimationMode(),
			MoveComp ? (int32)MoveComp->MovementMode.GetValue() : -1,
			MoveComp ? MoveComp->MaxWalkSpeed : -1.0f);
	}

	// Periodic movement diagnostic (every 2 seconds, max 5 reports)
	if (bAnimBPApplied && (TicksAfterBeginPlay % 120 == 0) && MeshComp && MoveDiagCount < 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			float Speed = GetCharacterMovement() ? GetCharacterMovement()->Velocity.Size2D() : 0.0f;
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			UCharacterMovementComponent* MoveComp = GetCharacterMovement();

			ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			float DistToPlayer = PlayerChar ? FVector::Dist(GetActorLocation(), PlayerChar->GetActorLocation()) : -1.0f;

			MoveDiagCount++;
			UE_LOG(LogTemp, Warning, TEXT("[EnemySentinel] MOVE[%d]: Speed=%.0f DistToPlayer=%.0f Montage=%s ActorZ=%.1f Falling=%d MoveMode=%d NavMeshOK=%d"),
				MoveDiagCount, Speed, DistToPlayer,
				CurrentMontage ? *CurrentMontage->GetName() : TEXT("none"),
				GetActorLocation().Z,
				MoveComp ? (int32)MoveComp->IsFalling() : -1,
				MoveComp ? (int32)MoveComp->MovementMode.GetValue() : -1,
				bNavMeshMovementWorking ? 1 : 0);
		}
	}
}

void ASLFEnemySentinel::TickDirectMovement(float DeltaTime)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return;

	// Check what montage is playing
	UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
	bool bIsLocomotionMontage = (CurrentMontage == WalkMontage || CurrentMontage == RunMontage || CurrentMontage == IdleMontage);
	bool bIsAttackOrOther = (CurrentMontage != nullptr && !bIsLocomotionMontage);

	// During attack/reaction montages: clear locomotion tracker, don't move
	if (bIsAttackOrOther)
	{
		ActiveLocomotionMontage = nullptr;
		return;
	}

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerChar) return;

	FVector MyLoc = GetActorLocation();
	FVector PlayerLoc = PlayerChar->GetActorLocation();
	float DistToPlayer = FVector::Dist2D(MyLoc, PlayerLoc);
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// Determine desired locomotion montage based on chase state (not velocity)
	UAnimMontage* DesiredMontage = nullptr;
	if (DistToPlayer > 600.0f && RunMontage)
	{
		DesiredMontage = RunMontage;
	}
	else if (DistToPlayer > 200.0f && WalkMontage)
	{
		DesiredMontage = WalkMontage;
	}
	else if (IdleMontage)
	{
		DesiredMontage = IdleMontage;
	}

	// Switch locomotion montage if needed
	if (DesiredMontage && DesiredMontage != ActiveLocomotionMontage)
	{
		if (ActiveLocomotionMontage)
		{
			AnimInst->Montage_Stop(0.2f, ActiveLocomotionMontage);
		}
		float PlayLen = AnimInst->Montage_Play(DesiredMontage, 1.0f);
		if (PlayLen > 0.0f && DesiredMontage->CompositeSections.Num() > 0)
		{
			FName SectionName = DesiredMontage->CompositeSections[0].SectionName;
			AnimInst->Montage_SetNextSection(SectionName, SectionName, DesiredMontage);
		}
		ActiveLocomotionMontage = DesiredMontage;
	}

	// Re-loop the current locomotion montage if it stopped naturally
	if (!CurrentMontage && ActiveLocomotionMontage)
	{
		float PlayLen = AnimInst->Montage_Play(ActiveLocomotionMontage, 1.0f);
		if (PlayLen > 0.0f && ActiveLocomotionMontage->CompositeSections.Num() > 0)
		{
			FName SectionName = ActiveLocomotionMontage->CompositeSections[0].SectionName;
			AnimInst->Montage_SetNextSection(SectionName, SectionName, ActiveLocomotionMontage);
		}
	}

	if (DistToPlayer > 200.0f)
	{
		// Chase player
		FVector Dir = (PlayerLoc - MyLoc).GetSafeNormal2D();

		// Face movement direction — mesh forward is +Y (Blender convention), so offset by -90° yaw
		FRotator FaceRot = Dir.Rotation();
		FaceRot.Yaw -= 90.0f;
		SetActorRotation(FaceRot);

		float ChaseSpeed = (DistToPlayer > 600.0f) ? 400.0f : 200.0f;

		FVector Delta = Dir * ChaseSpeed * DeltaTime;
		FVector NewLoc = MyLoc + Delta;
		NewLoc.Z = MyLoc.Z;

		FHitResult Hit;
		SetActorLocation(NewLoc, true, &Hit);

		if (MoveComp)
		{
			MoveComp->MaxWalkSpeed = ChaseSpeed;
			MoveComp->Velocity.X = Dir.X * ChaseSpeed;
			MoveComp->Velocity.Y = Dir.Y * ChaseSpeed;
		}
	}
	else
	{
		// In melee range — face player, stop
		FVector Dir = (PlayerLoc - MyLoc).GetSafeNormal2D();
		FRotator FaceRot = Dir.Rotation();
		FaceRot.Yaw -= 90.0f;
		SetActorRotation(FaceRot);

		if (MoveComp)
		{
			MoveComp->Velocity.X = 0.0f;
			MoveComp->Velocity.Y = 0.0f;
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
		{ TEXT("DA_Sentinel_Attack02"),      400.0f, false },
		{ TEXT("DA_Sentinel_Attack03_Fast"), 300.0f, false },
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
