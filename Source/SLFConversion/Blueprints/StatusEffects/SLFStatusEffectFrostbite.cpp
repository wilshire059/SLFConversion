// SLFStatusEffectFrostbite.cpp
#include "SLFStatusEffectFrostbite.h"

USLFStatusEffectFrostbite::USLFStatusEffectFrostbite()
{
	// Frost damage + stamina debuff
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 15.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectFrostbite] Initialized - Duration: %.1fs"), EffectDuration);
}
