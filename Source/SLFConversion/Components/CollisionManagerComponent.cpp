// CollisionManagerComponent.cpp
// C++ implementation for AC_CollisionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_CollisionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         11/11 (initialized in constructor)
// Functions:         9/9 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 1/1 (OnActorTraced)
// ═══════════════════════════════════════════════════════════════════════════════

#include "CollisionManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AC_StatusEffectManager.h"
#include "Components/StatusEffectManagerComponent.h"
#include "Blueprints/SLFWeaponBase.h"
#include "SLFPrimaryDataAssets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

UCollisionManagerComponent::UCollisionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Initialize config variables
	TraceSocketStart = FName("TraceStart");
	TraceSocketEnd = FName("TraceEnd");
	TraceRadius = 20.0;
	TraceDebugMode = EDrawDebugTrace::None;

	// Initialize runtime variables
	TargetMesh = nullptr;
	DamageMultiplier = 1.0;
	TraceSizeMultiplier = 1.0;
	LastStartPosition = FVector::ZeroVector;
	LastEndPosition = FVector::ZeroVector;

	// CRITICAL: Initialize TraceTypes to detect hittable objects
	// Without this, SphereTraceMultiForObjects will never hit anything!
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));           // Characters (enemies/player)
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));   // Destructibles, physics props
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));    // Physics-simulated bodies
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Destructible));   // Chaos Destructibles (GeometryCollection)
	TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1)); // Custom channel 1 (ObjectType=7, used by B_Destructible)
}

void UCollisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[CollisionManager] BeginPlay on %s"), *GetOwner()->GetName());

	InitializeTracePoints();
}

void UCollisionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Perform trace each tick when enabled
	SubsteppedTrace(50.0); // Default step size
}

// ═══════════════════════════════════════════════════════════════════════════════
// MULTIPLIER FUNCTIONS [1-2/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::SetMultipliers_Implementation(double InDamageMultiplier, double InTraceSizeMultiplier)
{
	DamageMultiplier = InDamageMultiplier;
	TraceSizeMultiplier = InTraceSizeMultiplier;

	UE_LOG(LogTemp, Log, TEXT("[CollisionManager] SetMultipliers - Damage: %.2f, TraceSize: %.2f"),
		DamageMultiplier, TraceSizeMultiplier);
}

void UCollisionManagerComponent::GetMultipliers_Implementation(double& OutDamageMultiplier, double& OutTraceSizeMultiplier)
{
	OutDamageMultiplier = DamageMultiplier;
	OutTraceSizeMultiplier = TraceSizeMultiplier;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TRACE FUNCTIONS [3-6/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::GetTraceLocations(FVector& OutTraceStart, FVector& OutTraceEnd) const
{
	OutTraceStart = FVector::ZeroVector;
	OutTraceEnd = FVector::ZeroVector;

	if (!TargetMesh)
	{
		return;
	}

	if (USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(TargetMesh))
	{
		if (SkelMesh->DoesSocketExist(TraceSocketStart) && SkelMesh->DoesSocketExist(TraceSocketEnd))
		{
			OutTraceStart = SkelMesh->GetSocketLocation(TraceSocketStart);
			OutTraceEnd = SkelMesh->GetSocketLocation(TraceSocketEnd);
			return;
		}
	}
	else if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(TargetMesh))
	{
		if (StaticMesh->DoesSocketExist(TraceSocketStart) && StaticMesh->DoesSocketExist(TraceSocketEnd))
		{
			OutTraceStart = StaticMesh->GetSocketLocation(TraceSocketStart);
			OutTraceEnd = StaticMesh->GetSocketLocation(TraceSocketEnd);
			return;
		}

		// FALLBACK: Sockets don't exist on this static mesh.
		// Compute trace line from the mesh bounding box along its longest axis.
		// This covers grip-to-tip for swords, katanas, greatswords, etc.
		UStaticMesh* Mesh = StaticMesh->GetStaticMesh();
		if (Mesh)
		{
			FBox Box = Mesh->GetBoundingBox();
			FVector Size = Box.GetSize();
			FVector Center = Box.GetCenter();

			// Find the longest local axis (the blade direction)
			FVector HalfExtent = FVector::ZeroVector;
			if (Size.X >= Size.Y && Size.X >= Size.Z)
				HalfExtent = FVector(Size.X * 0.5f, 0, 0);
			else if (Size.Y >= Size.X && Size.Y >= Size.Z)
				HalfExtent = FVector(0, Size.Y * 0.5f, 0);
			else
				HalfExtent = FVector(0, 0, Size.Z * 0.5f);

			// Transform from local mesh space to world space
			FTransform CompTransform = StaticMesh->GetComponentTransform();
			OutTraceStart = CompTransform.TransformPosition(Center - HalfExtent);
			OutTraceEnd = CompTransform.TransformPosition(Center + HalfExtent);
			return;
		}
	}

	// Last resort fallback: use component location with a forward extension
	OutTraceStart = TargetMesh->GetComponentLocation();
	FVector Forward = TargetMesh->GetForwardVector();
	OutTraceEnd = OutTraceStart + Forward * 100.0f;
}

void UCollisionManagerComponent::SubsteppedTrace_Implementation(double StepSize)
{
	FVector CurrentStart, CurrentEnd;
	GetTraceLocations(CurrentStart, CurrentEnd);

	// If last positions are zero, just do single trace
	if (LastStartPosition.IsNearlyZero() || LastEndPosition.IsNearlyZero())
	{
		LastStartPosition = CurrentStart;
		LastEndPosition = CurrentEnd;
	}

	// Calculate distance moved
	float DistanceMoved = FVector::Dist(LastStartPosition, CurrentStart);
	int32 NumSteps = FMath::Max(1, FMath::CeilToInt(DistanceMoved / StepSize));

	for (int32 Step = 0; Step <= NumSteps; ++Step)
	{
		float Alpha = static_cast<float>(Step) / static_cast<float>(NumSteps);

		FVector StepStart = FMath::Lerp(LastStartPosition, CurrentStart, Alpha);
		FVector StepEnd = FMath::Lerp(LastEndPosition, CurrentEnd, Alpha);

		TArray<FHitResult> HitResults;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());
		// Also ignore the character wielding this weapon
		if (AActor* OwnerActor = GetOwner())
		{
			if (AActor* AttachParent = OwnerActor->GetAttachParentActor())
			{
				ActorsToIgnore.Add(AttachParent);
			}
		}

		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StepStart,
			StepEnd,
			TraceRadius * TraceSizeMultiplier,
			TraceTypes,
			false, // bTraceComplex
			ActorsToIgnore,
			TraceDebugMode,
			HitResults,
			true // bIgnoreSelf
		);

		if (bHit)
		{
			ProcessTrace(HitResults);
		}
	}

	// Update last positions
	LastStartPosition = CurrentStart;
	LastEndPosition = CurrentEnd;
}

void UCollisionManagerComponent::ProcessTrace_Implementation(const TArray<FHitResult>& HitResults)
{
	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner)
	{
		return;
	}

	// Get the character that owns this weapon (weapon is attached to character)
	AActor* AttackingCharacter = WeaponOwner->GetAttachParentActor();
	if (!AttackingCharacter)
	{
		AttackingCharacter = WeaponOwner; // Fallback if weapon is the character itself
	}

	// Get weapon status effects from the weapon actor
	TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;
	ASLFWeaponBase* WeaponActor = Cast<ASLFWeaponBase>(WeaponOwner);
	if (WeaponActor)
	{
		// Get status effects from weapon's ItemInfo
		const TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication>& WeaponStatusEffects =
			WeaponActor->ItemInfo.EquipmentDetails.WeaponStatusEffectInfo;

		if (WeaponStatusEffects.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Weapon %s has %d status effects"),
				*WeaponActor->GetName(), WeaponStatusEffects.Num());

			// Convert to the format expected by HandleIncomingWeaponDamage
			// TMap<FGameplayTag, UPrimaryDataAsset*> where key is effect's tag
			for (const auto& EffectPair : WeaponStatusEffects)
			{
				if (IsValid(EffectPair.Key))
				{
					// Get the status effect's tag from the data asset
					UPDA_StatusEffect* StatusEffectData = Cast<UPDA_StatusEffect>(EffectPair.Key);
					if (StatusEffectData && StatusEffectData->Tag.IsValid())
					{
						StatusEffects.Add(StatusEffectData->Tag, EffectPair.Key);
						UE_LOG(LogTemp, Log, TEXT("[CollisionManager]   - Status Effect: %s, Tag: %s, Buildup: %.2f"),
							*EffectPair.Key->GetName(),
							*StatusEffectData->Tag.ToString(),
							EffectPair.Value.BuildupAmount);
					}
				}
			}
		}
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || TracedActors.Contains(HitActor))
		{
			continue;
		}

		// Skip hitting ourselves or our owner
		if (HitActor == WeaponOwner || HitActor == AttackingCharacter)
		{
			continue;
		}

		TracedActors.Add(HitActor);

		UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Hit: %s (Damage: %.1f)"),
			*HitActor->GetName(), 10.0 * DamageMultiplier);

		// Broadcast the hit (for any external listeners)
		OnActorTraced.Broadcast(HitActor, Hit, DamageMultiplier);

		// Apply damage directly to the hit actor's combat manager
		double Damage = 50.0 * DamageMultiplier;  // Base weapon damage with multiplier (10% of typical 500 HP)
		double PoiseDamage = 25.0 * DamageMultiplier;

		// Try player combat manager first
		UAC_CombatManager* TargetCombatManager = HitActor->FindComponentByClass<UAC_CombatManager>();
		if (TargetCombatManager)
		{
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying %.1f damage to %s via CombatManager, %d status effects"),
				Damage, *HitActor->GetName(), StatusEffects.Num());

			TargetCombatManager->HandleIncomingWeaponDamage(
				AttackingCharacter,
				nullptr,  // GuardSound
				nullptr,  // PerfectGuardSound
				Hit,
				Damage,
				PoiseDamage,
				StatusEffects
			);
		}
		else
		{
			// Try AI combat manager (UAICombatManagerComponent is the actual class used by enemies)
			UAICombatManagerComponent* AICombatManager = HitActor->FindComponentByClass<UAICombatManagerComponent>();
			if (AICombatManager)
			{
				UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying %.1f damage to AI %s"),
					Damage, *HitActor->GetName());

				AICombatManager->HandleIncomingWeaponDamage_AI(AttackingCharacter, Damage, PoiseDamage, Hit);

				// Apply status effects separately for AI targets via StatusEffectManager
				if (StatusEffects.Num() > 0 && WeaponActor)
				{
					// Find the AI's status effect manager
					UStatusEffectManagerComponent* StatusEffectMgr = HitActor->FindComponentByClass<UStatusEffectManagerComponent>();
					if (StatusEffectMgr)
					{
						const TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication>& WeaponStatusEffects =
							WeaponActor->ItemInfo.EquipmentDetails.WeaponStatusEffectInfo;

						for (const auto& EffectPair : WeaponStatusEffects)
						{
							if (IsValid(EffectPair.Key))
							{
								UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying status effect %s to AI %s (Buildup: %.2f, Rank: %d)"),
									*EffectPair.Key->GetName(), *HitActor->GetName(),
									EffectPair.Value.BuildupAmount, EffectPair.Value.Rank);

								StatusEffectMgr->AddOneShotBuildup(
									EffectPair.Key,
									EffectPair.Value.Rank,
									EffectPair.Value.BuildupAmount * DamageMultiplier
								);
							}
						}
					}
					else
					{
						// Try legacy AC_StatusEffectManager
						UAC_StatusEffectManager* LegacyStatusEffectMgr = HitActor->FindComponentByClass<UAC_StatusEffectManager>();
						if (LegacyStatusEffectMgr)
						{
							const TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication>& WeaponStatusEffects =
								WeaponActor->ItemInfo.EquipmentDetails.WeaponStatusEffectInfo;

							for (const auto& EffectPair : WeaponStatusEffects)
							{
								if (IsValid(EffectPair.Key))
								{
									UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applying status effect %s to AI %s via legacy manager"),
										*EffectPair.Key->GetName(), *HitActor->GetName());

									LegacyStatusEffectMgr->AddOneShotBuildup(
										EffectPair.Key,
										EffectPair.Value.Rank,
										EffectPair.Value.BuildupAmount * DamageMultiplier
									);
								}
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[CollisionManager] No StatusEffectManager found on AI %s"),
								*HitActor->GetName());
						}
					}
				}
			}
			else
			{
				// No combat manager - check if this is a physics object (like destructibles)
				// Apply both damage and physics impulse to trigger Chaos destruction
				UPrimitiveComponent* HitComponent = Hit.GetComponent();
				if (HitComponent && HitComponent->IsSimulatingPhysics())
				{
					// Calculate impulse direction (from attacker toward hit point)
					FVector ImpulseDirection = (Hit.ImpactPoint - AttackingCharacter->GetActorLocation()).GetSafeNormal();

					// Apply point damage via UGameplayStatics - this triggers Chaos destruction
					// GeometryCollectionComponent responds to TakeDamage and breaks when damage exceeds threshold
					// NOTE: GC_Barrel has DamageThreshold=5000, so we need to exceed that!
					float DamageAmount = 10000.0f * DamageMultiplier;  // Must exceed 5000 threshold to break
					UGameplayStatics::ApplyPointDamage(
						HitActor,
						DamageAmount,
						ImpulseDirection,
						Hit,
						AttackingCharacter->GetInstigatorController(),
						AttackingCharacter,
						nullptr  // DamageTypeClass
					);

					// Also apply a strong impulse for visual effect
					FVector Impulse = ImpulseDirection * 10000.0 * DamageMultiplier;
					HitComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint);

					UE_LOG(LogTemp, Log, TEXT("[CollisionManager] Applied damage (%.1f) and impulse to %s: %s"),
						DamageAmount, *HitActor->GetName(), *Impulse.ToString());
				}
			}
		}
	}
}

void UCollisionManagerComponent::ToggleTrace_Implementation(bool bEnabled)
{
	if (bEnabled)
	{
		// Reset traced actors and last positions
		TracedActors.Empty();
		LastStartPosition = FVector::ZeroVector;
		LastEndPosition = FVector::ZeroVector;

		// Debug: Log trace positions at start
		FVector DebugStart, DebugEnd;
		GetTraceLocations(DebugStart, DebugEnd);
		float TraceLength = FVector::Dist(DebugStart, DebugEnd);
		UE_LOG(LogTemp, Log, TEXT("[CollisionManager] ToggleTrace: true - Start: %s, End: %s, Length: %.1f, Radius: %.1f"),
			*DebugStart.ToString(), *DebugEnd.ToString(), TraceLength, TraceRadius * TraceSizeMultiplier);

		// Enable tick
		SetComponentTickEnabled(true);
	}
	else
	{
		// Disable tick
		SetComponentTickEnabled(false);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INITIALIZATION [7/9]
// ═══════════════════════════════════════════════════════════════════════════════

void UCollisionManagerComponent::InitializeTracePoints_Implementation()
{
	if (AActor* Owner = GetOwner())
	{
		// Try skeletal mesh first
		if (USkeletalMeshComponent* SkelMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
		{
			TargetMesh = SkelMesh;
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] InitializeTracePoints - Using SkeletalMesh"));
		}
		// Fall back to static mesh
		else if (UStaticMeshComponent* StaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>())
		{
			TargetMesh = StaticMesh;
			UE_LOG(LogTemp, Log, TEXT("[CollisionManager] InitializeTracePoints - Using StaticMesh"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[CollisionManager] InitializeTracePoints - No mesh found"));
		}
	}
}
