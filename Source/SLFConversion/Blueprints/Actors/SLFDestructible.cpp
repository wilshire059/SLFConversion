// SLFDestructible.cpp
#include "SLFDestructible.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ASLFDestructible::ASLFDestructible()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[Destructible] Created"));
}

void ASLFDestructible::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Log, TEXT("[Destructible] BeginPlay - Health: %.0f/%.0f"), CurrentHealth, MaxHealth);
}

void ASLFDestructible::ApplyDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (bIsDestroyed)
	{
		return;
	}

	CurrentHealth -= Damage;
	OnDestructibleDamaged.Broadcast(Damage, DamageCauser);
	UE_LOG(LogTemp, Log, TEXT("[Destructible] Damaged: %.0f, Remaining: %.0f"), Damage, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		DestroyObject(DamageCauser);
	}
}

void ASLFDestructible::DestroyObject_Implementation(AActor* Destroyer)
{
	if (!bIsDestroyed)
	{
		bIsDestroyed = true;
		UE_LOG(LogTemp, Log, TEXT("[Destructible] Destroyed by: %s"), Destroyer ? *Destroyer->GetName() : TEXT("None"));

		// Play effects
		if (DestructionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestructionEffect, GetActorLocation());
		}

		if (DestructionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DestructionSound, GetActorLocation());
		}

		SpawnDebris();
		SpawnLoot();

		OnDestructibleDestroyed.Broadcast(Destroyer);

		// Hide mesh
		if (DestructibleMesh) DestructibleMesh->SetVisibility(false);
		if (DestructibleMesh) DestructibleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Destroy actor after delay
		SetLifeSpan(5.0f);
	}
}

void ASLFDestructible::SpawnDebris_Implementation()
{
	if (DebrisClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(DebrisClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		UE_LOG(LogTemp, Log, TEXT("[Destructible] Spawned debris"));
	}
}

void ASLFDestructible::SpawnLoot_Implementation()
{
	for (TSubclassOf<UObject> ItemClass : LootOnDestroy)
	{
		if (ItemClass)
		{
			// TODO: Spawn pickup or add to inventory
			UE_LOG(LogTemp, Log, TEXT("[Destructible] Would spawn loot: %s"), *ItemClass->GetName());
		}
	}
}
