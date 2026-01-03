// SLFStatusEffectCorruption.cpp
#include "SLFStatusEffectCorruption.h"

USLFStatusEffectCorruption::USLFStatusEffectCorruption()
{
	// Dark corruption effect
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 20.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectCorruption] Initialized - Duration: %.1fs"), EffectDuration);
}
