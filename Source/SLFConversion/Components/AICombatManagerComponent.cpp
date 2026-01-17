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
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StatManagerComponent.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Interfaces/BPI_Enemy.h"
#include "Interfaces/BPI_ExecutionIndicator.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimMontage.h"
#include "AC_CombatManager.h"

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
	BrokenPoiseDuration = 3.0f;
	PoiseBreakLoopMontage = nullptr;
	PoiseRegenDelay = 2.0f;  // Wait 2 seconds after last hit before poise starts regenerating
	PoiseRegenRate = 25.0f;  // 25 points per second

	// Load default poise break asset for enemies
	static ConstructorHelpers::FObjectFinder<UDataAsset> PoiseBreakAssetFinder(
		TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExamplePoiseBreak_Enemy_Guard.DA_ExamplePoiseBreak_Enemy_Guard"));
	if (PoiseBreakAssetFinder.Succeeded())
	{
		PoiseBreakAsset = PoiseBreakAssetFinder.Object;
	}
	else
	{
		PoiseBreakAsset = nullptr;
	}

	// Load default executed montages DataTable (FExecutionInfo rows)
	static ConstructorHelpers::FObjectFinder<UDataTable> ExecutedMontagesTableFinder(
		TEXT("/Game/SoulslikeFramework/Data/_Datatables/DT_Enemy_ExecutedMontages.DT_Enemy_ExecutedMontages"));
	if (ExecutedMontagesTableFinder.Succeeded())
	{
		ExecutedMontagesTable = ExecutedMontagesTableFinder.Object;
	}
	else
	{
		ExecutedMontagesTable = nullptr;
	}

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
	// Default to Heavy so enemies get IK flinch on every hit
	// Heavy = IK flinch only (small chest reaction)
	// Light = Full montage only (stagger on poise break)
	// Both/Montage = IK flinch + full montage
	HitReactType = ESLFHitReactType::Heavy;
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
	bool bJustBrokePoise = false;  // Track if poise just broke THIS hit

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

			// Get poise BEFORE damage
			FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
			UObject* PoiseStatObj = nullptr;
			FStatInfo PoiseStatInfo;
			StatManager->GetStat(PoiseTag, PoiseStatObj, PoiseStatInfo);
			float OldPoise = PoiseStatInfo.CurrentValue;

			// Apply poise damage
			// NOTE: bTriggerRegen=false - we don't want immediate stat-level regen for poise
			// Poise regen should be controlled at the component level with a delay
			StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, -PoiseDamage, false, false);

			// Reset poise regen timer (poise won't start regenerating until delay passes)
			ResetPoiseRegenTimer();

			// Get poise AFTER damage
			StatManager->GetStat(PoiseTag, PoiseStatObj, PoiseStatInfo);
			float NewPoise = PoiseStatInfo.CurrentValue;

			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Applied %.2f poise damage (%.0f -> %.0f)"), PoiseDamage, OldPoise, NewPoise);

			// Check if poise just broke
			bJustBrokePoise = (OldPoise > 0.0f && NewPoise <= 0.0f);
			if (bJustBrokePoise)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] *** POISE BROKEN! *** (bPoiseBroken was %s)"),
				bPoiseBroken ? TEXT("ALREADY TRUE") : TEXT("false"));
			}

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

	// ELDEN RING STYLE POISE SYSTEM:
	// - Every hit plays a small IK flinch (chest movement) via IkWeight animation
	// - Full stagger/knockback ONLY happens when poise breaks
	// - HyperArmor blocks ALL reactions (both IK and stagger)

	// First, handle IK reaction on EVERY hit (unless HyperArmor)
	// This is the small chest flinch that doesn't interrupt the AI
	if (!bHyperArmor && !bPoiseBroken)
	{
		// Play IK flinch for Heavy, IK, Montage, or Both reaction types
		if (HitReactType == ESLFHitReactType::Heavy ||
			HitReactType == ESLFHitReactType::IK ||
			HitReactType == ESLFHitReactType::Montage ||
			HitReactType == ESLFHitReactType::Both)
		{
			// Animate IkWeight directly - this is what the AnimBP reads for IK bone movement
			PlayIKFlinch(1.0);  // Use full scale for now
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] IK flinch reaction started (duration: %.2f)"), IKReactionDuration);
		}
	}

	// Second, check for poise break - this triggers FULL stagger/knockback
	// Diagnostic logging for poise break condition
	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Poise break check: bJustBrokePoise=%s, bHyperArmor=%s, bPoiseBroken=%s"),
		bJustBrokePoise ? TEXT("TRUE") : TEXT("false"),
		bHyperArmor ? TEXT("TRUE") : TEXT("false"),
		bPoiseBroken ? TEXT("TRUE") : TEXT("false"));

	if (bJustBrokePoise && !bHyperArmor && !bPoiseBroken)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] *** POISE BREAK - TRIGGERING FULL STAGGER! ***"));

		// Call FinishPoiseBreak which:
		// 1. Sets bPoiseBroken = true (AnimBP reads this for state transition)
		// 2. Broadcasts OnPoiseBroken (BehaviorTree listens to this)
		// 3. Starts the poise break recovery timer
		FinishPoiseBreak();

		// Store hit direction for AnimBP to use
		CurrentHitNormal = HitResult.ImpactNormal;

		// Also play the direction-specific hit reaction montage
		HandleHitReaction(HitReactType, HitResult.ImpactNormal);
	}
	else if (!bJustBrokePoise && !bHyperArmor)
	{
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Hit absorbed - poise held (IK flinch only, no stagger) [bPoiseBroken=%s]"),
			bPoiseBroken ? TEXT("true") : TEXT("false"));
	}
	else if (bJustBrokePoise && bPoiseBroken)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Poise BROKE but bPoiseBroken was already TRUE - DUPLICATE BREAK IGNORED"));
	}
	else if (bHyperArmor)
	{
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Hit absorbed - HyperArmor active (no reaction)"));
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

	// Apply knockback to living AI characters
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		// Only apply knockback if not in hyper armor
		if (!bHyperArmor)
		{
			// Calculate knockback force based on reaction type
			// Light = small pushback, Heavy/Both = larger pushback
			float KnockbackStrength = 100.0f;
			if (ReactionType == ESLFHitReactType::Heavy || ReactionType == ESLFHitReactType::Both)
			{
				KnockbackStrength = 200.0f;
			}

			// Knockback direction is opposite of hit direction (push away from hit)
			FVector KnockbackDir = -HitDirection.GetSafeNormal();
			KnockbackDir.Z = 0.0f; // Keep horizontal
			KnockbackDir.Normalize();

			FVector Knockback = KnockbackDir * KnockbackStrength;
			Character->LaunchCharacter(Knockback, false, false);
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Applied knockback: %s"), *Knockback.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Knockback blocked by HyperArmor"));
		}
	}

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
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] No hit reaction montage available - tried path: %s"), *MontagePath);
	}
}

UAnimMontage* UAICombatManagerComponent::GetRelevantExecutedMontage_Implementation(const FGameplayTag& ExecutionTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetRelevantExecutedMontage - Tag: %s, DataTable: %s"),
		*ExecutionTag.ToString(),
		ExecutedMontagesTable ? *ExecutedMontagesTable->GetName() : TEXT("NULL"));

	if (!ExecutedMontagesTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetRelevantExecutedMontage - No ExecutedMontagesTable assigned!"));
		return nullptr;
	}

	// FExecutionInfo struct has: Tag (FGameplayTag) and Animation (TSoftObjectPtr<UAnimMontage>)
	// Look up row where Tag matches ExecutionTag
	TArray<FName> RowNames = ExecutedMontagesTable->GetRowNames();
	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetRelevantExecutedMontage - DataTable has %d rows"), RowNames.Num());

	for (const FName& RowName : RowNames)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Processing row: %s"), *RowName.ToString());

		// Get row data - use generic struct access since FExecutionInfo is a Blueprint struct
		uint8* RowData = ExecutedMontagesTable->FindRowUnchecked(RowName);
		if (!RowData)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Row data is null"));
			continue;
		}

		// Access Tag property (FGameplayTag)
		const UScriptStruct* RowStruct = ExecutedMontagesTable->GetRowStruct();
		if (!RowStruct)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] RowStruct is null"));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] RowStruct: %s"), *RowStruct->GetName());

		// Find the Tag property
		FGameplayTag RowTag;
		bool bFoundTagProp = false;
		for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;
			FString PropName = Property->GetName();
			UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Property: %s"), *PropName);

			// Tag property has GUID suffix like "Tag_15_..."
			if (PropName.StartsWith(TEXT("Tag_")))
			{
				bFoundTagProp = true;
				if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
				{
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Tag is StructProperty, Struct=%s"), *StructProp->Struct->GetName());
					if (StructProp->Struct == FGameplayTag::StaticStruct())
					{
						RowTag = *StructProp->ContainerPtrToValuePtr<FGameplayTag>(RowData);
						UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Row %s has Tag: '%s' (valid=%d)"), *RowName.ToString(), *RowTag.ToString(), RowTag.IsValid() ? 1 : 0);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Tag struct mismatch: got %s, expected GameplayTag"), *StructProp->Struct->GetName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Tag is NOT a StructProperty"));
				}
			}
		}

		if (!bFoundTagProp)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] No Tag_ property found in struct!"));
		}

		// Check if this row's tag matches
		if (RowTag.MatchesTagExact(ExecutionTag) || (ExecutionTag.IsValid() && RowTag.IsValid() && RowTag.MatchesTag(ExecutionTag)))
		{
			// Found matching row - get Animation property
			for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
			{
				FProperty* Property = *PropIt;
				FString PropName = Property->GetName();

				// Animation property has GUID suffix like "Animation_12_..."
				if (PropName.StartsWith(TEXT("Animation_")))
				{
					if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
					{
						FSoftObjectPtr* SoftPtr = SoftObjProp->GetPropertyValuePtr_InContainer(RowData);
						if (SoftPtr && !SoftPtr->IsNull())
						{
							UAnimMontage* Montage = Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
							if (Montage)
							{
								UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetRelevantExecutedMontage - Found matching montage: %s"), *Montage->GetName());
								return Montage;
							}
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] GetRelevantExecutedMontage - No matching row for tag: %s"), *ExecutionTag.ToString());
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
	// Set poise broken state - AnimBP reads this to transition to stagger state
	bPoiseBroken = true;

	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] FinishPoiseBreak - bPoiseBroken=TRUE, PoiseBreakAsset=%s, BrokenPoiseDuration=%.2f"),
		PoiseBreakAsset ? *PoiseBreakAsset->GetName() : TEXT("NULL"),
		BrokenPoiseDuration);

	// ═══════════════════════════════════════════════════════════════════════════════
	// STOP AI - Enemy must be completely stunned during poise break
	// ═══════════════════════════════════════════════════════════════════════════════
	AActor* Owner = GetOwner();
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		// Stop the behavior tree via BrainComponent
		if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
		{
			// Clear any focus target
			AIC->ClearFocus(EAIFocusPriority::Gameplay);

			// Stop the behavior tree
			if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
			{
				BrainComp->StopLogic(TEXT("PoiseBroken"));
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] AI BrainComponent STOPPED - enemy is stunned!"));
			}
		}

		// Stop any movement and apply knockback
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				MovementComp->StopMovementImmediately();
				UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Movement stopped"));
			}

			// Apply BIG knockback - this is the visual stagger effect
			FVector Knockback = -CurrentHitNormal * 400.0f; // Push enemy backwards from hit direction
			Knockback.Z = 100.0f; // Add slight upward component
			Character->LaunchCharacter(Knockback, true, true);
			UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Applied poise break KNOCKBACK: %s"), *Knockback.ToString());

			// Stop any currently playing montages (abilities/attacks)
			if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
			{
				AnimInst->Montage_Stop(0.2f);  // Quick blend out
				UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Stopped all montages"));

				// Play poise break stagger animation sequence:
				// 1. First play PoiseBreak_Start (initial stun reaction)
				// 2. Then loop PoiseBreak_Loop (hunched waiting pose) until recovery
				if (UPDA_PoiseBreakAnimData* PoiseBreakData = Cast<UPDA_PoiseBreakAnimData>(PoiseBreakAsset))
				{
					// Check if we have the loop animation (the "hunched waiting" pose)
					if (UAnimSequence* LoopAnim = PoiseBreakData->PoiseBreak_Loop)
					{
						// Play the LOOP animation with high loop count - this is the "BIG" stagger
						// The enemy will stay hunched over until OnPoiseBreakEnd stops it
						PoiseBreakLoopMontage = AnimInst->PlaySlotAnimationAsDynamicMontage(
							LoopAnim,
							FName("DefaultSlot"),
							0.3f,   // BlendInTime - slightly longer for dramatic effect
							0.2f,   // BlendOutTime
							1.0f,   // PlayRate
							999,    // LoopCount - loop many times (will be stopped by timer)
							-1.0f   // BlendOutTriggerTime
						);
						if (PoiseBreakLoopMontage)
						{
							UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] *** BIG STAGGER *** Playing LOOPING poise break animation: %s"),
								*LoopAnim->GetName());
						}
					}
					else if (UAnimSequence* StartAnim = PoiseBreakData->PoiseBreak_Start)
					{
						// Fallback to Start animation if no Loop available
						PoiseBreakLoopMontage = AnimInst->PlaySlotAnimationAsDynamicMontage(
							StartAnim,
							FName("DefaultSlot"),
							0.2f,
							0.2f,
							1.0f,
							1,     // Play once if no loop available
							-1.0f
						);
						UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Playing poise break START animation (no loop): %s"),
							*StartAnim->GetName());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Both PoiseBreak_Start and PoiseBreak_Loop are NULL!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] PoiseBreakAsset is NULL or invalid type"));
				}
			}
		}
	}

	// Broadcast OnPoiseBroken - BehaviorTree and B_Soulslike_Enemy listen to this
	// bBroken=true means poise is now broken (enemy is staggered)
	OnPoiseBroken.Broadcast(true);
	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] OnPoiseBroken broadcasted (bBroken=true)"));

	// ═══════════════════════════════════════════════════════════════════════════════
	// SET EXECUTION TARGET ON PLAYER - Player can now execute this enemy
	// bp_only: AC_CombatManager.POISE BREAK graph calls SetExecutionTarget on player
	// ═══════════════════════════════════════════════════════════════════════════════
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			APawn* PlayerPawn = PC->GetPawn();
			if (PlayerPawn)
			{
				// Find player's combat manager and set this enemy as execution target
				UAC_CombatManager* PlayerCombatManager = PlayerPawn->FindComponentByClass<UAC_CombatManager>();
				if (PlayerCombatManager)
				{
					PlayerCombatManager->SetExecutionTarget(Owner);
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] *** SET EXECUTION TARGET *** Player can execute: %s"),
						Owner ? *Owner->GetName() : TEXT("null"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Player has no AC_CombatManager component!"));
				}
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// SHOW EXECUTION INDICATOR - Player can now perform execution attack
	// ═══════════════════════════════════════════════════════════════════════════════
	// Try direct cast first (works for C++ classes implementing interface)
	if (IBPI_ExecutionIndicator* ExecutionIndicator = Cast<IBPI_ExecutionIndicator>(Owner))
	{
		ExecutionIndicator->ToggleExecutionIcon(true);
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Execution icon SHOWN via direct cast - player can execute!"));
	}
	// Fallback to interface call (works for Blueprint implementations)
	else if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_ExecutionIndicator::StaticClass()))
	{
		IBPI_ExecutionIndicator::Execute_ToggleExecutionIcon(Owner, true);
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Execution icon SHOWN via interface - player can execute!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Owner does NOT implement BPI_ExecutionIndicator (Class: %s)"),
			Owner ? *Owner->GetClass()->GetName() : TEXT("null"));
	}

	// Schedule poise break end
	if (UWorld* World = GetWorld())
	{
		// Ensure we have a valid duration
		float Duration = BrokenPoiseDuration > 0.0f ? BrokenPoiseDuration : 3.0f;

		World->GetTimerManager().SetTimer(
			PoiseBreakTimer, this,
			&UAICombatManagerComponent::OnPoiseBreakEnd_Implementation,
			Duration, false);

		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Poise break timer started (%.2f seconds) - enemy vulnerable!"), Duration);
	}
}

void UAICombatManagerComponent::OnPoiseBreakEnd_Implementation()
{
	bPoiseBroken = false;

	// Broadcast OnPoiseBroken with bBroken=false to indicate poise recovery
	OnPoiseBroken.Broadcast(false);
	UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] OnPoiseBreakEnd - Resuming AI (bBroken=false broadcasted)"));

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// ═══════════════════════════════════════════════════════════════════════════════
	// STOP POISE BREAK LOOP ANIMATION - Enemy recovers from stagger
	// ═══════════════════════════════════════════════════════════════════════════════
	if (PoiseBreakLoopMontage)
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
			{
				AnimInst->Montage_Stop(0.3f, PoiseBreakLoopMontage);
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Stopped poise break loop montage: %s"),
					*PoiseBreakLoopMontage->GetName());
			}
		}
		PoiseBreakLoopMontage = nullptr;
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// HIDE EXECUTION INDICATOR - Player can no longer execute
	// ═══════════════════════════════════════════════════════════════════════════════
	if (IBPI_ExecutionIndicator* ExecutionIndicator = Cast<IBPI_ExecutionIndicator>(Owner))
	{
		ExecutionIndicator->ToggleExecutionIcon(false);
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Execution icon HIDDEN via direct cast"));
	}
	else if (Owner->GetClass()->ImplementsInterface(UBPI_ExecutionIndicator::StaticClass()))
	{
		IBPI_ExecutionIndicator::Execute_ToggleExecutionIcon(Owner, false);
		UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] Execution icon HIDDEN via interface"));
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// CLEAR EXECUTION TARGET ON PLAYER - Player can no longer execute
	// bp_only: AC_CombatManager.POISE BREAK END clears the execution target
	// ═══════════════════════════════════════════════════════════════════════════════
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			APawn* PlayerPawn = PC->GetPawn();
			if (PlayerPawn)
			{
				UAC_CombatManager* PlayerCombatManager = PlayerPawn->FindComponentByClass<UAC_CombatManager>();
				if (PlayerCombatManager)
				{
					PlayerCombatManager->SetExecutionTarget(nullptr);
					UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] *** CLEARED EXECUTION TARGET *** Player can no longer execute"));
				}
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// RESUME AI - Enemy recovers from poise break
	// ═══════════════════════════════════════════════════════════════════════════════
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		// Restart the behavior tree
		if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
		{
			if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
			{
				BrainComp->RestartLogic();
				UE_LOG(LogTemp, Warning, TEXT("[AICombatManager] AI BrainComponent RESTARTED - enemy recovered!"));
			}
		}

		// Re-enable movement
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				MovementComp->SetMovementMode(MOVE_Walking);
				UE_LOG(LogTemp, Log, TEXT("[AICombatManager] Movement re-enabled"));
			}
		}
	}

	// Reset poise stat to max
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
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

// ═══════════════════════════════════════════════════════════════════════════════
// IK FLINCH ANIMATION
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::PlayIKFlinch(double Scale)
{
	// Start IK flinch animation by animating IkWeight
	// The AnimBP reads IkWeight to apply physical IK bone movement
	IKReactionPeakWeight = Scale;
	IKReactionStartTime = GetWorld()->GetTimeSeconds();

	// Set initial weight immediately for instant visual feedback
	IkWeight = Scale;

	UE_LOG(LogTemp, Log, TEXT("[AICombatManager] PlayIKFlinch - Starting (peak: %.2f, duration: %.2f)"),
		Scale, IKReactionDuration);

	// Clear any existing timer and start a new one
	GetWorld()->GetTimerManager().ClearTimer(IKReactionTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		IKReactionTimerHandle,
		this,
		&UAICombatManagerComponent::UpdateIKReaction,
		0.016f, // ~60fps update rate
		true,   // loop
		0.0f    // no delay
	);
}

void UAICombatManagerComponent::UpdateIKReaction()
{
	// Calculate elapsed time
	float ElapsedTime = GetWorld()->GetTimeSeconds() - IKReactionStartTime;
	float Alpha = ElapsedTime / IKReactionDuration;

	if (Alpha >= 1.0f)
	{
		// Animation complete - reset IkWeight and stop timer
		IkWeight = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(IKReactionTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[AICombatManager] IK flinch complete"));
		return;
	}

	// Use a bell curve (sin^2) for natural flinch motion
	// Ramps up quickly then back down
	// sin^2(pi * alpha) gives: 0 -> 1 -> 0 over alpha [0, 1]
	float CurveValue = FMath::Sin(PI * Alpha);
	CurveValue = CurveValue * CurveValue; // sin^2 for sharper peak

	IkWeight = IKReactionPeakWeight * CurveValue;
}

// ═══════════════════════════════════════════════════════════════════════════════
// POISE REGENERATION
// ═══════════════════════════════════════════════════════════════════════════════

void UAICombatManagerComponent::ResetPoiseRegenTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Clear any existing timers
	World->GetTimerManager().ClearTimer(PoiseRegenTickTimer);
	World->GetTimerManager().ClearTimer(PoiseRegenDelayTimer);

	// Don't start regen if poise is already broken
	if (bPoiseBroken) return;

	// Start the delay timer - poise will start regenerating after this
	World->GetTimerManager().SetTimer(
		PoiseRegenDelayTimer,
		this,
		&UAICombatManagerComponent::OnPoiseRegenDelayExpired,
		PoiseRegenDelay,
		false
	);
}

void UAICombatManagerComponent::OnPoiseRegenDelayExpired()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Don't regenerate if poise is broken
	if (bPoiseBroken) return;

	// Start regenerating poise (tick every 0.1 seconds)
	World->GetTimerManager().SetTimer(
		PoiseRegenTickTimer,
		this,
		&UAICombatManagerComponent::OnPoiseRegenTick,
		0.1f,
		true
	);
}

void UAICombatManagerComponent::OnPoiseRegenTick()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Stop regen if poise is broken
	if (bPoiseBroken)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PoiseRegenTickTimer);
		}
		return;
	}

	// Get stat manager
	if (!Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass())) return;

	UActorComponent* StatComp = nullptr;
	IBPI_GenericCharacter::Execute_GetStatManager(Owner, StatComp);
	UStatManagerComponent* StatManager = Cast<UStatManagerComponent>(StatComp);
	if (!StatManager) return;

	// Check current poise
	FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"));
	UObject* PoiseStatObj = nullptr;
	FStatInfo PoiseInfo;
	StatManager->GetStat(PoiseTag, PoiseStatObj, PoiseInfo);

	// Stop if poise is at max
	if (PoiseInfo.CurrentValue >= PoiseInfo.MaxValue)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PoiseRegenTickTimer);
		}
		return;
	}

	// Regenerate poise (PoiseRegenRate per second, tick every 0.1s)
	double RegenAmount = PoiseRegenRate * 0.1;
	StatManager->AdjustStat(PoiseTag, ESLFValueType::CurrentValue, RegenAmount, false, false);
}
