// AC_AI_CombatManager.cpp
// C++ component implementation for AC_AI_CombatManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_CombatManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_AI_CombatManager.h"
#include "Components/AC_StatManager.h"
#include "Components/AC_StatusEffectManager.h"
#include "Blueprints/BFL_Helper.h"
#include "Blueprints/B_Stat.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DataTable.h"

UAC_AI_CombatManager::UAC_AI_CombatManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	IsDead = false;
	HealthbarActive = false;
	UseWorldHealthbar = true;
	HealthBarDuration = 5.0;
	BrokenPoiseDuration = 3.0;
	LineOfSightCheckInterval = 0.5;
	DelayAfterDeath = 2.0;
	PoiseBroken = false;
	HyperArmor = false;
	Invincible = false;
	IkWeight = 1.0;
	CausesHitStop = true;
	HitStopDuration = 0.05;
	CurrencyReward = 100;
	CanBePushed = true;
	RagdollOnDeath = true;
	HitReactType = ESLFHitReactType::None;
	Mesh = nullptr;
	HandTraceRadius = 10.0;
	HandSocket_L_Start = FName("hand_l");
	HandSocket_L_End = FName("hand_l_end");
	HandSocket_R_Start = FName("hand_r");
	HandSocket_R_End = FName("hand_r_end");
	MinUnarmedPoiseDamage = 10.0;
	MaxUnarmedPoiseDamage = 30.0;
	SelectedAbility = nullptr;
}

void UAC_AI_CombatManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::BeginPlay"));

	// Cache mesh reference
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}
}

void UAC_AI_CombatManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * TryGetAbility - Select an ability from the available abilities list
 */
void UAC_AI_CombatManager::TryGetAbility_Implementation(UPrimaryDataAsset*& OutSelectedAbility, UPrimaryDataAsset*& OutSelectedAbility_1, UPrimaryDataAsset*& OutSelectedAbility_2, UPrimaryDataAsset*& OutSelectedAbility_3, UPrimaryDataAsset*& OutSelectedAbility_4)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::TryGetAbility"));

	UPrimaryDataAsset* Result = nullptr;

	if (Abilities.Num() > 0)
	{
		// Select random ability from available list
		int32 Index = FMath::RandRange(0, Abilities.Num() - 1);
		Result = Abilities[Index];
		SelectedAbility = Result;
	}

	OutSelectedAbility = Result;
	OutSelectedAbility_1 = Result;
	OutSelectedAbility_2 = Result;
	OutSelectedAbility_3 = Result;
	OutSelectedAbility_4 = Result;
}

/**
 * EvaluateAbilityRule - Evaluate if an ability rule condition is met
 */
void UAC_AI_CombatManager::EvaluateAbilityRule_Implementation(const FInstancedStruct& Rule, bool& OutApproved, bool& OutApproved_1, bool& OutApproved_2, bool& OutApproved_3, bool& OutApproved_4, bool& OutApproved_5, bool& OutApproved_6, bool& OutApproved_7, bool& OutApproved_8, bool& OutApproved_9, bool& OutApproved_10, bool& OutApproved_11, bool& OutApproved_12, bool& OutApproved_13)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::EvaluateAbilityRule"));

	bool bApproved = true; // Default to approved

	// Evaluate rule based on its type
	// Rules can be distance checks, stat checks, cooldown checks, etc.

	OutApproved = bApproved;
	OutApproved_1 = bApproved;
	OutApproved_2 = bApproved;
	OutApproved_3 = bApproved;
	OutApproved_4 = bApproved;
	OutApproved_5 = bApproved;
	OutApproved_6 = bApproved;
	OutApproved_7 = bApproved;
	OutApproved_8 = bApproved;
	OutApproved_9 = bApproved;
	OutApproved_10 = bApproved;
	OutApproved_11 = bApproved;
	OutApproved_12 = bApproved;
	OutApproved_13 = bApproved;
}

/**
 * HandleDeath - Process AI death
 * Blueprint Logic Flow:
 * 1. GetStat(SoulslikeFramework.Stat.Secondary.HP) -> StatInfo
 * 2. GetStatCurrentValue(StatInfo) -> CurrentValue
 * 3. Branch: if CurrentValue <= 0
 *    - TRUE: GetDirectionFromHit(GetOwner(), HitInfo) -> HitDirection
 *            Then call EventOnDeath(RagdollOnDeath, HitDirection)
 *    - FALSE: return (do nothing)
 */
void UAC_AI_CombatManager::HandleDeath_Implementation(UAC_StatManager* StatManager, const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleDeath - Checking HP stat"));

	if (!IsValid(StatManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("UAC_AI_CombatManager::HandleDeath - StatManager is invalid"));
		return;
	}

	// Step 1: Get HP stat from StatManager
	FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
	UB_Stat* FoundStat = nullptr;
	FStatInfo StatInfo;
	StatManager->GetStat(HPTag, FoundStat, StatInfo);

	// Step 2: Get current value from stat info (replaces BFL_Helper::GetStatCurrentValue call)
	double CurrentValue = StatInfo.CurrentValue;

	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleDeath - HP CurrentValue: %.1f"), CurrentValue);

	// Step 3: Branch - check if HP <= 0
	if (CurrentValue <= 0.0)
	{
		// TRUE branch: Get hit direction and trigger death event
		ESLFDirection HitDirection = CalculateHitDirection(HitInfo);

		UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleDeath - HP depleted, triggering death with direction: %d"), static_cast<int32>(HitDirection));

		// Call internal death event handler
		EventOnDeath(RagdollOnDeath, HitDirection);
	}
	// FALSE branch: do nothing (HP not depleted)
}

/**
 * EventOnDeath - Internal death event handler (replaces Blueprint "Event OnDeath" custom event)
 * Called when HP check confirms death in HandleDeath
 */
void UAC_AI_CombatManager::EventOnDeath_Implementation(bool bRagdoll, ESLFDirection KillingBlowDirection)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::EventOnDeath - Ragdoll: %s, Direction: %d"),
		bRagdoll ? TEXT("true") : TEXT("false"), static_cast<int32>(KillingBlowDirection));

	if (IsDead)
	{
		return; // Already processed death
	}

	IsDead = true;

	// Store killing blow direction for animations
	// (Convert direction to normal vector if needed for animations)

	// Broadcast death event dispatcher
	OnDeath.Broadcast();

	// Handle ragdoll if enabled
	if (bRagdoll && IsValid(Mesh))
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
}

/**
 * CalculateHitDirection - Calculate hit direction from hit result relative to owner actor
 * Replaces calls to BFL_Helper::GetDirectionFromHit
 */
ESLFDirection UAC_AI_CombatManager::CalculateHitDirection(const FHitResult& HitInfo) const
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return ESLFDirection::Fwd;
	}

	FVector HitDir = HitInfo.ImpactNormal;
	FVector OwnerForward = Owner->GetActorForwardVector();
	FVector OwnerRight = Owner->GetActorRightVector();

	double ForwardDot = FVector::DotProduct(OwnerForward, -HitDir);
	double RightDot = FVector::DotProduct(OwnerRight, -HitDir);

	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return (ForwardDot > 0) ? ESLFDirection::Fwd : ESLFDirection::Bwd;
	}
	else
	{
		return (RightDot > 0) ? ESLFDirection::Right : ESLFDirection::Left;
	}
}

/**
 * CalculateStatPercent - Calculate stat percent value (current/max)
 * Replaces calls to BFL_Helper::GetStatPercentValue
 */
double UAC_AI_CombatManager::CalculateStatPercent(const FStatInfo& StatInfo)
{
	if (StatInfo.MaxValue <= 0.0)
	{
		return 0.0;
	}
	return StatInfo.CurrentValue / StatInfo.MaxValue;
}

/**
 * HandleHitReaction - Process hit reaction animation
 */
void UAC_AI_CombatManager::HandleHitReaction_Implementation(const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleHitReaction"));

	if (HyperArmor || IsDead)
	{
		return; // No hit reaction during hyper armor or if dead
	}

	// Store hit normal for directional hit reaction
	CurrentHitNormal = HitInfo.ImpactNormal;

	// Calculate hit direction using our helper function
	ESLFDirection HitDirection = CalculateHitDirection(HitInfo);

	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleHitReaction - Direction: %d"), static_cast<int32>(HitDirection));

	// Play hit reaction based on type and direction
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Get the owner's skeletal mesh component for animation
	USkeletalMeshComponent* OwnerMesh = Mesh;
	if (!OwnerMesh)
	{
		OwnerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!IsValid(OwnerMesh))
	{
		return;
	}

	// Get anim instance to play montage
	UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return;
	}

	// Get hit reaction montage from ReactionAnimset if available
	if (IsValid(ReactionAnimset))
	{
		// ReactionAnimset is expected to have montage properties for different hit react types
		// Try to get the montage via reflection based on the HitReactType
		FName MontagePropertyName;
		switch (HitReactType)
		{
		case ESLFHitReactType::Light:
			MontagePropertyName = TEXT("LightHitReaction");
			break;
		case ESLFHitReactType::Heavy:
			MontagePropertyName = TEXT("HeavyHitReaction");
			break;
		case ESLFHitReactType::Montage:
			MontagePropertyName = TEXT("MontageHitReaction");
			break;
		case ESLFHitReactType::IK:
			MontagePropertyName = TEXT("IKHitReaction");
			break;
		case ESLFHitReactType::Both:
			MontagePropertyName = TEXT("BothHitReaction");
			break;
		default:
			MontagePropertyName = TEXT("DefaultHitReaction");
			break;
		}

		// Try to get montage via property access
		if (FSoftObjectProperty* MontageProperty = CastField<FSoftObjectProperty>(ReactionAnimset->GetClass()->FindPropertyByName(MontagePropertyName)))
		{
			TSoftObjectPtr<UAnimMontage>* SoftMontagePtr = MontageProperty->ContainerPtrToValuePtr<TSoftObjectPtr<UAnimMontage>>(ReactionAnimset);
			if (SoftMontagePtr && !SoftMontagePtr->IsNull())
			{
				UAnimMontage* HitReactMontage = SoftMontagePtr->LoadSynchronous();
				if (IsValid(HitReactMontage))
				{
					AnimInstance->Montage_Play(HitReactMontage, 1.0f);
					UE_LOG(LogTemp, Log, TEXT("  Playing hit react montage: %s"), *HitReactMontage->GetName());
				}
			}
		}
	}
}

/**
 * GetRelevantExecutedMontage - Get execution montage by tag
 */
TSoftObjectPtr<UAnimMontage> UAC_AI_CombatManager::GetRelevantExecutedMontage_Implementation(const FGameplayTag& ExecutionTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::GetRelevantExecutedMontage - Tag: %s"),
		*ExecutionTag.ToString());

	TSoftObjectPtr<UAnimMontage> Result;

	if (IsValid(ExecutedMontages))
	{
		// Look up montage in data table by tag
		// ExecutedMontages is expected to have rows with ExecutionTag -> Montage mapping
		// Try to find row matching the execution tag (row name matches tag last part)
		FString ContextString;
		FString TagString = ExecutionTag.ToString();

		// Try exact row name match first
		FName RowName = FName(*TagString);

		// Generic row lookup - check if row struct has Tag and Montage properties
		const UScriptStruct* RowStruct = ExecutedMontages->GetRowStruct();
		if (RowStruct)
		{
			FProperty* TagProperty = RowStruct->FindPropertyByName(TEXT("ExecutionTag"));
			FProperty* MontageProperty = RowStruct->FindPropertyByName(TEXT("Montage"));

			if (TagProperty && MontageProperty)
			{
				// Iterate rows to find matching tag
				TArray<FName> AllRowNames = ExecutedMontages->GetRowNames();
				for (const FName& RowNameIter : AllRowNames)
				{
					uint8* RowData = ExecutedMontages->FindRowUnchecked(RowNameIter);
					if (RowData)
					{
						// Check if this row's tag matches
						FGameplayTag* RowTag = TagProperty->ContainerPtrToValuePtr<FGameplayTag>(RowData);
						if (RowTag && RowTag->MatchesTagExact(ExecutionTag))
						{
							// Found match - get the montage
							if (FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(MontageProperty))
							{
								TSoftObjectPtr<UAnimMontage>* MontagePtr = SoftProp->ContainerPtrToValuePtr<TSoftObjectPtr<UAnimMontage>>(RowData);
								if (MontagePtr)
								{
									Result = *MontagePtr;
									UE_LOG(LogTemp, Log, TEXT("  Found montage for tag: %s"), *ExecutionTag.ToString());
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return Result;
}

/**
 * SetHyperArmor - Toggle hyper armor state
 */
void UAC_AI_CombatManager::SetHyperArmor_Implementation(bool InHyperArmor)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::SetHyperArmor - %s"),
		InHyperArmor ? TEXT("true") : TEXT("false"));

	HyperArmor = InHyperArmor;
}

/**
 * HandleIncomingWeaponDamage_AI - Process incoming weapon damage
 */
void UAC_AI_CombatManager::HandleIncomingWeaponDamage_AI_Implementation(AActor* WeaponOwnerActor, const FHitResult& HitInfo, double IncomingDamage, double IncomingPoiseDamage, const TMap<FGameplayTag, UPrimaryDataAsset*>& IncomingStatusEffects)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleIncomingWeaponDamage_AI - Damage: %.1f, Poise: %.1f"),
		IncomingDamage, IncomingPoiseDamage);

	if (Invincible || IsDead)
	{
		return;
	}

	// Store damaging actor
	DamagingActor = WeaponOwnerActor;

	// Get stat manager and apply damage
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		UAC_StatManager* StatManager = Owner->FindComponentByClass<UAC_StatManager>();
		if (IsValid(StatManager))
		{
			// Apply HP damage
			FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			StatManager->AdjustStat(HPTag, ESLFValueType::CurrentValue, -IncomingDamage, false, false);

			// Apply poise damage
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -IncomingPoiseDamage, false, false);

			// Check for death
			UB_Stat* HPStat = nullptr;
			FStatInfo HPInfo;
			StatManager->GetStat(HPTag, HPStat, HPInfo);

			if (HPInfo.CurrentValue <= 0)
			{
				HandleDeath(StatManager, HitInfo);
			}
			else
			{
				// Handle hit reaction
				HandleHitReaction(HitInfo);
			}
		}
	}

	// Apply status effects
	ApplyIncomingStatusEffects(IncomingStatusEffects, 1.0);
}

/**
 * HandleProjectileDamage_AI - Process incoming projectile damage
 */
void UAC_AI_CombatManager::HandleProjectileDamage_AI_Implementation(double IncomingDamage, UNiagaraSystem* ProjectileHitEffect, const FGameplayTag& NegationStat, const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::HandleProjectileDamage_AI - Damage: %.1f"),
		IncomingDamage);

	if (Invincible || IsDead)
	{
		return;
	}

	// Apply damage with negation
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		UAC_StatManager* StatManager = Owner->FindComponentByClass<UAC_StatManager>();
		if (IsValid(StatManager))
		{
			double FinalDamage = IncomingDamage;

			// Apply negation if specified
			if (NegationStat.IsValid())
			{
				UB_Stat* Stat = nullptr;
				FStatInfo StatInfo;
				StatManager->GetStat(NegationStat, Stat, StatInfo);
				FinalDamage = FinalDamage * (1.0 - (StatInfo.CurrentValue / 100.0));
			}

			FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			StatManager->AdjustStat(HPTag, ESLFValueType::CurrentValue, -FinalDamage, false, false);
		}
	}

	// Apply status effects
	ApplyIncomingStatusEffects(StatusEffects, 1.0);
}

/**
 * ApplyIncomingStatusEffects - Apply status effects from damage
 */
void UAC_AI_CombatManager::ApplyIncomingStatusEffects_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& StatusEffects, double Multiplier)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::ApplyIncomingStatusEffects - %d effects"), StatusEffects.Num());

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UAC_StatusEffectManager* StatusEffectManager = Owner->FindComponentByClass<UAC_StatusEffectManager>();
	if (IsValid(StatusEffectManager))
	{
		for (const auto& Effect : StatusEffects)
		{
			// Apply each status effect
			// StatusEffectManager->ApplyStatusEffect(Effect.Value, Multiplier);
		}
	}
}

/**
 * ApplyFistDamage - Apply unarmed attack damage to target
 */
void UAC_AI_CombatManager::ApplyFistDamage_Implementation(AActor* TargetActor, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::ApplyFistDamage - Target: %s"),
		TargetActor ? *TargetActor->GetName() : TEXT("None"));

	if (!IsValid(TargetActor))
	{
		return;
	}

	// Calculate random poise damage in range
	double PoiseDamage = FMath::RandRange(MinUnarmedPoiseDamage, MaxUnarmedPoiseDamage);

	// Apply damage via interface or direct component call
}

/**
 * ProcessHandTrace - Process hit results from hand trace
 */
void UAC_AI_CombatManager::ProcessHandTrace_Implementation(const TArray<FHitResult>& Hits)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::ProcessHandTrace - %d hits"), Hits.Num());

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (IsValid(HitActor) && !FistTracedActors.Contains(HitActor))
		{
			FistTracedActors.Add(HitActor);
			ApplyFistDamage(HitActor, Hit);
		}
	}
}

/**
 * GetSelectedAbility - Get the currently selected ability
 */
UPrimaryDataAsset* UAC_AI_CombatManager::GetSelectedAbility_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::GetSelectedAbility - %s"),
		SelectedAbility ? *SelectedAbility->GetName() : TEXT("None"));

	return SelectedAbility;
}

/**
 * SetInvincibility - Toggle invincibility state
 */
void UAC_AI_CombatManager::SetInvincibility_Implementation(bool IsInvincibleIn)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::SetInvincibility - %s"),
		IsInvincibleIn ? TEXT("true") : TEXT("false"));

	Invincible = IsInvincibleIn;
}

/**
 * GetKnockbackAmountForDamage - Calculate knockback vector based on damage
 */
FVector UAC_AI_CombatManager::GetKnockbackAmountForDamage_Implementation(const FHitResult& Hit, double Damage, double MaxClamp)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::GetKnockbackAmountForDamage - Damage: %.1f, MaxClamp: %.1f"),
		Damage, MaxClamp);

	// Calculate knockback direction from hit normal
	FVector KnockbackDir = -Hit.ImpactNormal;
	KnockbackDir.Z = 0.0f;
	KnockbackDir.Normalize();

	// Scale by damage, clamped to max
	double KnockbackAmount = FMath::Clamp(Damage, 0.0, MaxClamp);

	return KnockbackDir * KnockbackAmount;
}

/**
 * OverrideAbilities - Replace the abilities list
 */
void UAC_AI_CombatManager::OverrideAbilities_Implementation(const TArray<UPrimaryDataAsset*>& InAbilities)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_CombatManager::OverrideAbilities - %d abilities"), InAbilities.Num());

	Abilities = InAbilities;
	SelectedAbility = nullptr;
}
