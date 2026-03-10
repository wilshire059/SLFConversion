// SLFActionGuardCounter.cpp
#include "SLFActionGuardCounter.h"
#include "GameFramework/Character.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/AC_CombatManager.h"
#include "SLFPrimaryDataAssets.h"

USLFActionGuardCounter::USLFActionGuardCounter()
{
}

void USLFActionGuardCounter::OpenCounterWindow()
{
	bCounterWindowOpen = true;
	UE_LOG(LogTemp, Log, TEXT("[GuardCounter] Counter window OPEN (%.2fs)"), CounterWindow);

	if (OwnerActor)
	{
		if (UWorld* World = OwnerActor->GetWorld())
		{
			World->GetTimerManager().SetTimer(WindowTimerHandle, this,
				&USLFActionGuardCounter::CloseCounterWindow, CounterWindow, false);
		}
	}
}

void USLFActionGuardCounter::CloseCounterWindow()
{
	bCounterWindowOpen = false;
	UE_LOG(LogTemp, Log, TEXT("[GuardCounter] Counter window closed"));
}

bool USLFActionGuardCounter::CanExecuteAction_Implementation()
{
	if (!OwnerActor) return false;

	// Check combat manager's guard counter window (persists between action instantiations)
	UAC_CombatManager* CombatMgr = OwnerActor->FindComponentByClass<UAC_CombatManager>();
	if (CombatMgr && CombatMgr->bGuardCounterWindowOpen)
	{
		return true;
	}

	// Fallback to local state (if OpenCounterWindow was called on this instance)
	return bCounterWindowOpen;
}

void USLFActionGuardCounter::ExecuteAction_Implementation()
{
	if (!OwnerActor) return;

	UE_LOG(LogTemp, Log, TEXT("[GuardCounter] Executing guard counter (%.1fx damage, %.1fx poise)"),
		DamageMultiplier, PoiseDamageMultiplier);

	bCounterWindowOpen = false;

	// Close combat manager's window too
	UAC_CombatManager* CombatMgr = OwnerActor->FindComponentByClass<UAC_CombatManager>();
	if (CombatMgr)
	{
		CombatMgr->bGuardCounterWindowOpen = false;
	}

	// Get weapon animset for heavy attack montage
	UDataAsset* Animset = GetWeaponAnimset();
	UPDA_WeaponAnimset* WeaponAnimset = Cast<UPDA_WeaponAnimset>(Animset);
	if (!WeaponAnimset) return;

	// Use heavy combo montage for guard counter (first hit only)
	UAnimMontage* Montage = WeaponAnimset->OneH_HeavyComboMontage_R.LoadSynchronous();
	if (!Montage) return;

	if (OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
		UE_LOG(LogTemp, Log, TEXT("[GuardCounter] Playing montage: %s"), *Montage->GetName());
	}
}
