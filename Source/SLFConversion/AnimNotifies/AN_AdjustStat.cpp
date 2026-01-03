// AN_AdjustStat.cpp
// C++ Animation Notify implementation for AN_AdjustStat
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - adjusts a stat value during animation

#include "AnimNotifies/AN_AdjustStat.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StatManagerComponent.h"

UAN_AdjustStat::UAN_AdjustStat()
	: ValueType(ESLFValueType::CurrentValue)
	, Change(0.0)
	, TriggerRegen(false)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(100, 200, 100, 255); // Green for stat adjustment
#endif
}

void UAN_AdjustStat::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get StatManager and call AdjustStat with configured parameters
	UStatManagerComponent* StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (StatManager && StatTag.IsValid())
	{
		StatManager->AdjustStat(StatTag, ValueType, Change, TriggerRegen, true);
		UE_LOG(LogTemp, Log, TEXT("UAN_AdjustStat::Notify - Adjusted %s by %.2f on %s"),
			*StatTag.ToString(), Change, *Owner->GetName());
	}
}

FString UAN_AdjustStat::GetNotifyName_Implementation() const
{
	if (StatTag.IsValid())
	{
		return FString::Printf(TEXT("Adjust %s"), *StatTag.GetTagName().ToString());
	}
	return TEXT("Adjust Stat");
}
