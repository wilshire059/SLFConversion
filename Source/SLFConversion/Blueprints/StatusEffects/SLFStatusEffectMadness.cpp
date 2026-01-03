// SLFStatusEffectMadness.cpp
#include "SLFStatusEffectMadness.h"

USLFStatusEffectMadness::USLFStatusEffectMadness()
{
	// Madness - damages HP and FP
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 10.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectMadness] Initialized - Duration: %.1fs"), EffectDuration);
}
