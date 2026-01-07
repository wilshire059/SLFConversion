// SLFDestructible.cpp
#include "SLFDestructible.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/B_PickupItem.h"
#include "Engine/DataAsset.h"

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
			// Check if ItemClass is a data asset class
			UObject* ItemCDO = ItemClass->GetDefaultObject();
			UDataAsset* ItemAsset = Cast<UDataAsset>(ItemCDO);

			if (ItemAsset)
			{
				// Spawn a B_PickupItem with this item data
				FVector SpawnLocation = GetActorLocation() + FVector(
					FMath::RandRange(-50.0f, 50.0f),
					FMath::RandRange(-50.0f, 50.0f),
					50.0f);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(AB_PickupItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				if (AB_PickupItem* PickupItem = Cast<AB_PickupItem>(SpawnedActor))
				{
					// Item expects UPrimaryDataAsset*, cast from UDataAsset*
					PickupItem->Item = Cast<UPrimaryDataAsset>(ItemAsset);
					PickupItem->Count = 1;
					UE_LOG(LogTemp, Log, TEXT("[Destructible] Spawned loot: %s"), *ItemAsset->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Destructible] Item class %s is not a DataAsset"), *ItemClass->GetName());
			}
		}
	}
}
