// SLFStatusEffectBleed.cpp
#include "SLFStatusEffectBleed.h"

USLFStatusEffectBleed::USLFStatusEffectBleed()
{
	// Bleed does burst damage then ticks
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 10.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBleed] Initialized - Duration: %.1fs"), EffectDuration);
}
