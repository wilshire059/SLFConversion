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

	// 2. Get RelevantData (FInstancedStruct containing FSLFMontage)
	const FInstancedStruct& RelevantData = ActionData->RelevantData;

	// 3. Extract FSLFMontage from the FInstancedStruct
	const FSLFMontage* MontageData = RelevantData.GetPtr<FSLFMontage>();
	if (!MontageData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] RelevantData does not contain FSLFMontage"));
		return;
	}

	// 4. Get the AnimMontage soft pointer from FSLFMontage
	const TSoftObjectPtr<UAnimMontage>& AnimMontageSoft = MontageData->AnimMontage;
	if (AnimMontageSoft.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionPickupItemMontage] AnimMontage soft pointer is null"));
		return;
	}

	// 5. Call PlaySoftMontageReplicated on OwnerActor via BPI_GenericCharacter interface
	if (OwnerActor && OwnerActor->Implements<UBPI_GenericCharacter>())
	{
		// Convert TSoftObjectPtr<UAnimMontage> to TSoftObjectPtr<UObject> for interface call
		TSoftObjectPtr<UObject> SoftMontage(AnimMontageSoft.ToSoftObjectPath());

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
