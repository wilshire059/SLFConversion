// SLFActionExecute.cpp
// Logic: Get execution target from CombatManager (staggered/poise-broken enemy),
// move player into position, play paired execution/riposte animation on both attacker and victim
//
// bp_only flow (B_Action_Execute.json):
// 1. Get ExecutionTarget from CombatManager
// 2. Call BPI_Enemy::GetExecutionMoveToTransform(Target) → MoveToLocation, Rotation
// 3. Call GenericLocationAndRotationLerp(Scale=2.0, TargetLocation, TargetRotation)
// 4. Get execution montage from: Weapon → MovesetWeapons → ExecutionAsset → ExecuteFront.Animation
// 5. Play attacker montage via PlayMontageReplicated
// 6. Play victim montage via GetRelevantExecutedMontage → PlayMontageReplicated
// 7. Clear ExecutionTarget
#include "SLFActionExecute.h"
#include "AC_CombatManager.h"
#include "AC_EquipmentManager.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Interfaces/BPI_Enemy.h"
#include "Interfaces/BPI_Player.h"
#include "Interfaces/BPI_Executable.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "TimerManager.h"

USLFActionExecute::USLFActionExecute()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Initialized"));
}

void USLFActionExecute::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] *** EXECUTE ACTION *** Riposte/execution attack"));
	UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] OwnerActor: %s, Action: %s"),
		OwnerActor ? *OwnerActor->GetName() : TEXT("NULL"),
		Action ? *Action->GetName() : TEXT("NULL"));

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[ActionExecute] OwnerActor is NULL! Action cannot execute."));
		return;
	}

	// Get combat manager to find execution target
	UAC_CombatManager* CombatMgr = GetCombatManager();
	if (!CombatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No combat manager"));
		return;
	}

	// Get execution target (should be a poise-broken/staggered enemy)
	AActor* Target = CombatMgr->ExecutionTarget;
	if (!Target)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] No execution target set"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Executing on target: %s"), *Target->GetName());

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 1: Get execution move-to transform from target
	// bp_only: BPI_Enemy::GetExecutionMoveToTransform(Target) → MoveToLocation, Rotation
	// ═══════════════════════════════════════════════════════════════════════════════
	FVector MoveToLocation = FVector::ZeroVector;
	FRotator MoveToRotation = FRotator::ZeroRotator;

	if (Target->GetClass()->ImplementsInterface(UBPI_Enemy::StaticClass()))
	{
		IBPI_Enemy::Execute_GetExecutionMoveToTransform(Target, MoveToLocation, MoveToRotation);
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] GetExecutionMoveToTransform: Location=%s, Rotation=%s"),
			*MoveToLocation.ToString(), *MoveToRotation.ToString());
	}
	else
	{
		// Fallback: position in front of target facing them
		FVector TargetLocation = Target->GetActorLocation();
		FVector ToTarget = (TargetLocation - OwnerActor->GetActorLocation()).GetSafeNormal();
		MoveToLocation = TargetLocation - ToTarget * 100.0f;
		MoveToRotation = ToTarget.Rotation();
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Target doesn't implement BPI_Enemy, using fallback position"));
	}

	// Keep player at their current Z height (bp_only uses player's Z, not target's)
	MoveToLocation.Z = OwnerActor->GetActorLocation().Z;

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 2: Move player into execution position
	// bp_only: GenericLocationAndRotationLerp(Scale=2.0, Location, Rotation)
	// ═══════════════════════════════════════════════════════════════════════════════
	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		const double LerpScale = 2.0; // Fast lerp (0.5 seconds duration)
		IBPI_GenericCharacter::Execute_GenericLocationAndRotationLerp(
			OwnerActor, LerpScale, MoveToLocation, MoveToRotation);
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Moving player to execution position (Scale=%.1f)"), LerpScale);
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 3: Get execution montage from weapon animset
	// bp_only: Weapon → ItemInfo → EquipmentDetails → MovesetWeapons → ExecutionAsset → ExecuteFront.Animation
	// ═══════════════════════════════════════════════════════════════════════════════
	UAnimMontage* AttackerMontage = nullptr;
	FGameplayTag ExecutionTypeTag; // Tag from ExecuteFront to match victim's DataTable lookup

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
		UE_LOG(LogTemp, Log, TEXT("[ActionExecute] WeaponSlot: %s"), *WeaponSlot.ToString());

		// Get item at slot
		UPrimaryDataAsset* ItemAsset = nullptr;
		FGuid Id;
		EquipMgr->GetItemAtSlotSimple(WeaponSlot, ItemAsset, Id);

		if (UPDA_Item* WeaponItem = Cast<UPDA_Item>(ItemAsset))
		{
			UE_LOG(LogTemp, Log, TEXT("[ActionExecute] WeaponItem: %s"), *WeaponItem->GetName());

			// Get MovesetWeapons (PDA_WeaponAnimset) from EquipmentDetails
			UObject* MovesetWeapons = WeaponItem->ItemInformation.EquipmentDetails.MovesetWeapons;
			if (MovesetWeapons)
			{
				UE_LOG(LogTemp, Log, TEXT("[ActionExecute] MovesetWeapons: %s"), *MovesetWeapons->GetName());

				// Cast to WeaponAnimset to get ExecutionAsset
				if (UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(MovesetWeapons))
				{
					// Get ExecutionAsset
					UPrimaryDataAsset* ExecAsset = Animset->ExecutionAsset;
					if (ExecAsset)
					{
						UE_LOG(LogTemp, Log, TEXT("[ActionExecute] ExecutionAsset: %s"), *ExecAsset->GetName());

						// Cast to ExecutionAnimData to get ExecuteFront
						if (UPDA_ExecutionAnimData* ExecData = Cast<UPDA_ExecutionAnimData>(ExecAsset))
						{
							// Get ExecuteFront for frontal execution
							if (!ExecData->ExecuteFront.Animation.IsNull())
							{
								AttackerMontage = ExecData->ExecuteFront.Animation.LoadSynchronous();
								UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Found ExecuteFront montage: %s"),
									AttackerMontage ? *AttackerMontage->GetName() : TEXT("LOAD FAILED"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] ExecuteFront.Animation is null"));
							}

							// Get the execution type tag from the animation data
							// This tag matches the DataTable row for victim's executed montage
							if (ExecData->ExecuteFront.Tag.IsValid())
							{
								ExecutionTypeTag = ExecData->ExecuteFront.Tag;
								UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] ExecuteFront.Tag: %s"), *ExecutionTypeTag.ToString());
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] ExecuteFront.Tag is not valid"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] ExecutionAsset is not UPDA_ExecutionAnimData (class: %s)"), *ExecAsset->GetClass()->GetName());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Animset has no ExecutionAsset"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] MovesetWeapons is not UPDA_WeaponAnimset (class: %s)"), *MovesetWeapons->GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] WeaponItem has no MovesetWeapons"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No weapon item at slot, or not UPDA_Item"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No EquipmentManager found"));
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 4: Play attacker's execution animation
	// ═══════════════════════════════════════════════════════════════════════════════
	if (AttackerMontage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, AttackerMontage, 1.0, 0.0, NAME_None);
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Playing ATTACKER montage: %s"), *AttackerMontage->GetName());
	}
	else if (!AttackerMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] No attacker montage available!"));
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 5: Trigger victim's "being executed" via BPI_Executable::OnExecuted
	// bp_only: Uses a 0.1 second timer delay before calling OnExecuted
	// This allows the attacker's animation to start before the victim reacts
	// ExecutionTypeTag was set in STEP 3 from ExecData->ExecuteFront.Tag
	// ═══════════════════════════════════════════════════════════════════════════════
	if (Target->GetClass()->ImplementsInterface(UBPI_Executable::StaticClass()))
	{
		// bp_only: Set Timer by Event with 0.1 second delay before calling OnExecuted
		// This syncs the victim's reaction with the attacker's animation
		FTimerHandle ExecutionTimerHandle;
		TWeakObjectPtr<AActor> WeakTarget = Target;
		FGameplayTag CapturedTag = ExecutionTypeTag;

		if (UWorld* World = OwnerActor->GetWorld())
		{
			World->GetTimerManager().SetTimer(ExecutionTimerHandle, [WeakTarget, CapturedTag]()
			{
				if (WeakTarget.IsValid())
				{
					IBPI_Executable::Execute_OnExecuted(WeakTarget.Get(), CapturedTag);
					UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Called OnExecuted on victim with tag: %s (after 0.1s delay)"), *CapturedTag.ToString());
				}
			}, 0.1f, false);

			UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Set 0.1s timer before calling OnExecuted"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Target %s does NOT implement BPI_Executable!"), *Target->GetName());
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 5B: Camera behavior - pan to side during execution
	// bp_only: Uses LS_Cam_Execute level sequence (triggered 0.1s after execution starts)
	// ═══════════════════════════════════════════════════════════════════════════════
	bool bImplementsPlayerInterface = OwnerActor->GetClass()->ImplementsInterface(UBPI_Player::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] OwnerActor %s implements IBPI_Player: %s"),
		*OwnerActor->GetName(), bImplementsPlayerInterface ? TEXT("YES") : TEXT("NO"));

	if (bImplementsPlayerInterface)
	{
		// Load the execution camera sequence
		static const FString CameraSequencePath = TEXT("/Game/SoulslikeFramework/Cinematics/LS_Cam_Execute.LS_Cam_Execute");
		ULevelSequence* CameraSequence = LoadObject<ULevelSequence>(nullptr, *CameraSequencePath);

		UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] CameraSequence load result: %s"),
			CameraSequence ? *CameraSequence->GetName() : TEXT("NULL"));

		if (CameraSequence)
		{
			// Use default playback settings - sequence asset defines completion behavior
			FMovieSceneSequencePlaybackSettings PlaybackSettings;
			PlaybackSettings.bPauseAtEnd = false;
			PlaybackSettings.PlayRate = 1.0f;

			// Play the camera sequence via BPI_Player interface
			IBPI_Player::Execute_PlayCameraSequence(OwnerActor, CameraSequence, PlaybackSettings);
			UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Called PlayCameraSequence with: LS_Cam_Execute"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionExecute] Failed to load camera sequence: %s"), *CameraSequencePath);
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// STEP 6: Clear execution target after use
	// ═══════════════════════════════════════════════════════════════════════════════
	CombatMgr->SetExecutionTarget(nullptr);
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Cleared ExecutionTarget"));
}
