// SLFActionPickupItemMontage.cpp
// C++ implementation for B_Action_PickupItemMontage
//
// Blueprint Logic (from JSON):
// 1. Event ExecuteAction
// 2. Get Action.RelevantData (FInstancedStruct)
// 3. GetInstancedStructValue<FMontage>
// 4. If Valid: Break FMontage to get AnimMontage (TSoftObjectPtr)
// 5. Call PlaySoftMontageReplicated on OwnerActor with defaults

#include "SLFActionPickupItemMontage.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
#include "Interfaces/BPI_GenericCharacter.h"

USLFActionPickupItemMontage::USLFActionPickupItemMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Initialized"));
}

void USLFActionPickupItemMontage::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] ExecuteAction - Play item pickup animation"));

	// 1. Get the Action data asset
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (!ActionData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] Action data asset is null or wrong type"));
		return;
	}

	TSoftObjectPtr<UObject> SoftMontage;

	// 2. FIRST try direct ActionMontage property (simple and reliable)
	if (!ActionData->ActionMontage.IsNull())
	{
		SoftMontage = TSoftObjectPtr<UObject>(ActionData->ActionMontage.ToSoftObjectPath());
		UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Found montage via ActionMontage: %s"), *SoftMontage.ToString());
	}
	// 3. FALLBACK: Try RelevantData (FInstancedStruct containing FSLFMontage)
	else
	{
		const FInstancedStruct& RelevantData = ActionData->RelevantData;
		const FSLFMontage* MontageData = RelevantData.GetPtr<FSLFMontage>();
		if (MontageData && !MontageData->AnimMontage.IsNull())
		{
			SoftMontage = TSoftObjectPtr<UObject>(MontageData->AnimMontage.ToSoftObjectPath());
			UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Found montage via RelevantData: %s"), *SoftMontage.ToString());
		}
	}

	// 4. Check if we found a valid montage
	if (SoftMontage.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] No valid montage found in ActionMontage or RelevantData"));
		return;
	}

	// 5. Call PlaySoftMontageReplicated on OwnerActor via BPI_GenericCharacter interface
	if (OwnerActor && OwnerActor->Implements<UBPI_GenericCharacter>())
	{
		IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(
			OwnerActor,
			SoftMontage,
			1.0,      // PlayRate
			0.0,      // StartPosition
			NAME_None, // StartSection
			false     // bPrio
		);

		UE_LOG(LogTemp, Log, TEXT("[ActionPickupItemMontage] Called PlaySoftMontageReplicated on %s"), *OwnerActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] OwnerActor is null or doesn't implement BPI_GenericCharacter"));
	}
}
