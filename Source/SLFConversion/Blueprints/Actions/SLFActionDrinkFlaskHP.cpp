// SLFActionDrinkFlaskHP.cpp
// Logic: Play drink montage from Action data, adjust HP stat
#include "SLFActionDrinkFlaskHP.h"
#include "SLFPrimaryDataAssets.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/StatManagerComponent.h"

USLFActionDrinkFlaskHP::USLFActionDrinkFlaskHP()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Initialized"));
}

void USLFActionDrinkFlaskHP::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] ExecuteAction - Use healing flask"));

	if (!OwnerActor) return;

	// 1. Get the Action data asset for montage
	UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action);
	if (ActionData)
	{
		// Get RelevantData (FInstancedStruct containing FSLFMontage)
		const FInstancedStruct& RelevantData = ActionData->RelevantData;

		// Extract FSLFMontage from the FInstancedStruct
		const FSLFMontage* MontageData = RelevantData.GetPtr<FSLFMontage>();
		if (MontageData)
		{
			// Get the AnimMontage soft pointer from FSLFMontage
			const TSoftObjectPtr<UAnimMontage>& AnimMontageSoft = MontageData->AnimMontage;
			if (!AnimMontageSoft.IsNull())
			{
				// Play drink animation via BPI_GenericCharacter interface
				if (OwnerActor->Implements<UBPI_GenericCharacter>())
				{
					TSoftObjectPtr<UObject> SoftMontage(AnimMontageSoft.ToSoftObjectPath());
					IBPI_GenericCharacter::Execute_PlaySoftMontageReplicated(
						OwnerActor,
						SoftMontage,
						1.0,      // PlayRate
						0.0,      // StartPosition
						NAME_None, // StartSection
						false     // bPrio
					);
					UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Playing drink montage"));
				}
			}
		}
	}

	// 2. Heal the player by adjusting HP stat
	UStatManagerComponent* StatMgr = GetStatManager();
	if (StatMgr)
	{
		// Get HP tag
		FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));

		// Heal amount could come from flask data, but for now use a default value
		// In a full implementation, this would come from the flask item's properties
		float HealAmount = 100.0f;

		// Adjust HP stat (add health)
		StatMgr->AdjustStat(HPTag, ESLFValueType::CurrentValue, HealAmount, false, true);
		UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Healed %f HP"), HealAmount);
	}
}
