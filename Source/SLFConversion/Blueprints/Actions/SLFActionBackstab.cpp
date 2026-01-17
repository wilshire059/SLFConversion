// SLFActionBackstab.cpp
// Logic: Get execution target from CombatManager (staggered enemy or unaware enemy from behind),
// move player into position, play paired backstab animation on both attacker and victim
//
// bp_only flow (B_Action_Backstab.json):
// 1. Get ExecutionTarget from CombatManager
// 2. Call BPI_Enemy::GetExecutionMoveToTransform(Target) → MoveToLocation, Rotation
// 3. Call GenericLocationAndRotationLerp(Scale=2.0, TargetLocation, TargetRotation)
// 4. Get backstab montage from: Weapon → MovesetWeapons → ExecutionAsset → ExecuteBack.Animation
// 5. Play attacker montage via PlayMontageReplicated
// 6. Call OnBackstabbed on victim → triggers victim's executed montage
// 7. Clear ExecutionTarget
#include "SLFActionBackstab.h"
#include "AC_CombatManager.h"
#include "AC_EquipmentManager.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Interfaces/BPI_Enemy.h"
#include "Interfaces/BPI_Executable.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

USLFActionBackstab::USLFActionBackstab()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Initialized"));
}

void USLFActionBackstab::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] *** BACKSTAB ACTION *** Critical backstab attack"));
	UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] OwnerActor: %s, Action: %s"),
		OwnerActor ? *OwnerActor->GetName() : TEXT("NULL"),
		Action ? *Action->GetName() : TEXT("NULL"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[ActionBackstab] OwnerActor is NULL! Action cannot execute."));
		return;
	}

	// Get combat manager to find execution target
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (!CombatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] No combat manager"));
		return;
	}

	// Get execution target
	AActor* Target = CombatMgr->ExecutionTarget;
	if (!Target)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] No execution target set"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Executing backstab on: %s"), *Target->GetName());

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 1: Get execution move-to transform from target
	// bp_only: BPI_Enemy::GetExecutionMoveToTransform(Target) → MoveToLocation, Rotation
	// For backstab, we want to be BEHIND the enemy
	// ═══════════════════════════════════════════════════════════════════════════════
	FVector MoveToLocation = FVector::ZeroVector;
	FRotator MoveToRotation = FRotator::ZeroRotator;

	if (Target->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
	{
		// GetExecutionMoveToTransform gives us position in FRONT of enemy
		// For backstab, we want BEHIND - so we'll calculate it differently
		FVector TargetLocation = Target->GetActorLocation();
		FVector TargetBackward = -Target->GetActorForwardVector();
		MoveToLocation = TargetLocation + TargetBackward * 100.0f;
		MoveToRotation = Target->GetActorForwardVector().Rotation(); // Face same direction as enemy
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Backstab position: Location=%s, Rotation=%s"),
			*MoveToLocation.ToString(), *MoveToRotation.ToString());
	}
	else
	{
		// Fallback: position behind target
		FVector TargetLocation = Target->GetActorLocation();
		FVector ToTarget = (TargetLocation - OwnerActor->GetActorLocation()).GetSafeNormal();
		MoveToLocation = TargetLocation - ToTarget * 100.0f;
		MoveToRotation = ToTarget.Rotation();
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Target doesn't implement BPI_Enemy, using fallback position"));
	}

	// Keep player at their current Z height
	MoveToLocation.Z = OwnerActor->GetActorLocation().Z;

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 2: Move player into backstab position
	// bp_only: GenericLocationAndRotationLerp(Scale=2.0, Location, Rotation)
	// ═══════════════════════════════════════════════════════════════════════════════
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		const double LerpScale = 2.0; // Fast lerp (0.5 seconds duration)
		IBPI_GenericCharacter::Execute_GenericLocationAndRotationLerp(
			OwnerActor, LerpScale, MoveToLocation, MoveToRotation);
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Moving player to backstab position (Scale=%.1f)"), LerpScale);
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 3: Get backstab montage from weapon animset
	// bp_only: Weapon → ItemInfo → EquipmentDetails → MovesetWeapons → ExecutionAsset → ExecuteBack.Animation
	// ═══════════════════════════════════════════════════════════════════════════════
	UAnimMontage* AttackerMontage = nullptr;

	// Get EquipmentManager (check character first, then controller)
	UAC_EquipmentManager* EquipMgr = OwnerActor->FindComponentByClass<UAC_EquipmentManager>();
	if (!EquipMgr)
	{
		if (APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			if (AController* Controller = Pawn->GetController())
			{
				EquipMgr = Controller->FindComponentByClass<UAC_EquipmentManager>();
			}
		}
	}

	if (EquipMgr)
	{
		// Get active weapon slot (right hand)
		FGameplayTag WeaponSlot = EquipMgr->GetActiveWeaponSlot(true);
		UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] WeaponSlot: %s"), *WeaponSlot.ToString());

		// Get item at slot
		UPrimaryDataAsset* ItemAsset = nullptr;
		FGuid Id;
		EquipMgr->GetItemAtSlotSimple(WeaponSlot, ItemAsset, Id);

		if (UPDA_Item* WeaponItem = Cast<UPDA_Item>(ItemAsset))
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] WeaponItem: %s"), *WeaponItem->GetName());

			// Get MovesetWeapons (PDA_WeaponAnimset) from EquipmentDetails
			UObject* MovesetWeapons = WeaponItem->ItemInformation.EquipmentDetails.MovesetWeapons;
			if (MovesetWeapons)
			{
				UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] MovesetWeapons: %s"), *MovesetWeapons->GetName());

				// Cast to WeaponAnimset to get ExecutionAsset
				if (UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(MovesetWeapons))
				{
					// Get ExecutionAsset
					UPrimaryDataAsset* ExecAsset = Animset->ExecutionAsset;
					if (ExecAsset)
					{
						UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] ExecutionAsset: %s"), *ExecAsset->GetName());

						// Cast to ExecutionAnimData to get ExecuteBack (backstab uses back, not front)
						if (UPDA_ExecutionAnimData* ExecData = Cast<UPDA_ExecutionAnimData>(ExecAsset))
						{
							// Get ExecuteBack for backstab execution
							if (!ExecData->ExecuteBack.Animation.IsNull())
							{
								AttackerMontage = ExecData->ExecuteBack.Animation.LoadSynchronous();
								UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Found ExecuteBack montage: %s"),
									AttackerMontage ? *AttackerMontage->GetName() : TEXT("LOAD FAILED"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] ExecuteBack.Animation is null"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] ExecutionAsset is not UPDA_ExecutionAnimData (class: %s)"), *ExecAsset->GetClass()->GetName());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Animset has no ExecutionAsset"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] MovesetWeapons is not UPDA_WeaponAnimset (class: %s)"), *MovesetWeapons->GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] WeaponItem has no MovesetWeapons"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] No weapon item at slot, or not UPDA_Item"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] No EquipmentManager found"));
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 4: Play attacker's backstab animation
	// ═══════════════════════════════════════════════════════════════════════════════
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Playing ATTACKER montage: %s"), *AttackerMontage->GetName());
	}
	else if (!AttackerMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] No attacker montage available!"));
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 5: Trigger victim's backstab reaction via OnBackstabbed interface
	// bp_only: BPI_Executable::OnBackstabbed(Target, BackstabTag)
	// ═══════════════════════════════════════════════════════════════════════════════
	FGameplayTag BackstabTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.Backstab"), false);

	if (Target->GetClass()->ImplementsInterface(UBPI_Executable::StaticClass()))
	{
		IBPI_Executable::Execute_OnBackstabbed(Target, BackstabTag);
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Called OnBackstabbed on victim with tag: %s"), *BackstabTag.ToString());
	}

	// Play victim montage (always try, even if OnBackstabbed was called)
	UAnimMontage* VictimMontage = nullptr;

	UAICombatManagerComponent* TargetCombatMgr = Target->FindComponentByClass<UAICombatManagerComponent>();
	if (TargetCombatMgr)
	{
		VictimMontage = TargetCombatMgr->GetRelevantExecutedMontage(BackstabTag);
	}

	// Fallback: Load default executed montage directly if array was empty
	if (!VictimMontage)
	{
		static const FString DefaultExecutedMontagePath = TEXT("/Game/SoulslikeFramework/Demo/_Animations/Combat/Generic/AM_SLF_Generic_Executed.AM_SLF_Generic_Executed");
		VictimMontage = LoadObject<UAnimMontage>(nullptr, *DefaultExecutedMontagePath);
		if (VictimMontage)
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Loaded fallback victim montage: %s"), *VictimMontage->GetName());
		}
	}

	if (VictimMontage && Target->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(Target, VictimMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Warning, TEXT("[ActionBackstab] Playing VICTIM montage: %s"), *VictimMontage->GetName());
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 6: Clear execution target after use
	// ═══════════════════════════════════════════════════════════════════════════════
	CombatMgr->SetExecutionTarget(nullptr);
	UE_LOG(LogTemp, Log, TEXT("[ActionBackstab] Cleared ExecutionTarget"));
}
