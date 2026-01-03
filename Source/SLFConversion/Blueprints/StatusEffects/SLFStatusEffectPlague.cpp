// SLFStatusEffectPlague.cpp
#include "SLFStatusEffectPlague.h"

USLFStatusEffectPlague::USLFStatusEffectPlague()
{
	// Plague/Scarlet Rot - heavy DOT
	// Duration and tick interval are configured via the rank data in the data asset
	EffectDuration = 60.0f;

	UE_LOG(LogTemp, Log, TEXT("[StatusEffectPlague] Initialized - Duration: %.1fs"), EffectDuration);
}
