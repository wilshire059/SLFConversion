// SLFStatusEffectBurn.cpp
#include "SLFStatusEffectBurn.h"

USLFStatusEffectBurn::USLFStatusEffectBurn()
{
	// Fire damage over time
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 5.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectBurn] Initialized - Duration: %.1fs"), EffectDuration);
}
