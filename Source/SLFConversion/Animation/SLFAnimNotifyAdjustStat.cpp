// SLFAnimNotifyAdjustStat.cpp
#include "SLFAnimNotifyAdjustStat.h"
#include "Components/StatManagerComponent.h"

FString USLFAnimNotifyAdjustStat::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Adjust Stat: %s"), *StatTag.ToString());
}

void USLFAnimNotifyAdjustStat::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_AdjustStat] %s adjusting %s by %.2f"),
		*Owner->GetName(), *StatTag.ToString(), AdjustAmount);

	// Get StatManagerComponent and adjust stat
	if (UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>())
	{
		// Adjust current value, not from level up, trigger regen
		StatManager->AdjustStat(StatTag, ESLFValueType::CurrentValue, AdjustAmount, false, true);
	}
}
