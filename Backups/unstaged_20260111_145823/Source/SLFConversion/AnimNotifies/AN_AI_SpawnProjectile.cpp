// AN_AI_SpawnProjectile.cpp
// C++ Animation Notify implementation for AN_AI_SpawnProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - spawns projectile for AI targeting their current target

#include "AnimNotifies/AN_AI_SpawnProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AIBehaviorManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Blueprints/B_BaseProjectile.h"

UAN_AI_SpawnProjectile::UAN_AI_SpawnProjectile()
	: UseSocketRotation(true)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 0, 255, 255); // Magenta for projectile spawn
#endif
}

void UAN_AI_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAN_AI_SpawnProjectile::Notify - No ProjectileClass set on %s"), *Owner->GetName());
		return;
	}

	// From Blueprint: Get spawn transform from socket
	FTransform SpawnTransform;
	if (SpawnSocketName != NAME_None)
	{
		SpawnTransform = MeshComp->GetSocketTransform(SpawnSocketName);
	}
	else
	{
		SpawnTransform = Owner->GetActorTransform();
	}

	// From Blueprint: If not using socket rotation, use actor rotation instead
	if (!UseSocketRotation)
	{
		SpawnTransform.SetRotation(Owner->GetActorQuat());
	}

	// From Blueprint: Get target from AI_BehaviorManager
	AActor* TargetActor = nullptr;
	UAIBehaviorManagerComponent* BehaviorManager = Owner->FindComponentByClass<UAIBehaviorManagerComponent>();
	if (BehaviorManager)
	{
		TargetActor = BehaviorManager->GetTarget();
	}

	// From Blueprint: Call SpawnProjectile via BPI_GenericCharacter interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SpawnProjectile(
			Owner,
			TargetActor,
			ProjectileClass,
			SpawnTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			Owner,
			Cast<APawn>(Owner)
		);
		UE_LOG(LogTemp, Log, TEXT("UAN_AI_SpawnProjectile::Notify - Spawned %s at socket %s targeting %s on %s"),
			*ProjectileClass->GetName(),
			*SpawnSocketName.ToString(),
			TargetActor ? *TargetActor->GetName() : TEXT("None"),
			*Owner->GetName());
	}
}

FString UAN_AI_SpawnProjectile::GetNotifyName_Implementation() const
{
	if (ProjectileClass)
	{
		return FString::Printf(TEXT("AI Spawn %s"), *ProjectileClass->GetName());
	}
	return TEXT("AI Spawn Projectile");
}
