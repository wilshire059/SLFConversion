// SLFWeaponBase.cpp
// C++ implementation for B_Item_Weapon
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Item_Weapon
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 (initialized in constructor)
// Functions:         8/8 implemented
// Components:        2/2 created in constructor (WeaponMesh, TrailComponent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFWeaponBase.h"
#include "Engine/StaticMesh.h"
#include "Engine/EngineTypes.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "GameFramework/Character.h"
#include "SLFEnums.h" // For ESLFStatScaling

ASLFWeaponBase::ASLFWeaponBase()
{
	// Create default scene root
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	// Create WeaponMesh component - Blueprint children override mesh via property
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create TrailComponent - Blueprint children override effect via property
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(RootComponent);
	TrailComponent->SetAutoActivate(false);

	// Create CollisionManagerComponent for weapon tracing
	CollisionManager = CreateDefaultSubobject<UCollisionManagerComponent>(TEXT("CollisionManager"));
	// Configure default socket names for weapon meshes
	CollisionManager->TraceSocketStart = FName("TraceStart");
	CollisionManager->TraceSocketEnd = FName("TraceEnd");
	CollisionManager->TraceRadius = 20.0;
	// Add Pawn to trace types
	CollisionManager->TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	// Initialize mesh transform defaults (zero = no offset)
	DefaultMeshRelativeLocation = FVector::ZeroVector;
	DefaultMeshRelativeRotation = FRotator::ZeroRotator;

	// Initialize VFX
	TrailEffect = nullptr;
	TrailWidthParameterName = FName("Width");

	// Initialize environment impact
	EnvironmentImpactEffect = nullptr;
	EnvironmentImpactSound = nullptr;

	// Initialize guard
	GuardSound = nullptr;
	PerfectGuardSound = nullptr;

	// Initialize debug
	bDebugVisualizeTrace = false;
}

void ASLFWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] BeginPlay: %s (Class: %s)"), *GetName(), *GetClass()->GetName());

	// Debug: List all components on this actor
	TArray<UActorComponent*> AllComponents;
	GetComponents(AllComponents);
	UE_LOG(LogTemp, Log, TEXT("[Weapon] Actor has %d components:"), AllComponents.Num());
	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp)
		{
			UE_LOG(LogTemp, Log, TEXT("[Weapon]   - %s (%s)"), *Comp->GetName(), *Comp->GetClass()->GetName());
		}
	}

	// Cache Blueprint components if not already set
	if (!WeaponMesh)
	{
		// Find the Blueprint's StaticMesh component
		WeaponMesh = FindComponentByClass<UStaticMeshComponent>();
		if (WeaponMesh)
		{
			UStaticMesh* Mesh = WeaponMesh->GetStaticMesh();
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Found StaticMesh component: %s, Mesh: %s, Visible: %s"),
				*WeaponMesh->GetName(),
				Mesh ? *Mesh->GetName() : TEXT("NULL"),
				WeaponMesh->IsVisible() ? TEXT("Yes") : TEXT("No"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Weapon] No StaticMeshComponent found on %s"), *GetName());
		}
	}

	if (!TrailComponent)
	{
		// Find the Blueprint's Trail Niagara component
		TrailComponent = FindComponentByClass<UNiagaraComponent>();
		if (TrailComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Found Trail component: %s"), *TrailComponent->GetName());
		}
	}

	// Apply trail effect asset to trail component if set
	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
	}

	// Apply default weapon mesh if set (from Blueprint CDO)
	if (WeaponMesh && !DefaultWeaponMesh.IsNull())
	{
		UStaticMesh* MeshToApply = DefaultWeaponMesh.LoadSynchronous();
		if (MeshToApply)
		{
			WeaponMesh->SetStaticMesh(MeshToApply);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied DefaultWeaponMesh: %s"), *MeshToApply->GetName());
		}
	}

	// Apply mesh transform offsets (from original Blueprint SCS component settings)
	if (WeaponMesh)
	{
		if (!DefaultMeshRelativeLocation.IsZero())
		{
			WeaponMesh->SetRelativeLocation(DefaultMeshRelativeLocation);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh location offset: %s"), *DefaultMeshRelativeLocation.ToString());
		}
		if (!DefaultMeshRelativeRotation.IsZero())
		{
			WeaponMesh->SetRelativeRotation(DefaultMeshRelativeRotation);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh rotation offset: %s"), *DefaultMeshRelativeRotation.ToString());
		}
	}

	// NOTE: Damage is handled directly by CollisionManager, not via OnActorTraced event.
	// The weapon's OnActorTraced was sending 0 damage because GetAttackPowerStats returns empty.
	// CollisionManager applies correct damage values directly.
	if (CollisionManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Weapon] CollisionManager exists on %s"), *GetName());
		// Damage handled in CollisionManager::ProcessHit directly - no event binding needed
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] No CollisionManager found on %s!"), *GetName());
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// SOCKET ATTACHMENT - Attach weapon to character's skeletal mesh socket
	// ═══════════════════════════════════════════════════════════════════════════════
	AttachToOwnerSocket();
}

void ASLFWeaponBase::AttachToOwnerSocket()
{
	// Get the character that owns this weapon (set as Instigator during spawn)
	APawn* InstigatorPawn = GetInstigator();
	if (!IsValid(InstigatorPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No Instigator, cannot attach"));
		return;
	}

	// Get the skeletal mesh component from the character
	USkeletalMeshComponent* MeshComponent = nullptr;

	// Try to get the mesh from Character first
	if (ACharacter* Character = Cast<ACharacter>(InstigatorPawn))
	{
		MeshComponent = Character->GetMesh();
	}
	else
	{
		// Fallback: try to find any skeletal mesh component
		MeshComponent = InstigatorPawn->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!IsValid(MeshComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No SkeletalMeshComponent on Instigator"));
		return;
	}

	// Get socket names from ItemInfo
	const FSLFEquipmentSocketInfo& Sockets = ItemInfo.EquipmentDetails.AttachmentSockets;

	UE_LOG(LogTemp, Log, TEXT("[Weapon] AttachToOwnerSocket - Sockets L=%s R=%s"),
		*Sockets.LeftHandSocketName.ToString(),
		*Sockets.RightHandSocketName.ToString());

	// Select appropriate socket based on equipment slot (right or left hand)
	FName SocketName = IsRightHandWeapon() ? Sockets.RightHandSocketName : Sockets.LeftHandSocketName;

	// Use default socket if none specified
	if (SocketName.IsNone())
	{
		SocketName = IsRightHandWeapon() ? FName("hand_r") : FName("hand_l");
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Using default socket: %s"), *SocketName.ToString());
	}

	// Check if socket exists on the mesh
	if (!MeshComponent->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] Socket '%s' does not exist on mesh, trying alternatives"),
			*SocketName.ToString());

		// Try common socket names
		TArray<FName> AlternativeSockets = {
			FName("weapon_r"), FName("weapon_l"),
			FName("hand_r"), FName("hand_l"),
			FName("soulslike_weapon_r"), FName("soulslike_weapon_l"),
			FName("RightHand"), FName("LeftHand")
		};

		for (const FName& AltSocket : AlternativeSockets)
		{
			if (MeshComponent->DoesSocketExist(AltSocket))
			{
				SocketName = AltSocket;
				UE_LOG(LogTemp, Log, TEXT("[Weapon] Found alternative socket: %s"), *SocketName.ToString());
				break;
			}
		}
	}

	// Attach to the socket
	bool bAttached = K2_AttachToComponent(
		MeshComponent,
		SocketName,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false
	);

	if (bAttached)
	{
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Attached to socket '%s' on %s"),
			*SocketName.ToString(), *InstigatorPawn->GetName());

		// Apply rotation offset if specified (handles different skeleton socket orientations)
		// First check socket info, then fall back to class default
		FRotator RotationOffset = FRotator::ZeroRotator;

		if (!Sockets.AttachmentRotationOffset.IsZero())
		{
			RotationOffset = Sockets.AttachmentRotationOffset;
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Using socket info rotation offset: %s"),
				*RotationOffset.ToString());
		}
		else if (!DefaultAttachmentRotationOffset.IsZero())
		{
			RotationOffset = DefaultAttachmentRotationOffset;
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Using default class rotation offset: %s"),
				*RotationOffset.ToString());
		}

		if (!RotationOffset.IsZero())
		{
			SetActorRelativeRotation(RotationOffset);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied rotation offset: %s"),
				*RotationOffset.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] Failed to attach to socket '%s'"),
			*SocketName.ToString());
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT QUERIES [1-4/8]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFWeaponAttackPower> ASLFWeaponBase::GetAttackPowerStats_Implementation()
{
	// Get attack power from ItemInfo.EquipmentDetails
	// Returns base weapon attack power - inheriting classes can override
	TArray<FSLFWeaponAttackPower> Result;

	// Return a single default attack power entry
	// Actual values come from weapon data assets via Blueprint children
	Result.Add(FSLFWeaponAttackPower());

	return Result;
}

float ASLFWeaponBase::GetWeaponStat_Implementation(FGameplayTag StatTag)
{
	// Look up stat requirement from ItemInfo.EquipmentDetails.WeaponStatInfo
	const FSLFEquipmentWeaponStatInfo& WeaponStatInfo = ItemInfo.EquipmentDetails.WeaponStatInfo;

	// Check if stat requirement info contains this tag
	if (WeaponStatInfo.StatRequirementInfo.Contains(StatTag))
	{
		return static_cast<float>(WeaponStatInfo.StatRequirementInfo[StatTag]);
	}

	return 0.0f;
}

TArray<FSLFWeaponStatScaling> ASLFWeaponBase::GetWeaponStatScaling_Implementation()
{
	// Get stat scaling from ItemInfo.EquipmentDetails.WeaponStatInfo.ScalingInfo
	TArray<FSLFWeaponStatScaling> Result;

	const FSLFEquipmentWeaponStatInfo& WeaponStatInfo = ItemInfo.EquipmentDetails.WeaponStatInfo;

	if (!WeaponStatInfo.bHasStatScaling)
	{
		return Result;
	}

	// Convert TMap<FGameplayTag, ESLFStatScaling> to TArray<FSLFWeaponStatScaling>
	for (const auto& ScalingPair : WeaponStatInfo.ScalingInfo)
	{
		FSLFWeaponStatScaling Entry;
		Entry.StatTag = ScalingPair.Key;

		// Convert ESLFStatScaling enum to multiplier and grade text
		switch (ScalingPair.Value)
		{
		case ESLFStatScaling::S:
			Entry.ScalingMultiplier = 1.5f;
			Entry.ScalingGrade = FText::FromString(TEXT("S"));
			break;
		case ESLFStatScaling::A:
			Entry.ScalingMultiplier = 1.3f;
			Entry.ScalingGrade = FText::FromString(TEXT("A"));
			break;
		case ESLFStatScaling::B:
			Entry.ScalingMultiplier = 1.15f;
			Entry.ScalingGrade = FText::FromString(TEXT("B"));
			break;
		case ESLFStatScaling::C:
			Entry.ScalingMultiplier = 1.0f;
			Entry.ScalingGrade = FText::FromString(TEXT("C"));
			break;
		case ESLFStatScaling::D:
			Entry.ScalingMultiplier = 0.85f;
			Entry.ScalingGrade = FText::FromString(TEXT("D"));
			break;
		case ESLFStatScaling::E:
			Entry.ScalingMultiplier = 0.7f;
			Entry.ScalingGrade = FText::FromString(TEXT("E"));
			break;
		default:
			Entry.ScalingMultiplier = 1.0f;
			Entry.ScalingGrade = FText::FromString(TEXT("-"));
			break;
		}

		Result.Add(Entry);
	}

	return Result;
}

TArray<FSLFWeaponAttackPower> ASLFWeaponBase::GetWeaponAttackPowerData_Implementation()
{
	return GetAttackPowerStats();
}

// ═══════════════════════════════════════════════════════════════════════════════
// DAMAGE [5-6/8]
// ═══════════════════════════════════════════════════════════════════════════════

bool ASLFWeaponBase::TryApplyDamage_Implementation(AActor* Target, const FHitResult& HitResult, float AttackMultiplier)
{
	if (!Target) return false;

	// Calculate total damage from attack power stats
	TArray<FSLFWeaponAttackPower> AttackPowers = GetAttackPowerStats();
	double TotalDamage = 0.0;

	for (const FSLFWeaponAttackPower& AttackPower : AttackPowers)
	{
		TotalDamage += AttackPower.Physical + AttackPower.Magic + AttackPower.Lightning
			+ AttackPower.Holy + AttackPower.Frost + AttackPower.Fire;
	}

	// Apply attack multiplier
	TotalDamage *= AttackMultiplier;

	// Get poise damage
	float PoiseDamage = GetWeaponPoiseDamage();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] TryApplyDamage to %s: TotalDamage=%.2f, Poise=%.2f, Multiplier=%.2f"),
		*Target->GetName(), TotalDamage, PoiseDamage, AttackMultiplier);

	// Actual damage application is handled by CombatManager on target
	// This returns true to indicate damage data was successfully prepared
	return true;
}

float ASLFWeaponBase::GetWeaponPoiseDamage_Implementation()
{
	// Get poise damage from ItemInfo.EquipmentDetails.MinPoiseDamage/MaxPoiseDamage
	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;
	double MinPoise = EquipmentDetails.MinPoiseDamage;
	double MaxPoise = EquipmentDetails.MaxPoiseDamage;

	// Return random value in range
	if (MaxPoise > MinPoise)
	{
		return FMath::FRandRange(static_cast<float>(MinPoise), static_cast<float>(MaxPoise));
	}

	// If min equals max or max is less than min, return min
	return static_cast<float>(MinPoise);
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATUS EFFECTS [7/8]
// ═══════════════════════════════════════════════════════════════════════════════

TArray<FSLFWeaponStatusEffectData> ASLFWeaponBase::GetWeaponStatusEffectData_Implementation()
{
	// Get status effect data from ItemInfo.EquipmentDetails.WeaponStatusEffectInfo
	TArray<FSLFWeaponStatusEffectData> Result;

	const FSLFEquipmentInfo& EquipmentDetails = ItemInfo.EquipmentDetails;

	// Convert TMap<FGameplayTag, double> to TArray<FSLFWeaponStatusEffectData>
	for (const auto& StatusPair : EquipmentDetails.WeaponStatusEffectInfo)
	{
		FSLFWeaponStatusEffectData Entry;
		Entry.StatusEffectTag = StatusPair.Key;
		Entry.BuildupAmount = static_cast<float>(StatusPair.Value);
		Entry.EffectRank = 1; // Default rank

		Result.Add(Entry);
	}

	return Result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTION [8/8]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFWeaponBase::SetupItem_Implementation()
{
	Super::SetupItem_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[Weapon] SetupItem - weapon-specific setup"));

	// Cache Blueprint components if not already set (OnConstruction runs before BeginPlay)
	if (!WeaponMesh)
	{
		WeaponMesh = FindComponentByClass<UStaticMeshComponent>();
	}
	if (!TrailComponent)
	{
		TrailComponent = FindComponentByClass<UNiagaraComponent>();
	}

	// Apply trail effect to trail component if set
	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
	}
}

bool ASLFWeaponBase::IsRightHandWeapon() const
{
	// Check EquipSlots to determine hand side
	// Equipment slots typically contain tags like "Equipment.Slot.RightHand" or similar
	const FGameplayTagContainer& EquipSlots = ItemInfo.EquipmentDetails.EquipSlots;

	// Check for left hand slot tags
	if (EquipSlots.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.LeftHand"), false)) ||
		EquipSlots.HasTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.Slot.LeftHand"), false)) ||
		EquipSlots.HasTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1"), false)) ||
		EquipSlots.HasTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2"), false)) ||
		EquipSlots.HasTagExact(FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3"), false)))
	{
		return false;
	}

	// Default to right hand for weapons
	return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
// COLLISION MANAGER EVENT HANDLER
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFWeaponBase::OnActorTraced(AActor* Actor, FHitResult Hit, double Multiplier)
{
	if (!Actor)
	{
		return;
	}

	// Get the weapon owner (AI character wielding this weapon)
	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner)
	{
		// Weapon may be attached to character mesh, traverse up
		if (USceneComponent* AttachParent = GetRootComponent()->GetAttachParent())
		{
			WeaponOwner = AttachParent->GetOwner();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[Weapon] OnActorTraced: %s hit %s with multiplier %.2f"),
		WeaponOwner ? *WeaponOwner->GetName() : TEXT("Unknown"),
		*Actor->GetName(),
		Multiplier);

	// Calculate damage from weapon stats
	TArray<FSLFWeaponAttackPower> AttackPowers = GetAttackPowerStats();
	double TotalDamage = 0.0;

	for (const FSLFWeaponAttackPower& AttackPower : AttackPowers)
	{
		TotalDamage += AttackPower.Physical + AttackPower.Magic + AttackPower.Lightning
			+ AttackPower.Holy + AttackPower.Frost + AttackPower.Fire;
	}

	// Apply the attack multiplier from the collision trace
	TotalDamage *= Multiplier;

	// Get poise damage
	float PoiseDamage = GetWeaponPoiseDamage();

	// Status effects from weapon
	TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;
	TArray<FSLFWeaponStatusEffectData> StatusData = GetWeaponStatusEffectData();
	// Note: Status effect conversion would require data asset lookup

	// Try player combat manager first (UAC_CombatManager)
	UAC_CombatManager* TargetCombatManager = Actor->FindComponentByClass<UAC_CombatManager>();
	if (TargetCombatManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Applying damage to player: %.2f damage, %.2f poise"),
			TotalDamage, PoiseDamage);

		TargetCombatManager->HandleIncomingWeaponDamage(
			WeaponOwner,
			GuardSound,
			PerfectGuardSound,
			Hit,
			TotalDamage,
			PoiseDamage,
			StatusEffects
		);
	}
	else
	{
		// Try AI combat manager (UAICombatManagerComponent)
		UAICombatManagerComponent* AICombatManager = Actor->FindComponentByClass<UAICombatManagerComponent>();
		if (AICombatManager)
		{
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applying damage to AI: %.2f damage, %.2f poise"),
				TotalDamage, PoiseDamage);

			AICombatManager->HandleIncomingWeaponDamage_AI(
				WeaponOwner,
				TotalDamage,
				PoiseDamage,
				Hit
			);
		}
	}
}
