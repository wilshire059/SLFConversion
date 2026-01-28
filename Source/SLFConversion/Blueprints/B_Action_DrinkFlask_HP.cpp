// B_Action_DrinkFlask_HP.cpp
// C++ implementation for Blueprint B_Action_DrinkFlask_HP
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_DrinkFlask_HP.json
//
// bp_only Logic:
// 1. Get Action.RelevantData as FFlaskData (InstancedStruct)
// 2. Break FFlaskData to get StatChangesPercent array, DrinkingMontage, VFX
// 3. For each FStatChangePercent: call GetChangeAmountFromPercent, then AdjustStat
// 4. After loop: PlaySoftMontageReplicated with DrinkingMontage

#include "Blueprints/B_Action_DrinkFlask_HP.h"
#include "Components/StatManagerComponent.h"  // Use UStatManagerComponent
#include "Blueprints/B_Stat.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "StructUtils/InstancedStruct.h"

UB_Action_DrinkFlask_HP::UB_Action_DrinkFlask_HP()
{
}

void UB_Action_DrinkFlask_HP::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] ExecuteAction - Starting flask drinking"));

	// Get the Action data asset (Action is UDataAsset* in base class, cast to UPDA_ActionBase)
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (!ActionData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] No Action data asset! (Action ptr: %p)"), Action);
		return;
	}

	// Get RelevantData as FFlaskData from the InstancedStruct
	const FSLFFlaskData* FlaskData = ActionData->RelevantData.GetPtr<FSLFFlaskData>();
	if (!FlaskData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] Failed to get FFlaskData from RelevantData!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] Got FlaskData - StatChanges: %d, HasMontage: %s"),
		FlaskData->StatChangesPercent.Num(),
		!FlaskData->DrinkingMontage.IsNull() ? TEXT("YES") : TEXT("NO"));

	// Get stat manager for applying stat changes
	UStatManagerComponent* StatManager = GetStatManager();
	if (!StatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] No StatManager found!"));
		return;
	}

	// Apply each stat change (heal HP, restore FP, etc.)
	for (const FStatChangePercent& StatChange : FlaskData->StatChangesPercent)
	{
		// Calculate actual change amount from percentage
		double ChangeAmount = GetChangeAmountFromPercent(StatChange.StatTag, StatChange.PercentChange);

		UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] Applying stat change: %s, Percent: %.1f%%, Amount: %.1f"),
			*StatChange.StatTag.ToString(), StatChange.PercentChange, ChangeAmount);

		// Apply the stat adjustment
		StatManager->AdjustStat(StatChange.StatTag, StatChange.ValueType, ChangeAmount, false, StatChange.bTryActivateRegen);
	}

	// Play the drinking animation montage
	if (!FlaskData->DrinkingMontage.IsNull())
	{
		UAnimMontage* Montage = FlaskData->DrinkingMontage.LoadSynchronous();
		if (Montage)
		{
			// Get the owner's anim instance via out parameters
			UAnimInstance* AnimInstance = nullptr;
			UAnimInstance* AnimInstance1 = nullptr;
			GetOwnerAnimInstance(AnimInstance, AnimInstance1);

			if (AnimInstance)
			{
				float Duration = AnimInstance->Montage_Play(Montage, 1.0f);
				UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] Playing montage %s, Duration: %.2f"), *Montage->GetName(), Duration);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] No AnimInstance to play montage!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] Failed to load DrinkingMontage!"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] ExecuteAction complete"));
}

double UB_Action_DrinkFlask_HP::GetChangeAmountFromPercent_Implementation(const FGameplayTag& Stat, double PercentChange)
{
	// Get the stat manager to look up the stat's max value
	UStatManagerComponent* StatManager = GetStatManager();
	if (!StatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] GetChangeAmountFromPercent - No StatManager!"));
		return 0.0;
	}

	// Get the stat to find its MaxValue
	UObject* StatObject = nullptr;
	FStatInfo StatInfo;
	StatManager->GetStat(Stat, StatObject, StatInfo);

	if (!StatObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrinkFlask] GetChangeAmountFromPercent - Stat not found: %s"), *Stat.ToString());
		return 0.0;
	}

	// Calculate: MaxValue * (PercentChange / 100)
	double Result = StatInfo.MaxValue * (PercentChange / 100.0);
	UE_LOG(LogTemp, Log, TEXT("[DrinkFlask] GetChangeAmountFromPercent - %s MaxValue: %.1f, Percent: %.1f%%, Result: %.1f"),
		*Stat.ToString(), StatInfo.MaxValue, PercentChange, Result);

	return Result;
}
