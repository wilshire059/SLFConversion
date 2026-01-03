// SLFStatusEffectPoison.cpp
#include "SLFStatusEffectPoison.h"

USLFStatusEffectPoison::USLFStatusEffectPoison()
{
	// Configure poison defaults - ticking HP damage over time
	// Duration and tick interval are configured via the rank data in the data asset
	// TickStatChange array holds the stat adjustments to apply each tick
	EffectDuration = 30.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectPoison] Initialized - Duration: %.1fs"),
		EffectDuration);
}
