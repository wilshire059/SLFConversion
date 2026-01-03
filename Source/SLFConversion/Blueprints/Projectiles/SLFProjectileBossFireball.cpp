// SLFProjectileBossFireball.cpp
#include "SLFProjectileBossFireball.h"

ASLFProjectileBossFireball::ASLFProjectileBossFireball()
{
	ProjectileSpeed = 1500.0f;
	bIsHoming = true;
	HomingStrength = 0.5f;
	UE_LOG(LogTemp, Log, TEXT("[ProjectileBossFireball] Initialized - Speed: %.0f, Homing: %s"),
		ProjectileSpeed, bIsHoming ? TEXT("Yes") : TEXT("No"));
}
