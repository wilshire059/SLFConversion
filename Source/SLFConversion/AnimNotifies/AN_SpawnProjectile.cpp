// AN_SpawnProjectile.cpp
// C++ Animation Notify implementation for AN_SpawnProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - spawns a projectile at socket location
//
// UPDATED 2026-01-28: Now dynamically reads projectile class from active tool item's ItemClass property

#include "AnimNotifies/AN_SpawnProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Blueprints/B_BaseProjectile.h"
#include "Components/AC_EquipmentManager.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

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
	TSubclassOf<AActor> FinalProjectileClass = nullptr;

	// ALWAYS check the active tool item's ItemClass FIRST
	// This allows each item (spell, throwing knife, etc.) to specify its own projectile class
	// Only fall back to notify's hardcoded ProjectileClass if item doesn't have one
	UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Checking active tool item for projectile class..."));

	// Get equipment manager from pawn
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - Owner is not a Pawn!"));
	}
	else
	{
		// Debug: List all components on pawn
		TArray<UActorComponent*> AllComponents;
		OwnerPawn->GetComponents(AllComponents);
		UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Pawn has %d components:"), AllComponents.Num());
		for (UActorComponent* Comp : AllComponents)
		{
			if (Comp && Comp->GetClass()->GetName().Contains(TEXT("Equipment")))
			{
				UE_LOG(LogTemp, Log, TEXT("  - %s (class: %s)"), *Comp->GetName(), *Comp->GetClass()->GetName());
			}
		}

		UAC_EquipmentManager* EquipMgr = OwnerPawn->FindComponentByClass<UAC_EquipmentManager>();
		if (!EquipMgr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - No UAC_EquipmentManager found on pawn, trying controller..."));

			// Try to get from controller
			AController* Controller = OwnerPawn->GetController();
			if (Controller)
			{
				EquipMgr = Controller->FindComponentByClass<UAC_EquipmentManager>();
				if (EquipMgr)
				{
					UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Found UAC_EquipmentManager on controller"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - No UAC_EquipmentManager on controller either!"));
				}
			}
		}

		// Continue with item lookup if we found EquipMgr (from pawn OR controller)
		if (EquipMgr)
		{
			FGameplayTag ActiveToolSlot = EquipMgr->GetActiveToolSlot();
			UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - ActiveToolSlot: %s"), *ActiveToolSlot.ToString());

			if (!ActiveToolSlot.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - ActiveToolSlot is not valid!"));
			}
			else
			{
				UPrimaryDataAsset* ItemAsset = nullptr;
				FGuid ItemId;
				EquipMgr->GetItemAtSlotSimple(ActiveToolSlot, ItemAsset, ItemId);

				if (!ItemAsset)
				{
					UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - No item at ActiveToolSlot!"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Found item: %s (class: %s)"),
						*ItemAsset->GetName(), *ItemAsset->GetClass()->GetName());

					if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
					{
						// Get ItemClass from ItemInformation
						TSoftClassPtr<AActor> ItemClassSoft = ItemData->ItemInformation.ItemClass;
						UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - ItemClass path: %s, IsNull: %s"),
							*ItemClassSoft.ToString(), ItemClassSoft.IsNull() ? TEXT("YES") : TEXT("NO"));

						if (!ItemClassSoft.IsNull())
						{
							UClass* LoadedClass = ItemClassSoft.LoadSynchronous();
							if (LoadedClass)
							{
								FinalProjectileClass = LoadedClass;
								UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - Using ItemClass from item %s: %s"),
									*ItemData->GetName(), *FinalProjectileClass->GetName());
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - Failed to load ItemClass!"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Item %s has no ItemClass, will use notify default"), *ItemData->GetName());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - Item is not UPDA_Item! Actual class: %s"),
							*ItemAsset->GetClass()->GetName());
					}
				}
			}
		}
	}

	// Fall back to notify's hardcoded ProjectileClass only if item didn't specify one
	if (!FinalProjectileClass && ProjectileClass)
	{
		FinalProjectileClass = ProjectileClass;
		UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Using notify's default ProjectileClass: %s"), *FinalProjectileClass->GetName());
	}

	if (!FinalProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAN_SpawnProjectile::Notify - No ProjectileClass found for %s"), *Owner->GetName());
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

	// Use actor's forward direction for projectile direction (not socket rotation)
	// This makes projectiles go straight forward from the character
	if (!UseSocketRotation)
	{
		SpawnTransform.SetRotation(Owner->GetActorQuat());
	}
	else
	{
		// Even with UseSocketRotation, for projectiles we want forward direction
		// Get control rotation if available (where player is aiming), otherwise actor forward
		APawn* PawnOwner = Cast<APawn>(Owner);
		if (PawnOwner && PawnOwner->GetController())
		{
			FRotator ControlRotation = PawnOwner->GetControlRotation();
			// Only use yaw for horizontal aiming, keep pitch level for throwing
			ControlRotation.Pitch = 0.0f;
			ControlRotation.Roll = 0.0f;
			SpawnTransform.SetRotation(ControlRotation.Quaternion());
		}
		else
		{
			SpawnTransform.SetRotation(Owner->GetActorQuat());
		}
	}

	// Call SpawnProjectile via BPI_GenericCharacter interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SpawnProjectile(
			Owner,
			nullptr, // TargetActor - let projectile find target
			FinalProjectileClass,
			SpawnTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			Owner,
			Cast<APawn>(Owner)
		);
		UE_LOG(LogTemp, Log, TEXT("UAN_SpawnProjectile::Notify - Spawned %s at socket %s on %s"),
			*FinalProjectileClass->GetName(), *SpawnSocketName.ToString(), *Owner->GetName());
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
