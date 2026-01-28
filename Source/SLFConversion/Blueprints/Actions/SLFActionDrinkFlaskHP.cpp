// SLFActionDrinkFlaskHP.cpp
// Logic: Play drink montage from Action data, adjust HP stat
// bp_only uses FSLFFlaskData struct containing StatChangesPercent array, DrinkingMontage, VFX
#include "SLFActionDrinkFlaskHP.h"
#include "Components/StatManagerComponent.h"  // Use UStatManagerComponent, NOT UAC_StatManager
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/StatManagerComponent.h"
#include "Blueprints/B_Stat.h"

USLFActionDrinkFlaskHP::USLFActionDrinkFlaskHP()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Initialized"));
}

void USLFActionDrinkFlaskHP::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] ExecuteAction - Starting flask drinking"));

	if (!OwnerActor) return;

	// Get the Action data asset
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (!ActionData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDrinkFlaskHP] No Action data asset!"));
		return;
	}

	// Get RelevantData as FSLFFlaskData from the InstancedStruct (NOT FSLFMontage!)
	const FSLFFlaskData* FlaskData = ActionData->RelevantData.GetPtr<FSLFFlaskData>();
	if (!FlaskData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDrinkFlaskHP] Failed to get FSLFFlaskData from RelevantData - data may have been lost during migration!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Got FlaskData - StatChanges: %d, HasMontage: %s"),
		FlaskData->StatChangesPercent.Num(),
		!FlaskData->DrinkingMontage.IsNull() ? TEXT("YES") : TEXT("NO"));

	// Get stat manager for applying stat changes
	UStatManagerComponent* StatMgr = GetStatManager();
	if (!StatMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDrinkFlaskHP] No StatManager found!"));
		return;
	}

	// Apply each stat change from FlaskData (heal HP, restore FP, etc.)
	for (const FStatChangePercent& StatChange : FlaskData->StatChangesPercent)
	{
		// Calculate actual change amount from percentage
		// Get the stat's max value to calculate percentage
		UObject* StatObject = nullptr;
		FStatInfo StatInfo;
		StatMgr->GetStat(StatChange.StatTag, StatObject, StatInfo);

		double ChangeAmount = 0.0;
		if (StatObject)
		{
			ChangeAmount = StatInfo.MaxValue * (StatChange.PercentChange / 100.0);
		}

		UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Applying stat change: %s, Percent: %.1f%%, Amount: %.1f"),
			*StatChange.StatTag.ToString(), StatChange.PercentChange, ChangeAmount);

		// Apply the stat adjustment
		StatMgr->AdjustStat(StatChange.StatTag, StatChange.ValueType, ChangeAmount, false, StatChange.bTryActivateRegen);
	}

	// Play the drinking animation montage
	if (!FlaskData->DrinkingMontage.IsNull())
	{
		if (OwnerActor->Implements<UBPI_GenericCharacter>())
		{
			TSoftObjectPtr<UObject> SoftMontage(FlaskData->DrinkingMontage.ToSoftObjectPath());
			IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(
				OwnerActor,
				SoftMontage,
				1.0f,     // PlayRate
				0.0f,     // StartPosition
				NAME_None, // StartSection
				false     // bPrio
			);
			UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Playing drink montage: %s"), *FlaskData->DrinkingMontage.GetAssetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ActionDrinkFlaskHP] Owner doesn't implement BPI_GenericCharacter!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDrinkFlaskHP] No DrinkingMontage set in FlaskData!"));
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] ExecuteAction complete"));
}
