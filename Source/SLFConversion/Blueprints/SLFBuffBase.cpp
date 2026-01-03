// SLFBuffBase.cpp
// C++ implementation for B_Buff
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Buff
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         3/3 (initialized in constructor)
// Functions:         5/5 implemented (2 migrated + 3 lifecycle)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFBuffBase.h"
#include "StatManagerComponent.h"
#include "Curves/CurveFloat.h"
#include "SLFPrimaryDataAssets.h"

USLFBuffBase::USLFBuffBase()
{
	Rank = 1;
	BuffData = nullptr;
	OwnerActor = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1-2/5]
// ═══════════════════════════════════════════════════════════════════════════════

UStatManagerComponent* USLFBuffBase::GetOwnerStatManager_Implementation()
{
	if (OwnerActor)
	{
		return OwnerActor->FindComponentByClass<UStatManagerComponent>();
	}
	return nullptr;
}

float USLFBuffBase::GetMultiplierForCurrentRank_Implementation()
{
	// From JSON: BuffData->RankMultiplierCurve->GetFloatValue(Rank)
	// Returns 1.0 if curve is not valid
	if (BuffData && BuffData->RankMultiplierCurve)
	{
		float Multiplier = BuffData->RankMultiplierCurve->GetFloatValue(static_cast<float>(Rank));
		UE_LOG(LogTemp, Log, TEXT("[Buff] GetMultiplierForCurrentRank: Rank %d = %.2f"), Rank, Multiplier);
		return Multiplier;
	}

	UE_LOG(LogTemp, Log, TEXT("[Buff] GetMultiplierForCurrentRank: No curve, returning 1.0"));
	return 1.0f;
}

// ═══════════════════════════════════════════════════════════════════════════════
// LIFECYCLE [3-5/5]
// ═══════════════════════════════════════════════════════════════════════════════

void USLFBuffBase::ApplyBuff_Implementation()
{
	// Base implementation logs the buff application
	// Child classes override to apply actual stat modifiers
	if (BuffData)
	{
		FString TagName = BuffData->BuffTag.IsValid() ? BuffData->BuffTag.GetTagName().ToString() : BuffData->GetName();
		UE_LOG(LogTemp, Log, TEXT("[Buff] ApplyBuff: \"%s\" (Rank %d, Multiplier %.2f)"),
			*TagName, Rank, GetMultiplierForCurrentRank());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Buff] ApplyBuff: null BuffData (Rank %d)"), Rank);
	}
}

void USLFBuffBase::RemoveBuff_Implementation()
{
	// Base implementation logs the buff removal
	// Child classes override to remove actual stat modifiers
	if (BuffData)
	{
		FString TagName = BuffData->BuffTag.IsValid() ? BuffData->BuffTag.GetTagName().ToString() : BuffData->GetName();
		UE_LOG(LogTemp, Log, TEXT("[Buff] RemoveBuff: \"%s\""), *TagName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Buff] RemoveBuff: null BuffData"));
	}
}

void USLFBuffBase::OnGranted_Implementation()
{
	// From JSON: Prints 'Buff "{tag}" granted.' then nothing else in base class
	// Child classes override this event to apply actual effects
	if (BuffData)
	{
		FString TagName = BuffData->BuffTag.IsValid() ? BuffData->BuffTag.GetTagName().ToString() : BuffData->GetName();
		UE_LOG(LogTemp, Log, TEXT("[Buff] Buff \"%s\" granted."), *TagName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Buff] Buff (unknown) granted."));
	}
}

void USLFBuffBase::OnRemoved_Implementation()
{
	// From JSON: Prints 'Buff "{tag}" removed.' then nothing else in base class
	// Child classes override this event to remove actual effects
	if (BuffData)
	{
		FString TagName = BuffData->BuffTag.IsValid() ? BuffData->BuffTag.GetTagName().ToString() : BuffData->GetName();
		UE_LOG(LogTemp, Log, TEXT("[Buff] Buff \"%s\" removed."), *TagName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Buff] Buff (unknown) removed."));
	}
}

void USLFBuffBase::SetRank_Implementation(int32 NewRank)
{
	if (NewRank != Rank)
	{
		UE_LOG(LogTemp, Log, TEXT("[Buff] SetRank: %d -> %d"), Rank, NewRank);

		// Remove current buff effects
		RemoveBuff();

		// Update rank
		Rank = NewRank;

		// Reapply with new rank
		ApplyBuff();
	}
}
