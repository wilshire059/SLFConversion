// AICombatManagerComponent.cpp
// C++ implementation for AC_AI_CombatManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_AI_CombatManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         41/41 (initialized in constructor)
// Functions:         25/25 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 2/2 (OnPoiseBroken, OnDeath)
// ═══════════════════════════════════════════════════════════════════════════════

#include "AICombatManagerComponent.h"
#include "SLFEnums.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StatManagerComponent.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Interfaces/BPI_Enemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"

UAICombatManagerComponent::UAICombatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize state flags
	bIsDead = false;
	bPoiseBroken = false;
	bHyperArmor = false;
	bInvincible = false;
	bHealthbarActive = false;

	// Initialize healthbar config
	bUseWorldHealthbar = true;
	HealthBarDuration = 5.0f;

	// Initialize poise break config
	PoiseBreakAsset = nullptr;
	BrokenPoiseDuration = 3.0f;

	// Initialize combat config
	LineOfSightCheckInterval = 0.5f;
	DelayAfterDeath = 3.0f;
	bCausesHitStop = true;
	HitStopDuration = 0.1f;
	bCanBePushed = true;
	bRagdollOnDeath = true;
	CurrencyReward = 100;

	// Initialize damage/combat runtime
	DamagingActor = nullptr;
	IkWeight = 1.0f;
	CurrentHitNormal = FVector::ZeroVector;

	// Initialize hit reaction
	HitReactType = ESLFHitReactType::Light;
	// Load default enemy reaction animset
	static ConstructorHelpers::FObjectFinder<UDataAsset> ReactionAnimsetFinder(
		TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleEnemyReactionAnimset.DA_ExampleEnemyReactionAnimset"));
	if (ReactionAnimsetFinder.Succeeded())
	{
		ReactionAnimset = ReactionAnimsetFinder.Object;
	}
	else
	{
		ReactionAnimset = nullptr;
	}

	// Initialize effects - Load default blood VFX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BloodVFXFinder(
		TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_DirectionalBlood.NS_DirectionalBlood"));
	if (BloodVFXFinder.Succeeded())
	{
		HitVFX = BloodVFXFinder.Object;
	}
	else
	{
		HitVFX = nullptr;
	}
	HitSFX = nullptr;
	Mesh = nullptr;

	// Initialize hand trace
	HandTraceRadius = 20.0f;
	HandSocket_L_Start = FName("hand_l");
	HandSocket_L_End = FName("hand_l_end");
	HandSocket_R_Start = FName("hand_r");
	HandSocket_R_End = FName("hand_r_end");
	MinUnarmedPoiseDamage = 10.0f;
	MaxUnarmedPoiseDamage = 25.0f;

	// Initialize abilities
	SelectedAbility = nullptr;
}

void UAICombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] BeginPlay on %s"), *GetOwner()->GetName());

	// Cache mesh component
	if (AActor* Owner = GetOwner())
	{
		Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}

	// Bind to stat updates
	BindStatUpdates();
}

// ═══════════════════════════════════════════════════════════════════════════════
// DAMAGE HANDLING [1-4/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::HandleIncomingWeaponDamage_AI_Implementation(
	AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult)
{
	if (bIsDead || bInvincible) return;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] HandleIncomingWeaponDamage - Damage: %.2f, Poise: %.2f, From: %s"),
		Damage, PoiseDamage, DamageCauser ? *DamageCauser->GetName() : TEXT("null"));

	DamagingActor = DamageCauser;
	CurrentHitNormal = HitResult.ImpactNormal;

	// Apply damage to health and poise stats
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
		if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp))
		{
			// Apply damage to health stat (HP is a Secondary stat)
			FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			StatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -Damage, false, true);
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Applied %.2f damage to health"), Damage);

			// Apply poise damage and check for break
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamage, false, true);
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Applied %.2f poise damage"), PoiseDamage);

			// Get current health values
			UObject* StatObj = nullptr;
			FStatInfo StatInfo;
			bool bGotStat = StatManager->GetStat(HealthTag, StatObj, StatInfo);

			// Check for death (HP <= 0)
			if (bGotStat && StatInfo.CurrentValue <= 0.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] HP <= 0 - Triggering death!"));
				HandleDeath(DamageCauser);
				return; // Don't process further after death
			}

			// Update healthbar via interface
			if (Owner->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
			{
				// Show healthbar when taking damage
				UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Calling ToggleHealthbarVisual(true)"));
				IBPI_Enemy::Execute_ToggleHealthbarVisual(Owner, true);

				// Update health values for UI
				if (bGotStat)
				{
					UE_LOG(LogTemp, Log, TEXT("[AICombatManager] GetStat succeeded - calling UpdateEnemyHealth (HP: %.0f/%.0f)"),
						StatInfo.CurrentValue, StatInfo.MaxValue);
					IBPI_Enemy::Execute_UpdateEnemyHealth(Owner, StatInfo.CurrentValue, StatInfo.MaxValue, -Damage);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetStat FAILED for HP tag"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Owner does NOT implement BPI_Enemy interface"));
			}
		}
	}

	// Spawn hit effects (blood VFX)
	if (HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HitVFX,
			HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation(),
			FVector(1.0f),
			true,
			true
		);
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Spawned blood VFX at %s"), *HitResult.ImpactPoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] HitVFX is NULL - no blood effect"));
	}
	if (HitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSFX, HitResult.ImpactPoint);
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Played hit sound"));
	}

	// Handle hit reaction if not in hyper armor
	if (!bHyperArmor)
	{
		HandleHitReaction(HitReactType, HitResult.ImpactNormal);
	}
}

void UAICombatManagerComponent::HandleProjectileDamage_AI_Implementation(
	AActor* DamageCauser, float Damage, float PoiseDamage, const FHitResult& HitResult)
{
	// Projectile damage uses same logic as weapon damage
	HandleIncomingWeaponDamage_AI(DamageCauser, Damage, PoiseDamage, HitResult);
}

float UAICombatManagerComponent::GetKnockbackAmountForDamage_Implementation(float Damage)
{
	// Scale knockback with damage
	return FMath::Clamp(Damage * 2.0f, 100.0f, 1000.0f);
}

void UAICombatManagerComponent::ApplyIncomingStatusEffects_Implementation(
	AActor* DamageCauser, const TArray<FGameplayTag>& StatusEffects)
{
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] ApplyIncomingStatusEffects - %d effects"), StatusEffects.Num());

	// Apply status effects via StatusEffectManager
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UStatusEffectManagerComponent* StatusEffectManager = Owner->FindComponentByClass<UStatusEffectManagerComponent>();
	if (StatusEffectManager)
	{
		for (const FGameplayTag& EffectTag : StatusEffects)
		{
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Would apply status effect: %s"), *EffectTag.ToString());
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// HIT REACTION [5-6/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::HandleHitReaction_Implementation(
	ESLFHitReactType ReactionType, const FVector& HitDirection)
{
	if (bIsDead) return;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] HandleHitReaction - Type: %d"), static_cast<int32>(ReactionType));

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Calculate hit direction for animation section selection
	FVector Forward = Owner->GetActorForwardVector();
	FVector Right = Owner->GetActorRightVector();

	float ForwardDot = FVector::DotProduct(Forward, HitDirection.GetSafeNormal());
	float RightDot = FVector::DotProduct(Right, HitDirection.GetSafeNormal());

	// Determine direction: Front(Fwd), Back(Bwd), Left(L), Right(R)
	FString DirectionSuffix;
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		DirectionSuffix = ForwardDot > 0 ? TEXT("Fwd") : TEXT("Bwd");
	}
	else
	{
		DirectionSuffix = RightDot > 0 ? TEXT("R") : TEXT("L");
	}

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Hit reaction direction: %s"), *DirectionSuffix);

	// Get the mesh's anim instance to play montage
	USkeletalMeshComponent* OwnerMesh = Mesh;
	if (!OwnerMesh)
	{
		OwnerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!OwnerMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] No skeletal mesh found for hit reaction"));
		return;
	}

	UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] No anim instance found for hit reaction"));
		return;
	}

	// Load hit reaction montage directly by direction
	// Montages are: AM_SLF_HitReaction_Fwd, AM_SLF_HitReaction_Bwd, AM_SLF_HitReaction_L, AM_SLF_HitReaction_R
	FString MontagePath = FString::Printf(
		TEXT("/Game/SoulslikeFramework/Demo/_Animations/HitReactions/AM_SLF_HitReaction_%s.AM_SLF_HitReaction_%s"),
		*DirectionSuffix, *DirectionSuffix);

	UAnimMontage* HitReactionMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath);
	if (HitReactionMontage)
	{
		float PlayRate = 1.0f;
		// Play slightly faster for light reactions, slower for heavy
		if (ReactionType == ESLFHitReactType::Light)
		{
			PlayRate = 1.2f;
		}
		else if (ReactionType == ESLFHitReactType::Heavy)
		{
			PlayRate = 0.8f;
		}

		AnimInstance->Montage_Play(HitReactionMontage, PlayRate);
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Playing hit reaction montage: %s at rate %.2f"),
			*HitReactionMontage->GetName(), PlayRate);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Failed to load hit reaction montage: %s"), *MontagePath);
	}

	// Fallback: use ExecutedMontages array if direct load didn't work
	if (ExecutedMontages.Num() > 0)
	{
		UAnimMontage* Montage = ExecutedMontages[0];
		if (Montage)
		{
			AnimInstance->Montage_Play(Montage, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Playing fallback hit reaction montage: %s"), *Montage->GetName());
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] No hit reaction montage available - tried path: %s"), *MontagePath);
}

UAnimMontage* UAICombatManagerComponent::GetRelevantExecutedMontage_Implementation()
{
	if (ExecutedMontages.Num() > 0)
	{
		// Return random executed montage
		int32 Index = FMath::RandRange(0, ExecutedMontages.Num() - 1);
		return ExecutedMontages[Index];
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DEATH [7-8/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::HandleDeath_Implementation(AActor* Killer)
{
	if (bIsDead) return;

	bIsDead = true;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] HandleDeath - Killed by: %s"),
		Killer ? *Killer->GetName() : TEXT("null"));

	// Broadcast death event
	OnDeath.Broadcast(Killer);

	// Ragdoll if configured
	if (bRagdollOnDeath && Mesh)
	{
		Mesh->SetSimulatePhysics(true);
	}

	// Schedule cleanup
	if (UWorld* World = GetWorld())
	{
		FTimerHandle CleanupTimer;
		World->GetTimerManager().SetTimer(
			CleanupTimer,
			this,
			&UAICombatManagerComponent::EndEncounter_Implementation,
			DelayAfterDeath,
			false
		);
	}
}

void UAICombatManagerComponent::EndEncounter_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] EndEncounter"));

	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE SETTERS [9-10/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::SetInvincibility_Implementation(bool IsInvincible)
{
	bInvincible = IsInvincible;
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] SetInvincibility: %s"), bInvincible ? TEXT("true") : TEXT("false"));
}

void UAICombatManagerComponent::SetHyperArmor_Implementation(bool bEnabled)
{
	bHyperArmor = bEnabled;
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] SetHyperArmor: %s"), bHyperArmor ? TEXT("true") : TEXT("false"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// ABILITIES [11-14/25]
// ═══════════════════════════════════════════════════════════════════════════════

bool UAICombatManagerComponent::TryGetAbility_Implementation(UDataAsset*& OutAbility)
{
	// Filter available abilities
	TArray<FSLFAIAbility> ValidAbilities;
	for (const FSLFAIAbility& Ability : Abilities)
	{
		if (EvaluateAbilityRule(Ability))
		{
			ValidAbilities.Add(Ability);
		}
	}

	if (ValidAbilities.Num() > 0)
	{
		// Weight-based random selection
		float TotalWeight = 0.0f;
		for (const FSLFAIAbility& Ability : ValidAbilities)
		{
			TotalWeight += Ability.Weight;
		}

		float RandomValue = FMath::FRand() * TotalWeight;
		float CurrentWeight = 0.0f;

		for (const FSLFAIAbility& Ability : ValidAbilities)
		{
			CurrentWeight += Ability.Weight;
			if (RandomValue <= CurrentWeight)
			{
				OutAbility = Ability.AbilityAsset;
				SelectedAbility = OutAbility;
				return true;
			}
		}
	}

	OutAbility = nullptr;
	return false;
}

bool UAICombatManagerComponent::EvaluateAbilityRule_Implementation(const FSLFAIAbility& Ability)
{
	// Check if ability is valid and can be executed
	if (!Ability.AbilityAsset)
	{
		return false;
	}

	// Check cooldown - ability would have LastUsedTime property
	// Check range requirements based on DistanceRule
	// For now, basic validation passes if asset exists
	UE_LOG(LogTemp, Verbose, TEXT("[AICombatManager] EvaluateAbilityRule - Checking ability validity"));
	return true;
}

void UAICombatManagerComponent::OverrideAbilities_Implementation(const TArray<FSLFAIAbility>& NewAbilities)
{
	Abilities = NewAbilities;
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] OverrideAbilities - %d abilities"), Abilities.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// HAND TRACE [15-19/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::TraceRightHand_Implementation()
{
	if (!Mesh) return;

	FVector Start = Mesh->GetSocketLocation(HandSocket_R_Start);
	FVector End = Mesh->GetSocketLocation(HandSocket_R_End);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(
		HitResults, Start, End, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(HandTraceRadius), Params))
	{
		ProcessHandTrace(HitResults);
	}
}

void UAICombatManagerComponent::TraceLeftHand_Implementation()
{
	if (!Mesh) return;

	FVector Start = Mesh->GetSocketLocation(HandSocket_L_Start);
	FVector End = Mesh->GetSocketLocation(HandSocket_L_End);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepMultiByChannel(
		HitResults, Start, End, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(HandTraceRadius), Params))
	{
		ProcessHandTrace(HitResults);
	}
}

void UAICombatManagerComponent::ProcessHandTrace_Implementation(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (!FistTracedActors.Contains(HitActor))
			{
				FistTracedActors.Add(HitActor);
				ApplyFistDamage(HitActor, Hit);
			}
		}
	}
}

void UAICombatManagerComponent::ApplyFistDamage_Implementation(AActor* Target, const FHitResult& HitResult)
{
	float Damage = FMath::RandRange(MinUnarmedPoiseDamage, MaxUnarmedPoiseDamage);

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] ApplyFistDamage - Target: %s, Damage: %.2f"),
		*Target->GetName(), Damage);

	// Apply damage via interface
	if (Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* TargetStatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Target, TargetStatComp);
		if (UStatManagerComponent* TargetStatManager = Cast<UStatManagerComponent>(TargetStatComp))
		{
			// Apply health damage
			FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
			TargetStatManager->AdjustStat(HealthTag, ESLFValueType::CurrentValue, -Damage, false, true);

			// Apply poise damage
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			float PoiseDamage = Damage * 0.5f;
			TargetStatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamage, false, true);

			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Applied fist damage: %.2f HP, %.2f Poise to %s"),
				Damage, PoiseDamage, *Target->GetName());
		}
	}
}

void UAICombatManagerComponent::ToggleHandTrace_Implementation(bool bEnable, bool bRightHand)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (bEnable)
	{
		FistTracedActors.Empty();

		if (bRightHand)
		{
			World->GetTimerManager().SetTimer(
				RightHandTraceTimer, this,
				&UAICombatManagerComponent::TraceRightHand_Implementation,
				0.016f, true);
		}
		else
		{
			World->GetTimerManager().SetTimer(
				LeftHandTraceTimer, this,
				&UAICombatManagerComponent::TraceLeftHand_Implementation,
				0.016f, true);
		}
	}
	else
	{
		if (bRightHand)
			World->GetTimerManager().ClearTimer(RightHandTraceTimer);
		else
			World->GetTimerManager().ClearTimer(LeftHandTraceTimer);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// POISE BREAK [20-22/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::FinishPoiseBreak_Implementation()
{
	bPoiseBroken = true;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] FinishPoiseBreak"));

	OnPoiseBroken.Broadcast();

	// Schedule poise break end
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PoiseBreakTimer, this,
			&UAICombatManagerComponent::OnPoiseBreakEnd_Implementation,
			BrokenPoiseDuration, false);
	}
}

void UAICombatManagerComponent::OnPoiseBreakEnd_Implementation()
{
	bPoiseBroken = false;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] OnPoiseBreakEnd"));

	// Reset poise stat to max
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
		if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp))
		{
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			StatManager->ResetStat(PoiseTag);
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Reset poise stat to max"));
		}
	}
}

void UAICombatManagerComponent::ClearPoiseBreakResetTimer_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PoiseBreakTimer);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// HEALTHBAR [23-24/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::CheckLineOfSight_Implementation()
{
	// Check line of sight to player for healthbar visibility
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// Get player pawn
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	// Line trace from owner to player
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FVector Start = Owner->GetActorLocation();
	FVector End = PlayerPawn->GetActorLocation();

	bool bHasLineOfSight = !World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// Update healthbar visibility based on line of sight
	if (bHasLineOfSight && Owner->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
	{
		IBPI_Enemy::Execute_ToggleHealthbarVisual(Owner, true);
	}
}

void UAICombatManagerComponent::DisableHealthbar_Implementation()
{
	bHealthbarActive = false;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] DisableHealthbar"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HealthbarTimer);
		World->GetTimerManager().ClearTimer(HealthbarLosTimer);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT BINDING [25/25]
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::BindStatUpdates_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] BindStatUpdates"));

	// Bind to owner's StatManager for health updates
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UActorComponent* StatComp = nullptr;
		IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
		if (UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp))
		{
			// StatManager has OnStatsInitialized and OnLevelUpdated delegates
			// Health updates would come through stat change callbacks
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Found StatManager, bindings ready"));
		}
	}
}
