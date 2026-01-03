// AN_SpawnProjectile.cpp
// C++ Animation Notify implementation for AN_SpawnProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - spawns a projectile at socket location

#include "AnimNotifies/AN_SpawnProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Blueprints/B_BaseProjectile.h"

UAN_SpawnProjectile::UAN_SpawnProjectile()
	: UseSocketRotation(true)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 0, 255, 255); // Magenta for projectile spawn
#endif
}

void UAN_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - No ProjectileClass set on %s"), *Owner->GetName());
		return;
	}

	// Get spawn transform from socket
	FTransform SpawnTransform;
	if (SpawnSocketName != NAME_None)
	{
		SpawnTransform = MeshComp->GetSocketTransform(SpawnSocketName);
	}
	else
	{
		SpawnTransform = Owner->GetActorTransform();
	}

	// If not using socket rotation, use actor rotation instead
	if (!UseSocketRotation)
	{
		SpawnTransform.SetRotation(Owner->GetActorQuat());
	}

	// Call SpawnProjectile via BPI_GenericCharacter interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SpawnProjectile(
			Owner,
			nullptr, // TargetActor - let projectile find target
			TSubclassOf<AActor>(ProjectileClass),
			SpawnTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			Owner,
			Cast<APawn>(Owner)
		);
		UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Spawned %s at socket %s on %s"),
			*ProjectileClass->GetName(), *SpawnSocketName.ToString(), *Owner->GetName());
	}
}

FString UAN_SpawnProjectile::GetNotifyName_Implementation() const
{
	if (ProjectileClass)
	{
		return FString::Printf(TEXT("Spawn %s"), *ProjectileClass->GetName());
	}
	return TEXT("Spawn Projectile");
}
