// SLFEnemyGuard.cpp
// C++ implementation for B_Soulslike_Enemy_Guard

#include "SLFEnemyGuard.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "UObject/UnrealType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ChildActorComponent.h"

ASLFEnemyGuard::ASLFEnemyGuard()
{
}

void ASLFEnemyGuard::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[EnemyGuard] BeginPlay: %s"), *GetName());
	ApplyC3100Config();
}

void ASLFEnemyGuard::ApplyC3100Config()
{
	const FString C3100Dir = TEXT("/Game/EldenRingAnimations/c3100_guard/");

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	USkeletalMesh* C3100Mesh = LoadObject<USkeletalMesh>(nullptr, *(C3100Dir + TEXT("c3100_mesh")));
	if (!C3100Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] c3100_mesh not found, keeping default"));
		return;
	}

	MeshComp->SetSkeletalMeshAsset(C3100Mesh);
	MeshComp->SetRelativeScale3D(FVector(1.0f));
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	MeshComp->SetBoundsScale(200.0f);
	MeshComp->bEnableUpdateRateOptimizations = false;
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// Assign material
	UMaterialInterface* DefaultMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (DefaultMat)
	{
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); i++)
		{
			MeshComp->SetMaterial(i, DefaultMat);
		}
	}

	// Remove weapon child actor - the c3100 FLVER mesh has a weapon built into the geometry.
	// The Blueprint SCS has a "Weapon" ChildActorComponent that spawns B_Item_AI_Weapon_Greatsword.
	// We don't need it since the mesh already has the sword modeled in.
	{
		TArray<UChildActorComponent*> ChildActorComps;
		GetComponents<UChildActorComponent>(ChildActorComps);
		for (UChildActorComponent* CAC : ChildActorComps)
		{
			if (CAC && CAC->GetName().Contains(TEXT("Weapon")))
			{
				CAC->DestroyChildActor();
				CAC->DestroyComponent();
				UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Removed weapon child actor: %s"), *CAC->GetName());
			}
		}
	}

	// Set data assets on CombatManager
	if (CombatManagerComponent)
	{
		UPDA_CombatReactionAnimData* ReactionData = LoadObject<UPDA_CombatReactionAnimData>(nullptr, *(C3100Dir + TEXT("PDA_c3100_CombatReaction")));
		UPDA_PoiseBreakAnimData* PoiseData = LoadObject<UPDA_PoiseBreakAnimData>(nullptr, *(C3100Dir + TEXT("PDA_c3100_PoiseBreak")));

		if (ReactionData) CombatManagerComponent->ReactionAnimset = ReactionData;
		if (PoiseData) CombatManagerComponent->PoiseBreakAsset = PoiseData;
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Data assets applied"));
	}

	ConfigureAbilities();

	// Cache the AnimBP class - apply in Tick after deferred reinit (Bug #8)
	C3100AnimBPClass = LoadClass<UAnimInstance>(nullptr, *(C3100Dir + TEXT("ABP_c3100_Guard.ABP_c3100_Guard_C")));
	UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] AnimBP class: %s"),
		C3100AnimBPClass ? *C3100AnimBPClass->GetName() : TEXT("NOT FOUND"));

	// Cache locomotion montages (montage evaluation path produces correct cm-scale bone transforms,
	// unlike BlendSpacePlayer which collapses to meter-scale)
	IdleMontage = LoadObject<UAnimMontage>(nullptr, *(C3100Dir + TEXT("AM_c3100_Idle")));
	WalkMontage = LoadObject<UAnimMontage>(nullptr, *(C3100Dir + TEXT("AM_c3100_Walk")));
	RunMontage = LoadObject<UAnimMontage>(nullptr, *(C3100Dir + TEXT("AM_c3100_Run")));
	UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Locomotion montages: Idle=%s Walk=%s Run=%s"),
		IdleMontage ? TEXT("yes") : TEXT("no"),
		WalkMontage ? TEXT("yes") : TEXT("no"),
		RunMontage ? TEXT("yes") : TEXT("no"));
}

void ASLFEnemyGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TicksAfterBeginPlay++;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Re-apply AnimBP until it sticks (deferred reinit from SetSkeletalMeshAsset kills it)
	if (C3100AnimBPClass && TicksAfterBeginPlay > 5)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst || !AnimInst->IsA(C3100AnimBPClass))
		{
			MeshComp->SetAnimInstanceClass(C3100AnimBPClass);
			MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MeshComp->InitAnim(true);

			AnimInst = MeshComp->GetAnimInstance();
			if (AnimInst && !bAnimBPApplied)
			{
				UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] AnimBP applied (tick %d)"), TicksAfterBeginPlay);
				bAnimBPApplied = true;

				// Refresh AI state machine's cached AnimInstance (it cached the old one during BeginPlay)
				if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>())
				{
					AISMComp->RefreshCachedAnimInstance();
				}
			}
		}
	}

	// Fix montage durations at runtime (PostLoad DataModel overwrites SequenceLength)
	if (bAnimBPApplied && !bMontageDurationsFixed)
	{
		FixMontageDurations();
	}

	// ---- Montage-Based Locomotion ----
	// BlendSpacePlayer evaluation collapses bones to ~1/100 scale (meter vs cm).
	// Montage/SequencePlayer evaluation works correctly. So we drive locomotion
	// through montages played on the DefaultSlot, which overrides the state machine output.
	if (bAnimBPApplied && MeshComp)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			// Get actual movement speed from the character
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
					// Switch to new locomotion montage
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
						UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Montage locomotion started (tick %d, speed=%.0f)"),
							TicksAfterBeginPlay, Speed);
					}
				}
			}
			else
			{
				// Attack/other montage is playing - clear our tracking so we re-evaluate after it ends
				ActiveLocomotionMontage = nullptr;
			}
		}
	}

	// One-shot diagnostic
	if (!bDiagDone && TicksAfterBeginPlay > 10 && MeshComp)
	{
		bDiagDone = true;
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		FBoxSphereBounds Bounds = MeshComp->CalcBounds(MeshComp->GetComponentTransform());
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] DIAG: AnimBPApplied=%s AnimInst=%s Scale=(%.1f) BoundsExtent=(%.2f, %.2f, %.2f)"),
			bAnimBPApplied ? TEXT("true") : TEXT("false"),
			AnimInst ? *AnimInst->GetClass()->GetName() : TEXT("NULL"),
			MeshComp->GetRelativeScale3D().X,
			Bounds.BoxExtent.X, Bounds.BoxExtent.Y, Bounds.BoxExtent.Z);
	}

	// Periodic movement diagnostic (limited)
	if (bAnimBPApplied && (TicksAfterBeginPlay % 120 == 0) && MeshComp && MoveDiagCount < 10)
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			float Speed = GetCharacterMovement() ? GetCharacterMovement()->Velocity.Size2D() : 0.0f;
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();

			int32 NumBones = MeshComp->GetNumBones();
			FVector MinBone(BIG_NUMBER), MaxBone(-BIG_NUMBER);
			for (int32 i = 0; i < NumBones; i++)
			{
				FVector Loc = MeshComp->GetBoneTransform(i).GetLocation();
				MinBone = MinBone.ComponentMin(Loc);
				MaxBone = MaxBone.ComponentMax(Loc);
			}

			MoveDiagCount++;
			UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] MOVE_DIAG[%d]: Speed=%.0f Montage=%s SkeletonH=%.2f ActorZ=%.1f"),
				TicksAfterBeginPlay, Speed,
				CurrentMontage ? *CurrentMontage->GetName() : TEXT("none"),
				MaxBone.Z - MinBone.Z,
				GetActorLocation().Z);
		}
	}
}

void ASLFEnemyGuard::ConfigureAbilities()
{
	if (!CombatManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] No CombatManagerComponent for abilities"));
		return;
	}

	const FString C3100Dir = TEXT("/Game/EldenRingAnimations/c3100_guard/");

	struct FAbilityConfig { const TCHAR* AssetName; float MaxDist; bool bGapCloser; };
	const FAbilityConfig Configs[] = {
		{ TEXT("DA_c3100_Attack01"),      400.0f, false },
		{ TEXT("DA_c3100_Attack02"),      400.0f, false },
		{ TEXT("DA_c3100_Attack03_Fast"), 300.0f, false },
		{ TEXT("DA_c3100_HeavyAttack"),   350.0f, false },
	};

	TArray<FSLFAIAbility> NewAbilities;
	for (const auto& Cfg : Configs)
	{
		UDataAsset* AbilityAsset = LoadObject<UDataAsset>(nullptr, *(C3100Dir + Cfg.AssetName));
		if (!AbilityAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Could not load ability: %s"), Cfg.AssetName);
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
	UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Configured %d AI abilities"), NewAbilities.Num());
}

void ASLFEnemyGuard::ApplyMontageLocomotionFallback()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp || !IdleMontage) return;

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_Play(IdleMontage, 1.0f);
		bMontageLocomotionActive = true;
		UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Montage locomotion fallback activated"));
	}
}

void ASLFEnemyGuard::FixMontageDurations()
{
	bMontageDurationsFixed = true;

	// UAnimMontage::GetPlayLength() returns cached SequenceLength which may be 0
	// after PostLoad if the DataModel overwrites it. Fix by recalculating from segments.
	static FProperty* SeqLenProp = UAnimSequenceBase::StaticClass()->FindPropertyByName(TEXT("SequenceLength"));
	if (!SeqLenProp) return;

	const FString C3100Dir = TEXT("/Game/EldenRingAnimations/c3100_guard/");
	const TCHAR* MontageNames[] = {
		TEXT("AM_c3100_Attack01"), TEXT("AM_c3100_Attack02"),
		TEXT("AM_c3100_Attack03_Fast"), TEXT("AM_c3100_HeavyAttack"),
		TEXT("AM_c3100_HitReact"), TEXT("AM_c3100_HitReact_Light"),
		TEXT("AM_c3100_GuardHit"),
		TEXT("AM_c3100_Death_Front"), TEXT("AM_c3100_Death_Back"), TEXT("AM_c3100_Death_Left"),
		TEXT("AM_c3100_Dodge_Fwd"), TEXT("AM_c3100_Dodge_Bwd"),
		TEXT("AM_c3100_Dodge_Left"), TEXT("AM_c3100_Dodge_Right"),
		TEXT("AM_c3100_Idle"), TEXT("AM_c3100_Walk"), TEXT("AM_c3100_Run"),
	};

	int32 FixedCount = 0;
	for (const TCHAR* Name : MontageNames)
	{
		UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr, *(C3100Dir + Name));
		if (!Montage)
		{
			UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Montage %s: NOT FOUND"), Name);
			continue;
		}

		float CurrentLen = Montage->GetPlayLength();
		float CalcLen = Montage->CalculateSequenceLength();

		// Always log first 3 montages for diagnostics
		static int32 DiagCount = 0;
		if (DiagCount < 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] DIAG %s: GetPlayLength=%.3f CalcSeqLen=%.3f Tracks=%d"),
				Name, CurrentLen, CalcLen, Montage->SlotAnimTracks.Num());
			for (int32 t = 0; t < Montage->SlotAnimTracks.Num(); t++)
			{
				const FSlotAnimationTrack& Track = Montage->SlotAnimTracks[t];
				UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard]   Track[%d] Slot=%s Segments=%d"),
					t, *Track.SlotName.ToString(), Track.AnimTrack.AnimSegments.Num());
				for (int32 s = 0; s < Track.AnimTrack.AnimSegments.Num(); s++)
				{
					const FAnimSegment& Seg = Track.AnimTrack.AnimSegments[s];
					UAnimSequenceBase* Ref = Seg.GetAnimReference();
					UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard]     Seg[%d] Ref=%s Start=%.3f AnimStart=%.3f AnimEnd=%.3f Rate=%.1f Loops=%d"),
						s, Ref ? *Ref->GetName() : TEXT("NULL"),
						Seg.StartPos, Seg.AnimStartTime, Seg.AnimEndTime, Seg.AnimPlayRate, Seg.LoopingCount);
					if (Ref)
					{
						UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard]     Seg[%d] RefPlayLen=%.3f RefSkeleton=%s"),
							s, Ref->GetPlayLength(),
							Ref->GetSkeleton() ? *Ref->GetSkeleton()->GetName() : TEXT("NULL"));
					}
				}
			}
			DiagCount++;
		}

		if (CalcLen > 0.01f && FMath::Abs(CurrentLen - CalcLen) > 0.001f)
		{
			float* LenPtr = SeqLenProp->ContainerPtrToValuePtr<float>(Montage);
			if (LenPtr)
			{
				*LenPtr = CalcLen;
				FixedCount++;
				UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Fixed montage %s: %.3f -> %.3f sec"),
					Name, CurrentLen, CalcLen);
			}
		}
		else if (CalcLen <= 0.01f)
		{
			// CalcLen is also 0 - segments are broken. Force-set from source sequence.
			const FString AnimDir = C3100Dir + TEXT("Animations/");
			// Try to find the source sequence and get its length
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
							UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Fixed montage %s (segment): %.3f -> %.3f sec"),
								Name, CurrentLen, NewCalcLen);
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[EnemyGuard] Montage duration fix: %d/%d montages patched"),
		FixedCount, UE_ARRAY_COUNT(MontageNames));
}
